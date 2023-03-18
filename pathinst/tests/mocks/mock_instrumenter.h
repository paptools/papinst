#ifndef PATHINST_MOCK_INSTRUMENTER_H
#define PATHINST_MOCK_INSTRUMENTER_H

#include "pathinst/instrumenter.h"

#include <gmock/gmock.h>

namespace pathinst {
class MockInstrumenter : public Instrumenter {
public:
  MOCK_METHOD(std::string, GetFnCalleeInst, (const std::string &sig),
              (override));
  MOCK_METHOD(std::string, GetPathCapIncludeInst, (), (override));
};
} // namespace pathinst

#endif // PATHINST_MOCK_INSTRUMENTER_H
