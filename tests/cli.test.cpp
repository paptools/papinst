#include "pathinst/cli.h"
#include "pathinst/exception.h"

#include "gtest/gtest.h"

TEST(CliTests, GetUsageMessage_BeforeParseArgs_ReturnsFullUsageMessage) {
  const std::string expected("Usage:\n"
                             "  pathinst [options] -- <command>\n"
                             "  pathinst -V|--version\n"
                             "  pathinst -h|--help\n"
                             "\n"
                             "Description:\n"
                             "  The path instrumenter.\n");

  ASSERT_EQ(expected, pathinst::cli::GetUsageMessage());
}

TEST(CliTests, GetUsageMessage_AfterParseArgs_ReturnsFullUsageMessage) {
  const char *argv_short[] = {"prog", "--version"};
  pathinst::cli::ParseArgs(1, const_cast<char **>(argv_short));

  const std::string expected(
      "Usage:\n"
      "  pathinst [options] -- <command>\n"
      "  pathinst -V|--version\n"
      "  pathinst -h|--help\n"
      "\n"
      "Description:\n"
      "  The path instrumenter.\n"
      "\n"
      "Options:\n"
      "  -v [ --verbose ]      Enable detailed application output.\n"
      "  -V [ --version ]      Output version information and exit.\n"
      "  -h [ --help ]         Display this help and exit.\n");

  ASSERT_EQ(expected, pathinst::cli::GetUsageMessage());
}

TEST(CliTests, Options_DefaultInitialized_HasUninitializedStatus) {
  pathinst::cli::Options options;
  ASSERT_EQ(pathinst::cli::Options::Status::Uninitialized, options.status);
}

TEST(CliTests, ParseArgs_NoArgs_OptionsStatusIsFailure) {
  const char *argv[] = {"prog"};
  auto options = pathinst::cli::ParseArgs(1, const_cast<char **>(argv));
  ASSERT_EQ(pathinst::cli::Options::Status::Failure, options->status);
}

TEST(CliTests, ParseArgs_HelpFlag_SetsHelpOption) {
  const char *argv_short[] = {"prog", "-h"};
  auto options = pathinst::cli::ParseArgs(2, const_cast<char **>(argv_short));
  ASSERT_EQ(true, options->help);
  ASSERT_EQ(pathinst::cli::Options::Status::Success, options->status);

  const char *argv_long[] = {"prog", "--help"};
  options = pathinst::cli::ParseArgs(2, const_cast<char **>(argv_long));
  ASSERT_EQ(true, options->help);
  ASSERT_EQ(pathinst::cli::Options::Status::Success, options->status);
}

TEST(CliTests, ParseArgs_VersionFlag_SetsVersionOption) {
  const char *argv_short[] = {"prog", "-V"};
  auto options = pathinst::cli::ParseArgs(2, const_cast<char **>(argv_short));
  ASSERT_EQ(true, options->version);
  ASSERT_EQ(pathinst::cli::Options::Status::Success, options->status);

  const char *argv_long[] = {"prog", "--version"};
  options = pathinst::cli::ParseArgs(2, const_cast<char **>(argv_long));
  ASSERT_EQ(true, options->version);
  ASSERT_EQ(pathinst::cli::Options::Status::Success, options->status);
}

TEST(CliTests, ParseArgs_VerboseFlag_SetsVerboseOption) {
  const char *argv_short[] = {"prog", "-v"};
  auto options = pathinst::cli::ParseArgs(2, const_cast<char **>(argv_short));
  ASSERT_EQ(true, options->verbose);
  ASSERT_EQ(pathinst::cli::Options::Status::Success, options->status);

  const char *argv_long[] = {"prog", "--verbose"};
  options = pathinst::cli::ParseArgs(2, const_cast<char **>(argv_long));
  ASSERT_EQ(true, options->verbose);
  ASSERT_EQ(pathinst::cli::Options::Status::Success, options->status);
}

TEST(CliTests, ParseArgs_UserCommand_IsStoredInCommandMember) {
  const char *argv[] = {"prog", "-v", "--", "foo", "--file", "foo.cpp"};
  auto options = pathinst::cli::ParseArgs(6, const_cast<char **>(argv));
  std::vector<std::string> expected = {"foo", "--file", "foo.cpp"};
  ASSERT_EQ(expected, options->command);
  ASSERT_EQ(pathinst::cli::Options::Status::Success, options->status);
}

TEST(CliTests, ParseArgs_NoUserCommand_ThrowsMissingCommandException) {
  const char *argv[] = {"prog", "--"};
  ASSERT_THROW(
      try {
        pathinst::cli::ParseArgs(2, const_cast<char **>(argv));
      } catch (const pathinst::Exception &ex) {
        EXPECT_STREQ("the argument 'command' is required but missing",
                     ex.what());
        throw ex;
      },
      pathinst::Exception);
}
