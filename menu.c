#include <ncurses.h>
#include <string.h>
#include "menu.h"
#define COLOR_KAKA 5
#define COLOR_GREEN1 30
#define COLOR_YELLOW1 31
#define COLOR_SILVER1 32
#define COLOR_BRONZE1 33
#define COLOR_GOLD1 34
#define COLOR_ORANGE1 35
#define COLOR_SECRET 36
#define COLOR_Bisque 37
#define COLOR_PURPLE1 38
#define COLOR_ZERESHKI 39
#define COLOR_BLUE1 40
#define COLOR_SILVER2 41
#define COLOR_TREASURE 42

MEVENT mouse_check;
int changes;

// #define COLOR_RED 20
void print_menu(WINDOW *menu_win, int highlight, char **choices, int n_choices) {
    int x, y, i;
    x = 2;
    y = 2;
    int height = 13;
    int width = 45;
    box(menu_win, 0, 0);
    wattron(menu_win, COLOR_PAIR(1));
    wborder(menu_win, '|', '|', '-', '-', '+', '+', '+', '+');
    wattroff(menu_win, COLOR_PAIR(1));

    for (i = 0; i < n_choices; ++i) {
        int len = strlen(choices[i]);
        int padding = (45 - len) / 2;
        if(mouse_check.y == (LINES - height) / 2 + y && mouse_check.x >= (COLS - width) / 2 + padding && mouse_check.x <= (COLS - width) / 2 + padding + len){
            highlight = i + 1;
            changes = i + 1;
        }
        if (highlight == i + 1) {
            wattron(menu_win, A_BOLD | COLOR_PAIR(2));
            mvwprintw(menu_win, y, padding, "%s", choices[i]);
            wattroff(menu_win, A_BOLD | COLOR_PAIR(2));
        } else {
            wattron(menu_win, COLOR_PAIR(3));
            mvwprintw(menu_win, y, padding, "%s", choices[i]);
            wattroff(menu_win, COLOR_PAIR(3));
        }
        y += 2;
    }
    wrefresh(menu_win);
}

char* display_menu() {
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int c;
    static char result[50];
    printf("\033[?1003h\n");

    char *choices[] = {
        " GAME MENU ",
        " LOGIN ",
        " SIGN UP ",
        " SCOREBOARD ",
        " SETTINGS "
    };
    int n_choices = sizeof(choices) / sizeof(char *);

    initscr();
    start_color();
    init_color(COLOR_CYAN, 700, 1000, 1000);
    init_color(COLOR_KAKA, 1000, 600, 600);
    init_color(COLOR_RED, 1000, 0, 0);
    init_color(COLOR_GREEN1, 0, 1000, 0);
    init_color(COLOR_YELLOW1, 1000, 1000, 200);
    init_color(COLOR_SILVER1, 753, 753, 753);
    init_color(COLOR_BRONZE1, 801, 496, 195);
    init_color(COLOR_GOLD1, 1000, 840, 0);
    init_color(COLOR_ORANGE1, 1000, 647, 0);
    init_color(COLOR_SECRET, 118, 1000, 784);
    init_color(COLOR_Bisque, 1000, 894, 769);
    init_color(COLOR_PURPLE1, 655, 0, 843);
    init_color(COLOR_ZERESHKI, 530, 0, 78);
    init_color(COLOR_BLUE1, 0, 0, 1000);
    init_color(COLOR_SILVER2, 550, 550, 550);
    init_color(COLOR_TREASURE, 224 * 4, 217 * 4, 128 * 4);
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_KAKA, COLOR_BLACK);
    init_pair(6, COLOR_RED, COLOR_BLACK);
    init_pair(7, COLOR_GREEN1, COLOR_BLACK);
    init_pair(8, COLOR_YELLOW1, COLOR_BLACK);
    init_pair(9, COLOR_BLUE, COLOR_BLACK);
    init_pair(10, COLOR_SILVER1, COLOR_BLACK);
    init_pair(11, COLOR_BRONZE1, COLOR_BLACK);
    init_pair(12, COLOR_GOLD1, COLOR_BLACK);
    init_pair(13, COLOR_WHITE, COLOR_BLACK);
    init_pair(14, COLOR_ORANGE1, COLOR_BLACK);
    init_pair(15, COLOR_SECRET, COLOR_BLACK);
    init_pair(16, COLOR_Bisque, COLOR_BLACK);
    init_pair(17, COLOR_PURPLE1, COLOR_BLACK);
    init_pair(18, COLOR_ZERESHKI, COLOR_BLACK);
    init_pair(19, COLOR_BLUE1, COLOR_BLACK);
    init_pair(20, COLOR_SILVER2, COLOR_BLACK);
    init_pair(21, 247, COLOR_BLACK);
    init_pair(22, COLOR_TREASURE, COLOR_BLACK);
    init_pair(23, 43, COLOR_BLACK);
    init_pair(24, 44, COLOR_BLACK);
    init_pair(25, 45, COLOR_BLACK);
    init_pair(26, 46, COLOR_BLACK);
    init_pair(27, 47, COLOR_BLACK);
    init_pair(28, 48, COLOR_BLACK);
    init_pair(29, 49, COLOR_BLACK);
    init_pair(30, 50, COLOR_BLACK);
    init_pair(31, 51, COLOR_BLACK);
    init_pair(32, 52, COLOR_BLACK);
    init_pair(33, 53, COLOR_BLACK);
    init_pair(34, 54, COLOR_BLACK);
    init_pair(35, 55, COLOR_BLACK);
    init_pair(36, 56, COLOR_BLACK);
    init_pair(37, 57, COLOR_BLACK);
    init_pair(38, 58, COLOR_BLACK);
    init_pair(39, 59, COLOR_BLACK);
    init_pair(40, 60, COLOR_BLACK);
    init_pair(41, 61, COLOR_BLACK);
    init_pair(42, 62, COLOR_BLACK);
    init_pair(43, 63, COLOR_BLACK);
    init_pair(44, 64, COLOR_BLACK);
    init_pair(45, 65, COLOR_BLACK);
    init_pair(46, 66, COLOR_BLACK);
    init_pair(47, 67, COLOR_BLACK);
    init_pair(48, 68, COLOR_BLACK);
    init_pair(49, 69, COLOR_BLACK);
    init_pair(50, 70, COLOR_BLACK);
    init_pair(124, 124, COLOR_BLACK);
    init_pair(117, 117, COLOR_BLACK);
    init_pair(172, 172, COLOR_BLACK);
    init_pair(208, 208, COLOR_BLACK);
    init_pair(222, 222, COLOR_BLACK);
    init_pair(223, 223, COLOR_BLACK);
    curs_set(0);
    clear();
    noecho();
    cbreak();
    int startx, starty, width, height;
    height = 13;
    width = 45;
    starty = (LINES - height) / 2;
    startx = (COLS - width) / 2;
    int y_kaka = 15;
    attron(COLOR_PAIR(4));
    box(stdscr, '|', '-');
    attroff(COLOR_PAIR(4));
    attron(COLOR_PAIR(6));
    mvprintw(LINES / 2 - 24 + y_kaka, (COLS - width) / 2 - 20, "          __     ");
    mvprintw(LINES / 2 - 23 + y_kaka, (COLS - width) / 2 - 20, "      |__|__     ");
    mvprintw(LINES / 2 - 22 + y_kaka, (COLS - width) / 2 - 20, "       __|  |     ");
    attroff(COLOR_PAIR(6));
    attron(COLOR_PAIR(5));
    mvprintw(LINES / 2 - 21 + y_kaka, (COLS - width) / 2 - 20, "      ___|__     ");
    mvprintw(LINES / 2 - 20 + y_kaka, (COLS - width) / 2 - 20, "     /      \\   ");
    mvprintw(LINES / 2 - 19 + y_kaka, (COLS - width) / 2 - 20, "    /  _  _  \\ ");
    mvprintw(LINES / 2 - 18 + y_kaka, (COLS - width) / 2 - 20, "   |          |   ");
    mvprintw(LINES / 2 - 17 + y_kaka, (COLS - width) / 2 - 20, "   |     |    |   ");
    mvprintw(LINES / 2 - 16 + y_kaka, (COLS - width) / 2 - 20, "   |  ______  |   ");
    mvprintw(LINES / 2 - 15 + y_kaka, (COLS - width) / 2 - 20, "  |   |____|  |  ");
    mvprintw(LINES / 2 - 14 + y_kaka, (COLS - width) / 2 - 20, " |            | ");
    mvprintw(LINES / 2 - 13 + y_kaka, (COLS - width) / 2 - 20, " \\           /  ");
    mvprintw(LINES / 2 - 12 + y_kaka, (COLS - width) / 2 - 20, "  \\ ______ /  ");
    attroff(COLOR_PAIR(5));
    attron(COLOR_PAIR(6));
    mvprintw(LINES / 2 - 22 + y_kaka, (COLS - width) / 2 + 45, "       ___      ");
    mvprintw(LINES / 2 - 21 + y_kaka, (COLS - width) / 2 + 45, "      _|*|__     ");
    attroff(COLOR_PAIR(6));
    attron(COLOR_PAIR(5));
    mvprintw(LINES / 2 - 20 + y_kaka, (COLS - width) / 2 + 45, "     /      \\   ");
    mvprintw(LINES / 2 - 19 + y_kaka, (COLS - width) / 2 + 45, "    /  _  _  \\ ");
    mvprintw(LINES / 2 - 18 + y_kaka, (COLS - width) / 2 + 45, "   |  |_||_|  |   ");
    mvprintw(LINES / 2 - 17 + y_kaka, (COLS - width) / 2 + 45, "   |    _|    |   ");
    mvprintw(LINES / 2 - 16 + y_kaka, (COLS - width) / 2 + 45, "   |  ______  |   ");
    mvprintw(LINES / 2 - 15 + y_kaka, (COLS - width) / 2 + 45, "   |    |+|    |  ");
    mvprintw(LINES / 2 - 14 + y_kaka, (COLS - width) / 2 + 45, "   |            | ");
    mvprintw(LINES / 2 - 13 + y_kaka, (COLS - width) / 2 + 45, "    \\           /  ");
    mvprintw(LINES / 2 - 12 + y_kaka, (COLS - width) / 2 + 45, "      \\ ______ /  ");
    attroff(COLOR_PAIR(5));
    menu_win = newwin(height, width, starty, startx);
    keypad(menu_win, TRUE);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    mouseinterval(0);
    curs_set(0);

    mvprintw(LINES / 2 - 21, (COLS - width) / 2 + 6, "_____________________________");
    mvprintw(LINES / 2 - 20, (COLS - width) / 2 + 6, "|                           |");
    mvprintw(LINES / 2 - 19, (COLS - width) / 2 + 6, "|  Welcome to AID Rouge =)  |");
    mvprintw(LINES / 2 - 18, (COLS - width) / 2 + 6, "|🗿  🗿  🗿   🗿  🗿  🗿  🗿|");
    mvprintw(LINES / 2 - 17, (COLS - width) / 2 - 14, "____________________|  🗿  🗿  🗿   🗿  🗿  🗿  |______________________");
    mvprintw(LINES / 2 - 16, (COLS - width) / 2 - 14, "|_____________________________________________________________________|");
    mvprintw(LINES / 2 - 15, (COLS - width) / 2 - 14, "|                                                                     |");
    mvprintw(LINES / 2 - 14, (COLS - width) / 2 - 14, "|  Use arrow keys to go up and down, press enter to select a choice!  |");
    mvprintw(LINES / 2 - 13, (COLS - width) / 2 - 14, "|_____________________________________________________________________|");
    refresh();
    highlight = 1;
    changes = 0;
    choice = 0;
    print_menu(menu_win, highlight, choices, n_choices);
    while(1) {
        c = wgetch(menu_win);
        getmouse(&mouse_check);
        switch(c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = n_choices;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == n_choices)
                    highlight = 1;
                else
                    ++highlight;
                break;
            case 10:
                choice = changes;
                break;
            case KEY_MOUSE:
                if(changes == highlight){
                    choice = changes;
                    highlight = changes;
                }
                else{
                    highlight = 0;
                }
                break;
            case 27:
                snprintf(result, sizeof(result), "%s", "exit");
                clrtoeol();
                refresh();
                delwin(menu_win);
                endwin();
                return result;
            default:
                break;
        }
        changes = highlight;
        print_menu(menu_win, highlight, choices, n_choices);
        highlight = changes;
        if (choice != 0)
            break;
    }
    snprintf(result, sizeof(result), "%s", choices[choice - 1]);
    clrtoeol();
    refresh();
    delwin(menu_win);
    endwin();
    printf("\033[?1003l\n");
    changes = 0;
    highlight = 0;
    choice = 0;
    return result;
}
