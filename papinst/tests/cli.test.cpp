#include "papinst/cli.h"
#include "papinst/exception.h"

#include <gtest/gtest.h>

TEST(CliTests, GetUsageMessage_BeforeParseArgs_ReturnsFullUsageMessage) {
  const std::string expected("Usage:\n"
                             "  papinst [options] -- <command>\n"
                             "  papinst -V|--version\n"
                             "  papinst -h|--help\n"
                             "\n"
                             "Description:\n"
                             "  The path instrumenter.\n");

  ASSERT_EQ(expected, papinst::cli::GetUsageMessage());
}

TEST(CliTests, GetUsageMessage_AfterParseArgs_ReturnsFullUsageMessage) {
  const char *argv_short[] = {"prog", "--version"};
  papinst::cli::ParseArgs(1, const_cast<char **>(argv_short));

  const std::string expected(
      "Usage:\n"
      "  papinst [options] -- <command>\n"
      "  papinst -V|--version\n"
      "  papinst -h|--help\n"
      "\n"
      "Description:\n"
      "  The path instrumenter.\n"
      "\n"
      "Options:\n"
      "  -v [ --verbose ]      Enable detailed application output.\n"
      "  -n [ --dry-run ]      Display file edits to console instead of "
      "writing.\n"
      "  -V [ --version ]      Output version information and exit.\n"
      "  -h [ --help ]         Display this help and exit.\n");

  ASSERT_EQ(expected, papinst::cli::GetUsageMessage());
}

TEST(CliTests, Options_DefaultInitialized_HasUninitializedStatus) {
  papinst::cli::Options options;
  ASSERT_EQ(papinst::cli::Options::Status::Uninitialized, options.status);
}

TEST(CliTests, ParseArgs_NoArgs_OptionsStatusIsFailure) {
  const char *argv[] = {"prog"};
  auto options = papinst::cli::ParseArgs(1, const_cast<char **>(argv));
  ASSERT_EQ(papinst::cli::Options::Status::Failure, options->status);
}

TEST(CliTests, ParseArgs_HelpFlag_SetsHelpOption) {
  const char *argv_short[] = {"prog", "-h"};
  auto options = papinst::cli::ParseArgs(2, const_cast<char **>(argv_short));
  ASSERT_EQ(true, options->help);
  ASSERT_EQ(papinst::cli::Options::Status::Success, options->status);

  const char *argv_long[] = {"prog", "--help"};
  options = papinst::cli::ParseArgs(2, const_cast<char **>(argv_long));
  ASSERT_EQ(true, options->help);
  ASSERT_EQ(papinst::cli::Options::Status::Success, options->status);
}

TEST(CliTests, ParseArgs_VersionFlag_SetsVersionOption) {
  const char *argv_short[] = {"prog", "-V"};
  auto options = papinst::cli::ParseArgs(2, const_cast<char **>(argv_short));
  ASSERT_EQ(true, options->version);
  ASSERT_EQ(papinst::cli::Options::Status::Success, options->status);

  const char *argv_long[] = {"prog", "--version"};
  options = papinst::cli::ParseArgs(2, const_cast<char **>(argv_long));
  ASSERT_EQ(true, options->version);
  ASSERT_EQ(papinst::cli::Options::Status::Success, options->status);
}

TEST(CliTests, ParseArgs_VerboseFlag_SetsVerboseOption) {
  const char *argv_short[] = {"prog", "-v"};
  auto options = papinst::cli::ParseArgs(2, const_cast<char **>(argv_short));
  ASSERT_EQ(true, options->verbose);
  ASSERT_EQ(papinst::cli::Options::Status::Success, options->status);

  const char *argv_long[] = {"prog", "--verbose"};
  options = papinst::cli::ParseArgs(2, const_cast<char **>(argv_long));
  ASSERT_EQ(true, options->verbose);
  ASSERT_EQ(papinst::cli::Options::Status::Success, options->status);
}

TEST(CliTests, ParseArgs_UserCommand_IsStoredInCommandMember) {
  const char *argv[] = {"prog", "-v", "--", "foo", "--file", "foo.cpp"};
  auto options = papinst::cli::ParseArgs(6, const_cast<char **>(argv));
  std::vector<std::string> expected = {"foo", "--file", "foo.cpp"};
  ASSERT_EQ(expected, options->command);
  ASSERT_EQ(papinst::cli::Options::Status::Success, options->status);
}

TEST(CliTests, ParseArgs_NoUserCommand_ThrowsMissingCommandException) {
  const char *argv[] = {"prog", "--"};
  ASSERT_THROW(
      try {
        papinst::cli::ParseArgs(2, const_cast<char **>(argv));
      } catch (const papinst::Exception &ex) {
        EXPECT_STREQ("the argument 'command' is required but missing",
                     ex.what());
        throw ex;
      },
      papinst::Exception);
}
