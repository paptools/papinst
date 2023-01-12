message(STATUS "Finding Boost (NOTE: This may take a while the first time)")

set(BOOST_ENABLE_CMAKE ON)
set(BOOST_INCLUDE_LIBRARIES program_options)

include(FetchContent)

FetchContent_Declare(Boost
  GIT_REPOSITORY "https://github.com/boostorg/boost.git"
  GIT_TAG "boost-1.80.0"
  GIT_SHALLOW TRUE
  #GIT_PROGRESS $FETCHCONTENT_QUIET
)

FetchContent_MakeAvailable(Boost)
