// Local headers.
#include "papinst/ast_visitor.h"
#include "papinst/mock_ast_visitor.h"
#include "papinst/mock_instrumenter.h"

// Third-party headers.
#include <clang/Tooling/Tooling.h>
#include <gtest/gtest.h>

namespace {
class ProcessASTAction : public clang::ASTFrontendAction {
public:
  ProcessASTAction(llvm::unique_function<void(clang::ASTContext &)> process)
      : process_(std::move(process)) {
    assert(this->process_);
  }

  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile) {
    class Consumer : public clang::ASTConsumer {
    public:
      Consumer(llvm::function_ref<void(clang::ASTContext &Ctx)> process)
          : process_(process) {}

      void HandleTranslationUnit(clang::ASTContext &Ctx) override {
        process_(Ctx);
      }

    private:
      llvm::function_ref<void(clang::ASTContext &Ctx)> process_;
    };

    return std::make_unique<Consumer>(process_);
  }

private:
  llvm::unique_function<void(clang::ASTContext &)> process_;
};

class ASTVisitorTests : public ::testing::Test {
protected:
  std::shared_ptr<::testing::StrictMock<papinst::MockASTVisitorListener>>
      listener_;

  virtual void SetUp(void) override {
    listener_ = std::make_shared<
        ::testing::StrictMock<papinst::MockASTVisitorListener>>();
  }

  virtual void TearDown(void) override { listener_.reset(); }

  void RunVisitorOnCode(const std::string &code) {
    auto visitor = papinst::ASTVisitor::Create(listener_);
    clang::tooling::runToolOnCode(
        std::make_unique<ProcessASTAction>(
            [&](clang::ASTContext &Ctx) { visitor->TraverseAST(Ctx); }),
        code);
  }
};
} // namespace

TEST_F(ASTVisitorTests, Fn_Decl_Is_Not_Processed) {
  EXPECT_CALL(*listener_, Initialize).Times(1);
  const std::string code = "void fn();";
  RunVisitorOnCode(code);
}

TEST_F(ASTVisitorTests, Fn_Def_Is_Processed) {
  EXPECT_CALL(*listener_, Initialize).Times(1);
  EXPECT_CALL(*listener_, ProcessStmt).Times(1);
  EXPECT_CALL(*listener_, ProcessFnDef).Times(1);
  const std::string code = "void fn() {}";
  RunVisitorOnCode(code);
}

TEST_F(ASTVisitorTests, If_Stmt_Is_Processed) {
  EXPECT_CALL(*listener_, Initialize).Times(1);
  EXPECT_CALL(*listener_, ProcessStmt).Times(10);
  EXPECT_CALL(*listener_, ProcessFnDef).Times(1);
  EXPECT_CALL(*listener_, ProcessIfStmt).Times(1);
  const std::string code = R"(
int fn(int a) {
  if (a == 0) return 0;
  return 1;
}
)";
  RunVisitorOnCode(code);
}

TEST_F(ASTVisitorTests, If_Else_Stmts_Are_Processed) {
  EXPECT_CALL(*listener_, Initialize).Times(1);
  EXPECT_CALL(*listener_, ProcessStmt).Times(10);
  EXPECT_CALL(*listener_, ProcessFnDef).Times(1);
  EXPECT_CALL(*listener_, ProcessIfStmt).Times(1);
  const std::string code = R"(
int fn(int a) {
  if (a == 0) return 0;
  else return 1;
}
)";
  RunVisitorOnCode(code);
}

TEST_F(ASTVisitorTests, If_Elseif_Stmts_Are_Processed) {
  EXPECT_CALL(*listener_, Initialize).Times(1);
  EXPECT_CALL(*listener_, ProcessStmt).Times(17);
  EXPECT_CALL(*listener_, ProcessFnDef).Times(1);
  EXPECT_CALL(*listener_, ProcessIfStmt).Times(2);
  const std::string code = R"(
int fn(int a) {
  if (a == 0) return 0;
  else if (a == 1) return 1;
  return 2;
}
)";
  RunVisitorOnCode(code);
}

TEST_F(ASTVisitorTests, If_Elseif_Else_Stmts_Are_Processed) {
  EXPECT_CALL(*listener_, Initialize).Times(1);
  EXPECT_CALL(*listener_, ProcessStmt).Times(17);
  EXPECT_CALL(*listener_, ProcessFnDef).Times(1);
  EXPECT_CALL(*listener_, ProcessIfStmt).Times(2);
  const std::string code = R"(
int fn(int a) {
  if (a == 0) return 0;
  else if (a == 1) return 1;
  else return 2;
}
)";
  RunVisitorOnCode(code);
}

TEST_F(ASTVisitorTests, Switch_Stmts_Is_Processed) {
  EXPECT_CALL(*listener_, Initialize).Times(1);
  EXPECT_CALL(*listener_, ProcessStmt).Times(12);
  EXPECT_CALL(*listener_, ProcessFnDef).Times(1);
  EXPECT_CALL(*listener_, ProcessSwitchStmt).Times(1);
  const std::string code = R"(
int fn(int a) {
  switch (a) {
    case 0: return 0;
  }
  return 1;
}
)";
  RunVisitorOnCode(code);
}

TEST_F(ASTVisitorTests, While_Stmt_Is_Processed) {
  EXPECT_CALL(*listener_, Initialize).Times(1);
  EXPECT_CALL(*listener_, ProcessStmt).Times(11);
  EXPECT_CALL(*listener_, ProcessFnDef).Times(1);
  EXPECT_CALL(*listener_, ProcessWhileStmt).Times(1);
  const std::string code = R"(
int fn(int a) {
  while (a > 0) a--;
  return a;
}
)";
  RunVisitorOnCode(code);
}

TEST_F(ASTVisitorTests, For_Stmt_Is_Processed) {
  EXPECT_CALL(*listener_, Initialize).Times(1);
  EXPECT_CALL(*listener_, ProcessStmt).Times(16);
  EXPECT_CALL(*listener_, ProcessFnDef).Times(1);
  EXPECT_CALL(*listener_, ProcessForStmt).Times(1);
  const std::string code = R"(
int fn(int a) {
  for (int i = 0; i < a; i++) a--;
  return a;
}
)";
  RunVisitorOnCode(code);
}

TEST_F(ASTVisitorTests, Do_Stmt_Is_Processed) {
  EXPECT_CALL(*listener_, Initialize).Times(1);
  EXPECT_CALL(*listener_, ProcessStmt).Times(11);
  EXPECT_CALL(*listener_, ProcessFnDef).Times(1);
  EXPECT_CALL(*listener_, ProcessDoStmt).Times(1);
  const std::string code = R"(
int fn(int a) {
  do a--;
  while (a > 0);
  return a;
}
)";
  RunVisitorOnCode(code);
}
