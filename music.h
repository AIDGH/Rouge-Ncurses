#ifndef MUSIC_H
#define MUSIC_H
#include <ncurses.h>
void music_tick(void);
void music_stop_playlist(void);
int music_getch(WINDOW *win);

char* display_music_menu();
void play_music(const char *filepath);

#endif
