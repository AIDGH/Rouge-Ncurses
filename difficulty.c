#include <ncurses.h>
#include <string.h>
#include "difficulty.h"

extern char which_menu[50];
extern char which_difficulty[50];
extern int difficulty_efficiency;

char *difficulty_choices[] = {
    " Easy ",
    " Normal ",
    " Hard ",
    " Extreme "
};

int n_difficulty_choices = sizeof(difficulty_choices) / sizeof(char *);

void print_difficulty_change(WINDOW *menu_win, int highlight) {
    int x, y, i;
    x = 5;
    y = 5;
    box(menu_win, 0, 0);
    mvwprintw(menu_win, 0, (50 - strlen(" Choose Difficulty ")) / 2, "[Choose Difficulty]");
    mvwprintw(menu_win, 1, 2, "          ____ ____ ____ ____ ____ ");
    mvwprintw(menu_win, 2, 2, " _________|__| |__| |__| |__| |__| ");
    mvwprintw(menu_win, 3, 2, " |        |__| |__| |__| |__| |__| ");
    wattron(menu_win, A_BOLD | COLOR_PAIR(6));
    mvwprintw(menu_win, 14, 2, " |                                  ");
    mvwprintw(menu_win, 4, x - 2, "|");
    mvwprintw(menu_win, 6, x - 2, "|");
    mvwprintw(menu_win, 8, x - 2, "|");
    mvwprintw(menu_win, 10, x - 2, "|");
    mvwprintw(menu_win, 12, x - 2, "|");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(6));
    mvwprintw(menu_win, 12, 4, "         __   __   __   __   __ ");
    mvwprintw(menu_win, 13, 2, " |________|--| |--| |--| |--| |--| ");
    mvwprintw(menu_win, 14, 2, "          |--| |--| |--| |--| |--| ");
    mvwprintw(menu_win, 15, 2, "          \\_/  \\_/  \\_/  \\_/  \\_/");
    
    for (i = 0; i < n_difficulty_choices; ++i) {
        if (highlight == i + 1) {
            wattron(menu_win, A_BOLD | COLOR_PAIR(6));
            mvwprintw(menu_win, y, x - 2, "* %s", difficulty_choices[i]);
            wattroff(menu_win, A_BOLD | COLOR_PAIR(6));
        } else {
            mvwprintw(menu_win, y, x - 2, "| %s", difficulty_choices[i]);
            // mvwprintw(menu_win, y - 1, x - 2, "|");
        }
        y += 2;
    }
    wrefresh(menu_win);
}

void difficulty_change() {
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int c;

    int startx, starty, width, height;
    height = 18;
    width = 50;
    starty = (LINES - height) / 2;
    startx = (COLS - width) / 2;
    attron(COLOR_PAIR(1));
    box(stdscr, '|', '-');
    mvprintw(27, 110, "Press Again");
    attroff(COLOR_PAIR(1));
    menu_win = newwin(height, width, starty, startx);
    keypad(menu_win, TRUE);
    print_difficulty_change(menu_win, highlight);
    
    while (1) {
        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = n_difficulty_choices;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == n_difficulty_choices)
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
        print_difficulty_change(menu_win, highlight);
        if (choice != 0)
            break;
    }

    strcpy(which_difficulty, difficulty_choices[choice - 1]);
    if(!strcmp(which_difficulty, " Easy ")){
        difficulty_efficiency = 1;
    }
    else if(!strcmp(which_difficulty, " Normal ")){
        difficulty_efficiency = 2;
    }
    else if(!strcmp(which_difficulty, " Hard ")){
        difficulty_efficiency = 3;
    }
    else if(!strcmp(which_difficulty, " Extreme ")){
        difficulty_efficiency = 4;
    }
    clrtoeol();
    refresh();
    // getch();
    delwin(menu_win);
}