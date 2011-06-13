#ifndef ELOG_TYPE_INFO_H_
#define ELOG_TYPE_INFO_H_

#include <typeinfo>
#include "demangle.h"

namespace LOG {

template <typename T> class Type {};

// Type name of T.
// For each type T, the value of GlobalTypeName<T>::name as a pointer is
// unique for all compilation units, even if addresses of typeid(T) are
// different.
template <typename T>
struct GlobalTypeName {
  static const char* name;
};

template <typename T>
const char* GlobalTypeName<T>::name = typeid(T).name();

class TypeInfo {
 public:
  template <typename T>
  explicit TypeInfo(Type<T>)
      : global_type_name_(GlobalTypeName<T>::name) {
  }

  Demangle GetTypeName() const {
    return Demangle(global_type_name_);
  }

 private:
#define ELOG_I_DEFINE_TYPEINFO_OPERATOR(op) \
  friend bool operator op(const TypeInfo& lhs, const TypeInfo& rhs) { \
    return lhs.global_type_name_ op rhs.global_type_name_; \
  }

  ELOG_I_DEFINE_TYPEINFO_OPERATOR(==)
  ELOG_I_DEFINE_TYPEINFO_OPERATOR(!=)
  ELOG_I_DEFINE_TYPEINFO_OPERATOR(<)
  ELOG_I_DEFINE_TYPEINFO_OPERATOR(<=)
  ELOG_I_DEFINE_TYPEINFO_OPERATOR(>=)
  ELOG_I_DEFINE_TYPEINFO_OPERATOR(>)

#undef ELOG_I_DEFINE_TYPEINFO_OPERATOR

  const char* global_type_name_;
};


}  // namespace LOG

#endif  // ELOG_TYPE_INFO_H_
