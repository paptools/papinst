#include "papinst/ast_visitor.h"

// Local headers.
#include "papinst/instrumenter.h"

// Third-party headers.
#include <clang/AST/ASTContext.h>
#include <clang/Rewrite/Core/Rewriter.h>

// C++ standard library headers.
#include <iostream> // TODO: Remove this once debugging is done.
#include <map>
#include <memory>
// #include <set>
#include <sstream>
#include <string>

namespace papinst {
namespace {
// Returns the fully quality function signature.
std::string GetFunctionSignature(const clang::FunctionDecl *fn) {
  std::stringstream ss;
  ss << fn->getReturnType().getAsString() << " "
     << fn->getQualifiedNameAsString() << "(";
  bool first_param = true;
  for (auto &&param : fn->parameters()) {
    if (!first_param) {
      ss << ", ";
    }
    ss << param->getType().getAsString();
    first_param = false;
  }
  ss << ")";
  return ss.str();
}

class DefaultASTVisitorListener : public ASTVisitorListener {
public:
  DefaultASTVisitorListener(std::shared_ptr<Instrumenter> instrumenter)
      : instrumenter_(instrumenter), context_(nullptr), rewriter_(nullptr) {}
  ~DefaultASTVisitorListener() = default;

  void SetRewriter(clang::Rewriter &rewriter) override {
    rewriter_ = &rewriter;
  }

  void Initialize(clang::ASTContext &context) override { context_ = &context; }

  void ProcessFnDef(clang::FunctionDecl *decl) override {
    assert(context_);
    if (auto body = decl->getBody()) {
      // auto stmt_id = decl->getBody()->getID(*context_);
      // std::cout << "stmt ID: " << stmt_id << std::endl;
      auto sig = GetFunctionSignature(decl);
      auto compound_stmt =
          clang::dyn_cast<clang::CompoundStmt>(decl->getBody());
      rewriter_->InsertTextAfterToken(compound_stmt->getLBracLoc(),
                                      instrumenter_->GetFnCalleeInst(sig));
    }
  }

  void ProcessIfStmt(clang::IfStmt *stmt) override {
    assert(context_);
    auto then_stmt = stmt->getThen();
    auto then_id = then_stmt->getID(*context_);
    if (clang::isa<clang::CompoundStmt>(then_stmt)) {
      rewriter_->InsertTextAfterToken(then_stmt->getBeginLoc(),
                                      instrumenter_->GetStmtInst(then_id));
    } else {
      std::ostringstream oss;
      oss << "{" << instrumenter_->GetStmtInst(then_id)
          << rewriter_->getRewrittenText(then_stmt->getSourceRange()) << ";}";
      rewriter_->ReplaceText(then_stmt->getSourceRange(), oss.str());
    }

    if (auto else_stmt = stmt->getElse()) {
      auto else_id = else_stmt->getID(*context_);
      if (clang::isa<clang::CompoundStmt>(else_stmt)) {
        rewriter_->InsertTextAfterToken(else_stmt->getBeginLoc(),
                                        instrumenter_->GetStmtInst(else_id));
      } else {
        std::ostringstream oss;
        oss << "{" << instrumenter_->GetStmtInst(else_id)
            << rewriter_->getRewrittenText(else_stmt->getSourceRange()) << ";}";
        rewriter_->ReplaceText(else_stmt->getSourceRange(), oss.str());
      }
    }
  }

  void ProcessSwitchStmt(clang::SwitchStmt *stmt) override {}

  void ProcessWhileStmt(clang::WhileStmt *stmt) override {}

  void ProcessForStmt(clang::ForStmt *stmt) override {}

  void ProcessDoStmt(clang::DoStmt *stmt) override {}

  void ProcessReturnStmt(clang::ReturnStmt *stmt) override {}

private:
  std::shared_ptr<Instrumenter> instrumenter_;
  clang::ASTContext *context_;
  clang::Rewriter *rewriter_;
};

class DefaultASTVisitor : public ASTVisitor {
public:
  DefaultASTVisitor(std::shared_ptr<ASTVisitorListener> listener)
      : listener_(listener) {}

  virtual ~DefaultASTVisitor() = default;

  void SetRewriter(clang::Rewriter &rewriter) override {
    listener_->SetRewriter(rewriter);
  }

  bool TraverseAST(clang::ASTContext &context) override {
    context_ = &context;
    listener_->Initialize(context);
    return RecursiveASTVisitor::TraverseAST(context);
  }

  bool VisitFunctionDecl(clang::FunctionDecl *decl) override {
    assert(context_);
    // if (!context_->getSourceManager().isInSystemHeader(decl->getBeginLoc()))
    if (decl->isThisDeclarationADefinition()) {
      listener_->ProcessFnDef(decl);
    }
    return true;
  }

  bool VisitStmt(clang::Stmt *stmt) override {
    assert(context_);

    if (auto if_stmt = clang::dyn_cast<clang::IfStmt>(stmt)) {
      listener_->ProcessIfStmt(if_stmt);
    } else if (auto switch_stmt = clang::dyn_cast<clang::SwitchStmt>(stmt)) {
      listener_->ProcessSwitchStmt(switch_stmt);
    } else if (auto while_stmt = clang::dyn_cast<clang::WhileStmt>(stmt)) {
      listener_->ProcessWhileStmt(while_stmt);
    } else if (auto for_stmt = clang::dyn_cast<clang::ForStmt>(stmt)) {
      listener_->ProcessForStmt(for_stmt);
    } else if (auto do_stmt = clang::dyn_cast<clang::DoStmt>(stmt)) {
      listener_->ProcessDoStmt(do_stmt);
    } else if (auto return_stmt = clang::dyn_cast<clang::ReturnStmt>(stmt)) {
      listener_->ProcessReturnStmt(return_stmt);
    }
    return true;
  }

private:
  clang::ASTContext *context_;
  std::shared_ptr<ASTVisitorListener> listener_;
};
} // namespace

std::shared_ptr<ASTVisitorListener>
ASTVisitorListener::Create(std::shared_ptr<Instrumenter> instrumenter) {
  return std::make_shared<DefaultASTVisitorListener>(instrumenter);
}

std::shared_ptr<ASTVisitor>
ASTVisitor::Create(std::shared_ptr<ASTVisitorListener> listener) {
  return std::make_shared<DefaultASTVisitor>(listener);
}
} // namespace papinst
