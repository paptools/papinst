#include "wrapper/program_options.h"

#include <boost/program_options.hpp>
#include <gtest/gtest.h>

#include <optional>
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

TEST(ProgramOptionsTests, NoArgs_UsageMessageAndEarlyExitFailure) {
  const char *argv1[] = {"program"};
  auto options = wrapper::CreateProgramOptions(1, const_cast<char **>(argv1));
  EXPECT_EQ(EXIT_FAILURE, options->early_exit_code.value());
  EXPECT_FALSE(options->usage_message.empty());
}

TEST(ProgramOptionsTests, HelpFlag_UsageMessageAndEarlyExitSuccess) {
  const char *argv1[] = {"program", "-h"};
  auto options = wrapper::CreateProgramOptions(2, const_cast<char **>(argv1));
  EXPECT_EQ(EXIT_SUCCESS, options->early_exit_code.value());
  EXPECT_FALSE(options->usage_message.empty());

  const char *argv2[] = {"program", "--help"};
  options = wrapper::CreateProgramOptions(2, const_cast<char **>(argv2));
  EXPECT_EQ(EXIT_SUCCESS, options->early_exit_code.value());
  EXPECT_FALSE(options->usage_message.empty());
}

TEST(ProgramOptionsTests, UnexpectedPositional_Throws) {
  const char *argv1[] = {"program", "gcc"};
  EXPECT_THROW(wrapper::CreateProgramOptions(2, const_cast<char **>(argv1)),
               boost::program_options::error);

  const char *argv2[] = {"program", "gcc", "--"};
  EXPECT_THROW(wrapper::CreateProgramOptions(3, const_cast<char **>(argv2)),
               boost::program_options::error);

  const char *argv3[] = {"program", "gcc", "--", "g++"};
  EXPECT_THROW(wrapper::CreateProgramOptions(4, const_cast<char **>(argv3)),
               boost::program_options::error);
}

TEST(ProgramOptionsTests, NoCommand_ThrowsBoostProgramOptionsError) {
  const char *argv2[] = {"program", "--"};
  EXPECT_THROW(wrapper::CreateProgramOptions(2, const_cast<char **>(argv2)),
               boost::program_options::error);
}

TEST(ProgramOptionsTests, Command_CommandAndNoEarlyExit) {
  const char *argv1[] = {"program", "--", "gcc", "foo.cpp"};
  auto options = wrapper::CreateProgramOptions(4, const_cast<char **>(argv1));
  EXPECT_EQ(std::nullopt, options->early_exit_code);
  EXPECT_EQ(ToString({"gcc", "foo.cpp"}), ToString(options->command));
}
