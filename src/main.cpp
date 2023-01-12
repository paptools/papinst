#include "wrapper/logger.h"
#include "wrapper/program_options.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
std::string ToString(std::vector<std::string> const &args) {
  std::stringstream ss;
  for (auto const &arg : args) {
    ss << arg << " ";
  }
  std::string result = ss.str();
  if (!result.empty()) {
    result.pop_back();
  }
  return result;
}
} // namespace

int main(int argc, char **argv) {
  auto logger = wrapper::Logger::CreateSimpleLogger(std::cout);
  try {
    auto options = wrapper::CreateProgramOptions(argc, argv);
    if (options->early_exit_code) {
      logger->Log(options->usage_message);
      return options->early_exit_code.value();
    }

    logger->Info("wrapping command \"" + ToString(options->command) + "\"");

  } catch (std::exception const &e) {
    logger->Error(e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
