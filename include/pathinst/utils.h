#ifndef PATHINST_UTILS_H
#define PATHINST_UTILS_H

#include <string>
#include <vector>

namespace pathinst {
namespace utils {
std::string ToString(const std::vector<std::string> &v, const char sep);

bool IsSupportedCompiler(const std::string &compiler);

bool IsSourceFile(const std::string &filepath);

std::string GetFileContents(const std::string &filepath);

std::string CreateInstFile(const std::string &filepath);

void RemoveInstFile(const std::string &filepath);

void SetDryRun(bool dry_run);

bool GetDryRun(void);
} // namespace utils
} // namespace pathinst

#endif // PATHINST_UTILS_H
