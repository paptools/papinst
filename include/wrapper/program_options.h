#ifndef WRAPPER_PROGRAM_OPTIONS_H
#define WRAPPER_PROGRAM_OPTIONS_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace wrapper {
// Represents the program options requested at invocation.
struct ProgramOptions {
  std::string usage_message;
  std::vector<std::string> command;
  std::optional<int> early_exit_code;
};

// Creates a ProgramOptions object from the given command line arguments.
std::unique_ptr<ProgramOptions> CreateProgramOptions(int argc, char **argv);
} // namespace wrapper

#endif // WRAPPER_PROGRAM_OPTIONS_H
