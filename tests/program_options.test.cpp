#include "wrapper/program_options.h"

#include <gtest/gtest.h>

TEST(ProgramOptionsTests, HelpFlag_StatusOkWithUsageMessage) {
  const char *argv[] = {"program", "-h"};
  auto options = wrapper::CreateProgramOptions(2, const_cast<char **>(argv));
  EXPECT_EQ(wrapper::ProgramOptions::Status::Ok, options->parse_status);
  EXPECT_NE(std::string::npos, options->message.find("Usage"));

  const char *argv2[] = {"program", "--help"};
  options = wrapper::CreateProgramOptions(2, const_cast<char **>(argv2));
  EXPECT_EQ(wrapper::ProgramOptions::Status::Ok, options->parse_status);
  EXPECT_NE(std::string::npos, options->message.find("Usage"));
}
