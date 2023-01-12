#include "wrapper/program_options.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace {
std::string ToString(std::vector<std::string> const &args) {
  std::stringstream ss;
  for (auto const &arg : args) {
    ss << arg << ", ";
  }
  return ss.str();
}
} // namespace

TEST(ProgramOptionsTests, HelpFlag_StatusOkWithUsageMessage) {
  const char *argv1[] = {"program", "-h"};
  auto options = wrapper::CreateProgramOptions(2, const_cast<char **>(argv1));
  EXPECT_EQ(wrapper::ProgramOptions::Status::Ok, options->parse_status);
  EXPECT_NE(std::string::npos, options->message.find("Usage"));

  const char *argv2[] = {"program", "--help"};
  options = wrapper::CreateProgramOptions(2, const_cast<char **>(argv2));
  EXPECT_EQ(wrapper::ProgramOptions::Status::Ok, options->parse_status);
  EXPECT_NE(std::string::npos, options->message.find("Usage"));
}

TEST(ProgramOptionsTests, UnexpectedPositional_StatusErrorWithErrorMessage) {
  const char *argv1[] = {"program", "gcc"};
  auto options = wrapper::CreateProgramOptions(2, const_cast<char **>(argv1));
  EXPECT_EQ(wrapper::ProgramOptions::Status::Error, options->parse_status);
  EXPECT_NE(std::string::npos, options->message.find("Error"));

  const char *argv2[] = {"program", "gcc", "--"};
  options = wrapper::CreateProgramOptions(3, const_cast<char **>(argv2));
  EXPECT_EQ(wrapper::ProgramOptions::Status::Error, options->parse_status);
  EXPECT_NE(std::string::npos, options->message.find("Error"));

  const char *argv3[] = {"program", "gcc", "--", "g++"};
  options = wrapper::CreateProgramOptions(4, const_cast<char **>(argv3));
  EXPECT_EQ(wrapper::ProgramOptions::Status::Error, options->parse_status);
  EXPECT_NE(std::string::npos, options->message.find("Error"));
}

TEST(ProgramOptionsTests, NoCommand_StatusErrorWithErrorMessage) {
  const char *argv1[] = {"program"};
  auto options = wrapper::CreateProgramOptions(1, const_cast<char **>(argv1));
  EXPECT_EQ(wrapper::ProgramOptions::Status::Error, options->parse_status);
  EXPECT_NE(std::string::npos, options->message.find("Error"));

  const char *argv2[] = {"program", "--"};
  options = wrapper::CreateProgramOptions(2, const_cast<char **>(argv2));
  EXPECT_EQ(wrapper::ProgramOptions::Status::Error, options->parse_status);
  EXPECT_NE(std::string::npos, options->message.find("Error"));
}

TEST(ProgramOptionsTests, Command_StatusOkWithCommand) {
  const char *argv1[] = {"program", "--", "gcc", "foo.cpp"};
  auto options = wrapper::CreateProgramOptions(4, const_cast<char **>(argv1));
  EXPECT_EQ(wrapper::ProgramOptions::Status::Ok, options->parse_status);
  EXPECT_TRUE(options->message.empty());
  EXPECT_EQ(ToString({"gcc", "foo.cpp"}), ToString(options->command));
}
