#!/usr/bin/env bash
set -euo pipefail

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
build_dir="${BUILD_DIR:-/tmp/apc-introduction-build}"
cc="${CC:-clang}"
binary="$build_dir/apc-introduction"
raii_binary="$build_dir/apc-raii"
compat_object="$build_dir/win32_compat.o"
output_file="$build_dir/output.txt"
expected="Hello from APC. Got: 42"

mkdir -p "$build_dir"
"$cc" \
  -std=c11 -Wall -Wextra -Wpedantic -Werror \
  -Wno-empty-body -Wno-unused-parameter \
  -pthread -I"$root/test" \
  "$root/main.c" "$root/test/win32_compat.c" \
  -o "$binary"

"$binary" >"$output_file" 2>&1 &
process_id=$!

cleanup() {
  if kill -0 "$process_id" 2>/dev/null; then
    kill "$process_id"
    wait "$process_id" 2>/dev/null || true
  fi
}
trap cleanup EXIT INT TERM

observed=false
for _ in {1..100}; do
  if grep -Fq "$expected" "$output_file"; then
    observed=true
    break
  fi
  if ! kill -0 "$process_id" 2>/dev/null; then
    break
  fi
  sleep 0.02
done

cat "$output_file"
if [[ "$observed" != true ]]; then
  printf 'Expected APC output was not observed.\n' >&2
  exit 1
fi

kill "$process_id"
wait "$process_id" 2>/dev/null || true
trap - EXIT INT TERM

printf 'Observed the APC output and terminated the intentionally infinite example.\n'

"$cc" \
  -std=c11 -Wall -Wextra -Wpedantic -Werror -pthread -DAPC_NO_C_MAIN \
  -I"$root/test" -c "$root/test/win32_compat.c" -o "$compat_object"
"${CXX:-clang++}" \
  -std=c++20 -Wall -Wextra -Wpedantic -Werror \
  -Wno-empty-body -pthread -DAPC_OWNERSHIP_TEST -I"$root/test" \
  "$root/ownership.cpp" "$compat_object" -o "$raii_binary"

output_file="$build_dir/raii-output.txt"
expected="Hello from RAII APC. Got: 42"
"$raii_binary" >"$output_file" 2>&1 &
process_id=$!
trap cleanup EXIT INT TERM

observed=false
for _ in {1..100}; do
  if grep -Fq "$expected" "$output_file"; then
    observed=true
    break
  fi
  if ! kill -0 "$process_id" 2>/dev/null; then
    break
  fi
  sleep 0.02
done

cat "$output_file"
if [[ "$observed" != true ]]; then
  printf 'Expected RAII APC output was not observed.\n' >&2
  exit 1
fi

kill "$process_id"
wait "$process_id" 2>/dev/null || true
trap - EXIT INT TERM

printf 'Observed the RAII APC output and terminated its intentionally infinite test.\n'
