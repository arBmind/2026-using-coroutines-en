#!/usr/bin/env bash
set -euo pipefail

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
build_dir="${BUILD_DIR:-/tmp/coroutine-lifetime-build}"
cxx="${CXX:-clang++-22}"

if ! command -v "$cxx" >/dev/null 2>&1; then
  cxx=clang++
fi

mkdir -p "$build_dir"
"$cxx" -std=c++20 -Wall -Wextra -Wpedantic -Werror "$root/main.cpp" -o "$build_dir/lifetime"
[[ "$($build_dir/lifetime)" == "important" ]]

printf 'Compiled coroutine lifetime example with %s.\n' "$cxx"
