#ifndef COMPILER_WRAPPER_PROGRAM_OPTIONS_H
#define COMPILER_WRAPPER_PROGRAM_OPTIONS_H

#include <memory>
#include <string>
#include <vector>

namespace wrapper {
// Represents the program options requested at invocation.
struct ProgramOptions {
  enum class Status { Ok, Error };

  Status parse_status;
  std::string message;
  std::vector<std::string> command;

  ProgramOptions() : parse_status(Status::Error) {}
};

// Creates a ProgramOptions object from the given command line arguments.
std::unique_ptr<ProgramOptions> CreateProgramOptions(int argc, char **argv);
} // namespace wrapper

#endif // COMPILER_WRAPPER_PROGRAM_OPTIONS_H
