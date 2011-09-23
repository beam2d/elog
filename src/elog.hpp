// Copyright (c) 2011 Seiya Tokui <beam.web@gmail.com>. All Rights Reserved.
// This source code is distributed under MIT License in LICENSE file.

#ifndef ELOG_ELOG_HPP_
#define ELOG_ELOG_HPP_

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>

#ifdef _WIN32
# include <windows.h>
#else
# include <sys/time.h>
#endif

namespace LOG
{

// utility

template<typename T, typename U = void>
struct enable_if_exist
{ typedef U type; };

template<typename T>
struct identity
{ typedef T type; };

template<typename T,
         typename enable_if_exist<typename T::value_type, int>::type = 0,
         typename enable_if_exist<typename T::iterator, int>::type = 0>
std::true_type
is_range_fn(int);

template<typename T,
          typename std::enable_if<std::is_array<T>::value, int>::type = 0>
std::true_type
is_range_fn(int);

template<typename>
std::false_type
is_range_fn(...);

template<typename T>
struct is_range
    : identity<decltype(is_range_fn<T>(0))>::type
{};


template<typename T,
         typename enable_if_exist<typename T::first_type>::type = 0,
         typename enable_if_exist<typename T::second_type>::type = 0>
std::true_type
is_pair_fn(int);

template<typename T>
std::false_type
is_pair_fn(...);

template<typename T>
struct is_pair
    : identity<decltype(is_pair_fn<T>(0))>::type
{};


// time

inline double
gettime_sec()
{
#ifdef _WIN32
  LARGE_INTEGER t, f;
  QueryPerformanceCounter(&t);
  QueryPerformanceFrequency(&f);
  return t.QuadPart * 1.0 / f.QuadPart;
#else
  timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec * 1e-6;
#endif
}

template<typename Stream>
void
pritty_print_time(Stream& stream, double sec)
{
  long long min = sec / 60;
  sec -= min * 60;
  long long hour = min / 60;
  min -= hour * 60;
  long long day = hour / 24;
  hour -= day * 24;

  if (min > 0) {
    if (hour > 0) {
      if (day > 0) stream << day << "d ";
      stream << hour << "h ";
    }
    stream << min << "m ";
  }
  stream << sec << "s";
}


// print value to output stream

template<typename T, typename Stream>
void
pritty_print_internal(const T& t, Stream& stream, ...)
{ stream << t; }

template<typename T,
         typename Stream,
         typename std::enable_if<is_pair<T>::value, int>::type = 0>
void
pritty_print_internal(const T& pair, Stream& stream, int)
{
  stream << '(';
  pritty_print_internal(pair.first, stream, 0);
  stream << ", ";
  pritty_print_internal(pair.second, stream, 0);
  stream << ')';
}

template<typename T,
         typename Stream,
         typename std::enable_if<is_range<T>::value, int>::type = 0>
void
pritty_print_internal(const T& range, Stream& stream, int)
{
  stream << '[';

  bool is_tail = false;
  for (const auto& elem : range) {
    if (is_tail) stream << ", ";

    is_tail = true;
    pritty_print_internal(elem, stream, 0);
  }

  stream << ']';
}

template<typename Stream>
void
pritty_print_internal(signed char t, Stream& stream, int)
{ stream << static_cast<int>(t); }

template<typename Stream>
void
pritty_print_internal(unsigned char t, Stream& stream, int)
{ stream << static_cast<unsigned int>(t); }

template<typename T, typename Stream>
void
pritty_print(const T& t, Stream& stream)
{ pritty_print_internal(t, stream, 0); }


// argument list to stream
template<typename Stream>
Stream&
print_arguments(Stream& stream)
{ return stream; }

template<typename Stream, typename T, typename ... Args>
Stream&
print_arguments(Stream& stream, const T& t, Args... args)
{
  stream << t;
  return print_arguments(stream, args...);
}


// logger

struct logger_base
{
  virtual
  ~logger_base()
  {};

  virtual
  void
  write(const std::string& message) const = 0;
};

// TODO: Support multi-thread use
struct stream_logger
    : logger_base
{
  stream_logger()
      : os_(&std::clog)
  {}

  void
  set_stream(std::ostream& os)
  { os_ = &os; }

  virtual
  void
  write(const std::string& message) const
  { (*os_) << message; }

 private:
  std::ostream* os_;
};

enum avoid_odr
{ AVOID_ODR };

template<typename T, avoid_odr = AVOID_ODR>
struct static_holder
{ static T value; };

template<typename T, avoid_odr A>
T static_holder<T, A>::value;

struct global_logger_holder
{
  global_logger_holder()
      : logger(&static_holder<stream_logger>::value)
  {}

  logger_base* logger;
};

inline
logger_base&
get_logger()
{ return *static_holder<global_logger_holder>::value.logger; }

inline
void
set_stream(std::ostream& os)
{ static_holder<stream_logger>::value.set_stream(os); }


// message construction

struct message_builder
{
  template<typename T>
  void
  operator()(const T& t)
  { pritty_print(t, oss_); }

  std::string
  get() const
  { return oss_.str(); }

 private:
  std::ostringstream oss_;
};

struct log_emitter
{
  log_emitter()
      : logger_(get_logger())
  {}

  explicit
  log_emitter(logger_base& logger)
      : logger_(logger)
  {}

  template<typename T>
  log_emitter&
  operator<<(const T& t)
  {
    message_builder_(t);
    return *this;
  }

  log_emitter&
  self()
  { return *this; }

  void
  emit() const
  { logger_.write(message_builder_.get()); }

 private:
  logger_base& logger_;
  message_builder message_builder_;
};

struct log_emission_trigger
{
  void
  operator&(const log_emitter& emitter) const
  { emitter.emit(); }
};

struct check_error
    : std::exception
{};

struct check_emission_trigger
{
  void
  operator&(const log_emitter& emitter) const
  {
    emitter.emit();
    throw check_error();
  }
};


// benchmark

struct benchmark
{
  benchmark()
      : logger_(get_logger()),
        start_sec_(gettime_sec()),
        done_(false)
  {}

  explicit
  benchmark(logger_base& logger)
      : logger_(logger),
        start_sec_(gettime_sec()),
        done_(false)
  {}

  explicit
  operator bool() const
  { return false; }

  ~benchmark()
  { if (!done_) push_message(); }

  template<typename T>
  benchmark&
  operator<<(const T& t)
  {
    title_builder_ << t;
    return *this;
  }

  double
  gettime() const
  { return gettime_sec() - start_sec_; }

  void
  push_message()
  {
    done_ = true;
    const auto duration = gettime_sec() - start_sec_;

    log_emitter emitter(logger_);
    emitter << title_builder_.get() << ": " << duration << " sec";
    if (duration >= 60) {
      emitter << " (";
      pritty_print_time(emitter, duration);
      emitter << ")";
    }
    emitter.emit();
  }

 private:
  logger_base& logger_;
  double start_sec_;

  message_builder title_builder_;

  bool done_;
};

}  // namespace LOG

#define LOG() ::LOG::log_emission_trigger() & ::LOG::log_emitter().self()

#define CHECK(cond) \
  (cond) ? (void)0 : \
  ::LOG::check_emission_trigger() & ::LOG::log_emitter().self();

#define BENCHMARK(varname, ...) \
  if (auto varname = ::LOG::benchmark()); \
  else if (!&::LOG::print_arguments(varname, __VA_ARGS__)); \
  else

#endif  // ELOG_ELOG_HPP_
