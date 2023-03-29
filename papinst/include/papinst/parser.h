#ifndef PAPINST_PARSER_H
#define PAPINST_PARSER_H

// Third-party headers.
#include <spdlog/spdlog.h>

// C++ standard library headers.
#include <string>
#include <vector>

namespace papinst {
class Parser {
public:
  explicit Parser(std::shared_ptr<spdlog::logger> logger, bool dry_run);
  std::vector<std::string>
  ParseCompileCommand(std::vector<std::string> &command);

private:
  std::shared_ptr<spdlog::logger> logger_;
  bool dry_run_;
};
} // namespace papinst

#endif // PAPINST_PARSER_H
