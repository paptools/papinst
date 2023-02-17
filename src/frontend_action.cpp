#include "pathinst/frontend_action.h"
#include "pathinst/consumer.h"

#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/CompilerInstance.h>
#include <llvm/ADT/StringRef.h>

#include <memory>

std::unique_ptr<clang::ASTConsumer>
XFrontendAction::CreateASTConsumer(clang::CompilerInstance &compiler,
                                   llvm::StringRef inFile) {
  return std::unique_ptr<clang::ASTConsumer>(
      new XConsumer(compiler.getASTContext()));
}
