#include "papinst/utils.h"

// Local headers.
#include "papinst/logger.h"

// Third-party headers.
#include <boost/filesystem.hpp>
#include <fmt/format.h>

// C++ standard library headers.
#include <fstream>
#include <regex>
#include <string>
#include <vector>

namespace papinst {
namespace utils {
namespace {
bool s_dry_run = false;
} // namespace

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
  std::regex pattern(R"(^(gcc|g\+\+|clang|clang\+\+|cc|c\+\+))");
  return std::regex_search(compiler_path.filename().string(), pattern);
}

bool IsGNU(const std::string &compiler) {
  boost::filesystem::path compiler_path(compiler);
  std::regex pattern(R"(^(gcc|g\+\+))");
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

std::string CreateInstFile(std::shared_ptr<Logger> logger,
                           const std::string &filepath) {
  boost::filesystem::path inst_filepath(filepath);
  auto extension = inst_filepath.extension();
  inst_filepath.replace_extension(".papinst" + extension.string());
  logger->Debug(fmt::format("Creating file '{}'.", inst_filepath.string()));
  if (!s_dry_run) {
    boost::filesystem::copy_file(
        filepath, inst_filepath.string(),
        boost::filesystem::copy_options::overwrite_existing);
  }

  return inst_filepath.string();
}

void RemoveInstFile(std::shared_ptr<Logger> logger,
                    const std::string &filepath) {
  logger->Debug(fmt::format("Removing file '{}'.", filepath));
  if (!s_dry_run) {
    boost::filesystem::remove(filepath);
  }
}

void SetDryRun(bool dry_run) { s_dry_run = dry_run; }

bool GetDryRun(void) { return s_dry_run; }
} // namespace utils
} // namespace papinst
