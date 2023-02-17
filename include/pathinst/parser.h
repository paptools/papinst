#ifndef PATHINST_PARSER_H
#define PATHINST_PARSER_H

#include <string>
#include <vector>

namespace pathinst {
class Parser {
public:
  explicit Parser(bool dry_run);
  std::vector<std::string>
  ParseCompileCommand(std::vector<std::string> &command);

private:
  bool dry_run_;
};
} // namespace pathinst

#endif // PATHINST_PARSER_H
