#include "papinst/ast_consumer_listener.h"

// Local headers.
#include "papinst/ast_visitor.h"
#include "papinst/instrumenter.h"
#include "papinst/logger.h"
#include "papinst/match_callback.h"

// Third-party headers.
#include <clang/AST/ASTContext.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <fmt/format.h>
#include <llvm/Support/raw_ostream.h>

// C++ standard library headers.
#include <memory>
#include <string>
#include <vector>

namespace papinst {
namespace {
class DefaultASTConsumerListener : public ASTConsumerListener {
public:
  DefaultASTConsumerListener(std::shared_ptr<Logger> logger,
                             std::vector<std::string> &streams,
                             std::shared_ptr<Instrumenter> instrumenter,
                             std::shared_ptr<ASTVisitor> visitor)
      : logger_(logger), streams_(streams), instrumenter_(instrumenter),
        visitor_(visitor) {}

  virtual ~DefaultASTConsumerListener() = default;

  void ProcessTranslationUnit(clang::ASTContext &context) override {
    if (context.getDiagnostics().hasErrorOccurred()) {
      logger_->Error("Failed to preprocess file.");
      return;
    }

    logger_->Debug("Processing translation unit.");
    clang::Rewriter rewriter(context.getSourceManager(), context.getLangOpts());
    visitor_->SetRewriter(rewriter);
    visitor_->TraverseAST(context);

    // visitor_->TraverseDecl(context.getTranslationUnitDecl());

    // MatchCallback match_callback(context, rewriter, instrumenter_);
    // match_callback.start();

    auto &&source_manager = context.getSourceManager();
    auto file_id = source_manager.getMainFileID();
    auto file_entry = source_manager.getFileEntryForID(file_id);
    if (rewriter.getRewriteBufferFor(file_id) == nullptr) {
      logger_->Debug(
          fmt::format("No changes for file '{}'.", file_entry->getName()));
      // std::string_view(file_entry->getName())));
      return;
    }

    auto &edit_buffer = rewriter.getEditBuffer(file_id);
    edit_buffer.InsertTextAfter(/*OrigOffset*/ 0,
                                instrumenter_->GetTraceIncludeInst());

    auto buffer = rewriter.getRewriteBufferFor(file_id);
    streams_.emplace_back(std::string());
    llvm::raw_string_ostream os(streams_.back());
    buffer->write(os);
  }

  void ProcessError(const std::string &message) override {
    logger_->Error(message);
  }

private:
  std::shared_ptr<Logger> logger_;
  std::vector<std::string> &streams_;
  std::shared_ptr<Instrumenter> instrumenter_;
  std::shared_ptr<ASTVisitor> visitor_;
};
} // namespace

std::shared_ptr<ASTConsumerListener>
ASTConsumerListener::Create(std::shared_ptr<Logger> logger,
                            std::vector<std::string> &streams,
                            std::shared_ptr<Instrumenter> instrumenter,
                            std::shared_ptr<ASTVisitor> visitor) {
  return std::make_shared<DefaultASTConsumerListener>(logger, streams,
                                                      instrumenter, visitor);
}
} // namespace papinst
