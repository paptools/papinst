# Copyright (c) 2023-present, Ian Dinwoodie.
# Distributed under the MIT License (http://opensource.org/licenses/MIT).

################################################################################
# Path instrumenter executable library.

add_library(pathinst_lib
  "src/cli.cpp"
  "src/frontend_action.cpp"
  "src/instrumenter.cpp"
  "src/parser.cpp"
  "src/utils.cpp"
)

target_include_directories(pathinst_lib
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CLANG_INCLUDE_DIRS}
    ${LLVM_INCLUDE_DIRS}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests/>
)

target_compile_definitions(pathinst_lib
  PUBLIC
    ${CLANG_DEFINITIONS}
    ${LLVM_DEFINITIONS}
)

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(pathinst_lib
  PUBLIC
    Boost::program_options
    Boost::filesystem
    Boost::headers
    spdlog::spdlog
    clangTooling
    ${llvm_libs}
)

set_target_properties(pathinst_lib
  PROPERTIES
    OUTPUT_NAME "pathinst"
)

################################################################################
# Path instrumenter executable.

add_executable(pathinst
  "src/pathinst.main.cpp"
)

target_link_libraries(pathinst
  PRIVATE
    pathinst_lib
)

set_target_properties(pathinst
  PROPERTIES
    OUTPUT_NAME "pathinst"
)

################################################################################
# Path instrumenter executable wrapper script.

configure_file("tools/pcc" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/pcc" COPYONLY)

################################################################################
# Unit tests.

add_executable(pathinst_unit_tests
  "tests/cli.test.cpp"
  "tests/frontend_action.test.cpp"
)

target_include_directories(pathinst_unit_tests
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests>
)

target_link_libraries(pathinst_unit_tests
  PRIVATE
    pathinst_lib
    gtest
    gtest_main
    gmock
)

add_test(NAME integration_cli_tests
  COMMAND
    pytest "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_cli.py"
)

include(GoogleTest)
gtest_discover_tests(pathinst_unit_tests)

################################################################################
