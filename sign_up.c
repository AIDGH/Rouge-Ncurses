#include "sign_up.h"
#include "user.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

extern char our_user[50];
extern char which_user[50];
extern User current_user;

void get_input_sign_up(WINDOW *win, int y, int x, char *prompt, char *input, int length) {
    char full_prompt[100];
    snprintf(full_prompt, sizeof(full_prompt), "%s", prompt);
    mvwprintw(win, y, x, "%s", full_prompt);
    wrefresh(win);

    int ch, i = 0;
    while (1) {
        ch = wgetch(win);
        if (ch == '\n' || ch == 27) {
            break;
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            if (i > 0) {
                i--;
                mvwprintw(win, y, x + strlen(prompt) + i - 15, " ");
                wmove(win, y, x + strlen(prompt) + i);
            }
        } else if (i < length - 1) {
            input[i++] = ch;
            mvwprintw(win, y, x + strlen(prompt) + i - 1 - 15, "%c", ch);
        }
        wrefresh(win);
    }
    input[i] = '\0';
    noecho();
}

int username_exists_sign_up(const char *username) {
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

void user_save_data() {

    char filepath[100] = "users/";

    strcat(filepath, our_user);
    strcat(filepath, ".txt");

    mkdir("users", 0777);

    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        mvprintw(1, 0, "Error creating user file.");
        getch();
        return;
    }

    fprintf(file, "health: 100\n");
    fprintf(file, "points: 0\n");
    fprintf(file, "games count: 0\n");
    fprintf(file, "play time: 0\n");
    fprintf(file, "golds: 0\n");
    fprintf(file, "food: 100\n");
    fprintf(file, "weapons: 0 \n");
    fprintf(file, "spells: 0 \n");
    fprintf(file, "ancientkey: 0 \n");
    fprintf(file, "broken ancientkey: 0 \n");
    fprintf(file, "level: 1\n");
    fprintf(file, "game: \n");

    fclose(file);

    // mvprintw(1, 0, "User '%s' created successfully.", our_user);
    // getch();
}

void save_user_info(const char *username, const char *email, const char *password) {
    FILE *file = fopen("users.txt", "a");
    if (file == NULL) {
        printw("Error opening file!\n");
    } else {
        fprintf(file, "Username: %s\nEmail: %s\nPassword: %s\n\n", username, email, password);
        fclose(file);
    }
}

int validate_password(const char *password) {
    int length = strlen(password);
    int has_digit = 0, has_lower = 0, has_upper = 0;

    if (length < 8) return 0;

    for (int i = 0; i < length; i++) {
        if (isdigit(password[i])) has_digit = 1;
        else if (islower(password[i])) has_lower = 1;
        else if (isupper(password[i])) has_upper = 1;
    }

    return has_digit && has_lower && has_upper;
}

int validate_email(const char *email) {
    const char *at = strchr(email, '@');
    const char *dot = strrchr(email, '.');

    return at && dot && at < dot && dot - at > 1 && strlen(dot) > 1;
}

void generate_random_password(char *password, int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    srand(time(NULL));
    for (int i = 0; i < length - 1; i++) {
        int key = rand() % (int)(sizeof(charset) - 1);
        password[i] = charset[key];
    }
    password[length - 1] = '\0';
}

void user_input_screen() {
    int height = 17;
    int width = 60;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *win = newwin(height, width, start_y, start_x);
    box(win, 0, 0);
    mvwprintw(win, 0, (width - strlen("[User Information]")) / 2, "[User Information]");

    char username[50];
    char email[50];
    char password[50];

    int esc_pressed = 0;
    while (1) {
        get_input_sign_up(win, 2, 2, "Enter Username:                ", username, 50);
        if (username[0] == '\0') {
            esc_pressed = 1;
            break;
        }
        if (username_exists_sign_up(username)) {
            mvwprintw(win, 14, 2, "Username already exists. Try another.");
        } else {
            strcpy(our_user, username);
            strcpy(which_user, "user");
            strcpy(current_user.username, username);
            break;
        }
        wrefresh(win);
        memset(username, 0, sizeof(username));
    }

    if (!esc_pressed && username[0] != 0) {
        while (1) {
            get_input_sign_up(win, 4, 2, "Enter Email:                ", email, 50);
            if (email[0] == '\0') {
                esc_pressed = 1;
                break;
            }
            if (validate_email(email)) {
                strcpy(current_user.email, email);
                break;
            } else {
                mvwprintw(win, 14, 2, "Invalid email format. Try again.");
                memset(email, 0, sizeof(email));
            }
            wrefresh(win);
        }

        if (!esc_pressed) {
            int highlight = 0;
            char *choices[] = {"Enter Password", "Generate Random Password"};
            int n_choices = sizeof(choices) / sizeof(char *);

            keypad(win, TRUE);

            while (1) {
                for (int i = 0; i < n_choices; ++i) {
                    if (i == highlight) {
                        wattron(win, A_REVERSE);
                    }
                    mvwprintw(win, 6 + i * 2, 2, "%s", choices[i]);
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
                        if (highlight == 0) {
                            wmove(win, 6 + highlight * 2, 2 + strlen(choices[0]) + 1);
                            wrefresh(win);
                            get_input_sign_up(win, 8, 2, "Password:                ", password, 50);
                            if (password[0] == '\0') {
                                esc_pressed = 1;
                                break;
                            }
                            if (validate_password(password)) {
                                mvwprintw(win, 14, 2, "Password entered successfully.");
                                break;
                            } else {
                                mvwprintw(win, 14, 2, "Password must be 8+ chars, 1 digit, 1 upper, 1 lower.");
                            }
                        } else if (highlight == 1) {
                            generate_random_password(password, 12);
                            mvwprintw(win, 10, 2, "Generated Password: %s", password);
                            break;
                        }
                        wrefresh(win);
                        break;
                }
                if (password[0] != '\0' && validate_password(password)) {  
                    strcpy(current_user.password, password);
                    break;
                }
            }
        }

        if (!esc_pressed) {
            user_save_data();
            save_user_info(username, email, password);
            mvwprintw(win, 12, 2, "Information saved successfully!");
        }
    }

    if (esc_pressed) {
        mvwprintw(win, 12, 2, "Operation cancelled.");
    }

    wrefresh(win);
    getch();    
    delwin(win); 
}
