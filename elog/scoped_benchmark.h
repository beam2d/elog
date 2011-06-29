// Copyright (c) 2011 Seiya Tokui <beam.web@gmail.com>. All Rights Reserved.
// This source code is distributed under MIT License in LICENSE file.

#ifndef ELOG_SCOPED_BENCHMARK_H_
#define ELOG_SCOPED_BENCHMARK_H_

#include <string>
#include "benchmark_suite.h"
#include "elog.h"
#include "safe_bool.h"
#include "timer.h"

namespace LOG {

class ScopedBenchmark : public SafeBool<ScopedBenchmark> {
 public:
  explicit ScopedBenchmark(const std::string& title,
                           BenchmarkSuite* suite = NULL,
                           Logger* logger = NULL,
                           LogLevel level = INFO)
      : title_(title),
        logger_(logger ? *logger : GetLogger()),
        suite_(suite),
        level_(level),
        done_(false) {
    logger_.PushRawMessage(level, title + "...");
  }

  ~ScopedBenchmark() {
    Print();
  }

  bool BoolTest() const {
    return false;
  }

  double GetTime() const {
    return timer_;
  }

  void Print() {
    if (done_) return;
    done_ = true;

    PrintWithoutCheck();
  }

 private:
  void PrintWithoutCheck() const {
    const double time = timer_;
    if (suite_) {
      suite_->AddCase(title_, time);
    }
    const std::string message = BuildMessage(time);
    logger_.PushRawMessage(level_, message);
  }

  std::string BuildMessage(double time) const {
    std::ostringstream message;
    message << title_ << ": " << time << " sec";
    return message.str();
  }

  std::string title_;
  Timer timer_;
  Logger& logger_;
  BenchmarkSuite* suite_;
  LogLevel level_;

  bool done_;
};

}  // namespace LOG

#endif  // ELOG_SCOPED_BENCHMARK_H_
