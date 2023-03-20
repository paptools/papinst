#!/usr/bin/env bash

function main {
  local script_dir
  script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
  local repo_dir="${script_dir}/.."

  cd "${repo_dir}" || return 1
  find . \(-iname "*.h" -o -iname "*.cpp"\) -exec clang-format -i {} \;
  black -l 80 --preview .
  echo "Formatted paptools code."
}

main "$@"
