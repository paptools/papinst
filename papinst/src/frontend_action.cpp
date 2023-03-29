#include "papinst/frontend_action.h"

// Local headers.
#include "papinst/instrumenter.h"
#include "papinst/utils.h"

// Third-party headers.
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

// C++ standard library headers.
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace papinst {
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

  void HandleCFGBlock(clang::CFGBlock *block, bool is_main) {
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

  virtual void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
    if (auto &&fn = result.Nodes.getNodeAs<clang::FunctionDecl>("FnDef")) {
      if (context_.getSourceManager().isInSystemHeader(fn->getBeginLoc())) {
        return;
      }

      if (auto &&body = fn->getBody()) {
        s_fn_sig = GetFunctionSignature(fn);
        assert(llvm::isa<clang::CompoundStmt>(body));
        auto &&compound_stmt = llvm::cast<clang::CompoundStmt>(body);

        rewriter_.InsertTextAfterToken(
            compound_stmt->getLBracLoc(),
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
                                instrumenter_->GetTraceIncludeInst());

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
  // compiler.getDiagnostics().setClient(new DiagnosticConsumer(),
  //                                    /*ShouldOwnClient=*/true);
  return true;
}
} // namespace papinst
