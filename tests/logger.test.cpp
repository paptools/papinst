#include "wrapper/logger.h"

#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace {
std::string ToString(std::vector<std::string> const &args) {
  std::stringstream ss;
  for (auto const &arg : args) {
    ss << arg << "\n";
  }
  return ss.str();
}
} // namespace

TEST(SimpleLoggerTests, LevelCalls_LogExpectedMessages) {
  std::ostringstream oss;
  auto logger = wrapper::Logger::CreateSimpleLogger(oss);

  logger->Log("Log message");
  logger->Info("Info message");
  logger->Error("Error message");

  std::string expected = ToString({
      "Log message",
      "INFO: Info message",
      "ERROR: Error message",
  });
  EXPECT_EQ(expected, oss.str());
}
