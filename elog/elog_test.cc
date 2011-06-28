#include <iostream>
#include <sstream>
#include <string>
#include <gtest/gtest.h>
#include "elog.h"

namespace LOG {

class LOGTest : public ::testing::Test {
 public:
  LOGTest()
      : logger_(stream_) {
  }

 protected:
  virtual void SetUp() {
    Reset();
    SetLogger(logger_);
  }

  virtual void TearDown() {
    UseDefaultLogger();
  }

  void Reset() {
    stream_.str("");
  }

  std::string GetMessage() const {
    return stream_.str();
  }

  void VerifyLevel(LogLevel level) const {
    const char* log_level_name = LogLevelNames::names[level];
    VerifyMessage(log_level_name);
  }

  void VerifyMessage(const std::string& must_have_me) const {
    const std::string message = GetMessage();
    std::cout << "message: " << message << std::endl;
    EXPECT_NE(std::string::npos, message.find(must_have_me));
  }

 private:
  std::ostringstream stream_;
  StreamLogger logger_;
};

TEST_F(LOGTest, LevelOfNullArguments) {
  LOG();
  VerifyLevel(INFO);
}

TEST_F(LOGTest, MessageOfNullArguments) {
  static const char* kMessage = "message";
  LOG() << kMessage;
  VerifyMessage(kMessage);
}

TEST_F(LOGTest, INFOMessage) {
  LOG(INFO);
  VerifyLevel(INFO);
}

TEST_F(LOGTest, WARNMessage) {
  LOG(WARN);
  VerifyLevel(WARN);
}

TEST_F(LOGTest, ERRORMessage) {
  LOG(ERROR);
  VerifyLevel(ERROR);
}

TEST_F(LOGTest, FATALMessage) {
  EXPECT_THROW(LOG(FATAL), FatalLogError);
  VerifyLevel(FATAL);
}

}  // namespace LOG
