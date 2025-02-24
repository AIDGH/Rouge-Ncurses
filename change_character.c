#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include "change_character.h"

extern wchar_t character;
// ♚ L'\U0000265A' ♕ L'\U00002655' ♙ L'\U00002659' ⛐ L'\U000026D0' ඞ L'\U00000D9E' ⛇ L'\U000026C7'
// ⚛ L'\U0000269B' ✺ L'\U0000273A' ✾ L'\U0000273E' ☀ L'\U00002600' ❂ L'\U00002742' ✡ L'\U00002721' 
// ⛂ L'\U000026C2' ⛃ L'\U000026C3' ⛀ L'\U000026C0' ⛁ L'\U000026C1' ⏣ L'\U000023E3' ⌘ L'\U00002318'
// @ L'\U00000040' ✯ L'\U0000272F' ✠ L'\U00002720' ☫ L'\U0000262B' ☣ L'\U00002623' ☠ L'\U00002620'
// ◉ L'\U000025C9' ⛒ L'\U000026D2' ㊝ L'\U0000329D' (֍ L'\U0000058D' ♨ L'\U00002668') ☺ L'\U0000263A' ☻ L'\U0000263B' ◕ L'\U000025D5'
// ℗ L'\U00002117' ♾ L'\U0000267E' ⚙ L'\U00002699' ⍟ L'\U0000235F' ☮ L'\U0000262E' ◎ L'\U000025CE'
char *character_choices[10][10] = {
    {"♚", "♕", "♙", "⛐", "ඞ", "⛇"},
    {"⚛", "✺", "✾", "☀", "❂", "✡"},
    {"⛂", "⛃", "⛀", "⛁", "⏣", "⌘"},
    {"@", "✯", "✠", "☫", "☣", "☠"},
    {"◉", "⛒", "♨", "☺", "☻", "◕"},
    {"℗", "♾", "⚙", "⍟", "☮", "◎"},
};
wchar_t character_results[10][10] = {
    { L'\U0000265A', L'\U00002655', L'\U00002659', L'\U000026D0', L'\U00000D9E', L'\U000026C7'},
    {L'\U0000269B', L'\U0000273A', L'\U0000273E', L'\U00002600',L'\U00002742', L'\U00002721'},
    {L'\U000026C2', L'\U000026C3', L'\U000026C0', L'\U000026C1', L'\U000023E3', L'\U00002318'},
    {L'\U00000040', L'\U0000272F', L'\U00002720', L'\U0000262B', L'\U00002623', L'\U00002620'},
    {L'\U000025C9', L'\U000026D2', L'\U00002668', L'\U0000263A', L'\U0000263B', L'\U000025D5'},
    {L'\U00002117', L'\U0000267E', L'\U00002699', L'\U0000235F', L'\U0000262E', L'\U000025CE'},
};

int n_character_choices_vertical = 6;
int n_character_choices_horizontal = 6;

void print_character_change(WINDOW *menu_win, int highlight_vertical, int highlight_horizontal) {
    int x, y, i;
    x = 5;
    y = 19;
    box(menu_win, 0, 0);
    wattron(menu_win, COLOR_PAIR(25));
    wattron(menu_win, A_BLINK);
    int b1 = 0;
    mvwprintw(menu_win, 0, 0 + b1,  "                  _..oo8\"\"\"Y8b.._                      ");
    mvwprintw(menu_win, 1, 0 + b1,  "                .88888888o.    \"Yb.                      ");
    mvwprintw(menu_win, 2, 0 + b1,  "              .d888P\"\"Y8888b      \"b.                      ");
    mvwprintw(menu_win, 3, 0 + b1,  "             o88888    88888)       \"b                      ");
    mvwprintw(menu_win, 4, 0 + b1,  "            d888888b..d8888P         'b                      ");
    mvwprintw(menu_win, 5, 0 + b1,  "            88888888888888\"           8                      ");
    mvwprintw(menu_win, 6, 0 + b1,  "           (88DWB8888888P             8)                      ");
    mvwprintw(menu_win, 7, 0 + b1,  "            8888888888P               8                      ");
    mvwprintw(menu_win, 8, 0 + b1,  "            Y88888888P     ee        .P                      ");
    mvwprintw(menu_win, 9, 0 + b1,  "             Y888888(     8888      oP                      ");
    mvwprintw(menu_win, 10, 0 + b1,  "              \"Y88888b     \"\"     oP\"                      ");
    mvwprintw(menu_win, 11, 0 + b1,  "                \"Y8888o._     _.oP\"                      ");
    mvwprintw(menu_win, 12, 0 + b1,  "                  `\"\"Y888boodP\"\"'                      ");
    mvwprintw(menu_win, 13, 0,  " ");
    mvwprintw(menu_win, 13, 49,  " ");
    wattroff(menu_win, A_BLINK);
    wattroff(menu_win, COLOR_PAIR(25));
    mvwprintw(menu_win, 14, 0, "┌───────────────────[Character]──────────────────┐");
    mvwprintw(menu_win, 15, 2, "          ____ ____ ____ ____ ____ ");
    mvwprintw(menu_win, 16, 2, " _________|__| |__| |__| |__| |__| ");
    mvwprintw(menu_win, 17, 2, " |        |__| |__| |__| |__| |__| ");
    mvwprintw(menu_win, 28, 2, " |                                  ");
    mvwprintw(menu_win, 18, x - 2, "|");
    mvwprintw(menu_win, 19, x - 2, "|");
    mvwprintw(menu_win, 20, x - 2, "|");
    mvwprintw(menu_win, 21, x - 2, "|");
    mvwprintw(menu_win, 22, x - 2, "|");
    mvwprintw(menu_win, 23, x - 2, "|");
    mvwprintw(menu_win, 24, x - 2, "|");
    mvwprintw(menu_win, 25, x - 2, "|");
    mvwprintw(menu_win, 26, x - 2, "|");
    mvwprintw(menu_win, 27, x - 2, "|");
    mvwprintw(menu_win, 28, x - 2, "|");
    mvwprintw(menu_win, 29, x - 2, "|");
    mvwprintw(menu_win, 30, x - 2, "|");
    mvwprintw(menu_win, 30, 4, "         __   __   __   __   __ ");
    mvwprintw(menu_win, 31, 2, " |________|--| |--| |--| |--| |--| ");
    mvwprintw(menu_win, 32, 2, "          |--| |--| |--| |--| |--| ");
    mvwprintw(menu_win, 33, 2, "          \\_/  \\_/  \\_/  \\_/  \\_/");
    x = 13;
    for (i = 0; i < n_character_choices_vertical; ++i) {
        for(int j = 0; j < n_character_choices_horizontal; ++j){
            if (highlight_vertical == i + 1 && highlight_horizontal == j + 1){
                wattron(menu_win, A_BOLD | COLOR_PAIR(25));
                mvwprintw(menu_win, y, x + 4 * j, "%s", character_choices[i][j]);
                wattroff(menu_win, A_BOLD | COLOR_PAIR(25));
            }
             else{
                 wattron(menu_win, COLOR_PAIR(14));
                 mvwprintw(menu_win, y, x + 4 * j, "%s", character_choices[i][j]);
                 wattroff(menu_win, COLOR_PAIR(14));
            }
        }
        y += 2;
    }
    wrefresh(menu_win);
}

void character_change() {
    WINDOW *menu_win;
    int highlight_vertical = 1;
    int highlight_horizontal = 1;
    int choice = 0;
    int c;

    int startx, starty, width, height;
    height = 35;
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
    print_character_change(menu_win, highlight_vertical, highlight_horizontal);
    
    while (1) {
        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                if (highlight_vertical == 1)
                    highlight_vertical = n_character_choices_vertical;
                else
                    --highlight_vertical;
                break;
            case KEY_DOWN:
                if (highlight_vertical == n_character_choices_vertical)
                    highlight_vertical = 1;
                else
                    ++highlight_vertical;
                break;
            case KEY_LEFT:
                if (highlight_horizontal == 1)
                    highlight_horizontal = n_character_choices_horizontal;
                else
                    --highlight_horizontal;
                break;
            case KEY_RIGHT:
                if (highlight_horizontal == n_character_choices_horizontal)
                    highlight_horizontal = 1;
                else
                    ++highlight_horizontal;
                break;
            case 10:
                choice = highlight_vertical;
                character = character_results[highlight_vertical - 1][highlight_horizontal - 1];
                break;
            default:
                break;
        }
        print_character_change(menu_win, highlight_vertical, highlight_horizontal);
        if (choice != 0)
            break;
    }
    clrtoeol();
    refresh();
    wclear(menu_win);
    werase(menu_win);
    wrefresh(menu_win);
    delwin(menu_win);
}