#!/usr/bin/env bash
set -euo pipefail

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
build_dir="${BUILD_DIR:-/tmp/coroutine-introduction-build}"
cxx="${CXX:-}"

if [[ -z "$cxx" ]]; then
  if command -v clang++-22 >/dev/null 2>&1; then
    cxx=clang++-22
  else
    cxx=clang++
  fi
fi

major="$($cxx --version | sed -nE '1s/.*version ([0-9]+).*/\1/p')"
flags=(-Wall -Wextra -Wpedantic -Werror)
if [[ "${major:-0}" -ge 22 ]]; then
  flags+=(-std=c++23 -stdlib=libc++)
else
  flags+=(-std=c++20)
fi

mkdir -p "$build_dir"
for source in synchronous coroutine; do
  "$cxx" "${flags[@]}" "$root/$source.cpp" -o "$build_dir/$source"
  output="$($build_dir/$source)"
  [[ "$output" == "Hello Meeting C++ 2026!" ]]
done

printf 'Compiled with %s %s; both examples produced the expected greeting.\n' "$cxx" "$major"
