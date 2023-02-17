#ifndef PATHINST_FUNCTIONCALL_TRANSFORMER_H
#define PATHINST_FUNCTIONCALL_TRANSFORMER_H

#include "pathinst/transformer.h"

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <llvm/Support/raw_ostream.h>

namespace clang {
class ASTContext;
// class raw_ostream;
class Rewriter;
} // namespace clang

class FunctionCallTransformer : public Transformer {
public:
  explicit FunctionCallTransformer(clang::ASTContext &context,
                                   clang::Rewriter &rewriter);

  virtual void start(void) override;
  virtual void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;
};

#endif // PATHINST_FUNCTIONCALL_TRANSFORMER_H
