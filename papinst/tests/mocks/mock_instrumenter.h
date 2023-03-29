#ifndef PAPINST_MOCK_INSTRUMENTER_H
#define PAPINST_MOCK_INSTRUMENTER_H

// Local headers.
#include "papinst/instrumenter.h"

// Third-party headers.
#include <gmock/gmock.h>

namespace papinst {
class MockInstrumenter : public Instrumenter {
public:
  MOCK_METHOD(std::string, GetFnCalleeInst, (const std::string &sig),
              (override));
  MOCK_METHOD(std::string, GetTraceIncludeInst, (), (override));
  MOCK_METHOD(std::string, GetCfInst, (), (override));
};
} // namespace papinst

#endif // PAPINST_MOCK_INSTRUMENTER_H
