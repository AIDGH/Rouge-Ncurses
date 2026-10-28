#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")/.."
clang -std=gnu11 -g -O0 -fno-omit-frame-pointer -fsanitize=address,undefined \
-I/opt/homebrew/opt/sdl2/include -I/opt/homebrew/opt/sdl2/include/SDL2 -I/opt/homebrew/opt/sdl2_mixer/include -I/opt/homebrew/opt/ncurses/include \
tests/memory_checks.c tests/audio_stub.c menu.c ui_theme.c accounts.c account_ui.c outro.c music.c sign_up.c login.c before_game_menu.c guest_before_game_menu.c map_generator.c settings.c color_change.c profile.c scoreboard.c difficulty.c change_character.c user.c pause_menu.c game.c \
-L/opt/homebrew/opt/ncurses/lib -lncursesw -lsqlite3 -lm -o tests/memory-checks
