#ifndef PAPINST_AST_VISITOR_H
#define PAPINST_AST_VISITOR_H

// Third-party headers.
#include <clang/AST/RecursiveASTVisitor.h>

// C++ standard library headers.
#include <memory>

namespace papinst {
class ASTVisitorListener {
public:
  virtual ~ASTVisitorListener() = default;
  virtual void ProcessFnDef(clang::FunctionDecl *decl) = 0;
  virtual void ProcessIfStmt(clang::IfStmt *stmt) = 0;
  virtual void ProcessSwitchStmt(clang::SwitchStmt *stmt) = 0;
  virtual void ProcessWhileStmt(clang::WhileStmt *stmt) = 0;
  virtual void ProcessForStmt(clang::ForStmt *stmt) = 0;
  virtual void ProcessDoStmt(clang::DoStmt *stmt) = 0;
  // virtual void ProcessGotoStmt(clang::SwitchStmt *stmt) = 0;
  // virtual void ProcessCXXTryStmt(clang::SwitchStmt *stmt) = 0;
  // virtual void ProcessCXXForRangeStmt(clang::SwitchStmt *stmt) = 0;
  // virtual void ProcessCXXCatchStmt(clang::SwitchStmt *stmt) = 0;
};

class ASTVisitor : public clang::RecursiveASTVisitor<ASTVisitor> {
public:
  // Factory method.
  static std::shared_ptr<ASTVisitor>
  Create(std::shared_ptr<ASTVisitorListener> listener);

  // Virtual destructor.
  virtual ~ASTVisitor() = default;

  virtual bool TraverseAST(clang::ASTContext &context) = 0;
  virtual bool VisitFunctionDecl(clang::FunctionDecl *decl) = 0;
  virtual bool VisitStmt(clang::Stmt *stmt) = 0;
};
} // namespace papinst

#endif // PAPINST_AST_VISITOR_H
