# Copyright (c) 2023-present, Ian Dinwoodie.
# Distributed under the MIT License (http://opensource.org/licenses/MIT).

cmake_minimum_required(VERSION 3.14)

################################################################################
# CMake policies.

cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0135 NEW)

################################################################################
# Project settings.

project(paptools VERSION 0.1.0 LANGUAGES CXX C)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      "Debug"
      CACHE STRING "Choose Release or Debug" FORCE)
endif ()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Use standardized build/install paths.
include(GNUInstallDirs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

################################################################################
# Options.

option(PAPTOOLS_INSTALL "Generate the install targets." ON)

################################################################################
# Compiler settings.

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

# add_definitions(-Wall -Wextra -Werror)

################################################################################
# Third party libraries.

include("cmake/FindFmt.cmake")
include("cmake/FindBoost.cmake")
include("cmake/FindClang.cmake")
include("cmake/FindGoogleTest.cmake")
include("cmake/FindSpdlog.cmake")

################################################################################
# Project subdirectories.

# Declared here for use in the subdirectory install rules.
set(targets_export_name "${PROJECT_NAME}-targets")
set(install_targets "")

add_subdirectory("papinst")

################################################################################
# Install rules.

if (PAPTOOLS_INSTALL)
  set(project_cmake_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "")
  export(TARGETS ${install_targets}
    NAMESPACE "${PROJECT_NAME}::"
    FILE "${PROJECT_BINARY_DIR}/${targets_export_name}.cmake"
  )
  install(EXPORT "${targets_export_name}"
    FILE "${targets_export_name}.cmake"
    NAMESPACE "${PROJECT_NAME}::"
    DESTINATION "${project_cmake_dir}"
  )

  include(CMakePackageConfigHelpers)
  set(project_config "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake")
  configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${project_cmake_dir}"
  )
  set(version_config "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake")
  write_basic_package_version_file(
    "${version_config}"
    VERSION "${paptools_VERSION}"
    COMPATIBILITY AnyNewerVersion
  )
  install(FILES "${project_config}" "${version_config}"
    DESTINATION "${project_cmake_dir}"
  )
endif ()

################################################################################
