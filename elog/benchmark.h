#ifndef ELOG_BENCHMARK_H_
#define ELOG_BENCHMARK_H_

#include "scoped_benchmark.h"

#define BENCHMARK(...) ELOG_I_OVERLOAD(ELOG_I_BENCHMARK_, __VA_ARGS__)

#define ELOG_I_BENCHMARK_1(name) \
  if (::LOG::ScopedBenchmark name = \
      ::LOG::ScopedBenchmark(ELOG_I_STRINGIZE(name))); else

#define ELOG_I_BENCHMARK_2(suite, name) \
  if (::LOG::ScopedBenchmark name = \
      ::LOG::ScopedBenchmark(ELOG_I_STRINGIZE(name), &suite)); else

#endif  // ELOG_BENCHMARK_H_
