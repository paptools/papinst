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
  auto selection_statement_matcher = clang::ast_matchers::compoundStmt(clang::ast_matchers::hasParent(clang::ast_matchers::ifStmt()));
  function_finder.addMatcher(selection_statement_matcher, this);

  function_finder.matchAST(context);
}

void FunctionCallTransformer::run(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {

  if (const clang::CompoundStmt *curr_stmt = result.Nodes.getNodeAs<clang::CompoundStmt>("comoundStmt")) {
    spdlog::debug("Found compound statement at line {}.", context.getSourceManager().getSpellingLineNumber(curr_stmt->getBeginLoc()));
    static std::string print_stmt = "std::cout << \"hello\" << std::endl;";
    rewriter.InsertTextAfter(curr_stmt->getEndLoc(), print_stmt);
  }
}
