# Copyright (c) 2023-present, Ian Dinwoodie.
# Distributed under the MIT License (http://opensource.org/licenses/MIT).

set(fmt_version "9.1.0")
message(STATUS "Finding fmt ${fmt_version}")

find_package(fmt ${fmt_version})

if (NOT fmt_FOUND)
  message(STATUS "Fetching fmt ${fmt_version}")

  set(FMT_INSTALL ON CACHE BOOL "")

  include(FetchContent)
  FetchContent_Declare(fmt
    URL "https://github.com/fmtlib/fmt/archive/refs/tags/${fmt_version}.tar.gz"
  )
  FetchContent_MakeAvailable(fmt)
endif ()
