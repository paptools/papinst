message(STATUS "Finding Benchmark")

set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_ENABLE_EXCEPTIONS OFF)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)

include(FetchContent)

FetchContent_Declare(benchmark
  GIT_REPOSITORY "https://github.com/google/benchmark.git"
  GIT_TAG "v1.7.1"
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(benchmark)
