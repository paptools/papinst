cmake_minimum_required(VERSION 3.25)
project(Wrapper
  VERSION 0.1.0
  DESCRIPTION "A scriptable compile command wrapper."
  LANGUAGES CXX
)

################################################################################
# Project Settings.

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

################################################################################
# Third Party Dependencies.

include("cmake/FindBoost.cmake")
include("cmake/FindGoogleTest.cmake")

################################################################################
# Wrapper Library.

add_library(wrapper
  "src/logger.cpp"
  "src/program_options.cpp"
)

target_include_directories(wrapper
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(wrapper
  PUBLIC
    Boost::program_options
)

################################################################################
# Wrapper Executable.

add_executable(wcc
  "src/main.cpp"
)

target_link_libraries(wcc
  PRIVATE
    wrapper
)

################################################################################
# Project Tests.

enable_testing()

add_executable(unit_tests
  "tests/program_options.test.cpp"
  "tests/logger.test.cpp"
)

target_link_libraries(unit_tests
  PRIVATE
    wrapper
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)

################################################################################
