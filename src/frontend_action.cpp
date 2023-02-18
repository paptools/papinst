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
      : context_(context), rewriter_(rewriter) {}

  void start(void) {
    clang::ast_matchers::MatchFinder function_finder;

    // Add matcher for return statements.
    auto return_stmt_matcher =
        clang::ast_matchers::returnStmt().bind("returnStmt");
    function_finder.addMatcher(return_stmt_matcher, this);

    function_finder.matchAST(context_);
  }

  virtual void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
    static const std::string print_stmt =
        "std::cout << __FILE__ << ':' << __LINE__ << std::endl;";

    if (auto &&curr_stmt =
            result.Nodes.getNodeAs<clang::ReturnStmt>("returnStmt")) {
      if (context_.getSourceManager().isInSystemHeader(
              curr_stmt->getBeginLoc())) {
        return;
      }
      rewriter_.InsertTextBefore(curr_stmt->getBeginLoc(), print_stmt);
    }
  }

private:
  clang::ASTContext &context_;
  clang::Rewriter &rewriter_;
};

class ASTConsumer : public clang::ASTConsumer {
public:
  ASTConsumer(std::shared_ptr<spdlog::logger> logger,
              clang::ASTContext &context, std::vector<std::string> &streams)
      : logger_(logger),
        rewriter_(context.getSourceManager(), context.getLangOpts()),
        streams_(streams) {}

  virtual void HandleTranslationUnit(clang::ASTContext &context) override {
    MatchCallback match_callback(context, rewriter_);
    match_callback.start();

    auto &&source_manager = context.getSourceManager();
    auto file_id = source_manager.getMainFileID();
    auto file_entry = source_manager.getFileEntryForID(file_id);
    if (rewriter_.getRewriteBufferFor(file_id) == nullptr) {
      logger_->debug("No changes for file '{}'.",
                     std::string(file_entry->getName()));
      return;
    }

    auto &edit_buffer = rewriter_.getEditBuffer(file_id);
    edit_buffer.InsertTextAfter(/*OrigOffset*/ 0, "#include <iostream>\n");

    auto buffer = rewriter_.getRewriteBufferFor(file_id);
    streams_.emplace_back(std::string());
    llvm::raw_string_ostream os(streams_.back());
    buffer->write(os);
  }

private:
  std::shared_ptr<spdlog::logger> logger_;
  clang::Rewriter rewriter_;
  std::vector<std::string> &streams_;
};
} // namespace

FrontendAction::FrontendAction(std::shared_ptr<spdlog::logger> logger,
                               std::vector<std::string> &streams)
    : logger_(logger), streams_(streams) {}

std::unique_ptr<clang::ASTConsumer>
FrontendAction::CreateASTConsumer(clang::CompilerInstance &compiler,
                                  llvm::StringRef inFile) {
  return std::make_unique<ASTConsumer>(logger_, compiler.getASTContext(),
                                       streams_);
}
} // namespace pathinst
