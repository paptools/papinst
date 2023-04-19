#ifndef PAPINST_INSTRUMENTER_H
#define PAPINST_INSTRUMENTER_H

// C++ standard library headers.
#include <memory>
#include <string>

namespace papinst {
// Abstract base class for all instrumenters.
class Instrumenter {
public:
  virtual ~Instrumenter(void) = default;
  virtual std::string GetTraceCalleeInst(int id, const std::string &sig) = 0;
  virtual std::string GetTraceStmtInst(int stmt_id) = 0;
  virtual std::string GetTraceIncludeInst(void) = 0;
};

// Collection of factory methods for creating instrumenters.
namespace InstrumenterFactory {
std::shared_ptr<Instrumenter> CreateDefaultInstrumenter(void);
} // namespace InstrumenterFactory
} // namespace papinst

#endif // PAPINST_INSTRUMENTER_H
