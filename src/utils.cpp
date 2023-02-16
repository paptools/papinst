#include "pathinst/utils.h"

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
} // namespace utils
} // namespace pathinst
