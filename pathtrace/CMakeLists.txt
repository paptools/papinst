# Copyright (c) 2023-present, Ian Dinwoodie.
# Distributed under the MIT License (http://opensource.org/licenses/MIT).

################################################################################
# Path Trace library.

add_library(pathtrace STATIC
  "src/pathtrace.cpp"
)

target_include_directories(pathtrace
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  INTERFACE
    $<INSTALL_INTERFACE:include>
)

set(pathtrace_public_headers
  "include/pathtrace/pathtrace.h"
)

set_target_properties(pathtrace
  PROPERTIES
    PUBLIC_HEADER "${pathtrace_public_headers}"
)

target_link_libraries(pathtrace
  PUBLIC
    nlohmann_json::nlohmann_json
)

################################################################################
