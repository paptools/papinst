#include "wrapper/logger.h"

#include <memory>
#include <ostream>
#include <string>

namespace wrapper {
namespace {
class SimpleLogger : public Logger {
public:
  SimpleLogger(std::ostream &os) : os_(os) {}

  void Log(std::string message) override { os_ << message << std::endl; }

  void Info(std::string message) override {
    os_ << "INFO: " << message << std::endl;
  }

  void Error(std::string message) override {
    os_ << "ERROR: " << message << std::endl;
  }

private:
  std::ostream &os_;
};
} // namespace

std::shared_ptr<Logger> Logger::CreateSimpleLogger(std::ostream &os) {
  return std::make_shared<SimpleLogger>(os);
}
} // namespace wrapper
