#ifndef LOGGING_H__
#define LOGGING_H__

namespace MARTe {
namespace log {
enum level_e { SILENT = 0, ERROR, WARNING, INFO, DEBUG };
level_e GetLogLevel();

void error(const char *fmt, ...);
void warning(const char *fmt, ...);
void info(const char *fmt, ...);
void debug(const char *fmt, ...);
void init();
} // namespace log
} // namespace MARTe

#endif
