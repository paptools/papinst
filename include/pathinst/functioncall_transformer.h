#ifndef PATHINST_FUNCTIONCALL_TRANSFORMER_H
#define PATHINST_FUNCTIONCALL_TRANSFORMER_H

#include "pathinst/transformer.h"

#include <set>
#include <string>

namespace clang {
class ASTContext;
// class raw_ostream;
class Rewriter;
} // namespace clang

class FunctionCallTransformer : public Transformer {
private:
  std::set<std::string> functions;

public:
  explicit FunctionCallTransformer(clang::ASTContext &context,
                                   clang::Rewriter &rewriter);

  virtual void start() override;
  virtual void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;
  virtual void print(clang::raw_ostream &stream) override;
};

#endif // PATHINST_FUNCTIONCALL_TRANSFORMER_H
