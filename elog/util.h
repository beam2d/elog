#ifndef ELOG_UTIL_H_
#define ELOG_UTIL_H_

namespace LOG {

enum AvoidODR {
  AVOID_ODR
};

struct Call {
  template <typename Function>
  void operator()(Function function) const {
    function();
  }
};

class NonCopyable {
 protected:
  NonCopyable() {}
  ~NonCopyable() {}

 private:
  NonCopyable(const NonCopyable&);  // no implementation
  NonCopyable& operator=(const NonCopyable&);  // no implementation
};

}  // namespace LOG

#endif  // ELOG_UTIL_H_
