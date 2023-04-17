# Copyright (c) 2023-present, Ian Dinwoodie.
# Distributed under the MIT License (http://opensource.org/licenses/MIT).

set(fmt_version "9.1.0")
message(STATUS "Fetching fmt ${fmt_version}")

include(FetchContent)

FetchContent_Declare(fmt
  GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
  GIT_TAG "${fmt_version}"
)

FetchContent_MakeAvailable(fmt)
