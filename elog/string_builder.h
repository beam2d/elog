// Copyright (c) 2011 Seiya Tokui <beam.web@gmail.com>. All Rights Reserved.
// This source code is distributed under MIT License in LICENSE file.

#ifndef ELOG_STRING_BUILDER_H_
#define ELOG_STRING_BUILDER_H_

#include <sstream>
#include <string>

namespace LOG {

class StringBuilder {
 public:
  template <typename T>
  void AddValue(const T& t) {
    stream_ << t;
  }

  // Show (un)signed char as an integer value rather than a character.
  void AddValue(signed char t) {
    stream_ << static_cast<int>(t);
  }
  void AddValue(unsigned char t) {
    stream_ << static_cast<unsigned int>(t);
  }

  std::string GetString() const {
    return stream_.str();
  }

 private:
  std::ostringstream stream_;
};

}  // namespace LOG

#endif  // ELOG_STRING_BUILDER_H_

