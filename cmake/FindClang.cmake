message(STATUS "Finding Clang")

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})

message(STATUS "Found Clang ${LLVM_PACKAGE_VERSION}")
