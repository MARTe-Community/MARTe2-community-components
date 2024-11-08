#ifndef _EC_LOGGING_H__
#define _EC_LOGGING_H__

namespace ect {
namespace log {
enum level_e { SILENT = 0, ERROR, WARNING, INFO, DEBUG };
level_e GetLogLevel();

void error(const char *fmt, ...);
void warning(const char *fmt, ...);
void info(const char *fmt, ...);
void debug(const char *fmt,...);
void init();
} // namespace log
} // namespace ect

#endif
