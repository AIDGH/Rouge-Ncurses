#!/bin/bash
set -uo pipefail
cd -- "$(dirname -- "$0")"
if ! ./build-macos.sh; then
  printf '\nBuild failed. Press Enter to close.\n'; read -r _; exit 1
fi
export TERM=xterm-256color
export LANG=en_US.UTF-8
mkdir -p build-macos/logs
log="build-macos/logs/run-$(date +%Y%m%d-%H%M%S).log"
printf '\033[8;55;180t'
./build-macos/rouge-dungeon 2>"$log"
result=$?
stty sane
printf '\033[?1003l\033[?1002l\033[?1000l\033[?1006l\033[?1005l\033[?1015l\033>\033[0m\033[?25h\n'
if [[ $result -ne 0 && $result -ne 130 ]]; then
  printf 'Game ended with status %s. Diagnostic log: %s\n' "$result" "$PWD/$log"
fi
printf 'Press Enter to close.\n'; read -r _
exit "$result"
