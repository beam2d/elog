#ifndef ELOG_TIMER_H_
#define ELOG_TIMER_H_

#ifdef _WIN32
# include "get_time_win.h"
#else
# include "get_time_posix.h"
#endif

namespace LOG {

class Timer {
 public:
  Timer()
      : start_sec_(GetTimeSec()) {
  }

  operator double() const {
    const double now = GetTimeSec();
    return now - start_sec_;
  }

 private:
  double start_sec_;
};

}  // namespace LOG

#endif  // ELOG_TIMER_H_
