#!/usr/bin/env bash

function main {
  local script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
  local repo_dir="${script_dir}/.."

  cd "${repo_dir}"
  find . -iname *.h -o -iname *.cpp | xargs clang-format -i
  echo "Formatted paptools code."
}

main "$@"
