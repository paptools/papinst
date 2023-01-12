#include "wrapper/program_options.h"

#include <iostream>

int main(int argc, char **argv) {
  auto options = wrapper::CreateProgramOptions(argc, argv);
  if (!options->message.empty()) {
    std::cout << options->message << std::endl;
    return (options->parse_status == wrapper::ProgramOptions::Status::Ok)
               ? EXIT_SUCCESS
               : EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
