#!/bin/bash
set -euo pipefail
cd -- "$(dirname -- "$0")"
mkdir -p build-macos
if ! make --quiet all >build-macos/build.log 2>&1; then
  cat build-macos/build.log >&2
  exit 1
fi
