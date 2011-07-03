// Copyright (c) 2011 Seiya Tokui <beam.web@gmail.com>. All Rights Reserved.
// This source code is distributed under MIT License in LICENSE file.

#ifndef ELOG_GENERAL_LOG_H_
#define ELOG_GENERAL_LOG_H_

#include <sstream>
#include <string>
#include "logger.h"
#include "logger_factory.h"
#include "put_as_string.h"

namespace LOG {

template <LogLevel LEVEL>
class GeneralLog {
 public:
  GeneralLog(const char* source_file_name,
             int line_number,
             Logger* logger = NULL)
      : logger_(logger ? *logger : GetLogger()),
        source_file_name_(source_file_name),
        line_number_(line_number) {
  }

  template <typename T>
  GeneralLog& operator<<(const T& t) {
    PutAsString(t, stream_);
    return *this;
  }

  GeneralLog& GetReference() {
    return *this;
  }

  void PushMessage() const {
    logger_.PushMessage(LEVEL, source_file_name_, line_number_, stream_.str());
  }

 private:
  Logger& logger_;
  std::ostringstream stream_;
  const char* source_file_name_;
  int line_number_;
};

template <>
void GeneralLog<FATAL>::PushMessage() const {
  logger_.PushFatalMessageAndThrow(
      source_file_name_, line_number_, stream_.str());
}

template <>
void GeneralLog<CHECK>::PushMessage() const {
  logger_.PushCheckMessageAndThrow(
      source_file_name_, line_number_, stream_.str());
}

}  // namespace LOG

#endif  // ELOG_GENERAL_LOG_H_
