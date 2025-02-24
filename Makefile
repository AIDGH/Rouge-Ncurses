OBJS=menu.o outro.o music.o sign_up.o login.o before_game_menu.o guest_before_game_menu.o settings.o color_change.o profile.o scoreboard.o difficulty.o change_character.o user.o pause_menu.o game.o map_generator.o main.o
CFLAGS=-lncursesw  -fsanitize=address -g -lSDL2 -lSDL2_mixer
all: exec

run: exec
	./exec

exec: $(OBJS) Makefile
	gcc $(OBJS) $(CFLAGS) -o exec


%.o: %.c Makefile
	gcc -c -lncursesw -o $@ $<