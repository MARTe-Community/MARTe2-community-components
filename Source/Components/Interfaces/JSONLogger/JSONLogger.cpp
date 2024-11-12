/**
 * @file SysLogger.cpp
 * @brief Source file for class SysLogger
 * @date 17/03/2017
 * @author Andre Neto
 *
 * @copyright Copyright 2015 F4E | European Joint Undertaking for ITER and
 * the Development of Fusion Energy ('Fusion for Energy').
 * Licensed under the EUPL, Version 1.1 or - as soon they will be approved
 * by the European Commission - subsequent versions of the EUPL (the "Licence")
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at: http://ec.europa.eu/idabc/eupl
 *
 * @warning Unless required by applicable law or agreed to in writing,
 * software distributed under the Licence is distributed on an "AS IS"
 * basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the Licence permissions and limitations under the Licence.

 * @details This source file contains the definition of all the methods for
 * the class SysLogger (public, protected, and private). Be aware that some
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "JSONLogger.h"
#include "ErrorInformation.h"
#include "ErrorManagement.h"
#include "Logger.h"
#include "StreamString.h"
#include "StringHelper.h"
#include <cstdio>
#include <pthread.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

#define MAX_LEN 512
#define MAX_TRIES 10
#define KEY(str) "\"" str "\":"
#define STRFMT "\"%s\""
#define ESCAPE_CHR '\\'

namespace MARTe {

static const uint32 num_non_valid_char = 3;
static const char non_valid_char[num_non_valid_char] = {'\n', '\t', '"'};
static const char valid_char[num_non_valid_char] = {'n', 't', '"'};

/**
 @brief check if a char is a non-valid json string character
 **/
bool is_non_valid(const char c) {
  for (uint32 i = 0; i < num_non_valid_char; i++) {
    if (c == non_valid_char[i]) {
      return true;
    }
  }
  return false;
}

/**
 @brief count the number of non-valid json string characters inside a string
 **/
uint32 count_non_valid(const char *str, const uint32 len) {
  uint32 counter = 0;
  for (uint32 i = 0; i < len; i++) {
    if (is_non_valid(str[i]) && (i == 0 || str[i - 1] != ESCAPE_CHR)) {
      counter++;
    }
  }
  return counter;
}

/**
 @brief sanitize a string by replacing non-valid json string characters with
 their escape sequences (e.g. EOL -> '\n')
 **/
char *sanitize(const char *msg) {
  uint32 len = StringHelper::Length(msg);
  uint32 num_nvc = count_non_valid(msg, len);
  char *str = new char[len + num_nvc + 1];
  uint32 str_ind = 0;
  for (uint32 i = 0; i < len; i++) {
    if ((i == 0 || msg[i - 1] != ESCAPE_CHR)) {
      bool wrote = false;
      for (uint32 j = 0; j < num_non_valid_char; j++) {
        if (msg[i] == non_valid_char[j]) {
          str[str_ind++] = ESCAPE_CHR;
          str[str_ind++] = valid_char[j];
          wrote = true;
          break;
        }
      }
      if (!wrote) {
        str[str_ind++] = msg[i];
      }
    } else {
      str[str_ind++] = msg[i];
    }
  }
  str[str_ind] = 0;
  return str;
}

/**
 @brief convert a MARTe::TimeStamp to a YYYY-MM-DDThh:mm:ss string
 **/
void time_stamp(StreamString &str, const TimeStamp &ts) {
  str.Printf("%d-%02d-%02dT%02d:%02d:%02d.%06d\0", ts.GetYear(),
             1 + ts.GetMonth(), ts.GetDay() + 1, ts.GetHour(), ts.GetMinutes(),
             ts.GetSeconds(), ts.GetMicroseconds());
}

/**
 @brief convert an epoch timestamp to a YYYY-MM-DDThh:mm:ss string
 **/
void time_stamp(StreamString &str, uint64 epoch) {
  TimeStamp ts;
  ts.ConvertFromEpoch(epoch);
  time_stamp(str, ts);
}

/**
 @brief Flusher async rutine
 @details The flusher wait a defined amount of time and after try to flush the
 given file.

 After it checks if the file is still valid and if not try to open it again.
 This is done after the flushing because the content not flushed will be lost
 anyway.
 **/
void *flush(void *payload) {
  JSONLogger::flusher_t &info = *(JSONLogger::flusher_t *)payload;
  usleep(info.flush_delay_us);
  pthread_mutex_lock(&info.mutex);
  struct stat buff;
  fflush(info.file);
  if (info.file == NULL_PTR(FILE *)) {
    // if file not opened
    info.file = fopen(info.logpath, "a");
  } else if (info.file != stdout && info.file != stderr &&
             stat(info.logpath, &buff) != 0) {
    // if file has been removed or renamed
    fclose(info.file);
    info.file = fopen(info.logpath, "a");
  }

  if (info.file == NULL_PTR(FILE *)) {
    fprintf(stderr, "  \e[31m[ERROR]\e[0m Impossible to open logfile: %s\n",
            info.logpath);
    info.file = stdout;
  }
  info.idle = true;
  pthread_mutex_unlock(&info.mutex);
  return 0;
}

/**
 @brief start the flusher thread
 **/
void flush_after(JSONLogger::flusher_t &info) {
  pthread_t thread;
  info.idle = false;
  pthread_create(&thread, NULL_PTR(const pthread_attr_t *), flush,
                 (void *)&info);
}
} // namespace MARTe
/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
namespace MARTe {
JSONLogger::JSONLogger() : Object(), LoggerConsumerI() {
  info.logpath = NULL_PTR(char *);
  session_id = NULL_PTR(char *);
  unique_file = false;
  info.file = NULL_PTR(FILE *);
  info.idle = true;
  pthread_mutex_init(&info.mutex, NULL_PTR(pthread_mutexattr_t *));
}

JSONLogger::~JSONLogger() {
  pthread_mutex_lock(&info.mutex);
  if (info.file != NULL_PTR(FILE *) && info.file != stdout) {
    fclose(info.file);
  }
  pthread_mutex_unlock(&info.mutex);
  if (info.logpath != NULL_PTR(char *)) {
    delete[] info.logpath;
  }
  if (session_id != NULL_PTR(char *)) {
    delete[] session_id;
  }
  pthread_mutex_destroy(&info.mutex);
}

void JSONLogger::ConsumeLogMessage(LoggerPage *const logPage) {
  if (logPage != NULL_PTR(LoggerPage *)) {

    StreamString tsstr;
    time_stamp(tsstr, logPage->errorInfo.timeSeconds);
    StreamString errstr;
    ErrorManagement::ErrorCodeToStream(logPage->errorInfo.header.errorType,
                                       errstr);

    pthread_mutex_lock(&info.mutex);
    char *msg = sanitize(logPage->errorStrBuffer);
    fprintf(info.file,
            "{" KEY("session_id") STRFMT ", " KEY("timestamp") STRFMT
            ", " KEY("hr_time") "%llu"
                                ", " KEY("error_type") STRFMT
            ", " KEY("file_name") STRFMT
            ", " KEY("line_number") "%d"
                                    ", " KEY("message") STRFMT "}\n",
            session_id, tsstr.Buffer(), logPage->errorInfo.hrtTime,
            errstr.Buffer(), logPage->errorInfo.fileName,
            logPage->errorInfo.header.lineNumber, msg);
    delete[] msg;

    if (info.idle) {
      pthread_mutex_unlock(&info.mutex);
      flush_after(info);
    } else {
      pthread_mutex_unlock(&info.mutex);
    }
  }
}

bool JSONLogger::Initialise(StructuredDataI &data) {
  bool ok = Object::Initialise(data);
  session_id = new char[60];
  memset(session_id, 0, 60);

  time_t epoch = time(NULL);
  sprintf(session_id, "%ld", epoch);
  if (!data.Read("UniqueFile", unique_file)) {
    unique_file = false;
  }

  StreamString str;

  if (!(ok = data.Read("LogPath", str))) {
    fprintf(stderr, "  \e[31m[ERROR]\e[0m Missing mandatory field `LogPath`\n");
  } else if (StringHelper::Compare(str.Buffer(), "stdout") == 0) {
    fprintf(stderr, "  \e[34m[INFO]\e[0m Logging to `stdout`\n");
    info.file = stdout;
  } else if (StringHelper::Compare(str.Buffer(), "stderr") == 0) {
    fprintf(stderr, "  \e[34m[INFO]\e[0m Logging to `stderr`\n");
    info.file = stderr;
  } else {
    if (unique_file) {
      str += "_";
      str += session_id;
    }
    str += ".log";
    info.logpath = new char[str.Size() + 1];
    StringHelper::Copy(info.logpath, str.Buffer());
    info.logpath[str.Size()] = 0;
    fprintf(stderr, "  \e[34m[INFO]\e[0m Log path: %s\n", info.logpath);
    info.file = fopen(info.logpath, "a");
    if (info.file == NULL_PTR(FILE *)) {
      ok = false;
      fprintf(stderr, "  \e[31m[ERROR]\e[0m Impossible to open `%s`\n",
              info.logpath);
    }
  }
  if (!data.Read("FlushDelay", info.flush_delay_us)) {
    info.flush_delay_us = 1000;
    fprintf(stderr, "  \e[34m[INFO]\e[0m Flush delay: 1ms\n");
  }

  return ok;
}

CLASS_REGISTER(JSONLogger, "1.0")

} // namespace MARTe
