// Local headers.
#include "papinst/frontend_action.h"
#include "mocks/mock_instrumenter.h"
#include "mocks/mock_logger.h"

// Third-party headers.
#include <clang/Tooling/Tooling.h>
#include <gtest/gtest.h>

// C++ standard library headers.
#include <memory>
#include <string>
#include <vector>

namespace {
class FrontEndActionTests : public ::testing::Test {
public:
  FrontEndActionTests(void) : streams_(), logger_(nullptr) {}

  virtual void SetUp(void) override {
    logger_ = std::make_shared<papinst::MockLogger>();
  }

  virtual void TearDown(void) override {
    streams_.clear();
    logger_.reset();
  }

protected:
  std::shared_ptr<papinst::MockLogger> logger_;
  std::vector<std::string> streams_;
};
} // namespace

TEST_F(FrontEndActionTests, FnDecl_Invalid_Error) {
  auto instrumenter = std::make_shared<papinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetTraceIncludeInst).Times(0);
  EXPECT_CALL(*instrumenter, GetFnCalleeInst).Times(0);
  EXPECT_CALL(*instrumenter, GetCfInst).Times(0);

  const std::string code = "void fn()";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<papinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_FALSE(success);
  ASSERT_EQ(streams_.size(), 0);
}

TEST_F(FrontEndActionTests, FnDecl_Valid_NoInst) {
  auto instrumenter = std::make_shared<papinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetTraceIncludeInst).Times(0);
  EXPECT_CALL(*instrumenter, GetFnCalleeInst).Times(0);
  EXPECT_CALL(*instrumenter, GetCfInst).Times(0);

  const std::string code = "void fn();";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<papinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 0);
}

TEST_F(FrontEndActionTests, FnDef_Invalid_Error) {
  auto instrumenter = std::make_shared<papinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetTraceIncludeInst).Times(0);
  EXPECT_CALL(*instrumenter, GetFnCalleeInst).Times(0);
  EXPECT_CALL(*instrumenter, GetCfInst).Times(0);

  const std::string source_code = "void fn() {";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<papinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    source_code);
  ASSERT_FALSE(success);
  ASSERT_EQ(streams_.size(), 0);
}

TEST_F(FrontEndActionTests, FnDef_VoidEmpty_Inst) {
  auto instrumenter = std::make_shared<papinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetTraceIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));
  EXPECT_CALL(*instrumenter, GetCfInst).Times(0);

  const std::string code = "void fn() {}";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<papinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Avoid fn() {B}";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, FnDef_VoidReturn_Inst) {
  auto instrumenter = std::make_shared<papinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetTraceIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));
  EXPECT_CALL(*instrumenter, GetCfInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("C"));

  const std::string code = "void fn() { return; }";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<papinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Avoid fn() {B return; }";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, FnDef_MixedVoidImpls_Inst) {
  auto instrumenter = std::make_shared<papinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetTraceIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(2)
      .WillRepeatedly(::testing::Return("B"));
  EXPECT_CALL(*instrumenter, GetCfInst).Times(0);

  const std::string code = "void fn_1() {}\nvoid fn_2() { return; }";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<papinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Avoid fn_1() {B}\nvoid fn_2() {B return; }";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, FnDef_ReturnValue_Inst) {
  auto instrumenter = std::make_shared<papinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetTraceIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));
  EXPECT_CALL(*instrumenter, GetCfInst).Times(0);

  const std::string code = "int fn(int a) { return a; }";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<papinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Aint fn(int a) {B return a; }";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, FnDef_LocalConstructorAssignment_Inst) {
  auto instrumenter = std::make_shared<papinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetTraceIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));
  EXPECT_CALL(*instrumenter, GetCfInst).Times(0);

  const std::string code = "int fn(int a) { int b = int(1); return a + b; }";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<papinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected =
      "Aint fn(int a) {B int b = int(1); return a + b; }";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, FnDef_WithPriorFnDecl_Inst) {
  auto instrumenter = std::make_shared<papinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetTraceIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));
  EXPECT_CALL(*instrumenter, GetCfInst).Times(0);

  const std::string code = "int fn(int a);\n"
                           "int fn(int a) { int b = int(1); return a + b; }";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<papinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected =
      "Aint fn(int a);\n"
      "int fn(int a) {B int b = int(1); return a + b; }";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, FnDef_ContainsLambda_Inst) {
  auto instrumenter = std::make_shared<papinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetTraceIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(2)
      .WillRepeatedly(::testing::Return("B"));
  EXPECT_CALL(*instrumenter, GetCfInst).Times(0);

  const std::string code = "int fn(int a) {\n"
                           "  auto b = []() { return 1; };\n"
                           "  return a + b();\n"
                           "}";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<papinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Aint fn(int a) {B\n"
                               "  auto b = []() {B return 1; };\n"
                               "  return a + b();\n"
                               "}";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, ControlFlow_Ternary_Inst) {
  auto instrumenter = std::make_shared<papinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetTraceIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));
  EXPECT_CALL(*instrumenter, GetCfInst)
      .Times(2)
      .WillRepeatedly(::testing::Return("C"));

  const std::string code =
      "int fn(int a) { int b = 1; return (a + b ? 0 : 1); }";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<papinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected =
      "Aint fn(int a) {B int b = 1; return (a + b ? C0 : C1); }";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, ControlFlow_If_Inst) {
  auto instrumenter = std::make_shared<papinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetTraceIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));
  EXPECT_CALL(*instrumenter, GetCfInst)
      .Times(2)
      .WillRepeatedly(::testing::Return("C"));

  const std::string code = "int fn(int a) {\n"
                           "  if (a == 1) {\n"
                           "    return a;\n"
                           "  }\n"
                           "  return 0;\n"
                           "}";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<papinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Aint fn(int a) {B\n"
                               "  if (a == 1) {\n"
                               "    Creturn a;\n"
                               "  }\n"
                               "  Creturn 0;\n"
                               "}";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, ControlFlow_IfElse_Inst) {
  auto instrumenter = std::make_shared<papinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetTraceIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));
  EXPECT_CALL(*instrumenter, GetCfInst)
      .Times(2)
      .WillRepeatedly(::testing::Return("C"));

  const std::string code = "int fn(int a) {\n"
                           "  if (a == 1) {\n"
                           "    return a;\n"
                           "  } else {\n"
                           "    return 0;\n"
                           "  }\n"
                           "}";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<papinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Aint fn(int a) {B\n"
                               "  if (a == 1) {\n"
                               "    Creturn a;\n"
                               "  } else {\n"
                               "    Creturn 0;\n"
                               "  }\n"
                               "}";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, ControlFlow_IfElseif_Inst) {
  auto instrumenter = std::make_shared<papinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetTraceIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));
  EXPECT_CALL(*instrumenter, GetCfInst)
      .Times(3)
      .WillRepeatedly(::testing::Return("C"));

  const std::string code = "int fn(int a) {\n"
                           "  if (a == 1) {\n"
                           "    return a;\n"
                           "  } else if (a == 2) {\n"
                           "    return -1;\n"
                           "  }\n"
                           "  return 0;\n"
                           "}";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<papinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Aint fn(int a) {B\n"
                               "  if (a == 1) {\n"
                               "    Creturn a;\n"
                               "  } else if (a == 2) {\n"
                               "    Creturn -1;\n"
                               "  }\n"
                               "  Creturn 0;\n"
                               "}";
  ASSERT_EQ(streams_[0], expected);
}

TEST_F(FrontEndActionTests, ControlFlow_IfElseifElse_Inst) {
  auto instrumenter = std::make_shared<papinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetTraceIncludeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("A"));
  EXPECT_CALL(*instrumenter, GetFnCalleeInst)
      .Times(1)
      .WillRepeatedly(::testing::Return("B"));
  EXPECT_CALL(*instrumenter, GetCfInst)
      .Times(3)
      .WillRepeatedly(::testing::Return("C"));

  const std::string code = "int fn(int a) {\n"
                           "  if (a == 1) {\n"
                           "    return a;\n"
                           "  } else if (a == 2) {\n"
                           "    return -1;\n"
                           "  } else {\n"
                           "    return 0;\n"
                           "  }\n"
                           "}";
  bool success =
      clang::tooling::runToolOnCode(std::make_unique<papinst::FrontendAction>(
                                        logger_, streams_, instrumenter),
                                    code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams_.size(), 1);
  const std::string expected = "Aint fn(int a) {B\n"
                               "  if (a == 1) {\n"
                               "    Creturn a;\n"
                               "  } else if (a == 2) {\n"
                               "    Creturn -1;\n"
                               "  } else {\n"
                               "    Creturn 0;\n"
                               "  }\n"
                               "}";
  ASSERT_EQ(streams_[0], expected);
}
