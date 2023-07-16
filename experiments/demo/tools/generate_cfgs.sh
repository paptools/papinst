#!/usr/bin/env bash

function main {
  # local joern_exe="$(which joern)"
  local joern_exe="/Users/ird/dev/github/joernio/joern/joern"
  if [ -z "$joern_exe" ]; then
    echo "Joern not found. Please install it first."
    exit 1
  fi

  local script_dir
  script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
  local exp_dir="${script_dir}/.."

  cd "${exp_dir}" || return 1
  local cfg_dir
  cfg_dir="$(PWD)/figures/cfgs"
  mkdir -p "${cfg_dir}"

  cd joern || return 1
  $joern_exe --script joern.sc

  local fmt="png"
  # shellcheck disable=SC2038
  (cd dots \
    && find . -iname "*.dot" \
    | xargs -t -n1 -P2 -I{} dot -T"${fmt}" {} -o "${cfg_dir}"/{}."${fmt}")

  cd "${cfg_dir}" || return 1
  # shellcheck disable=SC2010
  ls . | grep \.dot\.${fmt} | sed "p;s/\.dot.${fmt}/\.${fmt}/" | xargs -n2 mv
}

main "$@"
