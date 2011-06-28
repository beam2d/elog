#ifndef ELOG_TYPE_INFO_H_
#define ELOG_TYPE_INFO_H_

#include <typeinfo>
#ifdef __GNUC__
# include <tr1/functional>
#else
# include <functional>
#endif
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
  friend struct std::tr1::hash<TypeInfo>;

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

namespace std {
namespace tr1 {

template <>
struct hash< ::LOG::TypeInfo>
    : std::unary_function< ::LOG::TypeInfo, std::size_t> {
  std::size_t operator()(::LOG::TypeInfo type_info) const {
    return hash<const char*>()(type_info.global_type_name_);
  }
};

}  // namespace std::tr1
}  // namespace std

#endif  // ELOG_TYPE_INFO_H_
