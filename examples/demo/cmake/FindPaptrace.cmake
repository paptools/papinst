# Copyright (c) 2023-present, Ian Dinwoodie.
# Distributed under the MIT License (http://opensource.org/licenses/MIT).

message(STATUS "Finding paptrace")

include(FetchContent)

FetchContent_Declare(paptrace
  GIT_REPOSITORY "https://github.com/paptools/paptrace.git"
  GIT_TAG "main"
  OVERRIDE_FIND_PACKAGE
)

#set(PAPTRACE_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(paptrace)

message(STATUS "Found paptrace ${paptrace_VERSION}")
