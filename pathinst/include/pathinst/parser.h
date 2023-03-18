#ifndef PATHINST_PARSER_H
#define PATHINST_PARSER_H

#include <spdlog/spdlog.h>
#include <string>
#include <vector>

namespace pathinst {
class Parser {
public:
  explicit Parser(std::shared_ptr<spdlog::logger> logger, bool dry_run);
  std::vector<std::string>
  ParseCompileCommand(std::vector<std::string> &command);

private:
  std::shared_ptr<spdlog::logger> logger_;
  bool dry_run_;
};
} // namespace pathinst

#endif // PATHINST_PARSER_H
