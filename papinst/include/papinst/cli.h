#ifndef PAPINST_CLI_OPTIONS_H
#define PAPINST_CLI_OPTIONS_H

#include <memory>
#include <string>
#include <vector>

namespace papinst {
namespace cli {
// This struct is used to hold option data parsed from the command line.
struct Options {
  enum class Status {
    Uninitialized,
    Success,
    Failure,
  };

  Status status;
  bool help;
  bool verbose;
  bool version;
  bool dry_run;
  std::vector<std::string> command;

  Options(void)
      : status(Status::Uninitialized), help(false), verbose(false),
        version(false), dry_run(false), command() {}
};

// Returns an Options object constructed from the given arguments.
std::shared_ptr<Options> ParseArgs(int argc, char **argv);

// Returns the usage message for this program.
//
// Note: ParseArgs must be run before any call to this function to populate
// the Options section of the usage message.
const std::string &GetUsageMessage(void);

// Returns the version message for this program.
const std::string &GetVersionMessage(void);
} // namespace cli
} // namespace papinst

#endif // PAPINST_CLI_OPTIONS_H
