#ifndef PATHINST_CLANG_PARSER_H
#define PATHINST_CLANG_PARSER_H

#include "pathinst/instrumenter.h"
#include "pathinst/parser.h"

#include <memory>
#include <string>
#include <vector>

namespace pathinst {
class ClangParser final : public Parser {
public:
  ClangParser(std::shared_ptr<Instrumenter> instrumenter)
      : instrumenter_(instrumenter) {}

  std::vector<std::string>
  ParseCompileCommand(const std::vector<std::string> &command) override;

private:
  std::shared_ptr<Instrumenter> instrumenter_;
};
} // namespace pathinst

#endif // PATHINST_CLANG_PARSER_H
