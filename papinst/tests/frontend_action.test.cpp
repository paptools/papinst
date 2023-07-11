// Local headers.
#include "papinst/frontend_action.h"
#include "papinst/mock_ast_consumer_listener.h"

// Third-party headers.
#include <clang/Tooling/Tooling.h>
#include <gtest/gtest.h>

// C++ standard library headers.
#include <memory>

TEST(FrontEndActionTests, ValidCode_ProcessTUCalled_ReturnsSuccess) {
  auto ast_consumer_listener =
      std::make_shared<papinst::MockASTConsumerListener>();
  EXPECT_CALL(*ast_consumer_listener, ProcessError).Times(0);
  EXPECT_CALL(*ast_consumer_listener, ProcessTranslationUnit).Times(1);

  bool success = clang::tooling::runToolOnCode(
      papinst::FrontendAction::Create(ast_consumer_listener), "int fn();");
  ASSERT_TRUE(success);
}

TEST(FrontEndActionTests, ParseError_ProcessErrorCalled_ReturnsError) {
  auto ast_consumer_listener =
      std::make_shared<papinst::MockASTConsumerListener>();
  EXPECT_CALL(*ast_consumer_listener, ProcessError).Times(1);
  EXPECT_CALL(*ast_consumer_listener, ProcessTranslationUnit).Times(0);

  bool success = clang::tooling::runToolOnCode(
      papinst::FrontendAction::Create(ast_consumer_listener),
      "unparsable code");
  ASSERT_FALSE(success);
}

TEST(FrontEndActionTests, PreprocessError_ProcessErrorCalled_ReturnsError) {
  auto ast_consumer_listener =
      std::make_shared<papinst::MockASTConsumerListener>();
  EXPECT_CALL(*ast_consumer_listener, ProcessError).Times(1);
  EXPECT_CALL(*ast_consumer_listener, ProcessTranslationUnit).Times(0);

  bool success = clang::tooling::runToolOnCode(
      papinst::FrontendAction::Create(ast_consumer_listener),
      "#error \"preprocess error\"");
  ASSERT_FALSE(success);
}
