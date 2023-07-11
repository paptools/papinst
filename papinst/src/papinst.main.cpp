// Local headers.
#include "papinst/cli.h"
#include "papinst/exception.h"
#include "papinst/logger.h"
#include "papinst/parser.h"
#include "papinst/utils.h"

// Third-party headers.
#include <fmt/format.h>

// C++ standard library headers.
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  auto logger = papinst::LoggerFactory::CreateConsoleLogger();

  try {
    auto cli_options = papinst::cli::ParseArgs(argc, argv);
    if (cli_options->status != papinst::cli::Options::Status::Success) {
      std::cout << papinst::cli::GetUsageMessage() << std::endl;
      return EXIT_FAILURE;
    } else if (cli_options->help) {
      std::cout << papinst::cli::GetUsageMessage() << std::endl;
      return EXIT_SUCCESS;
    } else if (cli_options->version) {
      std::cout << papinst::cli::GetVersionMessage() << std::endl;
      return EXIT_SUCCESS;
    }

    if (cli_options->verbose) {
      logger->SetLevel(papinst::Logger::Level::Debug);
    }

    // Parse and instrument.
    papinst::Parser parser(logger, cli_options->dry_run);
    auto orig_command = cli_options->command;
    auto inst_filepaths = parser.ParseCompileCommand(cli_options->command);

    // Execute the original command.
    std::string orig_command_str = papinst::utils::ToString(orig_command, ' ');
    std::string inst_command_str =
        papinst::utils::ToString(cli_options->command, ' ');
    logger->Debug(fmt::format("Executing command '{}'.", inst_command_str));
    int exit_status =
        std::system((cli_options->dry_run) ? orig_command_str.c_str()
                                           : inst_command_str.c_str());
    if (exit_status < 0) {
      logger->Error(fmt::format("Error: {}", strerror(errno)));
    } else if (WIFEXITED(exit_status)) {
      exit_status = WEXITSTATUS(exit_status);
    }
    for (auto &inst_filepath : inst_filepaths) {
      // papinst::utils::RemoveInstFile(inst_filepath);
      logger->Warning("File removal temporarily disabled.");
    }

    return exit_status;
  } catch (const papinst::Exception &ex) {
    logger->Error(fmt::format("Error: {}", ex.what()));
    return EXIT_FAILURE;
  } catch (const std::exception &ex) {
    logger->Error(
        fmt::format("An unexpected exception was caught: {}", ex.what()));
    return EXIT_FAILURE;
  }
}
