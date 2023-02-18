#include "pathinst/frontend_action.h"

#include <clang/Tooling/Tooling.h>
#include <fmt/core.h>
#include <gtest/gtest.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <vector>

namespace {
const std::string include = "#include <iostream>\n";
const std::string call =
    "std::cout << __FILE__ << ':' << __LINE__ << std::endl;";

class FrontEndActionTests : public ::testing::Test {
public:
  FrontEndActionTests(void)
      : logger_(spdlog::null_logger_mt("null_logger")), streams_() {}

  virtual ~FrontEndActionTests(void) override { spdlog::shutdown(); }

  virtual void SetUp(void) override { streams_.clear(); }

protected:
  std::shared_ptr<spdlog::logger> logger_;
  std::vector<std::string> streams_;
};
} // namespace
  //
TEST_F(FrontEndActionTests, InvalidCode_NotEdits_ReturnsFalse) {
  const std::string source_code = "void foo() {";
  bool success = clang::tooling::runToolOnCode(
      std::make_unique<pathinst::FrontendAction>(logger_, streams_),
      source_code);
  ASSERT_FALSE(success);
  ASSERT_EQ(streams_.size(), 0);
}

TEST_F(FrontEndActionTests, NoControlFlow_NoEdits_ReturnsTrue) {
  const std::string source_code = "void foo() { /* do nothing */ }";
  bool success = clang::tooling::runToolOnCode(
      std::make_unique<pathinst::FrontendAction>(logger_, streams_),
      source_code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 0);
}

TEST_F(FrontEndActionTests, ReturnStmt_AddsIncludeAndCall_ReturnsTrue) {
  const std::string code_template = "{}int main() {{ {}return 0; }}";
  bool success = clang::tooling::runToolOnCode(
      std::make_unique<pathinst::FrontendAction>(logger_, streams_),
      fmt::format(code_template, "", ""));
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  std::string expected = fmt::format(code_template, include, call);
  ASSERT_EQ(streams_[0], expected);
}
