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

struct EmptyFunction {
  template <typename T>
  void operator()(T) const {
  }
};

class Noncopyable {
 protected:
  Noncopyable() {}
  ~Noncopyable() {}

 private:
  Noncopyable(const Noncopyable&);  // no implementation
  Noncopyable& operator=(const Noncopyable&);  // no implementation
};

template <typename T>
void CheckedDelete(T* p) {
  typedef char T_must_be_complete[sizeof(T) ? 1 : -1];
  (void) sizeof(T_must_be_complete);
  delete p;
}

}  // namespace LOG

#endif  // ELOG_UTIL_H_
