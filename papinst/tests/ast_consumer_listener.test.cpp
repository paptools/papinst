// Local headers.
#include "papinst/ast_consumer_listener.h"
#include "papinst/frontend_action.h"
#include "papinst/mock_ast_visitor.h"
#include "papinst/mock_instrumenter.h"
#include "papinst/mock_logger.h"

// Third-party headers.
#include <clang/Tooling/Tooling.h>
#include <gtest/gtest.h>

// C++ standard library headers.
#include <memory>
#include <string>
#include <vector>

namespace {
class ProcessASTAction : public clang::ASTFrontendAction {
public:
  ProcessASTAction(llvm::unique_function<void(clang::ASTContext &)> process)
      : process_(std::move(process)) {
    assert(this->process_);
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef InFile) {
    class Consumer : public ASTConsumer {
    public:
      Consumer(llvm::function_ref<void(ASTContext &Ctx)> process)
          : process_(process) {}

      void HandleTranslationUnit(ASTContext &Ctx) override { process_(Ctx); }

    private:
      llvm::function_ref<void(ASTContext &Ctx)> process_;
    };

    return std::make_unique<Consumer>(process_);
  }

private:
  llvm::unique_function<void(clang::ASTContext &)> process_;
};

class ASTConsumerListenerTests : public ::testing::Test {
public:
  ASTConsumerListenerTests(void) : streams_(), logger_(nullptr) {}

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

TEST_F(ASTConsumerListenerTests, Test1) {
  auto visitor = std::make_shared<papinst::MockASTVisitor>();
  EXPECT_CALL(*visitor, VisitStmt).Times(1);
  auto instrumenter = std::make_shared<papinst::MockInstrumenter>();
  EXPECT_CALL(*instrumenter, GetTraceIncludeInst).Times(1);

  const std::string code = "int main() { return 1; }";
  bool success = clang::tooling::runToolOnCode(
      papinst::FrontendAction::Create(papinst::ASTConsumerListener::Create(
          logger_, streams_, instrumenter, visitor)),
      code);
  ASSERT_FALSE(success);
  ASSERT_EQ(streams_.size(), 0);
}
