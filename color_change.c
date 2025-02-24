#include <ncurses.h>
#include <string.h>
#include "color_change.h"

extern char which_menu[50];
extern char which_color[50];

char *color_choices[] = {
    " White ",
    " Yellow ",
    " Red ",
    " Blue ",
    " Green "
};

int n_color_choices = sizeof(color_choices) / sizeof(char *);

void print_colors(WINDOW *menu_win, int highlight) {
    int x, y, i;
    x = 5;
    y = 5;
    box(menu_win, 0, 0);
    mvwprintw(menu_win, 0, (50 - strlen(" Choose Color ")) / 2, "[Choose Color]");
    wattron(menu_win, A_BOLD | COLOR_PAIR(8));
    mvwprintw(menu_win, 1, 2, "          ____ ____ ____ ____ ____ ");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(8));
    wattron(menu_win, A_BOLD | COLOR_PAIR(6));
    mvwprintw(menu_win, 2, 2, " _________|__| |__| |__| |__| |__| ");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(6));
    wattron(menu_win, A_BOLD | COLOR_PAIR(9));
    mvwprintw(menu_win, 3, 2, " |        |__| |__| |__| |__| |__| ");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(9));
    wattron(menu_win, A_BOLD | COLOR_PAIR(7));
    mvwprintw(menu_win, 14, 2, " |                                  ");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(7));
    wattron(menu_win, A_BOLD | COLOR_PAIR(3));
    mvwprintw(menu_win, 4, x - 2, "|");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(3));
    wattron(menu_win, A_BOLD | COLOR_PAIR(3));
    mvwprintw(menu_win, 6, x - 2, "|");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(3));
    wattron(menu_win, A_BOLD | COLOR_PAIR(3));
    mvwprintw(menu_win, 8, x - 2, "|");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(3));
    wattron(menu_win, A_BOLD | COLOR_PAIR(3));
    mvwprintw(menu_win, 10, x - 2, "|");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(3));
    wattron(menu_win, A_BOLD | COLOR_PAIR(3));
    mvwprintw(menu_win, 12, x - 2, "|");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(3));
    wattron(menu_win, A_BOLD | COLOR_PAIR(3));
    mvwprintw(menu_win, 14, x - 2, "|");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(3));
    wattron(menu_win, A_BOLD | COLOR_PAIR(7));
    mvwprintw(menu_win, 14, 4, "         __   __   __   __   __ ");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(7));
    wattron(menu_win, A_BOLD | COLOR_PAIR(9));
    mvwprintw(menu_win, 15, 2, " |________|--| |--| |--| |--| |--| ");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(9));
    wattron(menu_win, A_BOLD | COLOR_PAIR(6));
    mvwprintw(menu_win, 16, 2, "          |--| |--| |--| |--| |--| ");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(6));
    wattron(menu_win, A_BOLD | COLOR_PAIR(8));
    mvwprintw(menu_win, 17, 2, "          \\_/  \\_/  \\_/  \\_/  \\_/");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(8));
    
    for (i = 0; i < n_color_choices; ++i) {
        if (highlight == i + 1) {
            if(i == 1){
                wattron(menu_win, A_BOLD | COLOR_PAIR(8));
                mvwprintw(menu_win, y, x - 2, "* %s", color_choices[i]);
                wattroff(menu_win, A_BOLD | COLOR_PAIR(8));
            }
            else if(i == 2){
                wattron(menu_win, A_BOLD | COLOR_PAIR(6));
                mvwprintw(menu_win, y, x - 2, "* %s", color_choices[i]);
                wattroff(menu_win, A_BOLD | COLOR_PAIR(6));
            }
            else if(i == 3){
                wattron(menu_win, A_BOLD | COLOR_PAIR(9));
                mvwprintw(menu_win, y, x - 2, "* %s", color_choices[i]);
                wattroff(menu_win, A_BOLD | COLOR_PAIR(9));
            }
            else if(i == 4){
                wattron(menu_win, A_BOLD | COLOR_PAIR(7));
                mvwprintw(menu_win, y, x - 2, "* %s", color_choices[i]);
                wattroff(menu_win, A_BOLD | COLOR_PAIR(7));
            }
            else if(i == 0){
                mvwprintw(menu_win, y, x - 2, "* %s", color_choices[i]);
            }
        } else {
            wattron(menu_win, A_BOLD | COLOR_PAIR(3));
            mvwprintw(menu_win, y, x - 2, "| %s", color_choices[i]);
            wattroff(menu_win, A_BOLD | COLOR_PAIR(3));
            // mvwprintw(menu_win, y - 1, x - 2, "|");
        }
        y += 2;
    }
    wrefresh(menu_win);
}

void color_change() {
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int c;
    start_color();

    int startx, starty, width, height;
    height = 20;
    width = 50;
    starty = (LINES - height) / 2;
    startx = (COLS - width) / 2;
    attron(COLOR_PAIR(1));
    box(stdscr, '|', '-');
    mvprintw(29, 110, "Press Again");
    attroff(COLOR_PAIR(1));
    menu_win = newwin(height, width, starty, startx);
    keypad(menu_win, TRUE);
    print_colors(menu_win, highlight);
    
    while (1) {
        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = n_color_choices;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == n_color_choices)
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
        print_colors(menu_win, highlight);
        if (choice != 0)
            break;
    }

    strcpy(which_color, color_choices[choice - 1]);
    clrtoeol();
    refresh();
    // getch();
    delwin(menu_win);
}