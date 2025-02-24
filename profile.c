#include <ncurses.h>
#include <string.h>
#include "user.h"
#include "profile.h"

extern User current_user;
extern char which_color[50];

void display_profile() {
    int width = 50;
    int height = 28;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);

    WINDOW *profile_win = newwin(height, width, start_y, start_x);
    box(profile_win, 0, 0);
    mvwprintw(profile_win, 12, 0, "                                                  ");
    mvwprintw(profile_win, 13, 0, "┌────────────────────Profile─────────────────────┐");

    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    wattron(profile_win, COLOR_PAIR(2));

    mvwprintw(profile_win, 14, 4, "┌────────────────────────────────────────┐");
    mvwprintw(profile_win, 15, 4, "│ Username: %-28s │", current_user.username);
    mvwprintw(profile_win, 16, 4, "├────────────────────────────────────────┤");
    mvwprintw(profile_win, 17, 4, "│ Points: %-30lld │", current_user.points);
    mvwprintw(profile_win, 18, 4, "│ Games Count: %-25d │", current_user.games_count);
    mvwprintw(profile_win, 19, 4, "│ Play Time: %-27d │", current_user.play_time);
    mvwprintw(profile_win, 20, 4, "│ Golds: %-31d │", current_user.golds);
    mvwprintw(profile_win, 21, 4, "├────────────────────────────────────────┤");
    mvwprintw(profile_win, 22, 4, "│ Email: %-31s │", current_user.email);

    char masked_password[50];
    memset(masked_password, '*', strlen(current_user.password));
    masked_password[strlen(current_user.password)] = '\0';

    mvwprintw(profile_win, 23, 4, "│ Password: %-28s │", masked_password);
    mvwprintw(profile_win, 24, 4, "│ Show Password          [ ]             │");
    mvwprintw(profile_win, 25, 4, "│ Hide Password          [ ]             │");
    mvwprintw(profile_win, 26, 4, "└────────────────────────────────────────┘");

    wattroff(profile_win, COLOR_PAIR(2));
    wrefresh(profile_win);

    int choice = 0;
    keypad(profile_win, TRUE);
    while (true) {
        if (!strcmp(which_color, " Yellow ")) {
            wattron(profile_win, COLOR_PAIR(1));
        } else if (!strcmp(which_color, " Red ")) {
            wattron(profile_win, COLOR_PAIR(6));
        } else if (!strcmp(which_color, " Blue ")) {
            wattron(profile_win, COLOR_PAIR(9));
        } else if (!strcmp(which_color, " Green ")) {
            wattron(profile_win, COLOR_PAIR(7));
        }
        mvwprintw(profile_win, 0, 0, "                                                                       ");
        mvwprintw(profile_win, 1, 0, "                    ███████████████                              ");
        mvwprintw(profile_win, 2, 0, "                ████████ ██████ ███                                       ");
        mvwprintw(profile_win, 3, 0, "                    ████  ████  ███                                    ");
        mvwprintw(profile_win, 4, 0, "                ███████████ ███████                                   ");
        mvwprintw(profile_win, 5, 0, "                    ███ ███████████                                    ");
        mvwprintw(profile_win, 6, 0, "                ██  ████      █████                                          ");
        mvwprintw(profile_win, 7, 0, "              ██████  ████████████                                        ");
        mvwprintw(profile_win, 8, 0, "              ██████                                                           ");
        mvwprintw(profile_win, 9, 0, "              ████  █████████████                                  ");
        mvwprintw(profile_win, 10, 0, "                ██  ██████  █████                                      ");
        mvwprintw(profile_win, 11, 0, "                ██  ██████  █████                                            ");
        if (!strcmp(which_color, " Yellow ")) {
            wattroff(profile_win, COLOR_PAIR(1));
        } else if (!strcmp(which_color, " Red ")) {
            wattroff(profile_win, COLOR_PAIR(6));
        } else if (!strcmp(which_color, " Blue ")) {
            wattroff(profile_win, COLOR_PAIR(9));
        } else if (!strcmp(which_color, " Green ")) {
            wattroff(profile_win, COLOR_PAIR(7));
        }
        mvwprintw(profile_win, 24, 30, choice == 0 ? "X" : " ");
        mvwprintw(profile_win, 25, 30, choice == 1 ? "X" : " ");
        wrefresh(profile_win);

        int ch = wgetch(profile_win);
        switch (ch) {
            case KEY_UP:
                choice = (choice == 0) ? 1 : 0;
                break;
            case KEY_DOWN:
                choice = (choice == 1) ? 0 : 1;
                break;
            case 10:
                if (choice == 0) {
                    mvwprintw(profile_win, 23, 16, "%-28s", current_user.password);
                } else if (choice == 1) {
                    mvwprintw(profile_win, 23, 16, "%-28s", masked_password);
                }
                wrefresh(profile_win);
                break;
            case 27:
                delwin(profile_win);
                endwin();
                return;
        }
    }
}





// mvwprintw(profile_win, 1, 0, "                █████████████████                              ");
//         mvwprintw(profile_win, 2, 0, "                █████████████████                                       ");
//         mvwprintw(profile_win, 3, 0, "                █████████████████                                 ");
//         mvwprintw(profile_win, 4, 0, "                ███  ████  ██████                                   ");
//         mvwprintw(profile_win, 5, 0, "                █████    ████████                                    ");
//         mvwprintw(profile_win, 6, 0, "                █████████████████                                          ");