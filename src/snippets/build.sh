#!/usr/bin/env bash
set -euo pipefail

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
"$root/apc-introduction/build.sh"
"$root/slide-projects/build.sh"
"$root/coroutine-introduction/build.sh"
"$root/coroutine-lifetime/build.sh"
"$root/compiled-patterns/build.sh"
