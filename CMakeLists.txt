# Copyright (c) 2023-present, Ian Dinwoodie.
# Distributed under the MIT License (http://opensource.org/licenses/MIT).

cmake_minimum_required(VERSION 3.14)

################################################################################
# Project settings.

project(pathinst VERSION 0.1.0 LANGUAGES CXX C)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose Release or Debug" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Use standardized build/install paths.
include(GNUInstallDirs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

################################################################################
# Compiler settings.

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
# Path instrumenter instrumentation library.

add_library(pathinst STATIC
  "src/pathinst.cpp"
)

target_include_directories(pathinst
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  INTERFACE
    $<INSTALL_INTERFACE:include>
)

set(pathinst_public_headers
  "include/pathinst/pathinst.h"
)

set_target_properties(pathinst
  PROPERTIES
    PUBLIC_HEADER "${pathinst_public_headers}"
)

################################################################################
# Path instrumenter executable library.

add_library(pathinst_exe_lib
  "src/cli.cpp"
  "src/frontend_action.cpp"
  "src/instrumenter.cpp"
  "src/parser.cpp"
  "src/utils.cpp"
)

target_include_directories(pathinst_exe_lib
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CLANG_INCLUDE_DIRS}
    ${LLVM_INCLUDE_DIRS}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests/>
)

target_compile_definitions(pathinst_exe_lib
  PUBLIC
    ${CLANG_DEFINITIONS}
    ${LLVM_DEFINITIONS}
)

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(pathinst_exe_lib
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

add_executable(pathinst_exe
  "src/pathinst.main.cpp"
)

target_link_libraries(pathinst_exe
  PRIVATE
    pathinst_exe_lib
)

set_target_properties(pathinst_exe
  PROPERTIES
    OUTPUT_NAME "pathinst"
)

################################################################################
# Path instrumenter executable wrapper script.

configure_file("tools/pcc" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/pcc" COPYONLY)

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
    pathinst_exe_lib
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
