#include "papinst/ast_visitor.h"

// Local headers.
#include "papinst/instrumenter.h"

// Third-party headers.
#include <clang/AST/ASTContext.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/Lexer.h>
#include <fmt/format.h>
#ifdef PAPINST_OUTPUT_CFG
#include <clang/Analysis/CFG.h>
#include <llvm/Support/GraphWriter.h>
#endif // PAPINST_OUTPUT_CFG
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/Core/Replacement.h>

// C++ standard library headers.
#include <iostream> // TODO: Remove this once debugging is done.
#include <memory>
#include <sstream>
#include <string>

namespace papinst {
namespace {
clang::tooling::Replacements s_replacements;

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

std::string ToEscapedString(const std::string &s) {
  std::string escaped;
  for (auto c : s) {
    switch (c) {
    case '\n':
      escaped += "\\n";
      break;
    case '\r':
      escaped += "\\r";
      break;
    case '\t':
      escaped += "\\t";
      break;
    case '\v':
      escaped += "\\v";
      break;
    case '\b':
      escaped += "\\b";
      break;
    case '\f':
      escaped += "\\f";
      break;
    case '\a':
      escaped += "\\a";
      break;
    case '\\':
      escaped += "\\\\";
      break;
    case '\"':
      escaped += "\\\"";
      break;
    case '\'':
      escaped += "\\\'";
      break;
    default:
      escaped += c;
      break;
    }
  }
  return escaped;
}

// TODO: Move to the instrumenter.
std::string GetTraceStmtInst(int id, const std::string &type,
                             const std::string &desc) {
  static const std::string template_str =
      "PAPTRACE_STMT_NODE({}, \"{}\", \"{}\");";
  return fmt::format(template_str, id, type, desc);
}

std::string ParamsToString(const std::vector<std::string> &params) {
  std::ostringstream oss;
  bool first_param = true;
  for (const auto &param : params) {
    if (!first_param) {
      oss << ", ";
    }
    oss << param;
    first_param = false;
  }
  return oss.str();
}

std::string GetBinaryOperatorSignature(const clang::BinaryOperator *op) {
  std::stringstream ss;
  ss << op->IgnoreUnlessSpelledInSource()->getType().getAsString() << " "
     << "operator" << op->getOpcodeStr().str() << "("
     << op->getLHS()->IgnoreUnlessSpelledInSource()->getType().getAsString()
     << ", "
     << op->getRHS()->IgnoreUnlessSpelledInSource()->getType().getAsString()
     << ")";
  return ss.str();
}

std::string GetUnaryOperatorSignature(const clang::UnaryOperator *op) {
  std::stringstream ss;
  ss << op->IgnoreUnlessSpelledInSource()->getType().getAsString() << " "
     << "operator" << clang::UnaryOperator::getOpcodeStr(op->getOpcode()).str();
  return ss.str();
}

// TODO: Move to the instrumenter.
std::string GetTraceCalleeInst(int id, const std::string &sig,
                               const std::vector<std::string> &params) {
  static const std::string template_str =
      "\nPAPTRACE_CALLEE_NODE({}, \"{}\", {});";
  return fmt::format(template_str, id, sig, ParamsToString(params));
}

// TODO: Move to the instrumenter.
std::string GetTraceCallerInst(int id, const std::string &sig,
                               const std::vector<std::string> &params) {
  static const std::string template_str =
      "\nPAPTRACE_CALLER_NODE({}, \"{}\", {}), ";
  return fmt::format(template_str, id, sig, ParamsToString(params));
}

// TODO: Move to the instrumenter.
std::string GetTraceOpInstBegin(int id, const std::string &sig) {
  static const std::string template_str = "(PAPTRACE_OP_NODE({}, \"{}\"), ";
  return fmt::format(template_str, id, sig);
}
std::string GetTraceOpInstEnd() { return ")"; }

clang::tooling::Replacement AppendSourceLoc(clang::ASTContext &context,
                                            const clang::SourceLocation &loc,
                                            const std::string &text) {
  auto end_loc = clang::Lexer::getLocForEndOfToken(
      loc, 0, context.getSourceManager(), context.getLangOpts());
  return clang::tooling::Replacement(context.getSourceManager(), end_loc, 0,
                                     text);
}

clang::tooling::Replacement PrependSourceLoc(clang::ASTContext &context,
                                             const clang::SourceLocation &loc,
                                             const std::string &text) {
  return clang::tooling::Replacement(context.getSourceManager(), loc, 0, text);
}

class DefaultASTVisitorListener : public ASTVisitorListener {
public:
  DefaultASTVisitorListener(std::shared_ptr<Instrumenter> instrumenter)
      : instrumenter_(instrumenter), context_(nullptr) {}
  ~DefaultASTVisitorListener() = default;

  void Initialize(clang::ASTContext &context) override { context_ = &context; }

  void ProcessFnDef(clang::FunctionDecl *decl) override {
    assert(context_);

    auto body = decl->getBody();
    // Skip if the function is not user defined.
    if (!body ||
        !context_->getSourceManager().isInMainFile(body->getBeginLoc())) {
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
    std::vector<std::string> params;
    for (auto param : decl->parameters()) {
      params.push_back(param->getNameAsString());
    }
    auto inst_text = GetTraceCalleeInst(id, sig, params);

    auto compound_stmt = clang::dyn_cast<clang::CompoundStmt>(decl->getBody());
    if (auto err = Add(AppendSourceLoc(*context_, compound_stmt->getLBracLoc(),
                                       inst_text))) {
      llvm::errs() << "Error: " << err << "\n";
    }
  }

  void ProcessIfStmt(clang::IfStmt *stmt) override {
    assert(context_);

    if (auto then_stmt = stmt->getThen()) {
      auto then_id = then_stmt->getID(*context_);
      auto desc = ToEscapedString(
          clang::Lexer::getSourceText(clang::CharSourceRange::getTokenRange(
                                          stmt->getCond()->getSourceRange()),
                                      context_->getSourceManager(),
                                      context_->getLangOpts())
              .str());
      auto inst_text = instrumenter_->GetTraceIfThenStmtInst(then_id, desc);
      if (clang::isa<clang::CompoundStmt>(then_stmt)) {
        if (auto err = Add(AppendSourceLoc(*context_, then_stmt->getBeginLoc(),
                                           inst_text))) {
          llvm::errs() << "Error: " << err << "\n";
        }
      } else {
        std::ostringstream oss;
        oss << " {" << inst_text;
        inst_text = oss.str();

        auto begin_loc = stmt->getRParenLoc();
        if (auto err = Add(AppendSourceLoc(*context_, begin_loc, inst_text))) {
          llvm::errs() << "Error: " << err << "\n";
        }

        auto semi_loc = clang::Lexer::getLocForEndOfToken(
            then_stmt->getEndLoc(), 0, context_->getSourceManager(),
            context_->getLangOpts());
        if (auto err = Add(AppendSourceLoc(*context_, semi_loc, "}"))) {
          llvm::errs() << "Error: " << err << "\n";
        }
      }
    }

    // Only handle Else statements here. Else-If statements will be handled by
    // calls to the ProcessIfStmt method.
    // TODO: There is currently no difference in the traces for (a) adjacent If
    // statements and (b) Else-If statements. We should update the trace to
    // distinguish between these two cases.
    auto else_stmt = stmt->getElse();
    if (else_stmt && !clang::isa<clang::IfStmt>(else_stmt)) {
      auto else_id = else_stmt->getID(*context_);
      auto desc = ToEscapedString(
          clang::Lexer::getSourceText(clang::CharSourceRange::getTokenRange(
                                          stmt->getCond()->getSourceRange()),
                                      context_->getSourceManager(),
                                      context_->getLangOpts())
              .str());
      auto inst_text = instrumenter_->GetTraceIfElseStmtInst(else_id, desc);
      if (clang::isa<clang::CompoundStmt>(else_stmt)) {
        if (auto err = Add(AppendSourceLoc(*context_, else_stmt->getBeginLoc(),
                                           inst_text))) {
          llvm::errs() << "Error: " << err << "\n";
        }
      } else {
        std::ostringstream oss;
        oss << " {" << inst_text;
        inst_text = oss.str();

        auto begin_loc = stmt->getElseLoc().getLocWithOffset(4);
        if (auto err = Add(AppendSourceLoc(*context_, begin_loc, inst_text))) {
          llvm::errs() << "Error: " << err << "\n";
        }

        auto semi_loc = clang::Lexer::getLocForEndOfToken(
            else_stmt->getEndLoc(), 0, context_->getSourceManager(),
            context_->getLangOpts());
        if (auto err = Add(AppendSourceLoc(*context_, semi_loc, "}"))) {
          llvm::errs() << "Error: " << err << "\n";
        }
      }
    }
  }

  void ProcessSwitchStmt(clang::SwitchStmt *stmt) override {
    assert(context_);

    if (auto case_stmt = stmt->getSwitchCaseList()) {
      clang::SourceLocation prev_semi_loc;
      do {
        auto sub_stmt = case_stmt->getSubStmt();
        auto id = sub_stmt->getID(*context_);

        auto desc = ToEscapedString(
            clang::Lexer::getSourceText(clang::CharSourceRange::getTokenRange(
                                            sub_stmt->getSourceRange()),
                                        context_->getSourceManager(),
                                        context_->getLangOpts())
                .str());

        auto inst_text = GetTraceStmtInst(id, "CaseStmt", desc);
        if (auto compound_stmt =
                clang::dyn_cast<clang::CompoundStmt>(sub_stmt)) {
          if (auto err = Add(AppendSourceLoc(
                  *context_, compound_stmt->getBeginLoc(), inst_text))) {
            llvm::errs() << "Error: " << err << "\n";
          }
        } else {
          auto semi_loc = clang::Lexer::getLocForEndOfToken(
              case_stmt->getEndLoc(), 0, context_->getSourceManager(),
              context_->getLangOpts());
          if (semi_loc != prev_semi_loc) {
            if (auto err = Add(AppendSourceLoc(
                    *context_, case_stmt->getColonLoc(), inst_text))) {
              llvm::errs() << "Error: " << err << "\n";
            }
            prev_semi_loc = semi_loc;
          }
        }

        case_stmt = case_stmt->getNextSwitchCase();
      } while (case_stmt);
    }
  }

  void ProcessWhileStmt(clang::WhileStmt *stmt) override {
    assert(context_);

    auto id = stmt->getID(*context_);
    auto desc = ToEscapedString(
        clang::Lexer::getSourceText(
            clang::CharSourceRange::getTokenRange(stmt->getWhileLoc(),
                                                  stmt->getRParenLoc()),
            context_->getSourceManager(), context_->getLangOpts())
            .str());
    std::ostringstream oss;
    oss << "{" << GetTraceStmtInst(id, "WhileStmt", desc);
    auto inst_text = oss.str();
    if (auto err =
            Add(PrependSourceLoc(*context_, stmt->getBeginLoc(), inst_text))) {
      llvm::errs() << "Error: " << err << "\n";
    }
    if (auto err = Add(AppendSourceLoc(*context_, stmt->getEndLoc(), "}"))) {
      llvm::errs() << "Error: " << err << "\n";
    }

    ProcessLoopBody(id, stmt->getBody(), stmt->getRParenLoc(),
                    stmt->getEndLoc());
  }

  void ProcessForStmt(clang::ForStmt *stmt) override {
    assert(context_);

    auto id = stmt->getID(*context_);
    auto desc = ToEscapedString(
        clang::Lexer::getSourceText(
            clang::CharSourceRange::getTokenRange(stmt->getForLoc(),
                                                  stmt->getRParenLoc()),
            context_->getSourceManager(), context_->getLangOpts())
            .str());
    std::ostringstream oss;
    oss << "{" << GetTraceStmtInst(id, "ForStmt", desc);
    auto inst_text = oss.str();
    if (auto err =
            Add(PrependSourceLoc(*context_, stmt->getBeginLoc(), inst_text))) {
      llvm::errs() << "Error: " << err << "\n";
    }
    if (auto err = Add(AppendSourceLoc(*context_, stmt->getEndLoc(), "}"))) {
      llvm::errs() << "Error: " << err << "\n";
    }

    ProcessLoopBody(id, stmt->getBody(), stmt->getRParenLoc(),
                    stmt->getEndLoc());
  }

  void ProcessDoStmt(clang::DoStmt *stmt) override {
    assert(context_);

    auto id = stmt->getID(*context_);
    auto desc = ToEscapedString(
        clang::Lexer::getSourceText(
            clang::CharSourceRange::getTokenRange(stmt->getWhileLoc(),
                                                  stmt->getRParenLoc()),
            context_->getSourceManager(), context_->getLangOpts())
            .str());
    std::ostringstream oss;
    oss << "{" << GetTraceStmtInst(id, "DoStmt", desc);
    auto inst_text = oss.str();
    if (auto err =
            Add(PrependSourceLoc(*context_, stmt->getBeginLoc(), inst_text))) {
      llvm::errs() << "Error: " << err << "\n";
    }
    if (auto err = Add(AppendSourceLoc(*context_, stmt->getEndLoc(), "}"))) {
      llvm::errs() << "Error: " << err << "\n";
    }

    auto body = stmt->getBody();
    ProcessLoopBody(id, body, stmt->getBeginLoc(), body->getEndLoc());
  }

  // IRD TODO: account for ternary return.
  void ProcessReturnStmt(clang::ReturnStmt *stmt) override {
    assert(context_);

    auto id = stmt->getID(*context_);
    auto desc = ToEscapedString(
        clang::Lexer::getSourceText(
            clang::CharSourceRange::getTokenRange(stmt->getSourceRange()),
            context_->getSourceManager(), context_->getLangOpts())
            .str());
    auto inst_text = GetTraceStmtInst(id, "ReturnStmt", desc);
    if (auto err =
            Add(PrependSourceLoc(*context_, stmt->getBeginLoc(), inst_text))) {
      llvm::errs() << "Error: " << err << "\n";
    }
  }

  void ProcessCallExpr(clang::CallExpr *expr) override {
    assert(context_);

    // Skip caller instrumentation for functions with callee instrumentation.
    auto callee = expr->getDirectCallee();
    assert(callee); // TODO: When does this fail?
    if (callee->isDefined() &&
        context_->getSourceManager().isInMainFile(callee->getBeginLoc())) {
      return;
    }

    auto id = expr->getID(*context_);
    auto sig = GetFunctionSignature(callee);
    std::vector<std::string> params;
    for (auto arg : expr->arguments()) {
      if (!arg->IgnoreUnlessSpelledInSource()) {
        continue;
      }

      auto arg_str = clang::Lexer::getSourceText(
          clang::CharSourceRange::getTokenRange(arg->getSourceRange()),
          context_->getSourceManager(), context_->getLangOpts());
      params.push_back(std::string(arg_str));
    }
    std::ostringstream oss;
    oss << "(" << GetTraceCallerInst(id, sig, params);
    auto inst_text = oss.str();

    auto replacement =
        PrependSourceLoc(*context_, expr->getBeginLoc(), inst_text);
    auto it = visited_repls_.find(replacement.getOffset());
    if (it != visited_repls_.end()) {
      auto prev_repl = it->second;
      inst_text = prev_repl.getReplacementText().str() + inst_text;
      replacement = PrependSourceLoc(*context_, expr->getBeginLoc(), inst_text);

      clang::tooling::Replacements replacements;
      auto ignored_result = replacements.add(replacement);
      for (auto &r : s_replacements) {
        ignored_result = replacements.add(r);
      }
      s_replacements = replacements;
      if (auto err = Add(AppendSourceLoc(*context_, expr->getEndLoc(), ")"))) {
        llvm::errs() << "Error: " << err << "\n";
      }

      // std::cout << "CURR OFFSET: " << replacement.getOffset() << ", CURR LEN:
      // " << replacement.getLength() << "\n"; std::cout << "CURR REPLACEMENT: "
      // << replacement.getReplacementText().str() << "\n";

      // auto adj_offset = prev_repl.getOffset() +
      // prev_repl.getReplacementText().size(); std::cout << "ADJ OFFSET: " <<
      // adj_offset << "\n";

      // clang::SourceLocation inst_loc =
      // expr->getBeginLoc().getLocWithOffset(inst_pos); prev_repl.getOffset() +
      // prev_repl.getReplacementText().size());
      // auto inst_loc = expr->getBeginLoc();
      //// convert inst_loc to string and output to std.
      // std::cout << "inst_loc: " <<
      // inst_loc.printToString(context_->getSourceManager()) << "\n"; auto
      // inst_pos =
      // s_replacements.getShiftedCodePosition(prev_repl.getOffset()); std::cout
      // << "inst_pos: " << inst_loc.getRawEncoding() << "\n"; std::cout <<
      // "inst_pos: " << inst_pos << "\n"; inst_loc =
      // inst_loc.getLocWithOffset(inst_pos); std::cout << "inst_loc: " <<
      // inst_loc.printToString(context_->getSourceManager()) << "\n";

      // auto inst_loc =
      // expr->getBeginLoc().getLocWithOffset(prev_repl.getReplacementText().size());
      // replacement = PrependSourceLoc(
      //     *context_,
      //     //expr->getBeginLoc().getLocWithOffset(prev_repl.getReplacementText().size()),
      //     expr->getBeginLoc(),
      //     inst_text);
      // if (auto err = Add(replacement)) {
      //   llvm::errs() << "Error: " << err << "\n";
      // }

      // if (auto err = Add(AppendSourceLoc(*context_, expr->getEndLoc(), ")")))
      // {
      //   llvm::errs() << "Error: " << err << "\n";
      // }
      return;
    } else {
      if (auto err = Add(replacement)) {
        llvm::errs() << "Error: " << err << "\n";
      }

      if (auto err = Add(AppendSourceLoc(*context_, expr->getEndLoc(), ")"))) {
        llvm::errs() << "Error: " << err << "\n";
      }
    }
  }

  void ProcessCXXThrowExpr(clang::CXXThrowExpr *expr) override {
    assert(context_);

    auto id = expr->getID(*context_);
    auto desc = ToEscapedString(
        clang::Lexer::getSourceText(
            clang::CharSourceRange::getTokenRange(expr->getSourceRange()),
            context_->getSourceManager(), context_->getLangOpts())
            .str());
    auto inst_text = GetTraceStmtInst(id, "CXXThrowExpr", desc);
    if (auto err =
            Add(PrependSourceLoc(*context_, expr->getBeginLoc(), inst_text))) {
      llvm::errs() << "Error: " << err << "\n";
    }
  }

  void ProcessBinaryOperator(clang::BinaryOperator *op) override {
    assert(context_);

    if (op->isAssignmentOp() || op->isAdditiveOp() ||
        op->isCompoundAssignmentOp()) {
      // op->dumpColor();
      auto id = op->getID(*context_);
      const std::string sig = GetBinaryOperatorSignature(op);
      auto lhs = op->getLHS()->IgnoreUnlessSpelledInSource();
      // const std::string lhs_str =
      //     clang::Lexer::getSourceText(
      //         clang::CharSourceRange::getTokenRange(lhs->getSourceRange()),
      //         context_->getSourceManager(), context_->getLangOpts())
      //         .str();
      auto rhs = op->getRHS()->IgnoreUnlessSpelledInSource();
      // const std::string rhs_str =
      //     clang::Lexer::getSourceText(
      //         clang::CharSourceRange::getTokenRange(rhs->getSourceRange()),
      //         context_->getSourceManager(), context_->getLangOpts())
      //         .str();
      auto inst_text = GetTraceOpInstBegin(id, sig);

      auto begin_loc = op->getLHS()->getBeginLoc();
      if (auto err = Add(PrependSourceLoc(*context_, begin_loc, inst_text))) {
        llvm::errs() << "Error: " << err << "\n";
      }

      auto end_loc = op->getRHS()->getEndLoc();
      inst_text = GetTraceOpInstEnd();
      if (auto err = Add(AppendSourceLoc(*context_, end_loc, inst_text))) {
        llvm::errs() << "Error: " << err << "\n";
      }
    } else {
      // op->dumpColor();
    }
  }

  void ProcessUnaryOperator(clang::UnaryOperator *op) override {
    assert(context_);

    if (op->isIncrementDecrementOp()) {
      auto id = op->getID(*context_);
      const std::string sig = GetUnaryOperatorSignature(op);
      // const std::string type =
      //     clang::UnaryOperator::getOpcodeStr(op->getOpcode()).str();

      auto inst_text = GetTraceOpInstBegin(id, sig);
      if (auto err =
              Add(PrependSourceLoc(*context_, op->getBeginLoc(), inst_text))) {
        llvm::errs() << "Error: " << err << "\n";
      }

      inst_text = GetTraceOpInstEnd();
      if (auto err =
              Add(AppendSourceLoc(*context_, op->getEndLoc(), inst_text))) {
        llvm::errs() << "Error: " << err << "\n";
      }
    } else {
      // op->dumpColor();
    }
  }

private:
  std::shared_ptr<Instrumenter> instrumenter_;
  clang::ASTContext *context_;
  std::map<unsigned int, clang::tooling::Replacement> visited_repls_;

  void ProcessLoopBody(int id, clang::Stmt *body,
                       const clang::SourceLocation &begin_loc,
                       const clang::SourceLocation &end_loc) {
    auto body_id = body->getID(*context_);
    auto inst_text = GetTraceStmtInst(body_id, "LoopIter", "LoopIter");
    if (auto compound_stmt = clang::dyn_cast<clang::CompoundStmt>(body)) {
      if (auto err = Add(AppendSourceLoc(
              *context_, compound_stmt->getBeginLoc(), inst_text))) {
        llvm::errs() << "Error: " << err;
      }
    } else {
      std::ostringstream oss;
      oss << " {" << inst_text;
      inst_text = oss.str();

      if (auto err = Add(AppendSourceLoc(*context_, begin_loc, inst_text))) {
        llvm::errs() << "Error: " << err << "\n";
      }

      auto semi_loc = clang::Lexer::getLocForEndOfToken(
          end_loc, 0, context_->getSourceManager(), context_->getLangOpts());
      if (auto err = Add(AppendSourceLoc(*context_, semi_loc, "}"))) {
        llvm::errs() << "Error: " << err << "\n";
      }
    }
  }

  llvm::Error Add(const clang::tooling::Replacement &replacement) {
    visited_repls_[replacement.getOffset()] = replacement;
    return s_replacements.add(replacement);
  }
};

class DefaultASTVisitor : public ASTVisitor {
public:
  DefaultASTVisitor(std::shared_ptr<ASTVisitorListener> listener)
      : listener_(listener) {}

  virtual ~DefaultASTVisitor() = default;

  bool TraverseAST(clang::ASTContext &context) override {
    context_ = &context;
    listener_->Initialize(context);
    return RecursiveASTVisitor::TraverseAST(context);
  }

  bool VisitFunctionDecl(clang::FunctionDecl *decl) override {
    assert(context_);
    if (decl->isThisDeclarationADefinition()) {
      listener_->ProcessFnDef(decl);
    }
    return true;
  }

  bool VisitStmt(clang::Stmt *stmt) override {
    assert(context_);

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
      } else if (auto call_expr = clang::dyn_cast<clang::CallExpr>(stmt)) {
        listener_->ProcessCallExpr(call_expr);
      } else if (auto throw_expr = clang::dyn_cast<clang::CXXThrowExpr>(stmt)) {
        listener_->ProcessCXXThrowExpr(throw_expr);
      } else if (auto binary_op =
                     clang::dyn_cast<clang::BinaryOperator>(stmt)) {
        listener_->ProcessBinaryOperator(binary_op);
      } else if (auto unary_op = clang::dyn_cast<clang::UnaryOperator>(stmt)) {
        listener_->ProcessUnaryOperator(unary_op);
      } else {
        // std::cout << "\n\nUnhandled stmt:" << std::endl;
        // stmt->dumpColor();
      }
    }
    return true;
  }

private:
  clang::ASTContext *context_;
  std::shared_ptr<ASTVisitorListener> listener_;
};
} // namespace

clang::tooling::Replacements &GetReplacements() { return s_replacements; }

std::shared_ptr<ASTVisitorListener>
ASTVisitorListener::Create(std::shared_ptr<Instrumenter> instrumenter) {
  return std::make_shared<DefaultASTVisitorListener>(instrumenter);
}

std::shared_ptr<ASTVisitor>
ASTVisitor::Create(std::shared_ptr<ASTVisitorListener> listener) {
  return std::make_shared<DefaultASTVisitor>(listener);
}
} // namespace papinst
