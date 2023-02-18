#include "pathinst/parser.h"
#include "pathinst/frontend_action.h"
#include "pathinst/utils.h"

#include <boost/filesystem.hpp>
#include <boost/process/search_path.hpp>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/raw_ostream.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace pathinst {
namespace {
std::map<std::string, int> s_source_file_pos;
std::set<std::string> s_unsupported_flags = {"-g"};
} // namespace

Parser::Parser(std::shared_ptr<spdlog::logger> logger, bool dry_run)
    : logger_(logger), dry_run_(dry_run) {}

std::vector<std::string>
Parser::ParseCompileCommand(std::vector<std::string> &command) {
  std::string command_str = pathinst::utils::ToString(command, ' ');
  logger_->debug("Parsing command '{}'.", command_str);

  auto compiler = command[0];
  if (!utils::IsSupportedCompiler(compiler)) {
    logger_->debug(
        "Executable '{}' is not a supported compiler. Skipping parse.",
        compiler);
    return {};
  }

  if (!boost::filesystem::exists(compiler)) {
    compiler = boost::process::search_path(compiler).string();
  }
  logger_->debug("User compiler is '{}'.", compiler);

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
    logger_->debug("No source files found in command '{}'. Skipping parse.",
                   command_str);
    return {};
  }

  utils::SetDryRun(dry_run_);

  std::vector<std::string> streams;
  std::vector<std::string> inst_filepaths;
  for (auto &source_file : source_files) {
    spdlog::debug("Parsing file '{}' with args '{}'.", source_file,
                  utils::ToString(parse_args, ' '));

    auto source_code = utils::GetFileContents(source_file);
    auto inst_filepath = utils::CreateInstFile(source_file);
    inst_filepaths.push_back(inst_filepath);
    command[s_source_file_pos[source_file]] = inst_filepath;

    bool success = clang::tooling::runToolOnCodeWithArgs(
        std::make_unique<FrontendAction>(logger_, streams), source_code,
        parse_args, inst_filepath, compiler);
    if (!success) {
      logger_->error("Failed to parse file '{}'.", source_file);
    }
  }

  for (auto &stream : streams) {
    std::cout << stream << std::endl;
  }

  return inst_filepaths;
}

} // namespace pathinst
