#ifndef LOGIN_H
#define LOGIN_H

#include <ncurses.h>

extern char our_user[50];
extern char which_menu[50];
extern char which_user[50];

void get_input_login(WINDOW *win, int y, int x, char *prompt, char *input, int length);
int username_exists_login(const char *username);
int check_password(const char *username, const char *password);
void login_screen();

#endif
