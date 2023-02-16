#ifndef PATHINST_UTILS_H
#define PATHINST_UTILS_H

#include <string>
#include <vector>

namespace pathinst {
namespace utils {
std::string ToString(const std::vector<std::string> &v, const char sep);

bool IsSourceFile(const std::string &filename);
} // namespace utils
} // namespace pathinst

#endif // PATHINST_UTILS_H
