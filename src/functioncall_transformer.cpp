#include "pathinst/functioncall_transformer.h"

#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <spdlog/spdlog.h>

FunctionCallTransformer::FunctionCallTransformer(clang::ASTContext &context,
                                                 clang::Rewriter &rewriter)
    : Transformer(context, rewriter) {}

void FunctionCallTransformer::start(void) {
  clang::ast_matchers::MatchFinder function_finder;

  auto call_expr_matcher = clang::ast_matchers::callExpr().bind("callExpr");
  function_finder.addMatcher(call_expr_matcher, this);

  // Add matcher for selection statements.
  auto selection_statement_matcher =
      clang::ast_matchers::stmt(clang::ast_matchers::anyOf(
          clang::ast_matchers::ifStmt(), clang::ast_matchers::switchStmt()));
  function_finder.addMatcher(selection_statement_matcher, this);

  function_finder.matchAST(context);
}

void FunctionCallTransformer::run(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {

  if (const clang::IfStmt *if_stmt =
          result.Nodes.getNodeAs<clang::IfStmt>("ifStmt")) {
    spdlog::warn("Found if statement.");
  }

  // CallExpr is the function call
  // FunctionDecl is fhe function definition
  if (const clang::CallExpr *call_expr =
          result.Nodes.getNodeAs<clang::CallExpr>("callExpr")) {
    if (const clang::FunctionDecl *function = call_expr->getDirectCallee()) {
      if (result.SourceManager->isInSystemHeader(
              function->getSourceRange().getBegin()))
        return;

      auto function_name = function->getNameAsString();
      rewriter.InsertTextAfter(call_expr->getBeginLoc(), "fn_");
    }
  }
}
