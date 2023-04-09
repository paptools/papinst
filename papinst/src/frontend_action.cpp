#include "papinst/frontend_action.h"

// Local headers.
#include "papinst/ast_consumer_listener.h"
#include "papinst/instrumenter.h"
#include "papinst/logger.h"
#include "papinst/utils.h"

// Third-party headers.
#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/CompilerInstance.h>
#include <llvm/ADT/StringRef.h>

// C++ standard library headers.
#include <memory>
#include <string>
#include <vector>

#include <clang/AST/ASTContext.h>

namespace papinst {
namespace {
class DefaultASTConsumer : public clang::ASTConsumer {
public:
  DefaultASTConsumer(std::shared_ptr<ASTConsumerListener> listener)
      : listener_(listener) {}

  virtual ~DefaultASTConsumer() = default;

  void HandleTranslationUnit(clang::ASTContext &context) override {
    if (listener_) {
      if (context.getDiagnostics().hasErrorOccurred()) {
        listener_->ProcessError("Error occurred during translation unit.");
      } else {
        listener_->ProcessTranslationUnit(context);
      }
    }
  }

private:
  std::shared_ptr<ASTConsumerListener> listener_;
};

class DefaultFrontendAction : public FrontendAction {
public:
  DefaultFrontendAction(
      std::shared_ptr<ASTConsumerListener> ast_consumer_listener)
      : ast_consumer_listener_(ast_consumer_listener) {}

  virtual ~DefaultFrontendAction() = default;

  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &compiler,
                    llvm::StringRef inFile) override {
    auto consumer =
        std::make_unique<DefaultASTConsumer>(ast_consumer_listener_);
    return std::move(consumer);
  }

private:
  std::shared_ptr<ASTConsumerListener> ast_consumer_listener_;
};
} // namespace

std::unique_ptr<FrontendAction> FrontendAction::Create(
    std::shared_ptr<ASTConsumerListener> ast_consumer_listener) {
  return std::make_unique<DefaultFrontendAction>(ast_consumer_listener);
}
} // namespace papinst
