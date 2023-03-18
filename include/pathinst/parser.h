#ifndef PATHINST_PARSER_H
#define PATHINST_PARSER_H

#include <string>
#include <vector>

namespace pathinst {
class Parser {
public:
  static bool IsSupportedCompiler(const std::string &compiler);

  ~Parser(void) {}
  virtual void ParseCompileCommand(const std::vector<std::string> &command) = 0;
};
} // namespace pathinst

#endif // PATHINST_PARSER_H
