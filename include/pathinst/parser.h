#ifndef PATHINST_PARSER_H
#define PATHINST_PARSER_H

#include <string>
#include <vector>

namespace pathinst {
class Parser {
public:
  std::vector<std::string>
  ParseCompileCommand(const std::vector<std::string> &command);
};
} // namespace pathinst

#endif // PATHINST_PARSER_H
