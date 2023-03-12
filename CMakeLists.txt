# Copyright (c) 2023-present, Ian Dinwoodie.
# Distributed under the MIT License (http://opensource.org/licenses/MIT).

cmake_minimum_required(VERSION 3.14)

################################################################################
# Project settings.

project(perfinst)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose Release or Debug" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

################################################################################
# Compiler settings.

# std::enable_if requires at least C++14.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

#add_definitions(-Wall -Wextra -Werror)

################################################################################
# Third party libraries.

include("cmake/FindBoost.cmake")
include("cmake/FindClang.cmake")
include("cmake/FindGoogleTest.cmake")
include("cmake/FindSpdlog.cmake")

################################################################################
# Path instrumenter library.

add_library(pathinstlib
  "src/cli.cpp"
  "src/frontend_action.cpp"
  "src/instrumenter.cpp"
  "src/parser.cpp"
  "src/utils.cpp"
)

target_include_directories(pathinstlib
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CLANG_INCLUDE_DIRS}
    ${LLVM_INCLUDE_DIRS}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests/>
)

target_compile_definitions(pathinstlib
  PUBLIC
    ${CLANG_DEFINITIONS}
    ${LLVM_DEFINITIONS}
)

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(pathinstlib
  PUBLIC
    Boost::program_options
    Boost::filesystem
    Boost::headers
    spdlog::spdlog
    clangTooling
    ${llvm_libs}
)

################################################################################
# Path instrumenter executable.

add_executable(pathinst
  "src/pathinst.main.cpp"
)

target_link_libraries(pathinst
  PRIVATE
    pathinstlib
)

################################################################################
# Unit tests.

add_executable(unit_tests
  "tests/cli.test.cpp"
  "tests/frontend_action.test.cpp"
)

target_include_directories(unit_tests
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests>
)

target_link_libraries(unit_tests
  PRIVATE
    pathinstlib
    gtest
    gtest_main
    gmock
)

add_test(NAME integration_cli_tests
  COMMAND
    pytest "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_cli.py"
)

include(GoogleTest)
gtest_discover_tests(unit_tests)

################################################################################
# Wrapper script.

configure_file("tools/pcc" "${CMAKE_BINARY_DIR}/pcc" COPYONLY)

################################################################################
