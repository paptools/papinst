# Copyright (c) 2023-present, Ian Dinwoodie.
# Distributed under the MIT License (http://opensource.org/licenses/MIT).

################################################################################
# Path Trace library.

add_library(paptrace STATIC
  "src/paptrace.cpp"
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
