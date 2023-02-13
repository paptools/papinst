# Copyright (c) 2023-present, Ian Dinwoodie.
# Distributed under the MIT License (http://opensource.org/licenses/MIT).

set(googletest_version "1.12.1")
message(STATUS "Fetching GoogleTest ${googletest_version}")

include(FetchContent)

FetchContent_Declare(googletest
  GIT_REPOSITORY "https://github.com/google/googletest.git"
  GIT_TAG "release-${googletest_version}"
  GIT_SHALLOW TRUE
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

