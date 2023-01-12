#include "wrapper/program_options.h"

#include <iostream>

int main(int argc, char **argv) {
  auto options = wrapper::CreateProgramOptions(argc, argv);
  if (!options->message.empty()) {
    std::cout << options->message << std::endl;
  }

  return EXIT_SUCCESS;
}
