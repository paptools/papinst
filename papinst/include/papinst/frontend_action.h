#ifndef PAPINST_FRONTEND_ACTION_H
#define PAPINST_FRONTEND_ACTION_H

// Local headers.
#include "papinst/ast_consumer_listener.h"
#include "papinst/instrumenter.h"
#include "papinst/logger.h"

// Third-party headers.
#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <llvm/ADT/StringRef.h>

// C++ standard library headers.
#include <memory>
#include <string>
#include <vector>

namespace papinst {
class FrontendAction : public clang::ASTFrontendAction {
public:
  // Factory method.
  static std::unique_ptr<FrontendAction>
  Create(std::shared_ptr<ASTConsumerListener> ast_consumer_listener);

  // Virtual destructor.
  virtual ~FrontendAction() = default;

  // Returns a pointer to the ASTConsumer instance.
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &compiler,
                    llvm::StringRef inFile) = 0;
};
} // namespace papinst

#endif // PAPINST_FRONTEND_ACTION_H
