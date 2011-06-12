#include <vector>
#include <tr1/functional>
#include <gtest/gtest.h>
#include "mutex.h"
#include "thread.h"

namespace LOG {

namespace {

/*
ThreadA       ThreadB
   |             |
Lock mutex1   Lock mutex2
flag on       Wait while !flag
push 0        Lock mutex1
Unlock mutex1 Wait...
Lock mutex2   push 1
Wait...       Unlock mutex1
   |          push 2
   |          Unlock mutex2
push 3           |
Unlock mutex2    |
 */

class MutexTester {
 public:
  MutexTester() : start_flag_(false) {
  }

  void Run() {
    std::tr1::function<void ()> thread_body_a =
        std::tr1::bind(&MutexTester::ThreadA, this);
    std::tr1::function<void ()> thread_body_b =
        std::tr1::bind(&MutexTester::ThreadB, this);

    Thread thread_a(thread_body_a);
    Thread thread_b(thread_body_b);

    thread_b.Run();
    thread_a.Run();
  }

  void ThreadA() {
    {
      MutexLock lock(mutex1_);
      start_flag_ = true;
      items_.push_back(0);
    }
    {
      MutexLock lock(mutex2_);
      items_.push_back(3);
    }
  }

  void ThreadB() {
    MutexLock lock2(mutex2_);
    while (!start_flag_) continue;
    {
      MutexLock lock1(mutex1_);
      items_.push_back(1);
    }
    items_.push_back(2);
  }

  void Verify() const {
    for (size_t i = 0; i < items_.size(); ++i) {
      EXPECT_EQ(i, items_[i]);
    }
  }

 private:
  std::vector<int> items_;
  Mutex mutex1_;
  Mutex mutex2_;
  volatile bool start_flag_;
};

}  // anonymous namespace

TEST(MutexTest, MutexLock) {
  MutexTester tester;
  tester.Run();
  tester.Verify();
}

}  // namespace LOG
