#ifndef ELOG_ATOMIC_H_
#define ELOG_ATOMIC_H_

#ifdef _WIN32
# include <windows.h>
#elif defined(__APPLE__) && __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1050
# define ELOG_I_ATOMIC_CAS_USE_MACOSX_OSATOMIC
# include <libkern/OSAtomic.h>
#elif __GNUC__ * 100 + __GNUC_MINOR__ >= 401
# define ELOG_I_ATOMIC_CAS_USE_GNUC_EXTENSION
#else
# error eLog does not support g++ < 4.01
#endif

namespace LOG {

// TODO(S.Tokui): Write unittest.
inline int CompareAndSwap(volatile int& val, int oldval, int newval) {
#ifdef _WIN32
  return InterlockedCompareExchange(&val, newval, oldval);
#elif defined(ELOG_I_ATOMIC_CAS_USE_MACOSX_OSATOMIC)
  return OSAtomicCompareAndSwapIntBarrier(oldval, newval, &val) ? oldval : val;
#elif defined(ELOG_I_ATOMIC_CAS_USE_GNUC_EXTENSION)
  return __sync_val_compare_and_swap(&val, oldval, newval);
#endif
}

// We wanted to let OnceFlag a POD type.
typedef volatile int OnceFlag;
static const OnceFlag ONCE_INIT = 0;
static const OnceFlag ONCE_CALLED = 1;

template <typename Function>
inline void CallOnce(OnceFlag& once_flag, Function function) {
  if (CompareAndSwap(once_flag, ONCE_INIT, ONCE_CALLED) == ONCE_INIT) {
    function();
  }
}

}  // namespace LOG

#endif  // ELOG_ATOMIC_H_
