#include "pathinst/parser.h"
#include "pathinst/utils.h"

#include <boost/filesystem.hpp>
#include <boost/process/search_path.hpp>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>
#include <spdlog/spdlog.h>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <system_error>
#include <vector>

namespace pathinst {
namespace {
std::map<std::string, int> s_source_file_pos;
std::set<std::string> s_unsupported_flags = {"-g"};

class MatchCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  explicit MatchCallback(clang::ASTContext &context, clang::Rewriter &rewriter)
      : context(context), rewriter(rewriter) {}

  void start(void) {
    clang::ast_matchers::MatchFinder function_finder;

    // Add matcher for selection statements.
    auto selection_statement_matcher = clang::ast_matchers::compoundStmt(
        clang::ast_matchers::hasParent(clang::ast_matchers::ifStmt()));
    function_finder.addMatcher(selection_statement_matcher, this);

    function_finder.matchAST(context);
  }

  virtual void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
    if (const clang::CompoundStmt *curr_stmt =
            result.Nodes.getNodeAs<clang::CompoundStmt>("comoundStmt")) {
      spdlog::debug("Found compound statement at line {}.",
                    context.getSourceManager().getSpellingLineNumber(
                        curr_stmt->getBeginLoc()));
      static std::string print_stmt = "std::cout << \"hello\" << std::endl;";
      rewriter.InsertTextAfter(curr_stmt->getEndLoc(), print_stmt);
    }
  }

private:
  clang::ASTContext &context;
  clang::Rewriter &rewriter;
};

class Consumer : public clang::ASTConsumer {
public:
  explicit Consumer(clang::ASTContext &context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &context) override {
    rewriter.setSourceMgr(context.getSourceManager(), context.getLangOpts());

    MatchCallback match_callback(context, rewriter);
    match_callback.start();

    auto file_id = context.getSourceManager().getMainFileID();
    auto buffer = rewriter.getRewriteBufferFor(file_id);
    auto file_entry = context.getSourceManager().getFileEntryForID(file_id);
    if (buffer != nullptr) {
      auto &edit_buffer = rewriter.getEditBuffer(file_id);
      edit_buffer.InsertTextAfter(/*OrigOffset*/ 0, "#include <iostream>\n");
      buffer = rewriter.getRewriteBufferFor(file_id);
      if (pathinst::utils::GetDryRun()) {
        buffer->write(llvm::outs());
      } else {
        spdlog::debug("Writing to file '{}'.",
                      std::string(file_entry->getName()));
        std::error_code ec;
        llvm::raw_fd_ostream ofs(file_entry->getName(), ec);
        if (ec) {
          spdlog::error("Error opening file '{}'", ec.message());
          return;
        }
        buffer->write(ofs);
        if (ofs.has_error()) {
          spdlog::error("Error writing to file '{}'", ec.message());
        }
      }
    } else {
      spdlog::debug("Nothing to write for file '{}'.",
                    std::string(file_entry->getName()));
    }
  }

private:
  clang::Rewriter rewriter;
};

class FrontendAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &compiler,
                    llvm::StringRef inFile) override {
    return std::unique_ptr<clang::ASTConsumer>(
        new Consumer(compiler.getASTContext()));
  }
};
} // namespace

Parser::Parser(bool dry_run) : dry_run_(dry_run) {}

std::vector<std::string>
Parser::ParseCompileCommand(std::vector<std::string> &command) {
  std::string command_str = pathinst::utils::ToString(command, ' ');
  spdlog::debug("Parsing command '{}'.", command_str);

  auto compiler = command[0];
  if (!utils::IsSupportedCompiler(compiler)) {
    spdlog::debug(
        "Executable '{}' is not a supported compiler. Skipping parse.",
        compiler);
    return {};
  }

  if (!boost::filesystem::exists(compiler)) {
    compiler = boost::process::search_path(compiler).string();
  }
  spdlog::debug("User compiler is '{}'.", compiler);

  // Separate out the source files from the compiler arguments.
  std::vector<std::string> parse_args;
  std::vector<std::string> source_files;
  for (int i = 1; i < command.size(); i++) {
    if (utils::IsSourceFile(command[i])) {
      source_files.push_back(command[i]);
      s_source_file_pos[command[i]] = i;
    } else if (s_unsupported_flags.find(command[i]) ==
               s_unsupported_flags.end()) {
      parse_args.push_back(command[i]);
    }
  }

  if (source_files.empty()) {
    spdlog::debug("No source files found in command '{}'. Skipping parse.",
                  command_str);
    return {};
  }

  utils::SetDryRun(dry_run_);

  std::vector<std::string> inst_filepaths;
  for (auto &source_file : source_files) {
    spdlog::debug("Parsing file '{}' with args '{}'.", source_file,
                  utils::ToString(parse_args, ' '));

    auto source_code = utils::GetFileContents(source_file);
    auto pch_container = std::make_shared<clang::PCHContainerOperations>();
    auto inst_filepath = utils::CreateInstFile(source_file);
    inst_filepaths.push_back(inst_filepath);
    command[s_source_file_pos[source_file]] = inst_filepath;

    bool success = clang::tooling::runToolOnCodeWithArgs(
        std::make_unique<FrontendAction>(), source_code, parse_args,
        inst_filepath, compiler, pch_container);
    if (!success) {
      spdlog::error("Failed to parse file '{}'.", source_file);
    }
  }

  return inst_filepaths;
}

} // namespace pathinst
