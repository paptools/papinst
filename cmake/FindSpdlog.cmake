# Copyright (c) 2023-present, Ian Dinwoodie.
# Distributed under the MIT License (http://opensource.org/licenses/MIT).

set(spdlog_version "1.11.0")
message(STATUS "Fetching spdlog ${spdlog_version}")

include(FetchContent)

FetchContent_Declare(spdlog
  GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
  GIT_TAG "v${spdlog_version}"
)

FetchContent_MakeAvailable(spdlog)
