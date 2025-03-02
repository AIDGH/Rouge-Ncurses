#ifndef USER_INPUT_H
#define USER_INPUT_H

#include <ncurses.h>

extern char our_user[50];

void get_input_sign_up(WINDOW *win, int y, int x, char *prompt, char *input, int length);
int username_exists_sign_up(const char *username);
void save_user_info(const char *username, const char *email, const char *password);
void user_input_screen();
int validate_password(const char *password);
int validate_email(const char *email);
void generate_random_password(char *password, int length);

#endif
