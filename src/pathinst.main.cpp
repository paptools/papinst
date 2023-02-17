#include "pathinst/cli.h"
#include "pathinst/exception.h"
#include "pathinst/parser.h"
#include "pathinst/utils.h"

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  try {
    auto cli_options = pathinst::cli::ParseArgs(argc, argv);
    if (cli_options->status != pathinst::cli::Options::Status::Success) {
      std::cout << pathinst::cli::GetUsageMessage() << std::endl;
      return EXIT_FAILURE;
    } else if (cli_options->help) {
      std::cout << pathinst::cli::GetUsageMessage() << std::endl;
      return EXIT_SUCCESS;
    } else if (cli_options->version) {
      std::cout << pathinst::cli::GetVersionMessage() << std::endl;
      return EXIT_SUCCESS;
    }

    if (cli_options->verbose) {
      spdlog::set_level(spdlog::level::debug);
    }
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%P] [%^%l%$] %v");

    // Parse and instrument.
    pathinst::Parser parser(cli_options->dry_run);
    auto orig_command = cli_options->command;
    auto inst_filepaths = parser.ParseCompileCommand(cli_options->command);

    // Execute the original command.
    std::string orig_command_str = pathinst::utils::ToString(orig_command, ' ');
    std::string inst_command_str =
        pathinst::utils::ToString(cli_options->command, ' ');
    spdlog::debug("Executing command '{}'.", inst_command_str);
    int exit_status =
        std::system((cli_options->dry_run) ? orig_command_str.c_str()
                                           : inst_command_str.c_str());
    if (exit_status < 0) {
      std::cerr << "Error: " << strerror(errno) << std::endl;
    } else if (WIFEXITED(exit_status)) {
      exit_status = WEXITSTATUS(exit_status);
    }
    for (auto &inst_filepath : inst_filepaths) {
      //pathinst::utils::RemoveInstFile(inst_filepath);
      spdlog::warn("File removal temporarily disabled.");
    }

    return exit_status;
  } catch (const pathinst::Exception &ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return EXIT_FAILURE;
  } catch (const std::exception &ex) {
    std::cerr << "An unexpected exception was caught:\n"
              << ex.what() << std::endl;
    return EXIT_FAILURE;
  }
}
