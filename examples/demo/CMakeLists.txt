cmake_minimum_required(VERSION 3.24)
project(paptools_demo C CXX)

################################################################################
# Project Settings.

set(CMAKE_CXX_STANDARD 17)
include(GNUInstallDirs)

################################################################################
# Options.
option(DEMO_PERF "Enable performance tests." OFF)

################################################################################
# Third Party Dependencies.

if (DEMO_PERF)
  include("cmake/FindPaptrace.cmake")

  # This assumes that paptools was installed using the provided build script.
  find_package(paptools REQUIRED
    PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../../install"
  )
endif ()

include("cmake/FindGoogleTest.cmake")
include("cmake/FindBenchmark.cmake")

################################################################################
# Demo Library.

add_library(demo
  "src/demo.cpp"
)

target_include_directories(demo
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if (DEMO_PERF)
  paptrace_trace_target(demo)
endif ()

################################################################################
# Project Tests.

enable_testing()

add_executable(demo_unit_tests
  "tests/demo.test.cpp"
)

target_link_libraries(demo_unit_tests
  PRIVATE
    demo
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(demo_unit_tests)

################################################################################
# Project Benchmarks.

add_executable(demo_benchmarks
  "benchmarks/demo.benchmark.cpp"
)

target_link_libraries(demo_benchmarks
  PRIVATE
    demo
    benchmark::benchmark_main
)

################################################################################
