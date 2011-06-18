#include <iostream>
#include <sstream>
#include <gtest/gtest.h>
#include "stream_logger.h"
using namespace std;

namespace LOG {

namespace {

static const LogLevel kLevels[] = { INFO, WARN, ERROR, FATAL };
static const size_t kNumLevels = sizeof(kLevels) / sizeof(kLevels[0]);
static const char* kMessage = "message";

}  // anonymous namespace

TEST(StreamLoggerTest, PushMessage) {
  for (size_t i = 0; i < kNumLevels; ++i) {
    const LogLevel level = kLevels[i];
    const char* log_level_name = log_level_names[level];

    std::ostringstream stream;
    StreamLogger logger(stream);
    logger.PushMessage(level, kMessage);

    std::string full_message = stream.str();
    std::cout << "log level: " << log_level_name << std::endl;
    EXPECT_EQ(1, full_message.find(log_level_name));
    EXPECT_NE(std::string::npos, full_message.find(kMessage));
  }
}

// TODO(S.Tokui): Write multi-thread test.

}  // namespace LOG
