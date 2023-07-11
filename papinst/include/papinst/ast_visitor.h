#ifndef PAPINST_AST_VISITOR_H
#define PAPINST_AST_VISITOR_H

// Local headers.
#include "papinst/instrumenter.h"

// Third-party headers.
#include <clang/AST/ASTContext.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Tooling/Core/Replacement.h>

// C++ standard library headers.
#include <memory>

namespace papinst {
class ASTVisitorListener {
public:
  // Factory method.
  static std::shared_ptr<ASTVisitorListener>
  Create(std::shared_ptr<Instrumenter> instrumenter);

  // Virtual destructor.
  virtual ~ASTVisitorListener() = default;

  virtual void Initialize(clang::ASTContext &context) = 0;
  virtual void ProcessFnDef(clang::FunctionDecl *decl) = 0;
  virtual void ProcessIfStmt(clang::IfStmt *stmt) = 0;
  virtual void ProcessSwitchStmt(clang::SwitchStmt *stmt) = 0;
  virtual void ProcessWhileStmt(clang::WhileStmt *stmt) = 0;
  virtual void ProcessForStmt(clang::ForStmt *stmt) = 0;
  virtual void ProcessDoStmt(clang::DoStmt *stmt) = 0;
  // virtual void ProcessCXXForRangeStmt(clang::SwitchStmt *stmt) = 0;
  // virtual void ProcessBreakStmt(clang::BreakStmt *stmt) = 0;
  // virtual void ProcessContinueStmt(clang::ContinueStmt *stmt) = 0;
  virtual void ProcessReturnStmt(clang::ReturnStmt *stmt) = 0;
  virtual void ProcessCallExpr(clang::CallExpr *expr) = 0;
  virtual void ProcessCXXThrowExpr(clang::CXXThrowExpr *expr) = 0;
  // virtual void ProcessCXXTryStmt(clang::SwitchStmt *stmt) = 0;
  // virtual void ProcessCXXCatchStmt(clang::SwitchStmt *stmt) = 0;
  // virtual void ProcessGotoStmt(clang::SwitchStmt *stmt) = 0;
  // virtual void ProcessLabelDecl(clang::LabelDecl *decl) = 0;
  virtual void ProcessBinaryOperator(clang::BinaryOperator *op) = 0;
  virtual void ProcessUnaryOperator(clang::UnaryOperator *op) = 0;
};

clang::tooling::Replacements &GetReplacements();

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
