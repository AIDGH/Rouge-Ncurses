#ifndef UI_THEME_H
#define UI_THEME_H
#include <ncurses.h>
extern int ui_old_visuals;
#define UI_TEXT(old_text,new_text) (ui_old_visuals ? (old_text) : (new_text))
int ui_choose_visuals(void);
void ui_old_backdrop(void);
void ui_settings_emblem(WINDOW *win);
void ui_connected_art(WINDOW *win);
void ui_menu_ornament(WINDOW *win, int top, int bottom);
void ui_theme(void);
void ui_frame(WINDOW *win, const char *title);
void ui_menu_backdrop(void);
#endif
