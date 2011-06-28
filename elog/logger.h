#ifndef ELOG_LOGGER_H_
#define ELOG_LOGGER_H_

#include <exception>
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
struct LogLevelNamesTemplate {
  static const char* names[4];
};

template <AvoidODR N>
const char* LogLevelNamesTemplate<N>::names[4] = {
  "INFO",
  "WARN",
  "ERROR",
  "FATAL"
};

typedef LogLevelNamesTemplate<AVOID_ODR> LogLevelNames;

template <typename OutputStream>
inline void OutputLogLevelName(LogLevel level, OutputStream& stream) {
  const char* log_level_name = LogLevelNames::names[level];
  stream << "[" << log_level_name << "] ";
}

template <typename OutputStream>
inline void OutputFileLine(const char* source_file_name,
                           int line_number,
                           OutputStream& stream) {
  stream << source_file_name << '(' << line_number << "): ";
}

struct FatalLogError : virtual std::exception {};

class Logger {
 public:
  virtual ~Logger() {}

  virtual void PushMessage(LogLevel level,
                           const char* source_file_name,
                           int line_number,
                           const std::string& message) = 0;
};

}  // namespace LOG

#endif  // ELOG_LOGGER_H_
