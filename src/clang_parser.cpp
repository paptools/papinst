#include "pathinst/clang_parser.h"
#include "pathinst/frontend_action.h"
#include "pathinst/utils.h"

#include <boost/filesystem.hpp>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace pathinst {
namespace {
std::string GetSourceCode(const std::string &filepath) {
  std::ifstream ifs(filepath);
  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));
  return content;
}
} // namespace

std::vector<std::string>
ClangParser::ParseCompileCommand(const std::vector<std::string> &command) {
  std::string command_str = pathinst::utils::ToString(command, ' ');
  spdlog::debug("\n\nParsing command '" + command_str + "'.");

  std::string compiler = command[0];
  if (!Parser::IsSupportedCompiler(compiler)) {
    spdlog::debug("Executable '" + compiler +
                  "' is not a supported compiler. Skipping instrumentation.");
    return {};
  }

  // Resolve the compiler path.
  compiler = boost::filesystem::system_complete(compiler).string();

  // Separate out the source files from the compiler arguments.
  std::vector<std::string> parse_args;
  std::vector<std::string> source_files;
  for (int i = 1; i < command.size(); i++) {
    if (utils::IsSourceFile(command[i])) {
      source_files.push_back(command[i]);
    } else {
      parse_args.push_back(command[i]);
    }
  }

  if (source_files.empty()) {
    spdlog::debug("No source files found in command '" +
                  utils::ToString(command, ' ') +
                  "'. Skipping instrumentation.");
    return {};
  }

  for (auto &source_file : source_files) {
    spdlog::debug("Parsing file '" + source_file + "'.");
    spdlog::debug("Parse args: " + utils::ToString(parse_args, ' '));

    utils::CreateFileBackup(source_file);
    auto source_code = GetSourceCode(source_file);
    auto pch_container = std::make_shared<clang::PCHContainerOperations>();

    bool success = clang::tooling::runToolOnCodeWithArgs(
        std::make_unique<XFrontendAction>(), source_code, parse_args,
        source_file, compiler, pch_container);

    spdlog::debug("Parse success: '" + std::string(success ? "true" : "false") +
                  "'.");
  }

  return source_files;
}
} // namespace pathinst
