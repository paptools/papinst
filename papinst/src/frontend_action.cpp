#include "papinst/frontend_action.h"

// Local headers.
#include "papinst/ast_consumer.h"
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

namespace papinst {
namespace {
// #include <llvm/ADT/SmallString.h>
// class DiagnosticConsumer : public clang::DiagnosticConsumer {
// public:
//   void HandleDiagnostic(clang::DiagnosticsEngine::Level level,
//                         const clang::Diagnostic &info) override {
//     if (error_.empty() && level >= clang::DiagnosticsEngine::Error) {
//       llvm::SmallString<100> data;
//       info.FormatDiagnostic(data);
//       error_ = data.str().str();
//       ++NumErrors;
//     }
//   }
//
// private:
//   std::string error_;
// };
} // namespace

FrontendAction::FrontendAction(std::shared_ptr<Logger> logger,
                               std::vector<std::string> &streams,
                               std::shared_ptr<Instrumenter> instrumenter)
    : logger_(logger), streams_(streams), instrumenter_(instrumenter) {}

std::unique_ptr<clang::ASTConsumer>
FrontendAction::CreateASTConsumer(clang::CompilerInstance &compiler,
                                  llvm::StringRef inFile) {
  return std::make_unique<papinst::ASTConsumer>(
      logger_, compiler.getASTContext(), streams_, instrumenter_);
}

bool FrontendAction::BeginInvocation(clang::CompilerInstance &compiler) {
  // set the diagnostic consumer
  // compiler.getDiagnostics().setClient(new DiagnosticConsumer(),
  //                                    /*ShouldOwnClient=*/true);
  return true;
}
} // namespace papinst
