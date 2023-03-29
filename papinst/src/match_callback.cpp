#include "papinst/match_callback.h"

// Local headers.
#include "papinst/instrumenter.h"

// Third-party headers.
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Analysis/CFG.h>
#include <clang/Rewrite/Core/Rewriter.h>

// C++ standard library headers.
#include <iostream> // TODO: Remove this once debugging is done.
#include <map>
#include <memory>
#include <sstream>
#include <string>

namespace papinst {
namespace {
std::map<int64_t, bool> s_inst_map;
std::string s_fn_sig;

// Returns the fully quality function signature.
std::string GetFunctionSignature(const clang::FunctionDecl *fn) {
  std::stringstream ss;
  ss << fn->getReturnType().getAsString() << " "
     << fn->getQualifiedNameAsString() << "(";
  bool first_param = true;
  for (auto &&param : fn->parameters()) {
    if (!first_param) {
      ss << ", ";
    }
    ss << param->getType().getAsString();
    first_param = false;
  }
  ss << ")";
  return ss.str();
}
} // namespace

MatchCallback::MatchCallback(clang::ASTContext &context,
                             clang::Rewriter &rewriter,
                             std::shared_ptr<Instrumenter> instrumenter)
    : context_(context), rewriter_(rewriter), instrumenter_(instrumenter),
      options_() {
  options_.AddImplicitDtors = true;
}

void MatchCallback::start(void) {
  clang::ast_matchers::MatchFinder finder;

  // auto &&return_stmt_matcher =
  //     clang::ast_matchers::returnStmt().bind("ReturnStmt");
  // finder.addMatcher(return_stmt_matcher, this);

  auto &&fn_def_matcher = clang::ast_matchers::functionDecl(
      clang::ast_matchers::decl().bind("FnDef"),
      clang::ast_matchers::isDefinition(),
      clang::ast_matchers::unless(clang::ast_matchers::isImplicit()));
  finder.addMatcher(fn_def_matcher, this);

  finder.matchAST(context_);
}

void MatchCallback::HandleCFGBlock(clang::CFGBlock *block, bool is_main) {
  if (!block)
    return;

  auto &&first_element = block->begin();
  if (auto &&cfg_stmt = first_element->getAs<clang::CFGStmt>()) {
    auto &&stmt = cfg_stmt->getStmt();
    // if (auto &&int_lit = llvm::dyn_cast<clang::IntegerLiteral>(stmt)) {
    //   // Use parent if this is an integer literal.
    //   stmt = context_.getParents(*stmt).begin()->get<clang::Stmt>();
    // }
    stmt->dumpColor();
    std::cout << "PARENT:" << std::endl;
    stmt = context_.getParents(*stmt).begin()->get<clang::Stmt>();
    stmt->dumpColor();

    // Return if stmt is if-stmt.
    if (llvm::isa<clang::IfStmt>(stmt)) {
      return;
    }

    if (s_inst_map.find(stmt->getID(context_)) == s_inst_map.end()) {
      rewriter_.InsertTextBefore(stmt->getBeginLoc(),
                                 instrumenter_->GetCfInst());
    } else {
      std::cerr << "ALREADY INSTRUMENTED" << std::endl;
    }
  }
}

void MatchCallback::run(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {
  if (auto &&fn = result.Nodes.getNodeAs<clang::FunctionDecl>("FnDef")) {
    if (context_.getSourceManager().isInSystemHeader(fn->getBeginLoc())) {
      return;
    }

    if (auto &&body = fn->getBody()) {
      s_fn_sig = GetFunctionSignature(fn);
      assert(llvm::isa<clang::CompoundStmt>(body));
      auto &&compound_stmt = llvm::cast<clang::CompoundStmt>(body);

      rewriter_.InsertTextAfterToken(compound_stmt->getLBracLoc(),
                                     instrumenter_->GetFnCalleeInst(s_fn_sig));

      if (std::unique_ptr<clang::CFG> cfg =
              clang::CFG::buildCFG(fn, body, &context_, options_)) {
        clang::LangOptions lang_opts;
        cfg->dump(lang_opts, /*ShowColors*/ true);

        for (auto &&block : cfg->reverse_nodes()) {
          HandleCFGBlock(block, fn->isMain());
        }
      }
    }
  }
}
} // namespace papinst
