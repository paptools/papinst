#include "pathinst/frontend_action.h"
#include "mocks/mock_instrumenter.h"

#include <clang/Tooling/Tooling.h>
#include <fmt/core.h>
#include <gtest/gtest.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <vector>

namespace {
const std::string include = "#include <pathinst/pathinst.h>\n";
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

TEST_F(FrontEndActionTests, InvalidCode_NotEdits_ReturnsFalse) {
  auto instrumenter = std::make_shared<pathinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetPathCapIncludeInst).Times(0);
  EXPECT_CALL(*instrumenter, GetFnCalleeInst).Times(0);

  const std::string source_code = "void foo() {";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<pathinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    source_code);
  ASSERT_FALSE(success);
  ASSERT_EQ(streams_.size(), 0);
}

TEST_F(FrontEndActionTests, OneEmptyFn) {
  auto instrumenter = std::make_shared<pathinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetPathCapIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));

  const std::string code = "void fn() {}";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<pathinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Avoid fn() {B}";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, TwoEmptyFns) {
  auto instrumenter = std::make_shared<pathinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetPathCapIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(2)
      .WillRepeatedly(::testing::Return("B"));

  const std::string code = "void fn_1() {}\nvoid fn_2() {}";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<pathinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Avoid fn_1() {B}\nvoid fn_2() {B}";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, ReturnValueFn) {
  auto instrumenter = std::make_shared<pathinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetPathCapIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));

  const std::string code = "int fn() { return 0; }";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<pathinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Aint fn() {B return 0; }";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, AssignAndReturnValueFn) {
  auto instrumenter = std::make_shared<pathinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetPathCapIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));

  const std::string code_template = "int fn() { int a = 1;\nreturn a; }";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<pathinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code_template);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Aint fn() {B int a = 1;\nreturn a; }";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, TernaryReturnValueFn) {
  auto instrumenter = std::make_shared<pathinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetPathCapIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));

  const std::string code_template =
      "int fn() { int a = 1;\nreturn (a ? 0 : 1); }";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<pathinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code_template);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Aint fn() {B int a = 1;\nreturn (a ? 0 : 1); }";
  ASSERT_EQ(streams_[0], expected);
}
