#!/bin/bash
set -euo pipefail
root="$(cd -- "$(dirname -- "$0")/.." && pwd)"
"$root/tests/build-memory-checks.sh"
scratch="$(mktemp -d "${TMPDIR:-/tmp}/rouge-ncurses-tests.XXXXXX")"
trap 'rm -rf -- "$scratch"' EXIT
cd "$scratch"
"$root/tests/memory-checks" "${1:-20}" "${2:-55}" "${3:-180}"
