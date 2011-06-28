#ifndef ELOG_LOGGER_FACTORY_H_
#define ELOG_LOGGER_FACTORY_H_

#include "scoped_ptr.h"
#include "singleton.h"
#include "stream_logger.h"
#include "util.h"

namespace LOG {

class LoggerFactory : Noncopyable {
 public:
  LoggerFactory()
      : logger_(GetDefaultLogger()) {
  }

  Logger& GetLogger() const {
    return *logger_;
  }

  void Reset(Logger* logger = NULL) {
    logger_ = logger ? logger : GetDefaultLogger();
  }

 private:
  static Logger* GetDefaultLogger() {
    return &Singleton<StreamLogger>::Get();
  }

  Logger* logger_;
};

inline Logger& GetLogger() {
  return Singleton<LoggerFactory>::Get().GetLogger();
}

}  // namespace LOG

#endif  // ELOG_LOGGER_FACTORY_H_
