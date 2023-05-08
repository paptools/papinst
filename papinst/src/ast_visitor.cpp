#include "papinst/ast_visitor.h"

// Local headers.
#include "papinst/instrumenter.h"

// Third-party headers.
#include <clang/AST/ASTContext.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/Lexer.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <fmt/format.h>
#ifdef PAPINST_OUTPUT_CFG
#include <clang/Analysis/CFG.h>
#include <llvm/Support/GraphWriter.h>
#endif // PAPINST_OUTPUT_CFG

// C++ standard library headers.
#include <iostream> // TODO: Remove this once debugging is done.
#include <map>
#include <memory>
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

// IRD TODO: Move to inst impl.
std::string GetTraceParamInst(int id, const std::string &param) {
  static const std::string template_str = "\nPAPTRACE_TRACE_PARAM({}, {});";
  return fmt::format(template_str, id, param);
}

std::string GetTraceStmtInst(int id, const std::string &type) {
  static const std::string template_str = "PAPTRACE_TRACE_STMT(\"{}\", {});";
  return fmt::format(template_str, type, id);
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
      // Skip if the function is not user defined.
      if (!context_->getSourceManager().isInMainFile(body->getBeginLoc())) {
        return;
      }

      auto sig = GetFunctionSignature(decl);
#ifdef PAPINST_OUTPUT_CFG
      auto options = clang::CFG::BuildOptions();
      if (std::unique_ptr<clang::CFG> cfg =
              clang::CFG::buildCFG(decl, body, context_, options)) {
        clang::LangOptions lang_opts;
        cfg->dump(lang_opts, /*ShowColors*/ true);
        llvm::WriteGraph(llvm::outs(), cfg.get(), true, sig);
      }
#endif // PAPINST_OUTPUT_CFG

      auto id = body->getID(*context_);
      std::ostringstream oss;
      oss << instrumenter_->GetTraceCalleeInst(id, sig);
      for (auto param : decl->parameters()) {
        oss << GetTraceParamInst(id, param->getNameAsString());
        auto param_str = param->getNameAsString();
      }

      auto compound_stmt =
          clang::dyn_cast<clang::CompoundStmt>(decl->getBody());
      rewriter_->InsertTextAfterToken(compound_stmt->getLBracLoc(), oss.str());
    }
  }

  void ProcessIfStmt(clang::IfStmt *stmt) override {
    assert(context_);

    auto then_stmt = stmt->getThen();
    auto then_id = then_stmt->getID(*context_);
    if (clang::isa<clang::CompoundStmt>(then_stmt)) {
      rewriter_->InsertTextAfterToken(
          then_stmt->getBeginLoc(),
          instrumenter_->GetTraceIfThenStmtInst(then_id));
    } else {
      auto semi_loc = clang::Lexer::getLocForEndOfToken(
          then_stmt->getEndLoc(), 0, context_->getSourceManager(),
          context_->getLangOpts());
      auto rewrite_range =
          clang::SourceRange(then_stmt->getBeginLoc(), semi_loc);
      std::ostringstream oss;
      oss << "{" << instrumenter_->GetTraceIfThenStmtInst(then_id)
          << rewriter_->getRewrittenText(rewrite_range) << "}";
      rewriter_->ReplaceText(rewrite_range, oss.str());
    }

    if (auto else_stmt = stmt->getElse()) {
      // Only handle Else statements here. Else-If statements will be handled by
      // calls to the ProcessIfStmt method.
      // TODO: There is currently no difference in the traces for (a) adjacent
      // If statements and (b) Else-If statements. We should update the trace to
      // distinguish between these two cases.
      if (!clang::isa<clang::IfStmt>(else_stmt)) {
        auto else_id = else_stmt->getID(*context_);
        if (clang::isa<clang::CompoundStmt>(else_stmt)) {
          rewriter_->InsertTextAfterToken(
              else_stmt->getBeginLoc(),
              instrumenter_->GetTraceIfElseStmtInst(else_id));
        } else {
          auto semi_loc = clang::Lexer::getLocForEndOfToken(
              else_stmt->getEndLoc(), 0, context_->getSourceManager(),
              context_->getLangOpts());
          auto rewrite_range =
              clang::SourceRange(else_stmt->getBeginLoc(), semi_loc);
          std::ostringstream oss;
          oss << "{" << instrumenter_->GetTraceIfElseStmtInst(else_id)
              << rewriter_->getRewrittenText(rewrite_range) << "}";
          rewriter_->ReplaceText(rewrite_range, oss.str());
        }
      }
    }
  }

  void ProcessSwitchStmt(clang::SwitchStmt *stmt) override {
    assert(context_);

    if (auto case_stmt = stmt->getSwitchCaseList()) {
      do {
        if (auto sub_stmt = case_stmt->getSubStmt()) {
          auto id = sub_stmt->getID(*context_);
          if (clang::isa<clang::CompoundStmt>(sub_stmt)) {
            auto compound_stmt = clang::dyn_cast<clang::CompoundStmt>(sub_stmt);
            rewriter_->InsertTextAfterToken(compound_stmt->getBeginLoc(),
                                            GetTraceStmtInst(id, "CaseStmt"));
          } else {
            if (!clang::isa<clang::SwitchCase>(sub_stmt)) {
              auto semi_loc = clang::Lexer::getLocForEndOfToken(
                  sub_stmt->getEndLoc(), 0, context_->getSourceManager(),
                  context_->getLangOpts());
              auto rewrite_range =
                  clang::SourceRange(sub_stmt->getBeginLoc(), semi_loc);
              std::ostringstream oss;
              oss << "{" << GetTraceStmtInst(id, "CaseStmt")
                  << rewriter_->getRewrittenText(rewrite_range) << "}";
              rewriter_->ReplaceText(rewrite_range, oss.str());
            }
          }
        }
        case_stmt = case_stmt->getNextSwitchCase();
      } while (case_stmt);
    }
  }

  void ProcessWhileStmt(clang::WhileStmt *stmt) override {
    assert(context_);

    if (auto body = stmt->getBody()) {
      auto id = body->getID(*context_);
      if (clang::isa<clang::CompoundStmt>(body)) {
        auto compound_stmt = clang::dyn_cast<clang::CompoundStmt>(body);
        rewriter_->InsertTextAfterToken(compound_stmt->getLBracLoc(),
                                        GetTraceStmtInst(id, "WhileStmt"));
      } else {
        auto semi_loc = clang::Lexer::getLocForEndOfToken(
            body->getEndLoc(), 0, context_->getSourceManager(),
            context_->getLangOpts());
        auto rewrite_range = clang::SourceRange(body->getBeginLoc(), semi_loc);
        std::ostringstream oss;
        oss << "{" << GetTraceStmtInst(id, "WhileStmt")
            << rewriter_->getRewrittenText(rewrite_range) << "}";
        rewriter_->ReplaceText(rewrite_range, oss.str());
      }
    }
  }

  void ProcessForStmt(clang::ForStmt *stmt) override {
    assert(context_);

    if (auto body = stmt->getBody()) {
      auto id = body->getID(*context_);
      if (clang::isa<clang::CompoundStmt>(body)) {
        auto compound_stmt = clang::dyn_cast<clang::CompoundStmt>(body);
        rewriter_->InsertTextAfterToken(compound_stmt->getLBracLoc(),
                                        GetTraceStmtInst(id, "ForStmt"));
      } else {
        auto semi_loc = clang::Lexer::getLocForEndOfToken(
            body->getEndLoc(), 0, context_->getSourceManager(),
            context_->getLangOpts());
        auto rewrite_range = clang::SourceRange(body->getBeginLoc(), semi_loc);
        std::ostringstream oss;
        oss << "{" << GetTraceStmtInst(id, "ForStmt")
            << rewriter_->getRewrittenText(rewrite_range) << "}";
        rewriter_->ReplaceText(rewrite_range, oss.str());
      }
    }
  }

  void ProcessDoStmt(clang::DoStmt *stmt) override {
    assert(context_);

    if (auto body = stmt->getBody()) {
      auto id = body->getID(*context_);
      if (clang::isa<clang::CompoundStmt>(body)) {
        auto compound_stmt = clang::dyn_cast<clang::CompoundStmt>(body);
        rewriter_->InsertTextAfterToken(compound_stmt->getLBracLoc(),
                                        GetTraceStmtInst(id, "DoStmt"));
      } else {
        auto semi_loc = clang::Lexer::getLocForEndOfToken(
            body->getEndLoc(), 0, context_->getSourceManager(),
            context_->getLangOpts());
        auto rewrite_range = clang::SourceRange(body->getBeginLoc(), semi_loc);
        std::ostringstream oss;
        oss << "{" << GetTraceStmtInst(id, "DoStmt")
            << rewriter_->getRewrittenText(rewrite_range) << "}";
        rewriter_->ReplaceText(rewrite_range, oss.str());
      }
    }
  }

  // IRD TODO: account for ternary return.
  void ProcessReturnStmt(clang::ReturnStmt *stmt) override {
    assert(context_);

    auto id = stmt->getID(*context_);
    std::ostringstream oss;
    oss << GetTraceStmtInst(id, "ReturnStmt")
        << rewriter_->getRewrittenText(stmt->getSourceRange());
    rewriter_->ReplaceText(stmt->getSourceRange(), oss.str());
  }

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

    // std::cout << "VisitStmt:" << std::endl;
    // stmt->dumpColor();
    if (context_->getSourceManager().isInMainFile(stmt->getBeginLoc())) {
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
