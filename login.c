#include "login.h"
#include "user.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

extern char which_menu[50];
extern char which_user[50];
extern char our_user[50];
extern User current_user;

void get_input_login(WINDOW *win, int y, int x, char *prompt, char *input, int length) {
    char full_prompt[100];
    snprintf(full_prompt, sizeof(full_prompt), "%s", prompt);
    mvwprintw(win, y, x, "%s", full_prompt);
    wrefresh(win);

    int ch, i = 0;
    while (1) {
        ch = wgetch(win);
        if (ch == '\n' || ch == 27) {
            if (ch == 27) {
                input[0] = '\0';
            }
            break;
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            if (i > 0) {
                i--;
                mvwprintw(win, y, x + strlen(prompt) + i, " ");
                wmove(win, y, x + strlen(prompt) + i);
            }
        } else if (i < length - 1) {
            input[i++] = ch;
            mvwprintw(win, y, x + strlen(prompt) + i - 1, "%c", ch);
        }
        wrefresh(win);
    }
    input[i] = '\0';
    noecho();
}
void get_password_login(WINDOW *win, int y, int x, char *prompt, char *input, int length) {
    char full_prompt[100];
    snprintf(full_prompt, sizeof(full_prompt), "%s", prompt);
    mvwprintw(win, y, x, "%s", full_prompt);
    wrefresh(win);

    int ch, i = 0;
    while (1) {
        ch = wgetch(win);
        if (ch == '\n' || ch == 27) {
            if (ch == 27) {
                input[0] = '\0';
            }
            break;
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            if (i > 0) {
                i--;
                mvwprintw(win, y, x + strlen(prompt) + i, " ");
                wmove(win, y, x + strlen(prompt) + i);
            }
        } else if (i < length - 1) {
            input[i++] = ch;
            mvwprintw(win, y, x + strlen(prompt) + i - 1, "*");
        }
        wrefresh(win);
    }
    input[i] = '\0';
    noecho();
}


int username_exists_login(const char *username) {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        return 0;
    }
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, username) != NULL) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

int check_password(const char *username, const char *password) {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        return 0;
    }
    char line[100];
    char stored_password[50];
    int username_found = 0;
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, username) != NULL) {
            username_found = 1;
            fgets(line, sizeof(line), file);
            fgets(line, sizeof(line), file);
            sscanf(line, "Password: %s\n", stored_password);
            break;
        }
    }
    fclose(file);
    return username_found && strcmp(password, stored_password) == 0;
}

void get_email(const char *username) {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        return;
    }
    char line[100];
    char stored_email[50];
    int username_found = 0;
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, username) != NULL) {
            username_found = 1;
            fgets(line, sizeof(line), file);
            sscanf(line, "Email: %s\n", stored_email);
            break;
        }
    }
    fclose(file);
    strcpy(current_user.email, stored_email);
    return;
}

void show_password_hint(WINDOW *win, const char *username) {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        return;
    }
    char line[100];
    char stored_password[50];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, username) != NULL) {
            fgets(line, sizeof(line), file);
            fgets(line, sizeof(line), file);
            sscanf(line, "Password: %s\n", stored_password);
            break;
        }
    }
    fclose(file);

    if (strlen(stored_password) >= 2) {
        mvwprintw(win, 14, 2, "Password hint: %c...%c     ", stored_password[0], stored_password[strlen(stored_password) - 1]);
    } else if (strlen(stored_password) == 1) {
        mvwprintw(win, 14, 2, "Password hint: %c     ", stored_password[0]);
    } else {
        mvwprintw(win, 14, 2, "Password hint: No hint available     ");
    }
}

void login_screen() {
    int height = 20;
    int width = 50;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *win = newwin(height, width, start_y, start_x);
    box(win, 0, 0);
    mvwprintw(win, 0, (width - strlen("[Login]")) / 2, "[Login]");
    noecho();
    char username[50];
    char password[50];

    int highlight = 1;
    char *choices[] = {"Login", "Continue as Guest"};
    int n_choices = sizeof(choices) / sizeof(char *);

    keypad(win, TRUE);

    while (1) {
        for (int i = 0; i < n_choices; ++i) {
            if (i == highlight) {
                wattron(win, A_REVERSE);
            }
            mvwprintw(win, 4 + i * 2, 2, "%s", choices[i]);
            wattroff(win, A_REVERSE);
        }
        int choice = wgetch(win);
        switch (choice) {
            case KEY_UP:
                if (highlight > 0) {
                    highlight--;
                }
                break;
            case KEY_DOWN:
                if (highlight < n_choices - 1) {
                    highlight++;
                }
                break;
            case 10:
                if (highlight == 1) {
                    mvwprintw(win, 12, 2, "Continuing as guest.           ");
                    wrefresh(win);
                    strcpy(which_menu, "Guest before game menu");
                    strcpy(which_user, "guest");
                    goto end;
                } else {
                    mvwprintw(win, 12, 2, "Entering login...              ");
                    wrefresh(win);
                    goto login;
                }
        }
        wrefresh(win);
    }

login:
    while (1) {
        wclear(win);
        box(win, 0, 0);
        mvwprintw(win, 0, (width - strlen("Login")) / 2, "Login");

        get_input_login(win, 2, 2, "Username: ", username, 50);
        if (username[0] == '\0') {
            strcpy(which_menu, "menu");
            break;
        }
        if (!username_exists_login(username)) {
            mvwprintw(win, 10, 2, "Username not found. Try again.         ");
            wrefresh(win);
            sleep(1);
            continue;
        }
        get_password_login(win, 4, 2, "Password: ", password, 50);
        if (password[0] == '\0') {
            strcpy(which_menu, "menu");
            break;
        }
        if (!check_password(username, password)) {
            mvwprintw(win, 10, 2, "Wrong password. Try again.             ");
            mvwprintw(win, 12, 2, "Press 'f' for password hint.           ");
            wrefresh(win);

            int ch = wgetch(win);
            if (ch == 'f' || ch == 'F') {
                show_password_hint(win, username);
                wrefresh(win);
                wgetch(win);
            }
            continue;
        }
        mvwprintw(win, 12, 2, "Login successful!                ");
        wrefresh(win);
        strcpy(our_user, username);
        strcpy(which_user, "user");
        strcpy(which_menu, "Before game menu");
        
        if (load_user_info(username, &current_user)) {
            mvwprintw(win, 14, 2, "User information loaded.");
            // if(current_user.have_game == 1){

            // }
            strcpy(current_user.password, password);
            get_email(username);
        } else {
            mvwprintw(win, 14, 2, "Error loading user information.");
        }

        break;
    }

end:
    wrefresh(win);
    getch();
    delwin(win);
}
