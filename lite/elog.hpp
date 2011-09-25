// Copyright (c) 2011 Seiya Tokui <beam.web@gmail.com>. All Rights Reserved.
// This source code is distributed under MIT License in LICENSE file.

#ifndef ELOG_HPP_
#define ELOG_HPP_

#include <ctime>
#include <chrono>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace LOG
{

// meta functions

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


template<typename T>
struct range_value
{
  typedef typename std::iterator_traits<typename T::iterator>::value_type type;
};

template<typename T, std::size_t N>
struct range_value<T[N]>
{ typedef T type; };


template<typename T,
         typename enable_if_exist<typename T::first_type, int>::type = 0,
         typename enable_if_exist<typename T::second_type, int>::type = 0>
std::true_type
is_pair_fn(int);

template<typename T>
std::false_type
is_pair_fn(...);

template<typename T>
struct is_pair
    : identity<decltype(is_pair_fn<T>(0))>::type
{};


// static variables definition

enum avoid_odr
{ AVOID_ODR };

template<typename T, avoid_odr = AVOID_ODR>
struct static_holder
{ static T value; };

template<typename T, avoid_odr A>
T static_holder<T, A>::value;


// pretty print to stream

template<typename T, typename Stream>
inline void
pretty_print_internal(const T& t, Stream& stream, ...)
{ stream << t; }

template<typename T,
         typename Stream,
         typename std::enable_if<is_pair<T>::value, int>::type = 0>
void
pretty_print_internal(const T& pair, Stream& stream, int)
{
  stream << '(';
  pretty_print(pair.first, stream);
  stream << ", ";
  pretty_print(pair.second, stream);
  stream << ')';
}

template<typename Range,
         typename Value =
         typename std::decay<typename range_value<Range>::type>::type>
struct range_pretty_printer
{
  template<typename Stream>
  void
  operator()(const Range& range, Stream& stream) const
  {
    stream << '[';

    bool is_tail = false;
    for (const auto& elem : range) {
      if (is_tail) stream << ", ";

      is_tail = true;
      pretty_print(elem, stream);
    }

    stream << ']';
  }
};

template<typename Range>
struct range_pretty_printer<Range, char>
{
  template<typename Stream>
  void
  operator()(const Range& str, Stream& stream) const
  { for (const auto ch : str) stream << ch; }
};

template<typename T,
         typename Stream,
         typename std::enable_if<is_range<T>::value, int>::type = 0>
inline void
pretty_print_internal(const T& range, Stream& stream, int)
{ range_pretty_printer<T>()(range, stream); }

template<typename T,
         typename Stream,
         typename std::enable_if<
           std::is_same<T, signed char>::value, int>::type = 0>
inline void
pretty_print_internal(const T& sc, Stream& stream, int)
{ stream << static_cast<int>(sc); }

template<typename T,
         typename Stream,
         typename std::enable_if<
           std::is_same<T, unsigned char>::value, int>::type = 0>
inline void
pretty_print_internal(const T& uc, Stream& stream, int)
{ stream << static_cast<unsigned int>(uc); }

template<typename T, typename Stream>
inline void
pretty_print(const T& t, Stream& stream)
{ pretty_print_internal(t, stream, 0); }


template<typename Stream>
void
pretty_print_timesec(Stream& stream, double sec)
{
  stream << sec << " sec";

  long long min = sec / 60;
  sec -= min * 60;
  long long hour = min / 60;
  min -= hour * 60;
  long long day = hour / 24;
  hour -= day * 24;

  if (min > 0) {
    stream << " (";
    if (hour > 0) {
      if (day > 0) stream << day << "d ";
      stream << hour << "h ";
    }
    stream << min << "m ";
    stream << sec << "s)";
  }
}


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


// get current datetime
inline std::tm
get_tm()
{
  auto timeval = std::time(nullptr);
  std::tm tm;

#ifdef _WIN32
  localtime_s(&tm, &timeval);
#else
  localtime_r(&timeval, &tm);
#endif

  return tm;
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
  {
    auto tm = get_tm();

    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "[%Y%m%d %X] ", &tm);

    (*os_) << buffer << message << std::endl;
  }

 private:
  std::ostream* os_;
};

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


// message construction and emission

struct message_builder
{
  message_builder()
  {}

  message_builder(const message_builder& mb)
  { oss_ << mb.oss_.str(); }

  template<typename T>
  void
  operator()(const T& t)
  { pretty_print(t, oss_); }

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
        start_(std::chrono::high_resolution_clock::now()),
        done_(false)
  {}

  explicit
  benchmark(logger_base& logger)
      : logger_(logger),
        start_(std::chrono::high_resolution_clock::now()),
        done_(false)
  {}

  benchmark(const benchmark& b)
      : logger_(b.logger_),
        start_(b.start_),
        title_builder_(b.title_builder_),
        done_(b.done_)
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
    title_builder_(t);
    return *this;
  }

  std::chrono::high_resolution_clock::duration
  duration() const
  { return std::chrono::high_resolution_clock::now() - start_; }

  double
  seconds() const
  {
    const auto d = duration();
    const auto us = std::chrono::duration_cast<std::chrono::microseconds>(d);
    return static_cast<double>(us.count()) / 1000000;
  }

  void
  push_message()
  {
    done_ = true;

    log_emitter emitter(logger_);
    emitter << title_builder_.get() << ": ";
    pretty_print_timesec(emitter, seconds());
    emitter.emit();
  }

 private:
  logger_base& logger_;
  std::chrono::high_resolution_clock::time_point start_;

  message_builder title_builder_;

  bool done_;
};

}  // namespace LOG

#define LOG() ::LOG::log_emission_trigger() & ::LOG::log_emitter()

#define CHECK(cond) \
  (cond) ? (void)0 : \
  ::LOG::check_emission_trigger() & ::LOG::log_emitter()

#define BENCHMARK(varname, ...) \
  if (auto varname = ::LOG::benchmark()); \
  else if (!&::LOG::print_arguments(varname, __VA_ARGS__)); \
  else

#endif  // ELOG_HPP_
