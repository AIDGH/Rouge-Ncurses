#!/bin/bash
set -euo pipefail
root="$(cd -- "$(dirname -- "$0")/.." && pwd)"
scratch="$(mktemp -d "${TMPDIR:-/tmp}/rouge-music-tests.XXXXXX")"
trap 'rm -rf -- "$scratch"' EXIT
clang -std=gnu11 -g -fsanitize=address,undefined -I/opt/homebrew/opt/sdl2/include -I/opt/homebrew/opt/sdl2/include/SDL2 -I/opt/homebrew/opt/sdl2_mixer/include -I/opt/homebrew/opt/ncurses/include "$root/tests/music_checks.c" "$root/music.c" "$root/ui_theme.c" -L/opt/homebrew/opt/ncurses/lib -lncursesw -o "$scratch/checks"
cd "$scratch"
./checks
