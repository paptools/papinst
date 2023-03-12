#!/usr/bin/env bash

function main {
  local script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
  local vunit_repo_dir="${script_dir}/.."

  cd "${vunit_repo_dir}"
  clang-format -style=llvm -i include/*/*.h src/*.cpp tests/*.cpp \
      tests/mocks/*.h examples/*.cpp
  echo "Formatted pathinst code."
}

main "$@"
