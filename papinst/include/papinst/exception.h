#ifndef PAPINST_EXCEPTION_H
#define PAPINST_EXCEPTION_H

// C++ standard library headers.
#include <exception>
#include <string>

namespace papinst {
// Base class for all papinst exceptions.
class Exception : public std::exception {
public:
  // Explicit C-string constructor.
  explicit Exception(const char *message) : message_(message) {}

  // Explicit std::string constructor.
  explicit Exception(const std::string &message) : message_(message) {}

  // Virtual destructor to allow for subclassing.
  virtual ~Exception(void) noexcept {}

  // Returns a pointer to the constant error description message.
  virtual const char *what(void) const noexcept { return message_.c_str(); }

private:
  // Holds the error description message.
  std::string message_;
};
} // namespace papinst

#endif // PAPINST_EXCEPTION_H
