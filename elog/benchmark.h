// Copyright (c) 2011 Seiya Tokui <beam.web@gmail.com>. All Rights Reserved.
// This source code is distributed under MIT License in LICENSE file.

#ifndef ELOG_BENCHMARK_H_
#define ELOG_BENCHMARK_H_

#include "scoped_benchmark.h"

#define BENCHMARK(...) ELOG_I_OVERLOAD(ELOG_I_BENCHMARK_, __VA_ARGS__)

#define ELOG_I_BENCHMARK_1(name) \
  if (::LOG::ScopedBenchmark name = \
      ::LOG::ScopedBenchmark(ELOG_I_STRINGIZE(name), "")); else

#define ELOG_I_BENCHMARK_2(suite, name) \
  ELOG_I_BENCHMARK_3(suite, name, "")

#define ELOG_I_BENCHMARK_3(suite, name, message) \
  if (::LOG::ScopedBenchmark name = \
      ::LOG::ScopedBenchmark(ELOG_I_STRINGIZE(name), message, &suite)); else

#endif  // ELOG_BENCHMARK_H_
