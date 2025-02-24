#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "user.h"

#define MAX_USERS 100
#define USERS_PER_PAGE 10

extern User current_user;
extern char our_user[50];

void display_scoreboard() {
    User users[MAX_USERS];
    int num_users = read_all_users(users, MAX_USERS);

    if (num_users == 0) {
        mvprintw(0, 0, "No users found.");
        getch();
        return;
    }

    for (int i = 0; i < num_users - 1; i++) {
        for (int j = 0; j < num_users - i - 1; j++) {
            if (users[j].points < users[j + 1].points) {
                User temp = users[j];
                users[j] = users[j + 1];
                users[j + 1] = temp;
            }
        }
    }

    int width = 76;
    int height = USERS_PER_PAGE + 7;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    int page = 0;
    int total_pages = (num_users + USERS_PER_PAGE - 1) / USERS_PER_PAGE;

    start_color();
    // init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    // init_pair(6, COLOR_GREEN, COLOR_BLACK);
    // init_pair(10, COLOR_MAGENTA, COLOR_BLACK);
    // init_pair(11, COLOR_BLUE, COLOR_BLACK);
    // init_pair(12, COLOR_RED, COLOR_BLACK);
    // init_pair(13, COLOR_WHITE, COLOR_BLACK);

    while (1) {
        // clear();
        attron(COLOR_PAIR(12));
        mvprintw(start_y - 6, start_x + (width - 24) / 2 + 4, "     _______       ");
        mvprintw(start_y - 5, start_x + (width - 24) / 2 + 4, "    /       \\     ");
        mvprintw(start_y - 4, start_x + (width - 24) / 2 + 4, "   /  CHAMP  \\    ");
        mvprintw(start_y - 3, start_x + (width - 24) / 2 + 4, "  /___________\\   ");
        mvprintw(start_y - 2, start_x + (width - 24) / 2 + 4, " |             |  ");
        mvprintw(start_y - 1, start_x + (width - 24) / 2 + 4, " |             |  ");
        mvprintw(start_y, start_x + (width - 24) / 2 + 4, " |_____________|  ");
        mvprintw(start_y + 1, start_x + (width - 24) / 2 + 4, "     |     |      ");
        mvprintw(start_y + 2, start_x + (width - 24) / 2 + 4, "     |     |      ");
        mvprintw(start_y + 3, start_x + (width - 24) / 2 + 4, "     |_____|      ");
        attroff(COLOR_PAIR(12));
        attron(COLOR_PAIR(6));
        mvprintw(start_y - 1, start_x + (width - 24) / 2 + 13 - strlen(users[0].username) / 2, "%s", users[0].username);
        attroff(COLOR_PAIR(6));

        WINDOW *score_win = newwin(height, width, start_y + 5, start_x);
        box(score_win, 0, 0);
        mvwprintw(score_win, 0, (width - strlen("Scoreboard")) / 2, "Scoreboard");

        wattron(score_win, COLOR_PAIR(1));
        mvwprintw(score_win, 2, 1, "Rank");
        mvwprintw(score_win, 2, 7, " Username");
        mvwprintw(score_win, 2, 29, "Points");
        mvwprintw(score_win, 2, 39, "Golds");
        mvwprintw(score_win, 2, 49, "Games Count");
        mvwprintw(score_win, 2, 64, "Experience");
        mvwprintw(score_win, 3, 1, "--------------------------------------------------------------------------");

        wchar_t medal = L'\U0000058D';
        int start_index = page * USERS_PER_PAGE;
        int end_index = start_index + USERS_PER_PAGE;
        if (end_index > num_users) end_index = num_users;

        for (int i = start_index; i < end_index; i++) {
            if (i == 0) {
                wattroff(score_win, COLOR_PAIR(1));
                wattron(score_win, COLOR_PAIR(12));
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 19, "/MYTH\\");
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 3, "🥇");
            } else if (i == 1) {
                wattroff(score_win, COLOR_PAIR(1));
                wattron(score_win, COLOR_PAIR(10));
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 19, "|GOAT|");
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 3, "🥈");
            } else if (i == 2) {
                wattroff(score_win, COLOR_PAIR(1));
                wattron(score_win, COLOR_PAIR(11));
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 19, "\\HERO/");
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 3, "🥉");
            } else if (i == 3) {
                wattroff(score_win, COLOR_PAIR(1));
                wattron(score_win, COLOR_PAIR(14));
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 3, "🏅");
            } else if (i == 4) {
                wattroff(score_win, COLOR_PAIR(1));
                wattron(score_win, COLOR_PAIR(16));
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 3, " %lc", medal);
            } else if (i == num_users - 1) {
                wattroff(score_win, COLOR_PAIR(1));
                wattron(score_win, COLOR_PAIR(13));
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 19, "Bruh...");
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 4, "%lc", L'\U0000203D');
            }

            if (!strcmp(users[i].username, our_user) || (!strcmp(users[i].username, "You") && !strcmp(our_user, "guest"))) {
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 1, "%d", i + 1);
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 7, " %s <--", users[i].username);
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 29, "%lld", users[i].points);
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 39, "%d", users[i].golds);
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 49, "%d", users[i].games_count);
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 64, "%d", users[i].play_time);
            } else {
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 1, "%d", i + 1);
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 7, " %s", users[i].username);
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 29, "%lld", users[i].points);
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 39, "%d", users[i].golds);
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 49, "%d", users[i].games_count);
                mvwprintw(score_win, (i % USERS_PER_PAGE) + 4, 64, "%d", users[i].play_time);
            }

            if (i == 0) {
                wattroff(score_win, COLOR_PAIR(12));
                wattron(score_win, COLOR_PAIR(1));
            } else if (i == 1) {
                wattroff(score_win, COLOR_PAIR(10));
                wattron(score_win, COLOR_PAIR(1));
            } else if (i == 2) {
                wattroff(score_win, COLOR_PAIR(11));
                wattron(score_win, COLOR_PAIR(1));
            } else if (i == 3) {
                wattroff(score_win, COLOR_PAIR(14));
                wattron(score_win, COLOR_PAIR(1));
            } else if (i == 4) {
                wattroff(score_win, COLOR_PAIR(16));
                wattron(score_win, COLOR_PAIR(1));
            } else if (i == num_users - 1) {
                wattroff(score_win, COLOR_PAIR(13));
                wattron(score_win, COLOR_PAIR(1));
            }
        }
        keypad(score_win, TRUE);
        keypad(stdscr, TRUE);
        wattroff(score_win, COLOR_PAIR(1));
        mvwprintw(score_win, height - 2, 1, "Page %d/%d", page + 1, total_pages);
        mvwprintw(score_win, height - 2, width - 19, "Press esc for quit");
        wrefresh(score_win);
        int ch = getch();
        if (ch == KEY_RIGHT) {
            if (page < total_pages - 1) {
                page++;
            } else {
                page = 0;
            }
        } else if (ch == KEY_LEFT) {
            if (page > 0) {
                page--;
            } else {
                page = total_pages - 1;
            }
        } else if (ch == 27) {
            break;
        }
        delwin(score_win);
    }
}
