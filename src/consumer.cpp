#include "pathinst/consumer.h"
#include "pathinst/functioncall_transformer.h"
#include "pathinst/utils.h"

#include <clang/AST/ASTContext.h>
#include <llvm/Support/raw_ostream.h>
#include <spdlog/spdlog.h>

#include <string>
#include <system_error>

XConsumer::XConsumer(clang::ASTContext &context) {}

void XConsumer::HandleTranslationUnit(clang::ASTContext &context) {
  rewriter.setSourceMgr(context.getSourceManager(), context.getLangOpts());

  FunctionCallTransformer fntransformer(context, rewriter);
  fntransformer.start();
  fntransformer.print(llvm::outs());

  auto file_id = context.getSourceManager().getMainFileID();
  auto buffer = rewriter.getRewriteBufferFor(file_id);
  if (buffer != nullptr) {
    if (pathinst::utils::GetDryRun()) {
      buffer->write(llvm::outs());
    } else {
      auto file_entry = context.getSourceManager().getFileEntryForID(file_id);
      spdlog::debug("Writing to file '{}'.", std::string(file_entry->getName()));
      std::error_code ec;
      llvm::raw_fd_ostream ofs(file_entry->getName(), ec);
      if (ec) {
        spdlog::error("Error opening file: {}", ec.message());
        return;
      }
      buffer->write(ofs);
      if (ofs.has_error()) {
        spdlog::error("Error writing to file: {}", ec.message());
      }
    }
  } else {
    spdlog::error("Empty buffer.");
  }
}
