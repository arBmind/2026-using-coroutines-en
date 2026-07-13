#!/usr/bin/env bash
set -euo pipefail

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
port="${SLIDEV_SCREENSHOT_PORT:-3037}"
output_dir="${SLIDEV_SCREENSHOT_DIR:-$root/screenshots}"
server_log="${TMPDIR:-/tmp}/slidev-screenshots-${port}.log"

cd "$root"

npx slidev src/slides.md --port "$port" >"$server_log" 2>&1 &
server_pid=$!

cleanup() {
  kill "$server_pid" 2>/dev/null || true
  wait "$server_pid" 2>/dev/null || true
}
trap cleanup EXIT INT TERM

for _ in {1..30}; do
  if curl --fail --silent "http://localhost:${port}/" >/dev/null 2>&1; then
    break
  fi
  if ! kill -0 "$server_pid" 2>/dev/null; then
    printf 'Slidev stopped before becoming ready. Log:\n' >&2
    sed -n '1,160p' "$server_log" >&2
    exit 1
  fi
  sleep 1
done

if ! curl --fail --silent "http://localhost:${port}/" >/dev/null 2>&1; then
  printf 'Timed out waiting for Slidev. Log:\n' >&2
  sed -n '1,160p' "$server_log" >&2
  exit 1
fi

node scripts/capture-slides.mjs \
  --base-url "http://localhost:${port}" \
  --output "$output_dir" \
  --slides src/slides.md
