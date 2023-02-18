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

namespace pathinst {
namespace {
class MatchCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  explicit MatchCallback(clang::ASTContext &context, clang::Rewriter &rewriter)
      : context(context), rewriter(rewriter) {}

  void start(void) {
    clang::ast_matchers::MatchFinder function_finder;

    // Add matcher for selection statements.
    auto selection_statement_matcher = clang::ast_matchers::compoundStmt(
        clang::ast_matchers::hasParent(clang::ast_matchers::ifStmt()));
    function_finder.addMatcher(selection_statement_matcher, this);

    function_finder.matchAST(context);
  }

  virtual void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
    if (const clang::CompoundStmt *curr_stmt =
            result.Nodes.getNodeAs<clang::CompoundStmt>("comoundStmt")) {
      spdlog::debug("Found compound statement at line {}.",
                    context.getSourceManager().getSpellingLineNumber(
                        curr_stmt->getBeginLoc()));
      static std::string print_stmt = "std::cout << \"hello\" << std::endl;";
      rewriter.InsertTextAfter(curr_stmt->getEndLoc(), print_stmt);
    }
  }

private:
  clang::ASTContext &context;
  clang::Rewriter &rewriter;
};

class Consumer : public clang::ASTConsumer {
public:
  explicit Consumer(clang::ASTContext &context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &context) override {
    rewriter.setSourceMgr(context.getSourceManager(), context.getLangOpts());

    MatchCallback match_callback(context, rewriter);
    match_callback.start();

    auto file_id = context.getSourceManager().getMainFileID();
    auto buffer = rewriter.getRewriteBufferFor(file_id);
    auto file_entry = context.getSourceManager().getFileEntryForID(file_id);
    if (buffer != nullptr) {
      auto &edit_buffer = rewriter.getEditBuffer(file_id);
      edit_buffer.InsertTextAfter(/*OrigOffset*/ 0, "#include <iostream>\n");
      buffer = rewriter.getRewriteBufferFor(file_id);
      if (pathinst::utils::GetDryRun()) {
        buffer->write(llvm::outs());
      } else {
        spdlog::debug("Writing to file '{}'.",
                      std::string(file_entry->getName()));
        std::error_code ec;
        llvm::raw_fd_ostream ofs(file_entry->getName(), ec);
        if (ec) {
          spdlog::error("Error opening file '{}'", ec.message());
          return;
        }
        buffer->write(ofs);
        if (ofs.has_error()) {
          spdlog::error("Error writing to file '{}'", ec.message());
        }
      }
    } else {
      spdlog::debug("Nothing to write for file '{}'.",
                    std::string(file_entry->getName()));
    }
  }

private:
  clang::Rewriter rewriter;
};

} // namespace

std::unique_ptr<clang::ASTConsumer>
FrontendAction::CreateASTConsumer(clang::CompilerInstance &compiler,
                                  llvm::StringRef inFile) {
  return std::unique_ptr<clang::ASTConsumer>(
      new Consumer(compiler.getASTContext()));
}
} // namespace pathinst
