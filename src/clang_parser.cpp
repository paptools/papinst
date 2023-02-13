#include "pathinst/clang_parser.h"
#include "pathinst/utils.h"

#include "spdlog/spdlog.h"
#include <clang-c/Index.h>

#include <iostream>
#include <string>
#include <vector>

namespace pathinst {
void ClangParser::ParseCompileCommand(const std::vector<std::string> &command) {
  std::string command_str = pathinst::utils::ToString(command, ' ');
  spdlog::debug("Parsing command: " + command_str);

  if (!Parser::IsSupportedCompiler(command[0])) {
    spdlog::debug(command[0] + " is not a supported compiler.");
    return;
  }

  CXIndex index = clang_createIndex(/*excludeDeclarationsFromPCH*/ 0,
                                    /*displayDiagnostics*/ 1);
  std::cout << "Clang Global Options: " << clang_CXIndex_getGlobalOptions(index)
            << std::endl;

  // For each file:
  {
    const std::string file_path = "/path/to/foo.cpp";
    spdlog::debug("Adding library include to: " + file_path);
    auto library_include_line = instrumenter_->GetLibraryIncludeLine();
    // Write library include line.
    spdlog::debug("Wrote line:\n" + library_include_line);

    // For each function in the file:
    {
      // Write lines to function.
    }
  }
}
} // namespace pathinst
