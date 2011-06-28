#ifndef ELOG_TYPED_LOG_H_
#define ELOG_TYPED_LOG_H_

#include <cstddef>
#include <sstream>
#include <string>
#include "logger.h"
#include "logger_factory.h"
#include "type_info.h"

namespace LOG {

class TypedLog {
 public:
  TypedLog(TypeInfo type_info,
           int verbosity,
           const char* source_file_name,
           int line_number,
           Logger* logger = NULL)
      : logger_(logger ? *logger : GetLogger()),
        type_info_(type_info),
        verbosity_(verbosity),
        source_file_name_(source_file_name),
        line_number_(line_number) {
  }

  template <typename T>
  TypedLog& operator<<(const T& t) {
    string_builder_ << t;
    return *this;
  }

  // Output 1-byte value as an integer rather than a character.
  TypedLog& operator<<(signed char t) {
    string_builder_ << static_cast<int>(t);
    return *this;
  }

  // ditto
  TypedLog& operator<<(unsigned char t) {
    string_builder_ << static_cast<unsigned int>(t);
    return *this;
  }

  TypedLog& GetReference() {
    return *this;
  }

  void PushMessage() const {
    const std::string& message = string_builder_.str();
    logger_.PushTypedMessage(type_info_, verbosity_,
                             source_file_name_, line_number_, message);
  }

 private:
  Logger& logger_;
  std::ostringstream string_builder_;
  TypeInfo type_info_;
  int verbosity_;
  const char* source_file_name_;
  int line_number_;
};

}  // namespace LOG

#endif  // ELOG_TYPED_LOG_H_
