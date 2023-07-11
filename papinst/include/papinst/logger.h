#ifndef PAPINST_LOGGER_H
#define PAPINST_LOGGER_H

#include <fmt/format.h>

// C++ standard library headers.
#include <memory>
#include <string>
#include <string_view>

template <typename... Args> using format_string_t = fmt::format_string<Args...>;

namespace papinst {
// Abstract base class for all loggers.
class Logger {
public:
  // Enum for log levels.
  enum class Level {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
  };

  // Virtual destructor to allow for subclassing.
  virtual ~Logger(void) = default;

  // Sets the log level.
  virtual void SetLevel(const Level &level) = 0;

  // Logs a debug message.
  virtual void Debug(const std::string_view &message) = 0;

  // Logs an info message.
  virtual void Info(const std::string_view &message) = 0;

  // Logs a warning message.
  virtual void Warning(const std::string_view &message) = 0;

  // Logs an error message.
  virtual void Error(const std::string_view &message) = 0;

  // Logs a critical message.
  virtual void Critical(const std::string_view &message) = 0;
};

// Collection of factory methods for creating loggers.
namespace LoggerFactory {
// Creates a logger that writes to the console.
std::shared_ptr<Logger> CreateConsoleLogger(void);
} // namespace LoggerFactory
} // namespace papinst

#endif // PAPINST_LOGGER_H
