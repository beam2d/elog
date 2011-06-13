#ifndef ELOG_TYPE_INFO_H_
#define ELOG_TYPE_INFO_H_

#include <typeinfo>
#include "demangle.h"

namespace LOG {

template <typename T> class Type {};

class TypeInfo {
 public:
  template <typename T>
  explicit TypeInfo(Type<T>)
      : typeid_name_(typeid(T).name()) {
  }

  Demangle GetTypeName() const {
    return Demangle(typeid_name_);
  }

 private:
  friend bool operator==(const TypeInfo& lhs, const TypeInfo& rhs);
  friend bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs);

  const char* typeid_name_;
};

inline bool operator==(const TypeInfo& lhs, const TypeInfo& rhs) {
  return lhs.typeid_name_ == rhs.typeid_name_;
}

inline bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs) {
  return !(lhs == rhs);
}

}  // namespace LOG

#endif  // ELOG_TYPE_INFO_H_
