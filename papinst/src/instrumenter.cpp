#include "papinst/instrumenter.h"

// Third-party headers.
#include <fmt/format.h>

// C++ standard library headers.
#include <memory>
#include <string>

namespace papinst {
namespace {
class DefaultInstrumenter : public Instrumenter {
public:
  ~DefaultInstrumenter(void) = default;

  std::string GetTraceCalleeInst(int id, const std::string &sig) override {
    static const std::string template_str =
        "\nPAPTRACE_TRACE_CALLEE({}, \"{}\");";
    return fmt::format(template_str, id, sig);
  }

  std::string GetTraceStmtInst(int stmt_id) override {
    static const std::string template_str = "\nPAPTRACE_TRACE_STMT(\"{}\");";
    return fmt::format(template_str, stmt_id);
  }

  std::string GetTraceIncludeInst(void) override {
    return "#include <paptrace/paptrace.h>\n";
  }
};
} // namespace

namespace InstrumenterFactory {
std::shared_ptr<Instrumenter> CreateDefaultInstrumenter(void) {
  return std::make_shared<DefaultInstrumenter>();
}
} // namespace InstrumenterFactory
} // namespace papinst
