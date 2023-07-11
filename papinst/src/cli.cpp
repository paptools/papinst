#include "papinst/cli.h"

// Local headers.
#include "papinst/exception.h"
#include "papinst/version.h"

// Third-party headers.
#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>

// C++ standard library headers.
#include <cstring>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace papinst {
namespace cli {
namespace {
namespace po = boost::program_options;
// Usage message leader formatted to docopt standards (http://docopt.org).
//
// Note: The Options section of the usage message is populated by the
// ParseOptions function.
std::string sUsageMessage("Usage:\n"
                          "  papinst [options] -- <command>\n"
                          "  papinst -V|--version\n"
                          "  papinst -h|--help\n"
                          "\n"
                          "Description:\n"
                          "  The path instrumenter.\n");

// Returns the position of the terminator if found, otherwise 0.
int FindTerminatorPos(int argc, char **argv) {
  for (int i = 0; i < argc; ++i) {
    if (std::strcmp(argv[i], "--") == 0) {
      return i;
    }
  }
  return 0;
}
} // namespace

std::shared_ptr<Options> ParseArgs(int argc, char **argv) {
  auto options = std::make_shared<Options>();
  options->status = Options::Status::Failure;

  // Define the user-visible options.
  po::options_description desc("Options");
  // clang-format off
  desc.add_options()
      ("verbose,v", po::bool_switch(&options->verbose),
       "Enable detailed application output.")
      ("dry-run,n", po::bool_switch(&options->dry_run),
       "Display file edits to console instead of writing.")
      ("version,V", po::bool_switch(&options->version),
       "Output version information and exit.")
      ("help,h", po::bool_switch(&options->help),
       "Display this help and exit.");
  // clang-format on

  // Append the options section to the usage message.
  std::stringstream ss;
  ss << "\n" << desc;
  sUsageMessage.append(ss.str());

  // Return early when the program has been called with no arguments.
  if (argc < 2) {
    return options;
  }

  try {
    // An empty positional descriptor is used to disallow positional arguments
    // located before the terminator.
    po::positional_options_description pos;
    po::variables_map vm;
    int term_pos = FindTerminatorPos(argc, argv);
    po::store(po::command_line_parser(term_pos ? term_pos : argc, argv)
                  .options(desc)
                  .positional(pos)
                  .run(),
              vm);
    po::notify(vm);

    // Store the user command.
    for (int i = term_pos + 1; i > 0 && i < argc; ++i) {
      options->command.push_back(argv[i]);
    }

    if (options->command.empty()) {
      throw po::error("the argument 'command' is required but missing");
    }
  } catch (const po::error &ex) {
    throw papinst::Exception(ex.what());
  }

  options->status = Options::Status::Success;
  return options;
}

const std::string &GetUsageMessage(void) { return sUsageMessage; }

const std::string &GetVersionMessage(void) {
  static const std::string message("papinst " + kVersion);
  return message;
}
} // namespace cli
} // namespace papinst
