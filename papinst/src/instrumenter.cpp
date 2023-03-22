#include "papinst/instrumenter.h"

#include <fmt/format.h>

#include <memory>
#include <string>

namespace papinst {
namespace {
class DefaultInstrumenter : public Instrumenter {
public:
  virtual ~DefaultInstrumenter(void) = default;
  virtual std::string GetFnCalleeInst(const std::string &sig) override {
    static const std::string template_str = "PAPTRACE_CALLEE_NODE(\"{}\");";
    return fmt::format(template_str, sig);
  }

  virtual std::string GetTraceIncludeInst(void) override {
    return "#include <paptrace/paptrace.h>\n";
  }

  virtual std::string GetCfInst(void) override { return "PAPTRACE_CF_NODE();"; }
};
} // namespace

namespace InstrumenterFactory {
std::shared_ptr<Instrumenter> CreateDefaultInstrumenter(void) {
  return std::make_shared<DefaultInstrumenter>();
}
} // namespace InstrumenterFactory
} // namespace papinst
