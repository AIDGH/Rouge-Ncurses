CC = cc
UNAME_S := $(shell uname -s)
SOURCES := main.c menu.c ui_theme.c accounts.c account_ui.c outro.c music.c sign_up.c login.c before_game_menu.c guest_before_game_menu.c map_generator.c settings.c color_change.c profile.c scoreboard.c difficulty.c change_character.c user.c pause_menu.c game.c
HEADERS := $(wildcard *.h *.inc)
CFLAGS ?= -std=gnu11 -g
ifeq ($(UNAME_S),Darwin)
BUILD := build-macos
CPPFLAGS += -I/opt/homebrew/opt/sdl2/include -I/opt/homebrew/opt/sdl2/include/SDL2 -I/opt/homebrew/opt/sdl2_mixer/include -I/opt/homebrew/opt/ncurses/include
LDFLAGS += -L/opt/homebrew/opt/sdl2/lib -L/opt/homebrew/opt/sdl2_mixer/lib -L/opt/homebrew/opt/ncurses/lib
else
BUILD := build-linux
LDLIBS += -lcrypto
CPPFLAGS += $(shell pkg-config --cflags sdl2 SDL2_mixer ncursesw 2>/dev/null)
endif
LDLIBS += -lncursesw -lSDL2 -lSDL2_mixer -lsqlite3 -lm
OBJECTS := $(SOURCES:%.c=$(BUILD)/obj/%.o)
TARGET := $(BUILD)/rouge-dungeon

.PHONY: all run clean
all: $(TARGET)
run: $(TARGET)
	./$(TARGET)
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@
$(BUILD)/obj/%.o: %.c $(HEADERS) Makefile
	@mkdir -p $(BUILD)/obj
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
clean:
	rm -rf $(BUILD)/obj $(TARGET)
