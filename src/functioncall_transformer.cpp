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

  // Add matcher for selection statements.
  auto selection_statement_matcher = clang::ast_matchers::ifStmt().bind("ifStmt");
  function_finder.addMatcher(selection_statement_matcher, this);

  function_finder.matchAST(context);
}

void FunctionCallTransformer::run(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {

  if (const clang::IfStmt *if_stmt =
          result.Nodes.getNodeAs<clang::IfStmt>("ifStmt")) {
    spdlog::warn("Found if statement.");
    static std::string print_stmt = "std::cout << \"hello\" << std::endl; ";
    rewriter.InsertTextBefore(if_stmt->getBeginLoc(), print_stmt);
  }
}
