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

#include <iomanip>
#include <ostream>
#include <string>
#include <vector>
#ifdef _MSC_VER
# include <functional>
# pragma comment(lib, "winmm.lib")
# include <mmsystem.h>
# include <windows.h>
#else  // _MSC_VER
# include <tr1/functional>
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

class scoped_verbose_benchmark : public safe_false {
  timer timer_;
  std::string title_;
  type_id id_;
  int verbosity_;

 public:
  template <typename Module>
  explicit scoped_verbose_benchmark(const std::string& title, int verbosity)
      : title_(title), id_(get_type_id<Module>), verbosity_(verbosity) {}

  ~scoped_verbose_benchmark() {
    if (logger.verbosity(id_) < verbosity_) return;
    std::ostringstream message;
    message << title_ << ": " << timer_ << " sec";
    logger.verbose_write(id_, verbosity_, message.str());
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
  ~scoped_benchmark_case();
};

class benchmark {
  friend class scoped_benchmark_case;

  std::string title_;
  std::vector<std::string> titles_;
  std::vector<double> times_;

  void end_case(std::size_t index, double time_) {
    if (times_.size() <= index) times_.resize(index + 1);
    times_[index] = time_;
  }

 public:
  explicit benchmark(const std::string& title) : title_(title) {}

  scoped_benchmark_case begin_case(const std::string& title) {
    scoped_benchmark_case sc(titles_.size(), *this);
    titles_.push_back(title);
    return sc;
  }

  void put_chart(std::ostream& os = std::cerr) {
    std::size_t len = title_.size();
    for (std::size_t i = 0; i < titles_.size(); ++i) {
      if (len < titles_[i].size()) len = titles_[i].size();
    }
    os << std::left;
    os << std::setw(len) << title_ << " | time (sec)" << std::endl;
    os << std::setw(len) << std::setfill('-') << ""
       << "-+-" << std::setw(15) << std::setfill('-') << "" << std::endl;
    os << std::setfill(' ');
    for (std::size_t i = 0; i < titles_.size(); ++i) {
      os << std::setw(len) << titles_[i] << " | " << times_[i] << std::endl;
    }
  }
};

inline scoped_benchmark_case::~scoped_benchmark_case() {
  benchmark_.end_case(index_, timer_);
}

}  // namespace LOG

#define BENCHMARK(...) \
  ELOG_DETAIL_OVERLOAD(ELOG_DETAIL_BENCHMARK_, __VA_ARGS__)
#define ELOG_DETAIL_BENCHMARK_1(title) \
  ELOG_DETAIL_BENCHMARK_2(title, ::LOG::INFO)
#define ELOG_DETAIL_BENCHMARK_2(title, level) \
  if (::LOG::scoped_benchmark BENCHMARK = \
      ::LOG::scoped_benchmark(title, level)); else

#define BENCHMARK_CASE(...) \
  ELOG_DETAIL_OVERLOAD(ELOG_DETAIL_BENCHMARK_CASE_, __VA_ARGS__)
#define ELOG_DETAIL_BENCHMARK_CASE_2(benchmark, title) \
  if (::LOG::scoped_benchmark_case BENCHMARK_CASE = \
      benchmark.begin_case(title)); else

#ifdef NDEBUG
# define DBENCHMARK(...) if (0); else
# define DBENCHMARK_CASE(...) if (0); else
#else
# define DBENCHMARK BENCHMARK
# define DBENCHMARK_CASE DBENCHMARK_CASE
#endif
