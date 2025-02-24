#include <ncurses.h>
#include <string.h>
#include <time.h>
#include "outro.h"
void display_message() {
    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);

    clear();
    noecho();
    cbreak();
    curs_set(0); // Hide the cursor

    int startx, starty, width, height;
    height = 5;
    width = 40;
    starty = (LINES - height) / 2;
    startx = (COLS - width) / 2;

    WINDOW *message_win = newwin(height, width, starty, startx);
    box(message_win, 0, 0);

    wattron(message_win, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(message_win, 1, (width - strlen("Thanks for playing my game =D")) / 2, "Thanks for playing my game =D");
    wattroff(message_win, COLOR_PAIR(1) | A_BOLD);

    wattron(message_win, COLOR_PAIR(2));
    box(stdscr, '|', '-');
    mvwprintw(message_win, 3, (width - strlen("Press any key to exit")) / 2, "Press any key to exit");
    wattroff(message_win, COLOR_PAIR(2));

    wrefresh(message_win);
    getchar();
    // sleep();

    delwin(message_win);
    endwin();
}