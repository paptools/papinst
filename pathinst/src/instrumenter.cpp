#include "pathinst/instrumenter.h"

#include <fmt/format.h>

#include <memory>
#include <string>

namespace pathinst {
namespace {
class DefaultInstrumenter : public Instrumenter {
public:
  virtual ~DefaultInstrumenter(void) = default;
  virtual std::string GetFnCalleeInst(const std::string &sig) override {
    static const std::string template_str = "PATHTRACE_CALLEE_NODE(\"{}\");";
    return fmt::format(template_str, sig);
  }

  virtual std::string GetPathCapIncludeInst(void) override {
    return "#include <pathtrace/pathtrace.h>\n";
  }
};
} // namespace

namespace InstrumenterFactory {
std::shared_ptr<Instrumenter> CreateDefaultInstrumenter(void) {
  return std::make_shared<DefaultInstrumenter>();
}
} // namespace InstrumenterFactory
} // namespace pathinst
