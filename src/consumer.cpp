#include "pathinst/consumer.h"
#include "pathinst/functioncall_transformer.h"

#include <clang/AST/ASTContext.h>
#include <llvm/Support/raw_ostream.h>
#include <spdlog/spdlog.h>

#include <system_error>
#include <string>

XConsumer::XConsumer(clang::ASTContext &context) {}

void XConsumer::HandleTranslationUnit(clang::ASTContext &context) {
  rewriter.setSourceMgr(context.getSourceManager(), context.getLangOpts());

  FunctionCallTransformer fntransformer(context, rewriter);
  fntransformer.start();
  fntransformer.print(llvm::outs());

  for (auto it = context.getSourceManager().fileinfo_begin(); it != context.getSourceManager().fileinfo_end(); ++it) {
    auto file_entry = it->first;
    spdlog::debug("Entry for file {}", std::string(file_entry->getName()));
    auto buffer = rewriter.getRewriteBufferFor(
        context.getSourceManager().translateFile(file_entry));
    if (buffer != nullptr) {
      std::string out_file = std::string(file_entry->getName()).append(".out");
      std::error_code ec;
      llvm::raw_fd_ostream ofs(out_file, ec);
      if (ec) {
        spdlog::error("Error opening file: {}", ec.message());
        continue;
      }
      buffer->write(ofs);
      if (ofs.has_error()) {
        spdlog::error("Error writing to file: {}", ec.message());
      }
    } else {
      spdlog::error("Empty buffer.");
    }
  }
}
