#include <ncurses.h>
#include <string.h>
#include "settings.h"

extern char which_menu[50];

char *setting_choices[] = {
    " Difficulty ",
    " Color Change ",
    " Character Change ",
    " Music ",
    " Main menu ",
    " Before game menu "
};

int n_setting_choices = sizeof(setting_choices) / sizeof(char *);

void print_setting_menu(WINDOW *menu_win, int highlight) {
    int x, y, i;
    x = 5;
    y = 15;
    box(menu_win, 0, 0);
    wattron(menu_win, COLOR_PAIR(14));
    wattron(menu_win, A_BLINK);
    mvwprintw(menu_win, 0, 0,  "                     __/\\_/\\__                     ");
    mvwprintw(menu_win, 1, 0,  "                    /\\       /\\                    ");
    mvwprintw(menu_win, 2, 0,  "                   /  \\_____/  \\                   ");
    mvwprintw(menu_win, 3, 0,  "                  |   /     \\   |                    ");
    mvwprintw(menu_win, 4, 0,  "                  |   |  %lc  |   |                    ",  L'\U0000058D');
    mvwprintw(menu_win, 5, 0,  "                  |   \\_____/   |                    ");
    mvwprintw(menu_win, 6, 0,  "                  |     \\_/     |                    ");
    mvwprintw(menu_win, 7, 0,  "                   \\           /                   ");
    mvwprintw(menu_win, 8, 0,  "                    \\_/\\_&_/\\_/                    ");
    mvwprintw(menu_win, 9, 0,  " ");
    mvwprintw(menu_win, 9, 49,  " ");
    wattroff(menu_win, A_BLINK);
    wattroff(menu_win, COLOR_PAIR(14));
    mvwprintw(menu_win, 10, 0, "┌────────────────────[Setting]───────────────────┐");
    mvwprintw(menu_win, 11, 2, "          ____ ____ ____ ____ ____ ");
    mvwprintw(menu_win, 12, 2, " _________|__| |__| |__| |__| |__| ");
    mvwprintw(menu_win, 13, 2, " |        |__| |__| |__| |__| |__| ");
    wattron(menu_win, A_BOLD | COLOR_PAIR(6));
    mvwprintw(menu_win, 24, 2, " |                                  ");
    mvwprintw(menu_win, 14, x - 2, "|");
    mvwprintw(menu_win, 16, x - 2, "|");
    mvwprintw(menu_win, 18, x - 2, "|");
    mvwprintw(menu_win, 20, x - 2, "|");
    mvwprintw(menu_win, 22, x - 2, "|");
    mvwprintw(menu_win, 24, x - 2, "|");
    mvwprintw(menu_win, 26, x - 2, "|");
    mvwprintw(menu_win, 28, x - 2, "|");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(6));
    mvwprintw(menu_win, 26, 4, "         __   __   __   __   __ ");
    mvwprintw(menu_win, 27, 2, " |________|--| |--| |--| |--| |--| ");
    mvwprintw(menu_win, 28, 2, "          |--| |--| |--| |--| |--| ");
    mvwprintw(menu_win, 29, 2, "          \\_/  \\_/  \\_/  \\_/  \\_/");
    
    for (i = 0; i < n_setting_choices; ++i) {
        if (highlight == i + 1) {
            wattron(menu_win, A_BOLD | COLOR_PAIR(6));
            mvwprintw(menu_win, y, x - 2, "* %s", setting_choices[i]);
            wattroff(menu_win, A_BOLD | COLOR_PAIR(6));
        } else {
            mvwprintw(menu_win, y, x - 2, "| %s", setting_choices[i]);
        }
        y += 2;
    }
    wrefresh(menu_win);
}

void setting_menu() {
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int c;

    int startx, starty, width, height;
    height = 31;
    width = 50;
    starty = (LINES - height) / 2 - 3;
    startx = (COLS - width) / 2;

    start_color();
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);


    attron(COLOR_PAIR(1));
    box(stdscr, '|', '-');
    mvprintw(27, 110, "Press Again");
    attroff(COLOR_PAIR(1));

    menu_win = newwin(height, width, starty, startx);
    keypad(menu_win, TRUE);
    print_setting_menu(menu_win, highlight);
    
    while (1) {
        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = n_setting_choices;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == n_setting_choices)
                    highlight = 1;
                else
                    ++highlight;
                break;
            case 10:
                choice = highlight;
                break;
            default:
                break;
        }
        print_setting_menu(menu_win, highlight);
        if (choice != 0)
            break;
    }

    strcpy(which_menu, setting_choices[choice - 1]);
    clrtoeol();
    refresh();
    // getch();
    delwin(menu_win);
}

