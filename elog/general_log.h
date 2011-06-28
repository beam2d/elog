#ifndef ELOG_GENERAL_LOG_H_
#define ELOG_GENERAL_LOG_H_

#include <sstream>
#include <string>
#include "logger.h"
#include "logger_factory.h"

namespace LOG {

class GeneralLog {
 public:
  explicit GeneralLog(LogLevel log_level,
                      Logger* logger = NULL)
      : logger_(logger ? *logger : GetLogger()),
        log_level_(log_level) {
  }

  template <typename T>
  GeneralLog& operator<<(const T& t) {
    string_builder_ << t;
    return *this;
  }

  void PushMessage() const {
    const std::string& message = string_builder_.str();
    logger_.PushMessage(log_level_, message);
  }

 private:
  Logger& logger_;
  std::ostringstream string_builder_;
  LogLevel log_level_;
};

}  // namespace LOG

#endif  // ELOG_GENERAL_LOG_H_
