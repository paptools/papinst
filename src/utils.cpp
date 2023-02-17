#include "pathinst/utils.h"

#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

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

bool IsSourceFile(const std::string &filename) {
  auto extension = filename.substr(filename.find_last_of(".") + 1);
  return extension == "c" || extension == "cpp" || extension == "cc" ||
         extension == "cxx" || extension == "h" || extension == "hpp" ||
         extension == "hh" || extension == "hxx";
}

void CreateFileBackup(const std::string &filepath) {
  // spdlog.debug("Creating backup of file '" + filepath + "'.");
  boost::filesystem::path backup(filepath);
  auto extension = backup.extension();
  backup.replace_extension(".pathinst" + extension.string());
  boost::filesystem::copy_file(
      filepath, backup.string(),
      boost::filesystem::copy_option::overwrite_if_exists);
}

void RestoreOriginalFile(const std::string &filepath) {
  // spdlog.debug("Restoring original file '" + filepath + "'.");
  boost::filesystem::path backup(filepath);
  auto extension = backup.extension();
  backup.replace_extension(".pathinst" + extension.string());
  boost::filesystem::rename(backup.string(), filepath);
}
} // namespace utils
} // namespace pathinst
