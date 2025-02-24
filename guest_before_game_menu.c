#include <ncurses.h>
#include <string.h>
#include "guest_before_game_menu.h"

extern char which_menu[50];

char *guest_choices_before_game[] = {
    " New Game ",
    " Scoreboard ",
    " Settings ",
    " Menu "
};

int n_guest_choices_before_game = sizeof(guest_choices_before_game) / sizeof(char *);

void guest_print_menu_before_game(WINDOW *menu_win, int highlight) {
    int x, y, i;
    x = 5;
    y = 19;
    int a1 = 14;
    box(menu_win, 0, 0);
    wattron(menu_win, COLOR_PAIR(208));
    mvwprintw(menu_win, 0, 0, "                                      ______                    ");
    mvwprintw(menu_win, 1, 0, "                           ______,---'__,---'                    ");
    mvwprintw(menu_win, 2, 0, "                       _,-'---_---__,---'                    ");
    mvwprintw(menu_win, 3, 0, "                /_    (,  ---____',                    ");
    mvwprintw(menu_win, 4, 0, "               /  ',,   `, ,-'                         ");
    wattroff(menu_win, COLOR_PAIR(208));
    wattron(menu_win, COLOR_PAIR(172));
    mvwprintw(menu_win, 5, 0, "              ;/)   ,',,_/,'                         ");
    mvwprintw(menu_win, 6, 0, "              | /\\   ,.'//\\                          ");
    mvwprintw(menu_win, 7, 0, "              `-` \\ ,,'    `.                            ");
    mvwprintw(menu_win, 8, 0, "                   `',   ,-- `.                             ");
    mvwprintw(menu_win, 9, 0, "                   '/ / |      `,         _                        ");
    mvwprintw(menu_win, 10, 0, "                   //'',.\\_    .\\\\      ,{==>-                       ");
    mvwprintw(menu_win, 11, 0, "                __//   __;_`-  \\ `;.__,;'                    ");
    mvwprintw(menu_win, 12, 0, "              ((,--,) (((,------;  `--'                     ");
    mvwprintw(menu_win, 13, 0, "              ```  '   ```                                ");
    wattroff(menu_win, COLOR_PAIR(172));
    wattron(menu_win, COLOR_PAIR(222));
    mvwprintw(menu_win, 11, 0, "                __//   __;_`-  \\");
    mvwprintw(menu_win, 12, 0, "              ((,--,) (((,------;");
    mvwprintw(menu_win, 13, 0, "              ```  '   ```");
    wattroff(menu_win, COLOR_PAIR(222));
    wattron(menu_win, COLOR_PAIR(223));
    mvwprintw(menu_win, 3, 0, "                /_    ");
    mvwprintw(menu_win, 4, 0, "               /  ',,   ");
    mvwprintw(menu_win, 5, 0, "              ;/)   ,',,");
    mvwprintw(menu_win, 6, 0, "              | /\\   ,.'");
    mvwprintw(menu_win, 7, 0, "              `-` \\ ,,'    ");
    mvwprintw(menu_win, 8, 0, "                   `',  ");
    wattroff(menu_win, COLOR_PAIR(223));
    mvwprintw(menu_win, 0 + a1, 0, "┌───────────────[Before Game Menu]───────────────┐");
    mvwprintw(menu_win, 1 + a1, 2, "          ____ ____ ____ ____ ____ ");
    mvwprintw(menu_win, 2 + a1, 2, " _________|__| |__| |__| |__| |__| ");
    mvwprintw(menu_win, 3 + a1, 2, " |        |__| |__| |__| |__| |__| ");
    wattron(menu_win, A_BOLD | COLOR_PAIR(6));
    mvwprintw(menu_win, 14 + a1, 2, " |                                  ");
    mvwprintw(menu_win, 4 + a1, x - 2, "|");
    mvwprintw(menu_win, 6 + a1, x - 2, "|");
    mvwprintw(menu_win, 8 + a1, x - 2, "|");
    mvwprintw(menu_win, 10 + a1, x - 2, "|");
    mvwprintw(menu_win, 12 + a1, x - 2, "|");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(6));
    mvwprintw(menu_win, 12 + a1, 4, "         __   __   __   __   __ ");
    mvwprintw(menu_win, 13 + a1, 2, " |________|--| |--| |--| |--| |--| ");
    mvwprintw(menu_win, 14 + a1, 2, "          |--| |--| |--| |--| |--| ");
    mvwprintw(menu_win, 15 + a1, 2, "          \\_/  \\_/  \\_/  \\_/  \\_/");
    
    for (i = 0; i < n_guest_choices_before_game; ++i) {
        if (highlight == i + 1) {
            wattron(menu_win, A_BOLD | COLOR_PAIR(6));
            mvwprintw(menu_win, y, x - 2, "* %s", guest_choices_before_game[i]);
            wattroff(menu_win, A_BOLD | COLOR_PAIR(6));
        } else {
            mvwprintw(menu_win, y, x - 2, "| %s", guest_choices_before_game[i]);
            // mvwprintw(menu_win, y - 1, x - 2, "|");
        }
        y += 2;
    }
    wrefresh(menu_win);
}

void guest_before_game_menu() {
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int c;

    int startx, starty, width, height;
    height = 33;
    width = 50;
    starty = (LINES - height) / 2 - 4;
    startx = (COLS - width) / 2;
    attron(COLOR_PAIR(1));
    box(stdscr, '|', '-');
    attroff(COLOR_PAIR(1));
    menu_win = newwin(height, width, starty, startx);
    keypad(menu_win, TRUE);
    guest_print_menu_before_game(menu_win, highlight);
    
    while (1) {
        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = n_guest_choices_before_game;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == n_guest_choices_before_game)
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
        guest_print_menu_before_game(menu_win, highlight);
        if (choice != 0)
            break;
    }

    strcpy(which_menu, guest_choices_before_game[choice - 1]);
    clrtoeol();
    refresh();
    // getch();
    delwin(menu_win);
}