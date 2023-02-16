#ifndef PATHINST_TRANSFORMER_H
#define PATHINST_TRANSFORMER_H

#include <clang/ASTMatchers/ASTMatchFinder.h>

namespace clang {
class ASTContext;
// class raw_ostream;
class Rewriter;
} // namespace clang

class Transformer : public clang::ast_matchers::MatchFinder::MatchCallback {
protected:
  clang::ASTContext &context;
  clang::Rewriter &rewriter;

public:
  explicit Transformer(clang::ASTContext &context, clang::Rewriter &rewriter);

  virtual void start() = 0;
  virtual void print(clang::raw_ostream &stream) = 0;
};

#endif // PATHINST_TRANSFORMER_H
