#include "pathinst/parser.h"

#include <boost/filesystem.hpp>

#include <regex>
#include <string>

namespace pathinst {
bool Parser::IsSupportedCompiler(const std::string &compiler) {
  boost::filesystem::path compiler_path(compiler);
  std::regex pattern(R"(^(gcc|g\+\+|clang|clang\+\+))");
  return std::regex_search(compiler_path.filename().string(), pattern);
}
} // namespace pathinst
