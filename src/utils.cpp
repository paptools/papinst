#include "pathinst/utils.h"

#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

#include <fstream>
#include <regex>
#include <string>
#include <vector>

namespace pathinst {
namespace utils {
std::string ToString(const std::vector<std::string> &v, const char sep) {
  std::string s;
  for (auto &&e : v) {
    s += e;
    s += sep;
  }
  if (!s.empty()) {
    s.pop_back();
  }
  return s;
}

bool IsSupportedCompiler(const std::string &compiler) {
  boost::filesystem::path compiler_path(compiler);
  std::regex pattern(R"(^(gcc|g\+\+|clang|clang\+\+))");
  return std::regex_search(compiler_path.filename().string(), pattern);
}

bool IsSourceFile(const std::string &filepath) {
  auto extension = filepath.substr(filepath.find_last_of(".") + 1);
  return extension == "c" || extension == "cpp" || extension == "cc" ||
         extension == "cxx";
}

std::string GetFileContents(const std::string &filepath) {
  std::ifstream ifs(filepath);
  std::string contents((std::istreambuf_iterator<char>(ifs)),
                       (std::istreambuf_iterator<char>()));
  return contents;
}

void CreateFileBackup(const std::string &filepath) {
  spdlog::debug("Backing up file '" + filepath + "'.");
  boost::filesystem::path backup(filepath);
  auto extension = backup.extension();
  backup.replace_extension(".pathinst" + extension.string());
  boost::filesystem::copy_file(filepath, backup.string());
}

void RestoreOriginalFile(const std::string &filepath) {
  spdlog::debug("Restoring original file '" + filepath + "'.");
  boost::filesystem::path backup(filepath);
  auto extension = backup.extension();
  backup.replace_extension(".pathinst" + extension.string());
  boost::filesystem::rename(backup.string(), filepath);
}
} // namespace utils
} // namespace pathinst
