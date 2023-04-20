# Copyright (c) 2023-present, Ian Dinwoodie.
# Distributed under the MIT License (http://opensource.org/licenses/MIT).

message(STATUS "Finding Boost")

set(BOOST_INCLUDE_LIBS program_options filesystem)

find_package(Boost REQUIRED CONFIG COMPONENTS ${BOOST_INCLUDE_LIBS})

if (NOT Boost_FOUND)
  set(boost_version "1.80.0")
  message(STATUS "Fetching Boost ${boost_version} (NOTE: This may take a while the first time)")

  set(BOOST_ENABLE_CMAKE ON)

  include(FetchContent)
  FetchContent_Declare(Boost
    GIT_REPOSITORY "https://github.com/boostorg/boost.git"
    GIT_TAG "boost-${boost_version}"
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(Boost)
endif ()

message(STATUS "Found Boost ${Boost_VERSION}")
