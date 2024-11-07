/**
 * @file JSONLogger.h
 * @brief Header file for class JSONLogger
 * @date 18/06/2024
 * @author Martino Ferrari
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

 * @details This header file contains the declaration of the class JSONLogger
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef SYSLOGGER_H_
#define SYSLOGGER_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "LoggerConsumerI.h"
#include "Object.h"
#include <pthread.h>

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

namespace MARTe {
/**
 @brief A LoggerConsumerI which outputs the log messages to a `jsonl` log
 file.
 
 @details The `JSONLogger` outputs log to file or standard output (or error) using
 a jsonl format. In the output the log information (timestamp, high resolution timer, 
 error tpe, filepath, line and message) are stored as a simple json object.

 To avoid performance bottlenecks an async flusher takes care of flushing the file 
 and in case that the output file has been deleted or moved it also ensure to create
 a new one. 
 
 The `JSONLogger` configuration require only the following parameters:
 ```cue
 {
    Class: "JSONLogger"
    LogPath: string | "stdout" | "stderr" // log destination path or stdout or stderr
    UniqueFile?: bool | *false // use unique file name
    FlushDelay?: uint32 | *1000 // flush delay in microseconds
 }
 ``` 
 
 If you plan to use it within your MARTe application please remember to set it up inside
 the `LoggerService`.
  */
class JSONLogger : public Object, public LoggerConsumerI {
public:
  CLASS_REGISTER_DECLARATION()

  /**
   * @brief Constructor. NOOP.
   */
  JSONLogger();

  /**
   * @brief Destructor. Closes the console.NOOP.
   */
  virtual ~JSONLogger();

  /**
   * @brief Prints the logPage in the console output.
   * @param logPage the log message to be printed.
   */
  virtual void ConsumeLogMessage(LoggerPage *logPage);

  /**
   * @brief Calls Object::Initialise and reads the logger parameters
   * @param[in] data see Object::Initialise.
   * @return true if Object::Initialise returns true and if the compulsory are
   * correctly set..
   */
  virtual bool Initialise(StructuredDataI &data);

  /**
   * @brief struct to be shared with flusher thread.
   **/
  struct flusher_t {
    char *logpath;
    FILE *file;
    bool idle;
    uint32 flush_delay_us;
    pthread_mutex_t mutex;
  };

private:
  char *session_id;
  bool unique_file;

  flusher_t info;
};
} // namespace MARTe

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* SYSLOGGER_H_ */
