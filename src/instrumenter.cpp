#include "pathinst/instrumenter.h"

#include "spdlog/spdlog.h"

#include <sstream>
#include <string>

namespace pathinst {
const std::string Instrumenter::GetLibraryIncludeLine(void) const {
  static std::string include_line = "#include \"pathinst/pathinst.h\"\n";
  return include_line;
}
} // namespace pathinst
