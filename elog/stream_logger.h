#ifndef ELOG_STREAM_LOGGER_H_
#define ELOG_STREAM_LOGGER_H_

#include <iostream>
#include "logger.h"
#include "mutex.h"

namespace LOG {

class StreamLogger : public Logger {
 public:
  explicit StreamLogger(std::ostream& stream = std::clog)
      : stream_(stream),
        level_(INFO) {
  }

  void set_level(LogLevel level) {
    level_ = level;
  }

  virtual void PushMessage(LogLevel level,
                           const char* source_file_name,
                           int line_number,
                           const std::string& message) {
    if (!IsLogLevelSevereEnough(level, level_)) return;
    PushMessageWithoutCheck(level, source_file_name, line_number, message);
    ThrowOnFatalLog(level);
  }

 private:
  void PushMessageWithoutCheck(LogLevel level,
                               const char* source_file_name,
                               int line_number,
                               const std::string& message) {
    MutexLock lock(mutex_);
    OutputLogLevelName(level, stream_);
    OutputFileLine(source_file_name, line_number, stream_);
    stream_ << message << std::endl;
  }

  void ThrowOnFatalLog(LogLevel level) const {
    if (level == FATAL) {
      throw FatalLogError();
    }
  }

  std::ostream& stream_;
  Mutex mutex_;
  LogLevel level_;
};

}  // namespace LOG

#endif  // ELOG_STREAM_LOGGER_H_
