#include "Logging.h"
#include <pthread.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

namespace MARTe {
namespace log {
static bool __init = false;
static level_e __log_level = ERROR;

level_e GetLogLevel() { return __log_level; }

void __log(level_e l, const char *header, const char *fmt, va_list &args) {
  if (!__init) {
    init();
  }
  if (__log_level >= l) {
    printf("%s", header);
    vprintf(fmt, args);
  }
}

void error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  __log(ERROR, "[ \e[1;31mERROR\e[m   ] ", fmt, args);
  va_end(args);
}

void warning(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  __log(WARNING, "[ \e[33mWARNING\e[m ] ", fmt, args);
  va_end(args);
}

void info(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  __log(INFO, "[ \e[34mINFO\e[m    ] ", fmt, args);
  va_end(args);
}
void debug(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  __log(DEBUG, "[ \e[35mDEBUG\e[m   ] ", fmt, args);
  va_end(args);
}

void init() {
  if (!__init) {
    __init = true;
  }
  char *log_level = getenv("LOG_LEVEL");
  log::level_e level = log::ERROR;
  if (log_level != NULL) {
    if (!strcmp(log_level, "SILENT")) {
      level = log::SILENT;
    } else if (!strcmp(log_level, "WARNING")) {
      level = log::WARNING;
    } else if (!strcmp(log_level, "INFO")) {
      level = log::INFO;
    } else if (!strcmp(log_level, "DEBUG")) {
      level = log::DEBUG;
    }
  }
  __log_level = level;
}

} // namespace log
} // namespace MARTe
