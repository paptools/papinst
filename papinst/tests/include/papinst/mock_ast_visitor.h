#ifndef PAPINST_MOCK_AST_VISITOR_H
#define PAPINST_MOCK_AST_VISITOR_H

// Local headers.
#include "papinst/ast_visitor.h"

// Third-party headers.
#include <gmock/gmock.h>

namespace papinst {
class MockASTVisitorListener : public ASTVisitorListener {
public:
  MOCK_METHOD(void, Initialize, (clang::ASTContext & context), (override));
  MOCK_METHOD(void, ProcessFnDef, (clang::FunctionDecl * decl), (override));
  MOCK_METHOD(void, ProcessIfStmt, (clang::IfStmt * stmt), (override));
  MOCK_METHOD(void, ProcessSwitchStmt, (clang::SwitchStmt * stmt), (override));
  MOCK_METHOD(void, ProcessWhileStmt, (clang::WhileStmt * stmt), (override));
  MOCK_METHOD(void, ProcessForStmt, (clang::ForStmt * stmt), (override));
  MOCK_METHOD(void, ProcessDoStmt, (clang::DoStmt * stmt), (override));
  MOCK_METHOD(void, ProcessReturnStmt, (clang::ReturnStmt * stmt), (override));
  MOCK_METHOD(void, ProcessCallExpr, (clang::CallExpr * expr), (override));
  MOCK_METHOD(void, ProcessCXXThrowExpr, (clang::CXXThrowExpr * expr),
              (override));
};

class MockASTVisitor : public ASTVisitor {
public:
  MOCK_METHOD(bool, VisitStmt, (clang::Stmt * stmt), (override));
};
} // namespace papinst

#endif // PAPINST_MOCK_AST_VISITOR_H
