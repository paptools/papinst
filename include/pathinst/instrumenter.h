#ifndef PATHINST_INSTRUMENTER_H
#define PATHINST_INSTRUMENTER_H

#include <memory>
#include <string>

namespace pathinst {
class Instrumenter {
public:
  virtual ~Instrumenter(void) = default;
  virtual std::string GetFnCalleeInst(const std::string &sig) = 0;
  virtual std::string GetPathCapIncludeInst(void) = 0;
};

namespace InstrumenterFactory {
std::shared_ptr<Instrumenter> CreateDefaultInstrumenter(void);
} // namespace InstrumenterFactory
} // namespace pathinst

#endif // PATHINST_INSTRUMENTER_H
