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

// light weight logging utility for C++

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>
#ifdef __GNUC__
# include <tr1/unordered_map>
# include <cxxabi.h>
#else
# include <unordered_map>
#endif
#ifdef _WIN32
# include <windows.h>
# undef ERROR  // Log-level ERROR conflicts with the macro of windows.h
#else
# include <pthread.h>
#endif

namespace LOG {

enum avoid_odr { AVOID_ODR };

class noncopyable {
  noncopyable(const noncopyable&);
  noncopyable& operator=(const noncopyable&);
 protected:
  noncopyable() {}
  ~noncopyable() {}
};

template <typename T> struct global { static T value; };
template <typename T> T global<T>::value;

// module type identifier
typedef const char* type_id;
template <typename T> type_id get_type_id() { return typeid(T).name(); }

class demangled_name : noncopyable {
#ifdef __GNUC__
  char* name_;
 public:
  explicit demangled_name(type_id id) {
    int s;
    name_ = abi::__cxa_demangle(id, 0, 0, &s);
  }
  ~demangled_name() { free(name_); }
  operator const char*() const { return name_; }
#else
  const char* name_;
 public:
  explicit demangled_name(type_id id) : name_(id) {}
  operator const char*() const { return name_; }
#endif
};

template <typename Mutex> class lock_guard : noncopyable {
  Mutex& mutex_;
 public:
  explicit lock_guard(Mutex& m) : mutex_(m) {
    m.lock();
  }
  ~lock_guard() {
    mutex_.unlock();
  }
};

class mutex : noncopyable {
#ifdef _WIN32
  CRITICAL_SECTION critical_section_;
 public:
  mutex() { InitializeCriticalSection(&critical_section_); }
  ~mutex() { DeleteCriticalSection(&critical_section_); }
  void lock() { EnterCriticalSection(&critical_section_); }
  void unlock() { LeaveCriticalSection(&critical_section_); }
#else  // _WIN32
  pthread_mutex_t pthread_mutex_;
 public:
  mutex() { pthread_mutex_init(&pthread_mutex_, 0); }
  ~mutex() { pthread_mutex_destroy(&pthread_mutex_); }
  void lock() { pthread_mutex_lock(&pthread_mutex_); }
  void unlock() { pthread_mutex_unlock(&pthread_mutex_); }
#endif  // _WIN32
};

enum log_level {
  INFO = 0,
  WARN,
  ERROR,
  FATAL,
  CHECK
};

inline const char* log_level_name(log_level level) {
  switch (level) {
    case INFO: return "INFO";
    case WARN: return "WARN";
    case ERROR: return "ERROR";
    case FATAL: return "FATAL";
    case CHECK: return "CHECK";
    default: return 0;
  }
}

class logger_t {
  std::ostream* stream_;
  std::tr1::unordered_map<type_id, int> module_verbosities_;
  mutex mutex_;
  log_level level_;

 public:
  logger_t() : stream_(&std::cerr), level_(INFO) {}

  void level(log_level l) { level_ = l; }
  log_level level() const { return level_; }
  void stream(std::ostream& s) { stream_ = &s; }

  template <typename Module> void verbosity(int verbosity) {
    module_verbosities_[get_type_id<Module>()] = verbosity;
  }

  void write(log_level l, const std::string& message) {
    if (l < level_) return;
    lock_guard<mutex> lock(mutex_);
    (*stream_) << '[' << log_level_name(l) << "] " << message << std::endl;
  }

  template <typename Module>
  void verbose_write(int verbosity, const std::string& message) {
    if (module_verbosities_[get_type_id<Module>()] < verbosity) return;
    lock_guard<mutex> lock(mutex_);
    (*stream_) << '[' << demangled_name(get_type_id<Module>())
               << ", " << verbosity << "] " << message << std::endl;
  }
};
static logger_t& logger = global<logger_t>::value;

// Thrown on (D)CHECK failure
class check_error : virtual std::exception {};

// Thrown by (D)LOG(FATAL)
class fatal_log : virtual std::exception {};

// Logging class for LOG(level)
class general_log {
  std::ostringstream oss_;
  log_level level_;

 public:
  explicit general_log(log_level l) : level_(l) {}
  void write() const {
    logger.write(level_, oss_.str());
  }
  template <typename T> general_log& operator<<(const T& t) {
    oss_ << t;
    return *this;
  }
};

// Module-specified logging class for LOG(module, verbosity)
template <typename Module> class verbose_log {
  std::ostringstream oss_;
  int verbosity_;

 public:
  explicit verbose_log(int v) : verbosity_(v) {}
  void write() const {
    logger.verbose_write<Module>(verbosity_, oss_.str());
  }
  template <typename T> verbose_log& operator<<(const T& t) {
    oss_ << t;
    return *this;
  }
};

// Logging class for CHECK
template <typename Error> class exception_log {
  std::ostringstream oss_;
  log_level level_;

 public:
  explicit exception_log(log_level l = CHECK) : level_(l) {}
  void write() const {
    logger.write(level_, oss_.str());
    throw Error();
  }
  template <typename T> exception_log& operator<<(const T& t) {
    oss_ << t;
    return *this;
  }
};

struct log_write_trigger {
  template <typename T> void operator&(const T& t) const {
    t.write();
  }
};
struct null_stream {
  template <typename T> null_stream& operator<<(T) const { return *this; }
};
struct void_op {
  void operator&(null_stream) const {}
};

template <log_level L> struct log_of_level {
  typedef general_log type;
};
template <> struct log_of_level<FATAL> {
  typedef exception_log<fatal_log> type;
};

}  // namespace LOG

#define ELOG_DETAIL_STRINGIZE(x) ELOG_DETAIL_STRINGIZE_I(x)
#define ELOG_DETAIL_STRINGIZE_I(x) #x
#define ELOG_DETAIL_CAT_I(x, y) x ## y
#ifdef _MSC_VER
# define ELOG_DETAIL_LINE __COUNTER__
# define ELOG_DETAIL_PAREN_L (
# define ELOG_DETAIL_PAREN_R )
# define ELOG_DETAIL_EXPAND(x) ELOG_DETAIL_EXPAND_I(x)
# define ELOG_DETAIL_EXPAND_I(x) x
# define ELOG_DETAIL_CAT(x, y) ELOG_DETAIL_CAT_I ELOG_DETAIL_EXPAND((x, y))
#else
# define ELOG_DETAIL_LINE __LINE__
# define ELOG_DETAIL_CAT(x, y) ELOG_DETAIL_CAT_I(x, y)
#endif

// Macro overload
#ifdef _MSC_VER
# define ELOG_DETAIL_TUPLE_LEN(...) \
    ELOG_DETAIL_TUPLE_LEN_I \
    ELOG_DETAIL_PAREN_L _, __VA_ARGS__, 2, 1, 0, _ ELOG_DETAIL_PAREN_R
#else
# define ELOG_DETAIL_TUPLE_LEN(...) \
  ELOG_DETAIL_TUPLE_LEN_I(_, ##__VA_ARGS__, 2, 1, 0, _)
#endif
#define ELOG_DETAIL_TUPLE_LEN_I(_0, _1, _2, n, ...) n
#define ELOG_DETAIL_OVERLOAD(name, ...) \
  ELOG_DETAIL_CAT(name, ELOG_DETAIL_TUPLE_LEN(__VA_ARGS__))(__VA_ARGS__)

// ELOG_PREFIX can be defined by users
#ifndef ELOG_PREFIX
# define ELOG_PREFIX __FILE__ "(" ELOG_DETAIL_STRINGIZE(ELOG_DETAIL_LINE) "): "
#endif

// LOG is overloaded
#define LOG(...) ELOG_DETAIL_OVERLOAD(ELOG_DETAIL_ELOG_, __VA_ARGS__)
#define ELOG_DETAIL_ELOG_0() ELOG_DETAIL_ELOG_1(INFO)
#define ELOG_DETAIL_ELOG_1(level) \
  ::LOG::log_write_trigger() & \
  (::LOG::log_of_level< ::LOG::level>::type(::LOG::level)) << ELOG_PREFIX
#define ELOG_DETAIL_ELOG_2(module, verbosity) \
  ::LOG::log_write_trigger() & \
  (::LOG::verbose_log<module>(verbosity)) << ELOG_PREFIX

#define CHECK(cond) \
  (cond) ? (void)0 : ::LOG::log_write_trigger() & \
  (::LOG::exception_log< ::LOG::check_error>()) << ELOG_PREFIX

#ifdef NDEBUG
# define ELOG_DETAIL_NULL_STREAM \
  1 ? (void)0 : ::LOG::null_stream::void_op() & ::LOG::null_stream()
# define DLOG(...) ELOG_DETAIL_NULL_STREAM
# define DCHECK(...) ELOG_DETAIL_NULL_STREAM
#else
# define DLOG LOG
# define DCHECK CHECK
#endif  // NDEBUG
