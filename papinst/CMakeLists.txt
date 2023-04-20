# Copyright (c) 2023-present, Ian Dinwoodie.
# Distributed under the MIT License (http://opensource.org/licenses/MIT).

################################################################################
# Platform-agnostic performance instrumenter library.

add_library(papinst
  "src/ast_consumer_listener.cpp"
  "src/cli.cpp"
  "src/frontend_action.cpp"
  "src/instrumenter.cpp"
  "src/logger.cpp"
  "src/parser.cpp"
  "src/utils.cpp"
  "src/ast_visitor.cpp"
)

target_include_directories(papinst
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CLANG_INCLUDE_DIRS}
    ${LLVM_INCLUDE_DIRS}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests/>
)

target_compile_definitions(papinst
  PUBLIC
    ${CLANG_DEFINITIONS}
    ${LLVM_DEFINITIONS}
)

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(papinst
  PUBLIC
    Boost::program_options
    Boost::filesystem
    Boost::headers
    spdlog::spdlog
    clangTooling
    ${llvm_libs}
    fmt::fmt
)

################################################################################
# Platform-agnostic performance instrumenter executable.

add_executable(papinst_exe "src/papinst.main.cpp")

target_link_libraries(papinst_exe PRIVATE papinst)

set_target_properties(papinst_exe PROPERTIES OUTPUT_NAME "papinst")

################################################################################
# Path instrumenter executable wrapper script.

configure_file("tools/pcc" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/pcc" COPYONLY)

################################################################################
# Unit tests.

add_executable(papinst_unit_tests
  #"tests/ast_consumer_listener.test.cpp"
  "tests/ast_visitor.test.cpp"
  "tests/cli.test.cpp"
  "tests/frontend_action.test.cpp"
)

target_include_directories(papinst_unit_tests
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests/include>
)

target_link_libraries(papinst_unit_tests
  PRIVATE
    papinst
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(papinst_unit_tests)

add_test(NAME papinst_integration_cli_tests
         COMMAND pytest "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_cli.py")

################################################################################
# Install rules.

if (PAPTOOLS_INSTALL)
  set(papinst_install_targets
    papinst
    papinst_exe
  )
  install(TARGETS ${papinst_install_targets}
    EXPORT "${targets_export_name}"
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
  install(PROGRAMS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/pcc"
    DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
  list(APPEND install_targets ${papinst_install_targets})
  set(install_targets ${install_targets} PARENT_SCOPE)
endif ()

################################################################################
