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

  for (auto it = context.getSourceManager().fileinfo_begin();
       it != context.getSourceManager().fileinfo_end(); ++it) {
    auto file_entry = it->first;
    spdlog::debug("Entry for file {}", std::string(file_entry->getName()));
    auto buffer = rewriter.getRewriteBufferFor(
        context.getSourceManager().translateFile(file_entry));
    if (buffer != nullptr) {
      if (pathinst::utils::GetDryRun()) {
        buffer->write(llvm::outs());
      } else {
        std::error_code ec;
        llvm::raw_fd_ostream ofs(file_entry->getName(), ec);
        if (ec) {
          spdlog::error("Error opening file: {}", ec.message());
          continue;
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
}
