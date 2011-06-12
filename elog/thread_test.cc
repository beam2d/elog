#include <tr1/functional>
#include <gtest/gtest.h>
#include "thread.h"

namespace LOG {

namespace {

class Flag {
 public:
  Flag() : flag_(false) {
  }

  bool flag() const {
    return flag_;
  }

  void SetFlag() {
    flag_ = true;
  }

 private:
  bool flag_;
};

}  // anonymous namespace

TEST(ThreadTest, CreateAndJoin) {
  Flag flag;
  std::tr1::function<void ()> thread_body =
      std::tr1::bind(&Flag::SetFlag, std::tr1::ref(flag));

  Thread thread(thread_body);
  thread.Run();
  thread.Join();
  EXPECT_TRUE(flag.flag());
}

}  // namespace LOG