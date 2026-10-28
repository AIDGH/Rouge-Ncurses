#ifndef CHANGE_CHARACTER_H
#define CHANGE_CHARACTER_H
#include <ncurses.h>
#include <wchar.h>
void draw_player_avatar(WINDOW *win,int y,int x);

extern wchar_t character;

void character_change();

#endif
