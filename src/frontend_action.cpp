#include "pathinst/frontend_action.h"
#include "pathinst/utils.h"

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <system_error>
#include <vector>

namespace pathinst {
namespace {
class MatchCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  MatchCallback(clang::ASTContext &context, clang::Rewriter &rewriter)
      : context(context), rewriter(rewriter) {}

  void start(void) {
    clang::ast_matchers::MatchFinder function_finder;

    // Add matcher for return statements.
    auto return_stmt_matcher =
        clang::ast_matchers::returnStmt().bind("returnStmt");
    function_finder.addMatcher(return_stmt_matcher, this);

    function_finder.matchAST(context);
  }

  virtual void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
    static const std::string print_stmt =
        "std::cout << __FILE__ << ':' << __LINE__ << std::endl;";

    if (auto &&curr_stmt =
            result.Nodes.getNodeAs<clang::ReturnStmt>("returnStmt")) {
      if (context.getSourceManager().isInSystemHeader(
              curr_stmt->getBeginLoc())) {
        return;
      }
      spdlog::info("Found return statement at line {}.",
                   context.getSourceManager().getSpellingLineNumber(
                       curr_stmt->getBeginLoc()));
      rewriter.InsertTextBefore(curr_stmt->getBeginLoc(), print_stmt);
    }
  }

private:
  clang::ASTContext &context;
  clang::Rewriter &rewriter;
};

class ASTConsumer : public clang::ASTConsumer {
public:
  ASTConsumer(clang::ASTContext &context, std::vector<std::string> &streams)
      : rewriter(context.getSourceManager(), context.getLangOpts()),
        streams_(streams) {}

  virtual void HandleTranslationUnit(clang::ASTContext &context) override {
    MatchCallback match_callback(context, rewriter);
    match_callback.start();

    auto &&source_manager = context.getSourceManager();
    auto file_id = source_manager.getMainFileID();
    auto file_entry = source_manager.getFileEntryForID(file_id);
    if (rewriter.getRewriteBufferFor(file_id) == nullptr) {
      spdlog::debug("No changes for file '{}'.",
                    std::string(file_entry->getName()));
      return;
    }

    auto &edit_buffer = rewriter.getEditBuffer(file_id);
    edit_buffer.InsertTextAfter(/*OrigOffset*/ 0, "#include <iostream>\n");

    auto buffer = rewriter.getRewriteBufferFor(file_id);
    streams_.emplace_back(std::string());
    llvm::raw_string_ostream os(streams_.back());
    buffer->write(os);
  }

private:
  clang::Rewriter rewriter;
  std::vector<std::string> &streams_;
};
} // namespace

FrontendAction::FrontendAction(std::vector<std::string> &streams)
    : streams_(streams) {}

std::unique_ptr<clang::ASTConsumer>
FrontendAction::CreateASTConsumer(clang::CompilerInstance &compiler,
                                  llvm::StringRef inFile) {
  return std::make_unique<ASTConsumer>(compiler.getASTContext(), streams_);
}
} // namespace pathinst
