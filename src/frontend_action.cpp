#include "pathinst/frontend_action.h"
#include "pathinst/instrumenter.h"
#include "pathinst/utils.h"

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace pathinst {
namespace {
std::map<int64_t, bool> s_inst_map;
std::string s_fn_sig;

class DiagnosticConsumer : public clang::DiagnosticConsumer {
public:
  void HandleDiagnostic(clang::DiagnosticsEngine::Level level,
                        const clang::Diagnostic &info) override {
    if (error_.empty() && level >= clang::DiagnosticsEngine::Error) {
      llvm::SmallString<100> data;
      info.FormatDiagnostic(data);
      error_ = data.str().str();
      ++NumErrors;
    }
  }

private:
  std::string error_;
};

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

class MatchCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  MatchCallback(clang::ASTContext &context, clang::Rewriter &rewriter,
                std::shared_ptr<Instrumenter> instrumenter)
      : context_(context), rewriter_(rewriter), instrumenter_(instrumenter),
        options_() {
    options_.AddImplicitDtors = true;
  }

  void start(void) {
    clang::ast_matchers::MatchFinder finder;

    // Add matcher for return statements.
    // finder.addMatcher(
    //    clang::ast_matchers::returnStmt().bind("returnStmt"),
    //    this);
    finder.addMatcher(
        clang::ast_matchers::functionDecl(clang::ast_matchers::anything())
            .bind("func"),
        this);

    finder.matchAST(context_);
  }

  void HandleCFGBlock(clang::CFGBlock *block, bool is_main) {
    if (!block)
      return;

    // if (block->Terminator.isValid()) return;

    // std::cout << "\nCFG BLOCK" << std::endl;
    // block->dump();

    auto &&first_element = block->begin();
    if (auto &&cfg_stmt = first_element->getAs<clang::CFGStmt>()) {
      auto &&stmt = cfg_stmt->getStmt();

      if (auto &&int_lit = llvm::dyn_cast<clang::IntegerLiteral>(stmt)) {
        // Use parent if this is an integer literal.
        stmt = context_.getParents(*stmt).begin()->get<clang::Stmt>();
      }
      // stmt->dumpColor();

      if (s_inst_map.find(stmt->getID(context_)) != s_inst_map.end()) {
        // std::cout << "ALREADY INSTRUMENTED" << std::endl;
        return;
      } else {
        // rewriter_.InsertTextBefore(stmt->getBeginLoc(),
        //                            fmt::format(s_new_path_template,
        //                            s_fn_sig));
      }

      // std::cout << "PARENT" << std::endl;
      // auto&& parent =
      // context_.getParents(*stmt->getStmt()).begin()->get<clang::Stmt>();
      // parent->dumpColor();

      // rewriter_.InsertTextBefore(parent->getBeginLoc(), "/*inst*/");

      // for (auto &&p : parent) {
      //   auto&& p_stmt = p.get<clang::Stmt>();
      //   std::cout << "PARENT IS A STMT" << std::endl;
      //   p_stmt->dumpColor();
      //   rewriter_.InsertTextAfter(p_stmt->getBeginLoc(), "/*block stmt*/");
      //   //rewriter_.InsertTextAfter(
      //   //    p_stmt->getBeginLoc(),
      //   //    fmt::format(s_print_stmt, p_stmt->getID(context_)));
      // }
    } else {
      // std::cout << "FIRST ELEMENT IS NOT A STMT" << std::endl;
    }
  }

  void HandleFnDecl(const clang::FunctionDecl *fn, clang::ASTContext *context) {
    if (auto &&body = fn->getBody()) {
      s_fn_sig = GetFunctionSignature(fn);
      // std::cout << "\nFunction: " << s_fn_sig << std::endl;
      // fn->dumpColor();

      assert(llvm::isa<clang::CompoundStmt>(body));
      auto &&compound_stmt = llvm::cast<clang::CompoundStmt>(body);

      // rewriter_.InsertText(compound_stmt->getLBracLoc(),
      //                     fmt::format(s_new_path_template, s_fn_sig),
      //                    false, true);
      rewriter_.InsertTextAfterToken(compound_stmt->getLBracLoc(),
                                     instrumenter_->GetFnCalleeInst(s_fn_sig));
    }

    //  body->dumpColor();

    //  auto &&first_child_it = body->children().begin();
    //  if (first_child_it != body->children().end()) {
    //    auto &&first_child = *first_child_it;
    //    std::cout << "FUNCTION DECL BODY -> FIRST CHILD" << std::endl;
    //    first_child->dumpColor();
    //    rewriter_.InsertText(first_child->getBeginLoc(),
    //                         fmt::format(s_new_path_template, s_fn_sig),
    //                         true, true);

    //    // rewriter_.InsertTextBefore(first_child->getBeginLoc(),
    //    //                            fmt::format(s_new_path_template,
    //    //                            s_fn_sig));
    //  } else {
    //    std::cout << "FUNCTION DECL BODY -> HAS NO FIRST CHILD" << std::endl;
    //  }
    //} else {
    //  assert(false && "FUNCTION DECL HAS NO BODY");
    //}

    //  first_child->dumpColor();
    //  rewriter_.InsertTextBefore(
    //      first_child->getBeginLoc(), fmt::format(s_new_path_template,
    //      s_fn_sig));
    //  s_inst_map[first_child->getID(context_)] = true;
    //} else {
    //  std::cout << "FN DECL BODY HAS NO CHILDREN" << std::endl;
    //}

    // std::cout << "WORKING WITH THE CFG" << std::endl;
    // if (std::unique_ptr<clang::CFG> cfg =
    //         clang::CFG::buildCFG(fn, body, context, options_)) {
    //   // clang::LangOptions LO;
    //   // cfg->dump(LO, /*ShowColors*/ true);

    //  for (auto &&block : cfg->reverse_nodes()) {
    //    HandleCFGBlock(block, fn->isMain());
    //  }
    //}

    // std::cout << "DONE HANDLING FN DECL" << std::endl;
  }

  virtual void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
#if 0
    if (auto &&stmt = result.Nodes.getNodeAs<clang::ReturnStmt>("returnStmt")) {
      if (context_.getSourceManager().isInSystemHeader(stmt->getBeginLoc())) {
        return;
      }
      //rewriter_.InsertTextBefore(
      //    stmt->getBeginLoc(), fmt::format(s_print_stmt, stmt->getID(context_)));
      rewriter_.InsertTextBefore(stmt->getBeginLoc(), "/*block stmt*/");
      return;
    }

#else
    if (auto &&fn = result.Nodes.getNodeAs<clang::FunctionDecl>("func")) {
      if (context_.getSourceManager().isInSystemHeader(fn->getBeginLoc())) {
        return;
      }
      HandleFnDecl(fn, result.Context);
    }
#endif
  }

private:
  clang::ASTContext &context_;
  clang::Rewriter &rewriter_;
  std::shared_ptr<Instrumenter> instrumenter_;
  clang::CFG::BuildOptions options_;
};

class ASTConsumer : public clang::ASTConsumer {
public:
  ASTConsumer(std::shared_ptr<spdlog::logger> logger,
              clang::ASTContext &context, std::vector<std::string> &streams,
              std::shared_ptr<Instrumenter> instrumenter)
      : logger_(logger),
        rewriter_(context.getSourceManager(), context.getLangOpts()),
        streams_(streams), instrumenter_(instrumenter) {}

  virtual void HandleTranslationUnit(clang::ASTContext &context) override {
    if (context.getDiagnostics().hasErrorOccurred()) {
      logger_->error("Failed to preprocess file.");
      return;
    }

    MatchCallback match_callback(context, rewriter_, instrumenter_);
    match_callback.start();

    auto &&source_manager = context.getSourceManager();
    auto file_id = source_manager.getMainFileID();
    auto file_entry = source_manager.getFileEntryForID(file_id);
    if (rewriter_.getRewriteBufferFor(file_id) == nullptr) {
      logger_->debug("No changes for file '{}'.",
                     std::string(file_entry->getName()));
      return;
    }

    auto &edit_buffer = rewriter_.getEditBuffer(file_id);
    edit_buffer.InsertTextAfter(/*OrigOffset*/ 0,
                                instrumenter_->GetPathCapIncludeInst());

    auto buffer = rewriter_.getRewriteBufferFor(file_id);
    streams_.emplace_back(std::string());
    llvm::raw_string_ostream os(streams_.back());
    buffer->write(os);
  }

private:
  std::shared_ptr<spdlog::logger> logger_;
  clang::Rewriter rewriter_;
  std::vector<std::string> &streams_;
  std::shared_ptr<Instrumenter> instrumenter_;
};
} // namespace

FrontendAction::FrontendAction(std::shared_ptr<spdlog::logger> logger,
                               std::vector<std::string> &streams,
                               std::shared_ptr<Instrumenter> instrumenter)
    : logger_(logger), streams_(streams), instrumenter_(instrumenter) {}

std::unique_ptr<clang::ASTConsumer>
FrontendAction::CreateASTConsumer(clang::CompilerInstance &compiler,
                                  llvm::StringRef inFile) {
  return std::make_unique<ASTConsumer>(logger_, compiler.getASTContext(),
                                       streams_, instrumenter_);
}

bool FrontendAction::BeginInvocation(clang::CompilerInstance &compiler) {
  // set the diagnostic consumer
  compiler.getDiagnostics().setClient(new DiagnosticConsumer(),
                                      /*ShouldOwnClient=*/true);
  return true;
}
} // namespace pathinst
