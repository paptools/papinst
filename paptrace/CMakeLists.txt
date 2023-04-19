# Copyright (c) 2023-present, Ian Dinwoodie.
# Distributed under the MIT License (http://opensource.org/licenses/MIT).

################################################################################
# Google Test library.

#add_library(mygtest STATIC
#  "src/gmock-gtest-all.cc"
#  "include/gmock/gmock.hh"
#  "include/gtest/gtest.hh"
#)
#target_compile_definitions(mygtest PUBLIC GTEST_HAS_STD_WSTRING=1)
#target_include_directories(mygtest SYSTEM
#  PUBLIC
#    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#  INTERFACE
#    $<INSTALL_INTERFACE:include>
#)
#
#find_package(Threads)
#if(Threads_FOUND)
#  target_link_libraries(gtest ${CMAKE_THREAD_LIBS_INIT})
#else()
#  target_compile_definitions(gtest PUBLIC GTEST_HAS_PTHREAD=0)
#endif()

################################################################################
# Path Trace library.

add_library(paptrace STATIC
  "src/paptrace.cpp"
  "src/gtest.cc"
)

target_include_directories(paptrace
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  INTERFACE
    $<INSTALL_INTERFACE:include>
)

set(paptrace_public_headers
  "include/paptrace/paptrace.h"
)

set_target_properties(paptrace
  PROPERTIES
    PUBLIC_HEADER "${paptrace_public_headers}"
)

target_link_libraries(paptrace
  PUBLIC
    nlohmann_json::nlohmann_json
)

################################################################################
