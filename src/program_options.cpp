#include "wrapper/program_options.h"

#include <boost/program_options.hpp>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace wrapper {
namespace {
std::string k_usage_statement = R"(Usage:
  wcc [options] -- <compiler> [compiler options]
  wcc -h|--help
)";

int FindTerminator(int argc, char **argv) {
  for (int i = 0; i < argc; ++i) {
    if (strcmp(argv[i], "--") == 0)
      return i;
  }
  return 0;
}
} // namespace

std::unique_ptr<ProgramOptions> CreateProgramOptions(int argc, char **argv) {
  namespace po = boost::program_options;

  auto options = std::make_unique<ProgramOptions>();
  po::options_description desc("Options");
  bool help_requested = false;
  desc.add_options()("help,h", po::bool_switch(&help_requested),
                     "Display this help message");
  std::stringstream ss;
  ss << k_usage_statement << desc;
  options->usage_message = ss.str();

  // We use an empty positional description to disallow positional arguments
  // that are located before the terminator.
  po::positional_options_description pos;
  po::variables_map vm;
  int term_pos = FindTerminator(argc, argv);
  po::store(po::command_line_parser(term_pos ? term_pos : argc, argv)
                .options(desc)
                .positional(pos)
                .run(),
            vm);
  po::notify(vm);

  for (int i = term_pos + 1; i > 0 && i < argc; ++i) {
    options->command.push_back(argv[i]);
  }

  if (argc < 2) {
    options->early_exit_code = EXIT_FAILURE;
    return options;
  } else if (help_requested) {
    options->early_exit_code = EXIT_SUCCESS;
    return options;
  } else {
    if (options->command.empty()) {
      throw po::error("no command specified");
    }
  }

  return options;
}
} // namespace wrapper
