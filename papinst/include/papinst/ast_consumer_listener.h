#ifndef PAPINST_AST_CONSUMER_LISTENER_H
#define PAPINST_AST_CONSUMER_LISTENER_H

// Local headers.
#include "papinst/ast_visitor.h"
#include "papinst/instrumenter.h"
#include "papinst/logger.h"

// Third-party headers.
#include <clang/AST/ASTContext.h>

// C++ standard library headers.
#include <memory>
#include <string>
#include <vector>

namespace papinst {
class ASTConsumerListener {
public:
  // Factory method.
  static std::shared_ptr<ASTConsumerListener>
  Create(std::shared_ptr<Logger> logger, std::vector<std::string> &streams,
         std::shared_ptr<Instrumenter> instrumenter,
         std::shared_ptr<ASTVisitor> visitor);

  // Virtual destructor.
  virtual ~ASTConsumerListener() = default;

  // Process translation unit.
  virtual void ProcessTranslationUnit(clang::ASTContext &context) = 0;

  // Process error.
  virtual void ProcessError(const std::string &message) = 0;
};
} // namespace papinst

#endif // PAPINST_AST_CONSUMER_LISTENER_H
