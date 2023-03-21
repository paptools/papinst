# Copyright (c) 2023-present, Ian Dinwoodie. Distributed under the MIT License
# (http://opensource.org/licenses/MIT).

cmake_minimum_required(VERSION 3.14)

# ##############################################################################
# CMake policies.

cmake_policy(SET CMP0048 NEW)

# ##############################################################################
# Project settings.

project(
  paptools
  VERSION 0.1.0
  LANGUAGES CXX C)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      "Debug"
      CACHE STRING "Choose Release or Debug" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Use standardized build/install paths.
include(GNUInstallDirs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# ##############################################################################
# Compiler settings.

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# add_definitions(-Wall -Wextra -Werror)

# ##############################################################################
# Third party libraries.

include("cmake/FindBoost.cmake")
include("cmake/FindClang.cmake")
include("cmake/FindGoogleTest.cmake")
include("cmake/FindJson.cmake")
include("cmake/FindSpdlog.cmake")

# ##############################################################################
# Project subdirectories.

add_subdirectory("papinst")
add_subdirectory("pathtrace")

# ##############################################################################
