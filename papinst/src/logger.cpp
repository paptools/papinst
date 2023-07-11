#include "papinst/logger.h"

// Third-party headers.
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

// C++ standard library headers.
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace papinst {
namespace {
std::unordered_map<Logger::Level, spdlog::level::level_enum> s_level_map = {
    {Logger::Level::Debug, spdlog::level::debug},
    {Logger::Level::Info, spdlog::level::info},
    {Logger::Level::Warning, spdlog::level::warn},
    {Logger::Level::Error, spdlog::level::err},
    {Logger::Level::Critical, spdlog::level::critical},
};

class ConsoleLogger : public Logger {
public:
  ConsoleLogger(void)
      : logger_(std::make_shared<spdlog::logger>(
            "papinst",
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>())) {
    logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%P] [%^%l%$] %v");
  }

  virtual ~ConsoleLogger(void) = default;

  void SetLevel(const Level &level) override {
    logger_->set_level(s_level_map.at(level));
  }

  void Debug(const std::string_view &message) override {
    logger_->debug(message);
  }

  void Info(const std::string_view &message) override {
    logger_->info(message);
  }

  void Warning(const std::string_view &message) override {
    logger_->warn(message);
  }

  void Error(const std::string_view &message) override {
    logger_->error(message);
  }

  void Critical(const std::string_view &message) override {
    logger_->critical(message);
  }

private:
  std::shared_ptr<spdlog::logger> logger_;
};
} // namespace

namespace LoggerFactory {
std::shared_ptr<Logger> CreateConsoleLogger(void) {
  return std::make_shared<ConsoleLogger>();
}
} // namespace LoggerFactory
} // namespace papinst
