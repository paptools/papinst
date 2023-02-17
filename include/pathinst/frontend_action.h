#ifndef PATHINST_FRONTEND_ACTION_H
#define PATHINST_FRONTEND_ACTION_H

#include <clang/Frontend/FrontendActions.h>
#include <llvm/ADT/StringRef.h>
#include <clang/AST/ASTConsumer.h>

#include <memory>

namespace clang {
class CompilerInstance;
}

class XFrontendAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &compiler,
                    llvm::StringRef inFile) override;
};

#endif // PATHINST_FRONTEND_ACTION_H
