# Copyright (c) 2023-present, Ian Dinwoodie.
# Distributed under the MIT License (http://opensource.org/licenses/MIT).

set(spdlog_version "1.11.0")
message(STATUS "Fetching spdlog ${spdlog_version}")

set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "Enable external FMTLIB in spdlog" FORCE)
set(SPDLOG_INSTALL ON CACHE BOOL "")

include(FetchContent)
FetchContent_Declare(spdlog
  URL "https://github.com/gabime/spdlog/archive/refs/tags/v${spdlog_version}.tar.gz"
)
FetchContent_MakeAvailable(spdlog)
