#ifndef PAPINST_AST_CONSUMER_H
#define PAPINST_AST_CONSUMER_H

// Local headers.
#include "papinst/instrumenter.h"
#include "papinst/logger.h"

// Third-party headers.
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/Rewrite/Core/Rewriter.h>

// C++ standard library headers.
#include <memory>
#include <string>
#include <vector>

namespace papinst {
class ASTConsumer : public clang::ASTConsumer {
public:
  ASTConsumer(std::shared_ptr<Logger> logger, clang::ASTContext &context,
              std::vector<std::string> &streams,
              std::shared_ptr<Instrumenter> instrumenter);

  void HandleTranslationUnit(clang::ASTContext &context) override;

private:
  std::shared_ptr<Logger> logger_;
  clang::Rewriter rewriter_;
  std::vector<std::string> &streams_;
  std::shared_ptr<Instrumenter> instrumenter_;
};
} // namespace papinst

#endif // PAPINST_AST_CONSUMER_H
