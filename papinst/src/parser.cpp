#include "papinst/parser.h"

// Local headers.
#include "papinst/ast_consumer_listener.h"
#include "papinst/frontend_action.h"
#include "papinst/instrumenter.h"
#include "papinst/logger.h"
#include "papinst/utils.h"

// Third-party headers.
#include <boost/filesystem.hpp>
#include <boost/process/search_path.hpp>
#include <clang/Tooling/Tooling.h>
#include <fmt/format.h>
#include <llvm/Support/raw_ostream.h>

// C++ standard library headers.
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace papinst {
namespace {
std::map<std::string, int> s_source_file_pos;
std::set<std::string> s_unsupported_flags = {"-g"};
} // namespace

Parser::Parser(std::shared_ptr<Logger> logger, bool dry_run)
    : logger_(logger), dry_run_(dry_run) {}

std::vector<std::string>
Parser::ParseCompileCommand(std::vector<std::string> &command) {
  std::string command_str = papinst::utils::ToString(command, ' ');
  logger_->Debug(fmt::format("Parsing command '{}'.", command_str));

  auto compiler = command[0];
  if (!utils::IsSupportedCompiler(compiler)) {
    logger_->Debug(fmt::format(
        "Executable '{}' is not a supported compiler. Skipping parse.",
        compiler));
    return {};
  }

  if (!boost::filesystem::exists(compiler)) {
    compiler = boost::process::search_path(compiler).string();
  }
  logger_->Debug(fmt::format("User compiler is '{}'.", compiler));

  // Separate out the source files from the compiler arguments.
  // bool is_enabled = false;
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
  parse_args.push_back("-v"); // TODO: Enable only when verbose flag is set.

  if (source_files.empty()) {
    logger_->Debug(fmt::format(
        "No source files found in command '{}'. Skipping parse.", command_str));
    return {};
  }

  utils::SetDryRun(dry_run_);

  std::vector<std::string> streams;
  std::vector<std::string> inst_filepaths;
  for (auto &source_file : source_files) {
    logger_->Debug(fmt::format("Parsing file '{}' with args '{}'.", source_file,
                               utils::ToString(parse_args, ' ')));

    auto source_code = utils::GetFileContents(source_file);
    auto inst_filepath = utils::CreateInstFile(logger_, source_file);
    inst_filepaths.push_back(inst_filepath);
    command[s_source_file_pos[source_file]] = inst_filepath;

    auto instrumenter = InstrumenterFactory::CreateDefaultInstrumenter();
    bool success = clang::tooling::runToolOnCodeWithArgs(
        FrontendAction::Create(ASTConsumerListener::Create(
            logger_, streams, instrumenter,
            ASTVisitor::Create(ASTVisitorListener::Create(instrumenter)))),
        source_code, parse_args, inst_filepath, compiler);
    if (!success) {
      logger_->Error(fmt::format("Failed to parse file '{}'.", source_file));
    }
  }
  command.push_back(
      "-I/Users/ird/dev/github/iandinwoodie/paptools/paptrace/include");

  for (int i = 0; i < streams.size(); i++) {
    if (dry_run_) {
      std::cout << streams[i] << std::endl;
    } else {
      std::ofstream ofs(inst_filepaths[i],
                        std::ios_base::out | std::ios_base::trunc);
      ofs << streams[i];
      ofs.close();
    }
  }

  return inst_filepaths;
}

} // namespace papinst
