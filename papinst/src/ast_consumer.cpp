#include "papinst/ast_consumer.h"

// Local headers.
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
ASTConsumer::ASTConsumer(std::shared_ptr<Logger> logger,
                         clang::ASTContext &context,
                         std::vector<std::string> &streams,
                         std::shared_ptr<Instrumenter> instrumenter)
    : logger_(logger),
      rewriter_(context.getSourceManager(), context.getLangOpts()),
      streams_(streams), instrumenter_(instrumenter) {}

void ASTConsumer::HandleTranslationUnit(clang::ASTContext &context) {
  if (context.getDiagnostics().hasErrorOccurred()) {
    logger_->Error("Failed to preprocess file.");
    return;
  }

  MatchCallback match_callback(context, rewriter_, instrumenter_);
  match_callback.start();

  auto &&source_manager = context.getSourceManager();
  auto file_id = source_manager.getMainFileID();
  auto file_entry = source_manager.getFileEntryForID(file_id);
  if (rewriter_.getRewriteBufferFor(file_id) == nullptr) {
    logger_->Debug(
        fmt::format("No changes for file '{}'.", file_entry->getName()));
    // std::string_view(file_entry->getName())));
    return;
  }

  auto &edit_buffer = rewriter_.getEditBuffer(file_id);
  edit_buffer.InsertTextAfter(/*OrigOffset*/ 0,
                              instrumenter_->GetTraceIncludeInst());

  auto buffer = rewriter_.getRewriteBufferFor(file_id);
  streams_.emplace_back(std::string());
  llvm::raw_string_ostream os(streams_.back());
  buffer->write(os);
}
} // namespace papinst
