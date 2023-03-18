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
} // namespace utils
} // namespace pathinst
