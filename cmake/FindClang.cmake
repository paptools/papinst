message(STATUS "Finding Clang")

find_package(Clang REQUIRED CONFIG)

message(STATUS "Found Clang ${LLVM_PACKAGE_VERSION}")
