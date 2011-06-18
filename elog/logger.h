#ifndef ELOG_LOGGER_H_
#define ELOG_LOGGER_H_

#include <string>
#include "type_info.h"
#include "util.h"

namespace LOG {

enum LogLevel {
  INFO,
  WARN,
  ERROR,
  FATAL
};

inline bool IsLogLevelSevereEnough(LogLevel log_level, LogLevel logger_level) {
  return log_level >= logger_level;
}

template <AvoidODR>
struct LogLevelNames {
  static const char* names[4];
};

template <AvoidODR N>
const char* LogLevelNames<N>::names[4] = {
  "INFO",
  "WARN",
  "ERROR",
  "FATAL"
};

static const char** log_level_names = LogLevelNames<AVOID_ODR>::names;

template <typename OutputStream>
inline void OutputLogLevelName(LogLevel level, OutputStream& stream) {
  const char* log_level_name = log_level_names[level];
  stream << "[" << log_level_name << "] ";
}

class Logger {
 public:
  virtual ~Logger() {}

  virtual void PushMessage(LogLevel level,
                           const std::string& message) = 0;
};

}  // namespace LOG

#endif  // ELOG_LOGGER_H_
