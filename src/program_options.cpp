#include "wrapper/program_options.h"

#include <boost/program_options.hpp>

#include <memory>
#include <sstream>
#include <string>

namespace wrapper {
namespace {
std::string k_usage_message =
    "Usage: wcc [options] -- <compiler> [compiler options]\n";
} // namespace

std::unique_ptr<ProgramOptions> CreateProgramOptions(int argc, char **argv) {
  namespace po = boost::program_options;
  auto options = std::make_unique<ProgramOptions>();
  try {
    po::options_description desc("Options");
    bool help_requested = false;
    desc.add_options()("help,h", po::bool_switch(&help_requested),
                       "Display this help message");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (help_requested) {
      std::ostringstream oss;
      oss << k_usage_message << desc;
      options->message = oss.str();
    }

    options->parse_status = ProgramOptions::Status::Ok;
  } catch (const po::error &e) {
    options->message = e.what();
  }

  return options;
}
} // namespace wrapper
