#ifndef PAPINST_MOCK_INSTRUMENTER_H
#define PAPINST_MOCK_INSTRUMENTER_H

// Local headers.
#include "papinst/instrumenter.h"

// Third-party headers.
#include <gmock/gmock.h>

// C++ standard library headers.
#include <string>

namespace papinst {
class MockInstrumenter : public Instrumenter {
public:
  MOCK_METHOD(std::string, GetTraceCalleeInst, (int id, const std::string &sig),
              (override));
  MOCK_METHOD(std::string, GetTraceIfThenStmtInst,
              (int stmt_id, const std::string &desc), (override));
  MOCK_METHOD(std::string, GetTraceIfElseStmtInst,
              (int stmt_id, const std::string &desc), (override));
  MOCK_METHOD(std::string, GetTraceIncludeInst, (), (override));
};
} // namespace papinst

#endif // PAPINST_MOCK_INSTRUMENTER_H
