message(STATUS "Finding Clang")

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})

message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
message(STATUS "Clang include dirs: ${CLANG_INCLUDE_DIRS}")
message(STATUS "LLVM definitions: ${LLVM_DEFINITIONS}")
message(STATUS "Clang definitions: ${CLANG_DEFINITIONS}")

message(STATUS "Found Clang ${LLVM_PACKAGE_VERSION}")
