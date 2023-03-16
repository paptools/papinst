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

TEST_F(FrontEndActionTests, InvalidCode_NoInst_ReturnsFalse) {
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

TEST_F(FrontEndActionTests, FnDecl_NoInst_ReturnsTrue) {
  auto instrumenter = std::make_shared<pathinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetPathCapIncludeInst).Times(0);
  EXPECT_CALL(*instrumenter, GetFnCalleeInst).Times(0);

  const std::string code = "void fn();";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<pathinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 0);
}

TEST_F(FrontEndActionTests, VoidFnEmpty_Inst_ReturnsTrue) {
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

TEST_F(FrontEndActionTests, VoidFnWithReturn_Inst_ReturnsTrue) {
  auto instrumenter = std::make_shared<pathinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetPathCapIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));

  const std::string code = "void fn() { return; }";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<pathinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Avoid fn() {B return; }";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, MixedVoidFns_Inst_ReturnsTrue) {
  auto instrumenter = std::make_shared<pathinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetPathCapIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(2)
      .WillRepeatedly(::testing::Return("B"));

  const std::string code = "void fn_1() {}\nvoid fn_2() { return; }";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<pathinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Avoid fn_1() {B}\nvoid fn_2() {B return; }";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, ValueFn_Inst_ReturnsTrue) {
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

TEST_F(FrontEndActionTests, ValueFnWithLocalVar_Inst_ReturnsTrue) {
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

TEST_F(FrontEndActionTests, ValueFnWithGlobalVar_Inst_ReturnsTrue) {
  auto instrumenter = std::make_shared<pathinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetPathCapIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));

  const std::string code_template = "int a;\nint fn() { a = 1;\nreturn a; }";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<pathinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code_template);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Aint a;\nint fn() {B a = 1;\nreturn a; }";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, ValueFnWithCallAssignment_Inst_ReturnsTrue) {
  auto instrumenter = std::make_shared<pathinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetPathCapIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));

  const std::string code_template = "int fn() { auto a = int(1);\nreturn a; }";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<pathinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code_template);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Aint fn() {B auto a = int(1);\nreturn a; }";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, ControlFlow_Ternary_Inst) {
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

TEST_F(FrontEndActionTests, ControlFlow_If_Inst) {
  auto instrumenter = std::make_shared<pathinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetPathCapIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));

  const std::string code_template = "int fn() {\n"
                                    "  int a = 1;\n"
                                    "  if (a == 1) {\n"
                                    "    return a;\n"
                                    "  }\n"
                                    "  return 0;\n"
                                    "}";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<pathinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code_template);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Aint fn() {B\n"
                               "  int a = 1;\n"
                               "  if (a == 1) {\n"
                               "    return a;\n"
                               "  }\n"
                               "  return 0;\n"
                               "}";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, ControlFlow_IfElse_Inst) {
  auto instrumenter = std::make_shared<pathinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetPathCapIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));

  const std::string code_template = "int fn() {\n"
                                    "  int a = 1;\n"
                                    "  if (a == 1) {\n"
                                    "    return a;\n"
                                    "  } else {\n"
                                    "    return 0;\n"
                                    "  }\n"
                                    "}";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<pathinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code_template);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Aint fn() {B\n"
                               "  int a = 1;\n"
                               "  if (a == 1) {\n"
                               "    return a;\n"
                               "  } else {\n"
                               "    return 0;\n"
                               "  }\n"
                               "}";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, ControlFlow_IfElseif_Inst) {
  auto instrumenter = std::make_shared<pathinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetPathCapIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));

  const std::string code_template = "int fn() {\n"
                                    "  int a = 1;\n"
                                    "  if (a == 1) {\n"
                                    "    return a;\n"
                                    "  } else if (a == 2) {\n"
                                    "    return -1;\n"
                                    "  }\n"
                                    "  return 0;\n"
                                    "}";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<pathinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code_template);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Aint fn() {B\n"
                                    "  int a = 1;\n"
                                    "  if (a == 1) {\n"
                                    "    return a;\n"
                                    "  } else if (a == 2) {\n"
                                    "    return -1;\n"
                                    "  }\n"
                                    "  return 0;\n"
                                    "}";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, ControlFlow_IfElseifElse_Inst) {
  auto instrumenter = std::make_shared<pathinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetPathCapIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));

  const std::string code_template = "int fn() {\n"
                                    "  int a = 1;\n"
                                    "  if (a == 1) {\n"
                                    "    return a;\n"
                                    "  } else if (a == 2) {\n"
                                    "    return -1;\n"
                                    "  } else {\n"
                                    "    return 0;\n"
                                    "  }\n"
                                    "}";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<pathinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code_template);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Aint fn() {B\n"
                                    "  int a = 1;\n"
                                    "  if (a == 1) {\n"
                                    "    return a;\n"
                                    "  } else if (a == 2) {\n"
                                    "    return -1;\n"
                                    "  } else {\n"
                                    "    return 0;\n"
                                    "  }\n"
                                    "}";
  ASSERT_EQ(streams_[0], expected);
}
