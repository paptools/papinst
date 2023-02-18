#ifndef PATHINST_FRONTEND_ACTION_H
#define PATHINST_FRONTEND_ACTION_H

#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <llvm/ADT/StringRef.h>

#include <memory>

namespace pathinst {
class FrontendAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &compiler,
                    llvm::StringRef inFile) override;
};
} // namespace pathinst

#endif // PATHINST_FRONTEND_ACTION_H
