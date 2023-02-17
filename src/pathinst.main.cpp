#include "pathinst/clang_parser.h"
#include "pathinst/cli.h"
#include "pathinst/exception.h"
#include "pathinst/parser.h"
#include "pathinst/utils.h"

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
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

    // Parse and instrument.
    auto parser = std::make_shared<pathinst::ClangParser>(
        std::make_shared<pathinst::Instrumenter>());
    auto source_files = parser->ParseCompileCommand(cli_options->command);

    // Execute the original command.
    std::string command = pathinst::utils::ToString(cli_options->command, ' ');
    spdlog::debug("Executing command '" + command + "'.");
    int exit_status = std::system(command.c_str());
    if (exit_status < 0) {
      std::cerr << "Error: " << strerror(errno) << std::endl;
    } else if (WIFEXITED(exit_status)) {
      exit_status = WEXITSTATUS(exit_status);
    }
    for (auto &source_file : source_files) {
      pathinst::utils::RestoreOriginalFile(source_file);
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
