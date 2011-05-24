#include <sstream>
#include <string>
#include <gtest/gtest.h>
#include "elog.h"
using namespace std;

class logger_test : public ::testing::Test {
  ostringstream o_;

 protected:
  virtual void SetUp() {
    reset();
    elog::logger.stream(o_);
  }

  virtual void TearDown() {
    elog::logger.stream(cerr);
  }

  void reset() {
    o_.str("");
  }

  string str() const {
    return o_.str();
  }
};

TEST_F(logger_test, general_usage) {
  LOG() << "message";
  string msg = str();
  EXPECT_EQ(0, msg.find("[INFO]"));
  EXPECT_NE(string::npos, msg.find("message"));
}

TEST_F(logger_test, level) {
  reset();
  LOG() << "message";
  EXPECT_FALSE(str().empty());

  LOG(INFO) << "message";
  EXPECT_FALSE(str().empty());

  reset();
  LOG(WARN) << "message";
  EXPECT_FALSE(str().empty());

  reset();
  LOG(ERROR) << "message";
  EXPECT_FALSE(str().empty());

  reset();
  EXPECT_THROW(ELOG(FATAL) << "message", elog::fatal_log);
  EXPECT_FALSE(str().empty());

  elog::logger.level(elog::LOGLEVEL_ERROR);

  reset();
  LOG() << "message";
  EXPECT_TRUE(str().empty());

  reset();
  LOG(INFO) << "message";
  EXPECT_TRUE(str().empty());

  reset();
  LOG(WARN) << "message";
  EXPECT_TRUE(str().empty());

  reset();
  LOG(ERROR) << "message";
  EXPECT_FALSE(str().empty());
}

TEST_F(logger_test, check) {
  EXPECT_NO_THROW(CHECK(1) << "message");
  EXPECT_THROW(CHECK(0) << "message", elog::check_error);
  EXPECT_FALSE(str().empty());
  EXPECT_EQ(0, str().find("[CHECK]"));
}

class some_module {};
class another_module {};

TEST_F(logger_test, vlog) {
  elog::logger.verbosity<some_module>(1);

  LOG(some_module, 0) << "message";
  EXPECT_FALSE(str().empty());
  EXPECT_EQ(0, str().find("[some_module(0)]"));
  EXPECT_NE(string::npos, str().find("message"));

  reset();
  LOG(some_module, 1) << "message";
  EXPECT_FALSE(str().empty());

  reset();
  LOG(some_module, 2) << "message";
  EXPECT_TRUE(str().empty());

  reset();
  LOG(some_module, 3) << "message";
  EXPECT_TRUE(str().empty());

  reset();
  elog::logger.verbosity<another_module>(3);
  LOG(another_module, 3) << "message";
  EXPECT_FALSE(str().empty());
}
