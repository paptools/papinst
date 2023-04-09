#ifndef PAPINST_MOCK_LOGGER_H
#define PAPINST_MOCK_LOGGER_H

// Local headers.
#include "papinst/instrumenter.h"

// Third-party headers.
#include <gmock/gmock.h>

// C++ standard library headers.
#include <string_view>

namespace papinst {
class MockLogger : public Logger {
public:
  MOCK_METHOD(void, SetLevel, (const Level &level), (override));
  MOCK_METHOD(void, Debug, (const std::string_view &message), (override));
  MOCK_METHOD(void, Info, (const std::string_view &message), (override));
  MOCK_METHOD(void, Warning, (const std::string_view &message), (override));
  MOCK_METHOD(void, Error, (const std::string_view &message), (override));
  MOCK_METHOD(void, Critical, (const std::string_view &message), (override));
};
} // namespace papinst

#endif // PAPINST_MOCK_LOGGER_H
