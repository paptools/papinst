#ifndef PAPINST_MATCH_CALLBACK_H
#define PAPINST_MATCH_CALLBACK_H

// Local headers.
#include "papinst/instrumenter.h"

// Third-party headers.
#include <clang/AST/ASTContext.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Analysis/CFG.h>
#include <clang/Rewrite/Core/Rewriter.h>

// C++ standard library headers.
#include <memory>

namespace papinst {
class MatchCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  MatchCallback(clang::ASTContext &context, clang::Rewriter &rewriter,
                std::shared_ptr<Instrumenter> instrumenter);
  void start(void);

  void HandleCFGBlock(clang::CFGBlock *block, bool is_main);

  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;

private:
  clang::ASTContext &context_;
  clang::Rewriter &rewriter_;
  std::shared_ptr<Instrumenter> instrumenter_;
  clang::CFG::BuildOptions options_;
};
} // namespace papinst

#endif // PAPINST_MATCH_CALLBACK_H
