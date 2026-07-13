#!/usr/bin/env bash
set -euo pipefail

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
"$root/coroutine-introduction/build.sh"
"$root/coroutine-lifetime/build.sh"
"$root/compiled-patterns/build.sh"
