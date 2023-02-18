#ifndef PATHINST_FRONTEND_ACTION_H
#define PATHINST_FRONTEND_ACTION_H

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
  explicit FrontendAction(std::shared_ptr<spdlog::logger> logger,
                          std::vector<std::string> &streams);
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &compiler,
                    llvm::StringRef inFile) override;

private:
  std::shared_ptr<spdlog::logger> logger_;
  std::vector<std::string> &streams_;
};
} // namespace pathinst

#endif // PATHINST_FRONTEND_ACTION_H
