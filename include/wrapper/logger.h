#ifndef WRAPPER_LOGGER_H
#define WRAPPER_LOGGER_H

#include <memory>
#include <ostream>
#include <string>

namespace wrapper {
// Abstract logger class.
class Logger {
public:
  static std::shared_ptr<Logger> CreateSimpleLogger(std::ostream &os);
  virtual ~Logger() {}
  virtual void Log(std::string message) = 0;
  virtual void Info(std::string message) = 0;
  virtual void Error(std::string message) = 0;
};
} // namespace wrapper

#endif // WRAPPER_LOGGER_H
