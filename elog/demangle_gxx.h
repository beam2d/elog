#ifndef ELOG_DEMANGLE_GXX_H_
#define ELOG_DEMANGLE_GXX_H_

#include <tr1/memory>
#include <cxxabi.h>

namespace LOG {

class Demangle {
 public:
  explicit Demangle(const char* mangled_name) {
    int status;
    char* demangled_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);
    type_name_.reset(demangled_name);
  }

  const char* GetName() const {
    return type_name_.get();
  }

 private:
  std::tr1::shared_ptr<char> type_name_;
};

}  // namespace LOG

#endif  // ELOG_DEMANGLE_GXX_H_
