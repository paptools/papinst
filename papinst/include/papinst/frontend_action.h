#ifndef PAPINST_FRONTEND_ACTION_H
#define PAPINST_FRONTEND_ACTION_H

// Local headers.
#include "papinst/instrumenter.h"
#include "papinst/logger.h"

// Third-party headers.
#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>

// C++ standard library headers.
#include <memory>
#include <string>
#include <vector>

namespace papinst {
class FrontendAction : public clang::ASTFrontendAction {
public:
  FrontendAction(std::shared_ptr<Logger> logger,
                 std::vector<std::string> &streams,
                 std::shared_ptr<Instrumenter> instrumenter);
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &compiler,
                    llvm::StringRef inFile) override;

  virtual bool BeginInvocation(clang::CompilerInstance &compiler) override;

private:
  std::shared_ptr<Logger> logger_;
  std::vector<std::string> &streams_;
  std::shared_ptr<Instrumenter> instrumenter_;
};
} // namespace papinst

#endif // PAPINST_FRONTEND_ACTION_H
