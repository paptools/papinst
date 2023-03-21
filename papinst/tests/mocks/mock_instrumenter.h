#ifndef PAPINST_MOCK_INSTRUMENTER_H
#define PAPINST_MOCK_INSTRUMENTER_H

#include "papinst/instrumenter.h"

#include <gmock/gmock.h>

namespace papinst {
class MockInstrumenter : public Instrumenter {
public:
  MOCK_METHOD(std::string, GetFnCalleeInst, (const std::string &sig),
              (override));
  MOCK_METHOD(std::string, GetTraceIncludeInst, (), (override));
};
} // namespace papinst

#endif // PAPINST_MOCK_INSTRUMENTER_H
