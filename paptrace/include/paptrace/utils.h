#ifndef PAPTRACE_UTILS_H
#define PAPTRACE_UTILS_H

// Third-party headers.
#include <gtest/gtest.h>

// C++ standard library headers.
#include <sstream>
#include <string>

namespace paptrace {
namespace utils {
template <typename T>::std::string PrintToString(const T &value) {
  std::stringstream ss;
  ::testing::internal::UniversalTersePrinter<T>::Print(value, &ss);
  return ss.str();
}
} // namespace utils
} // namespace paptrace

#endif // PAPTRACE_UTILS_H
