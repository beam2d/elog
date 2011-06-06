#pragma once

// Copyright (c) 2011 Seiya Tokui <beam.web@gmail.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, this list
// of conditions and the following disclaimer. Redistributions in binary form must
// reproduce the above copyright notice, this list of conditions and the following
// disclaimer in the documentation and/or other materials provided with the
// distribution. Neither the name of the beam util author(s) nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// benchmark utility on eLog

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#ifdef _MSC_VER
# pragma comment(lib, "winmm.lib")
# include <mmsystem.h>
# include <windows.h>
#else  // _MSC_VER
# include <sys/time.h>
#endif  // _MSC_VER
#include "elog.h"

namespace LOG {

enum msvc_timer_type_t {
  QUERY_PERFORMANCE_COUNTER = 0,
  TIME_GET_TIME
};
static msvc_timer_type_t& msvc_timer_type = global<msvc_timer_type_t>::value;

inline double get_time() {
#ifdef _MSC_VER
  if (msvc_timer_type == TIME_GET_TIME) return timeGetTime() / 1000.0;
  LARGE_INTEGER t, f;
  QueryPerformanceCounter(&t);
  QueryPerformanceFrequency(&f);
  return t.QuadPart * 1.0 / f.QuadPart;
#else  // _MSC_VER
  timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec * 1e-6;
#endif  // _MSC_VER
}

class timer {
  double start_;
 public:
  timer() : start_(get_time()) {}
  operator double() const { return get_time() - start_; }
};

struct safe_false {
  operator char safe_false::*() const { return 0; }
};

class scoped_benchmark : public safe_false {
  timer timer_;
  std::string title_;
  log_level level_;

 public:
  explicit scoped_benchmark(const std::string& title, log_level level = INFO)
      : title_(title), level_(level) {}

  ~scoped_benchmark() {
    if (level_ < logger.level()) return;
    std::ostringstream message;
    message << title_ << ": " << timer_ << " sec";
    logger.write(level_, message.str());
  }

  timer get_timer() const { return timer_; }
};

class benchmark;

class scoped_benchmark_case : public safe_false {
  std::size_t index_;
  timer timer_;
  benchmark& benchmark_;
 public:
  scoped_benchmark_case(std::size_t index, benchmark& bench)
      : index_(index), benchmark_(bench) {}
  ~scoped_benchmark_case() { end(); }
  void end() const;
};

class benchmark {
  std::string title_;
  std::vector<std::pair<std::string, double> > chart_;
  int precision_;

  static int pos_decimal_point(double x) {
    int pos = 1;
    while (x >= 10) {
      x /= 10;
      ++pos;
    }
    return pos;
  }

 public:
  explicit benchmark(const std::string& title)
      : title_(title), precision_(5) {}

  const std::string& title() const { return title_; }
  const std::vector<std::pair<std::string, double> >& chart() const {
    return chart_;
  }

  scoped_benchmark_case begin_case(const std::string& title) {
    const std::size_t index = chart_.size();
    chart_.push_back(std::make_pair(title, -1.0));
    return scoped_benchmark_case(index, *this);
  }

  void end_case(std::size_t index, double time) {
    if (chart_[index].second < 0) chart_[index].second = time;
  }

  void precision(int prec) { precision_ = prec; }

  void put_chart(std::ostream& os = std::cerr) const {
    static const char TIMESEC[] = "time (sec)";
    std::size_t len = title_.size();
    int max_dec = 1;
    for (std::size_t i = 0; i < chart_.size(); ++i) {
      len = std::max(len, chart_[i].first.size());
      max_dec = std::max(max_dec, pos_decimal_point(chart_[i].second));
    }
    const int time_width = max_dec + precision_ + 1;
    const int sep_right_width =
        std::max(time_width, static_cast<int>(sizeof(TIMESEC)) - 1);
    os << std::left
       << std::setw(len) << title_ << " | " << TIMESEC << '\n';
    os << std::setw(len) << std::setfill('-') << "" << "-+-"
       << std::setw(sep_right_width) << std::setfill('-') << "" << '\n';
    os << std::setfill(' ') << std::fixed << std::setprecision(precision_);
    for (std::size_t i = 0; i < chart_.size(); ++i) {
      os << std::left << std::setw(len) << chart_[i].first << " | "
         << std::right << std::setw(time_width)
         << chart_[i].second << '\n';
    }
    os << std::flush;
  }
};

inline void scoped_benchmark_case::end() const {
  benchmark_.end_case(index_, timer_);
}

}  // namespace LOG

#define BENCHMARK(...) \
  ELOG_DETAIL_OVERLOAD(ELOG_DETAIL_BENCHMARK_, __VA_ARGS__)
#define ELOG_DETAIL_BENCHMARK_1(title) \
  if (::LOG::scoped_benchmark BENCHMARK = \
      ::LOG::scoped_benchmark(title, ::LOG::INFO)); else
#define ELOG_DETAIL_BENCHMARK_2(bench, title) \
  if (::LOG::scoped_benchmark_case BENCHMARK = \
      bench.begin_case(title)); else
