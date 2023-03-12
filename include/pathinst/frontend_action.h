#ifndef PATHINST_FRONTEND_ACTION_H
#define PATHINST_FRONTEND_ACTION_H

#include "pathinst/instrumenter.h"

#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <vector>

namespace pathinst {
class FrontendAction : public clang::ASTFrontendAction {
public:
  FrontendAction(std::shared_ptr<spdlog::logger> logger,
                 std::vector<std::string> &streams,
                 std::shared_ptr<Instrumenter> instrumenter);
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &compiler,
                    llvm::StringRef inFile) override;

private:
  std::shared_ptr<spdlog::logger> logger_;
  std::vector<std::string> &streams_;
  std::shared_ptr<Instrumenter> instrumenter_;
};
} // namespace pathinst

#endif // PATHINST_FRONTEND_ACTION_H
