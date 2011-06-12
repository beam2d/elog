#ifndef ELOG_UTIL_H_
#define ELOG_UTIL_H_

namespace LOG {

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
