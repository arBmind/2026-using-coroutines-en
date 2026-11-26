#!/usr/bin/env bash
set -euo pipefail

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
snippets_root="$(cd -- "$root/.." && pwd)"
desktop_project="$snippets_root/desktop-duplication-cpp"
hourglass_project="$snippets_root/hourglass-app2"
build_dir="${BUILD_DIR:-/tmp/slide-projects-build}"
cxx="${CXX:-clang++}"

strip_regions() {
  sed '/^[[:space:]]*\/\/ #\(end\)\?region /d' "$1"
}

if [[ -e "$desktop_project/.git" ]]; then
  [[ -z "$(git -C "$desktop_project" status --porcelain)" ]]
  diff -u \
    <(strip_regions "$root/desktop-duplication-cpp/src/DuplicationController.cpp") \
    "$desktop_project/src/DuplicationController.cpp"
fi

if [[ -e "$hourglass_project/.git" ]]; then
  [[ -z "$(git -C "$hourglass_project" status --porcelain)" ]]
  hourglass_excerpt="$root/hourglass-app2/src/Hourglass/Model"
  hourglass_upstream="$hourglass_project/src/Hourglass/Model"
  diff -u \
    <(strip_regions "$hourglass_excerpt/NetworkTask.h" | sed -n '/^        auto await_transform(/,$p') \
    <(sed -n '/^        auto await_transform(/,/^    };$/p' "$hourglass_upstream/NetworkTask.h" | sed '$d')
  for function in refreshFromServerImpl startTimeTrackerImpl; do
    range="/^auto RedmineAccount::$function()/,/^}$/p"
    diff -u \
      <(sed -n "$range" "$hourglass_excerpt/RedmineAccount.cpp") \
      <(sed -n "$range" "$hourglass_upstream/RedmineAccount.cpp")
  done
fi

rg -q '#region resume-on-awaitable' "$root/desktop-duplication-cpp/src/win32/Thread.h"
rg -q '#region run-capture-coroutine' "$root/desktop-duplication-cpp/src/DuplicationController.cpp"
rg -q '#region qt-reply-await-bridge' "$root/hourglass-app2/src/Hourglass/Model/NetworkTask.h"
rg -q '#region start-time-tracker-coroutine' "$root/hourglass-app2/src/Hourglass/Model/RedmineAccount.cpp"

mkdir -p "$build_dir"
flags=(-std=c++20 -Wall -Wextra -Wpedantic -Werror -I"$root/test/stubs")
"$cxx" "${flags[@]}" "$root/test/void_coroutine.cpp" -o "$build_dir/void-coroutine"
"$cxx" "${flags[@]}" "$root/test/resume_on.cpp" -o "$build_dir/resume-on"
"$build_dir/void-coroutine"
"$build_dir/resume-on"

printf 'Tested the slide-owned coroutine sources.\n'
