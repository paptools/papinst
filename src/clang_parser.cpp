#include "pathinst/clang_parser.h"
#include "pathinst/frontend_action.h"
#include "pathinst/utils.h"

#include "spdlog/spdlog.h"
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/ADT/Twine.h>

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

std::vector<std::string> GetCompileArgs(
    const std::vector<clang::tooling::CompileCommand> &compile_commands) {
  std::vector<std::string> compileArgs;

  for (auto &cmd : compile_commands) {
    for (auto &arg : cmd.CommandLine)
      compileArgs.push_back(arg);
  }

  if (compileArgs.empty() == false) {
    compileArgs.erase(begin(compileArgs));
    compileArgs.pop_back();
  }

  return compileArgs;
}

std::vector<std::string>
GetSyntaxOnlyToolArgs(const std::vector<std::string> &extraArgs,
                      llvm::StringRef fileName) {
  std::vector<std::string> args;

  args.push_back("clang-tool");
  args.push_back("-fsyntax-only");

  args.insert(args.end(), extraArgs.begin(), extraArgs.end());
  args.push_back(fileName.str());

  return args;
}

bool CustomRunToolOnCodeWithArgs(
    std::unique_ptr<clang::FrontendAction> frontend_action,
    const llvm::Twine &Code, const std::vector<std::string> &args,
    const llvm::Twine &filename,
    const clang::tooling::FileContentMappings &virtual_mapped_files =
        clang::tooling::FileContentMappings()) {
  llvm::SmallString<16> fileNameStorage;
  llvm::StringRef fileNameRef =
      filename.toNullTerminatedStringRef(fileNameStorage);

  llvm::IntrusiveRefCntPtr<clang::FileManager> files(
      new clang::FileManager(clang::FileSystemOptions()));
  auto pchContainer = std::make_shared<clang::PCHContainerOperations>();

  clang::tooling::ToolInvocation invocation(
      GetSyntaxOnlyToolArgs(args, fileNameRef), std::move(frontend_action),
      files.get(), pchContainer);

  // llvm::SmallString<1024> codeStorage;
  // invocation.mapVirtualFile(fileNameRef,
  //                           code.toNullTerminatedStringRef(codeStorage));

  // for (auto &filenameWithContent : virtual_mapped_files)
  //   invocation.mapVirtualFile(filenameWithContent.first,
  //                             filenameWithContent.second);

  return invocation.run();
}

} // namespace

void ClangParser::ParseCompileCommand(const std::vector<std::string> &command) {
  std::string command_str = pathinst::utils::ToString(command, ' ');
  spdlog::debug("\n\nParsing command '" + command_str + "'.");

  if (!Parser::IsSupportedCompiler(command[0])) {
    spdlog::debug("Executable '" + command[0] +
                  "' is not a supported compiler. Skipping instrumentation.");
    return;
  }

  std::vector<const char *> argv;
  argv.push_back("clang-tool");
  argv.push_back("--");
  std::vector<std::string> files;
  for (auto &arg : command) {
    if (utils::IsSourceFile(arg)) {
      files.push_back(arg);
    } else {
      argv.push_back(arg.c_str());
    }
  }
  int argc = argv.size();

  if (files.empty()) {
    spdlog::debug("No source files found in command '" + command_str +
                  "'. Skipping instrumentation.");
    return;
  }

  std::string error_msg;
  auto compilation_database =
      clang::tooling::FixedCompilationDatabase::loadFromCommandLine(
          argc, &argv[0], error_msg);
  if (!compilation_database || error_msg.size() > 0) {
    spdlog::error("Error loading compilation database: " +
                  (error_msg.empty() ? "unspecified error" : error_msg));
    return;
  }

  for (auto &file : files) {
    spdlog::debug("Parsing file '" + file + "'.");
    auto source_code = GetSourceCode(file);
    auto compile_commands = compilation_database->getCompileCommands(file);
    auto compile_args = GetCompileArgs(compile_commands);
    compile_args.push_back("-I/opt/homebrew/opt/llvm/include");
    compile_args.push_back("-I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/14.0.0/include/");
    auto x_frontend_action = std::make_unique<XFrontendAction>();
    CustomRunToolOnCodeWithArgs(std::move(x_frontend_action), source_code,
                                compile_args, file);
    spdlog::error("Error parsing file '" + file + "'.");
  }
}
} // namespace pathinst
