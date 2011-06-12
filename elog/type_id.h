#ifndef ELOG_TYPE_ID_H_
#define ELOG_TYPE_ID_H_

#include <typeinfo>
#include "demangle.h"

namespace LOG {

template <typename T> class Type {};

class TypeId {
 public:
  template <typename T>
  explicit TypeId(Type<T>)
      : typeid_name_(typeid(T).name()) {
  }

  Demangle GetTypeName() const {
    return Demangle(typeid_name_);
  }

 private:
  friend bool operator==(const TypeId& lhs, const TypeId& rhs);
  friend bool operator!=(const TypeId& lhs, const TypeId& rhs);

  const char* typeid_name_;
};

inline bool operator==(const TypeId& lhs, const TypeId& rhs) {
  return lhs.typeid_name_ == rhs.typeid_name_;
}

inline bool operator!=(const TypeId& lhs, const TypeId& rhs) {
  return !(lhs == rhs);
}

}  // namespace LOG
#endif  // ELOG_TYPE_ID_H_
