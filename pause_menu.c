#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "pause_menu.h"
#include "music.h"

extern char which_pause_menu[50];
extern char *selected_music;
extern char which_user[50];

char *pause_menu_choices[] = {
    " Resume Game ",
    " Change Character ",
    " Change Color ",
    " Select Music ",
    " Pause Music ",
    " Resume Music ",
    " Save and Quit "
};

int n_pause_menu_choices = sizeof(pause_menu_choices) / sizeof(char *);

void print_pause_menu(WINDOW *menu_win, int highlight) {
    int x, y, i;
    x = 5;
    y = 5;
    box(menu_win, 0, 0);
    mvwprintw(menu_win, 0, (50 - strlen(" Choose Option ")) / 2, "[Choose Option]");
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
    mvwprintw(menu_win, 14, x - 2, "|");
    mvwprintw(menu_win, 16, x - 2, "|");
    mvwprintw(menu_win, 18, x - 2, "|");
    wattroff(menu_win, A_BOLD | COLOR_PAIR(6));
    mvwprintw(menu_win, 18, 4, "         __   __   __   __   __ ");
    mvwprintw(menu_win, 19, 2, " |________|--| |--| |--| |--| |--| ");
    mvwprintw(menu_win, 20, 2, "          |--| |--| |--| |--| |--| ");
    mvwprintw(menu_win, 21, 2, "          \\_/  \\_/  \\_/  \\_/  \\_/");
    
    for (i = 0; i < n_pause_menu_choices; ++i) {
        if (highlight == i + 1) {
            wattron(menu_win, A_BOLD | COLOR_PAIR(6));
            if(strcmp(selected_music, "./Base_Sounds/silent.wav") && i == 3){
                mvwprintw(menu_win, y, x - 2, "*  Change Music ");
            }
            else if(!strcmp(which_user , "guest") && i == 6){
                mvwprintw(menu_win, y, x - 2, "*  Quit ");
            }
            else{
                mvwprintw(menu_win, y, x - 2, "* %s", pause_menu_choices[i]);
            }
            wattroff(menu_win, A_BOLD | COLOR_PAIR(6));
        } else {
            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && (i == 4 || i == 5)){
                wattron(menu_win, COLOR_PAIR(20));
            }
            if(strcmp(selected_music, "./Base_Sounds/silent.wav") && i == 3){
                mvwprintw(menu_win, y, x - 2, "|  Change Music ");
            }
            else if(!strcmp(which_user , "guest") && i == 6){
                mvwprintw(menu_win, y, x - 2, "|  Quit ");
            }
            else{
                mvwprintw(menu_win, y, x - 2, "| %s", pause_menu_choices[i]);
            }
            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && (i == 4 || i == 5)){
                wattroff(menu_win, COLOR_PAIR(20));
            }
            // mvwprintw(menu_win, y - 1, x - 2, "|");
        }
        y += 2;
    }
    wrefresh(menu_win);
}

void pause_menu() {
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int c;

    int startx, starty, width, height;
    height = 24;
    width = 50;
    starty = (LINES - height) / 2;
    startx = (COLS - width) / 2;
    menu_win = newwin(height, width, starty, startx);
    if(!strcmp(which_pause_menu, " Save and Quit ")){
        wclear(menu_win);
        for(int i = 0; i < 3; i++){
            wattron(menu_win, COLOR_PAIR(20));
            mvwprintw(menu_win, height / 2, width / 2 - 5, "Loading.  ");
            wattroff(menu_win, COLOR_PAIR(20));
            wrefresh(menu_win);
            usleep(300000);
            wattron(menu_win, COLOR_PAIR(21));
            mvwprintw(menu_win,  height / 2, width / 2 - 5, "Loading.. ");
            wattroff(menu_win, COLOR_PAIR(21));
            wrefresh(menu_win);
            usleep(300000);
            wattron(menu_win, COLOR_PAIR(10));
            mvwprintw(menu_win,  height / 2, width / 2 - 5, "Loading...");
            wattroff(menu_win, COLOR_PAIR(10));
            wrefresh(menu_win);
            usleep(300000);
        }
        return;
    }
    keypad(menu_win, TRUE);
    print_pause_menu(menu_win, highlight);
    
    while (1) {
        int a = 0;
        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                a = 1;
                if (highlight == 1)
                    highlight = n_pause_menu_choices;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == n_pause_menu_choices)
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
        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && (highlight == 5 || highlight == 6)){
            highlight = 7;
            if(a){
                highlight = 4;
            }
        }
        print_pause_menu(menu_win, highlight);
        if (choice != 0)
            break;
    }
    strcpy(which_pause_menu, pause_menu_choices[choice - 1]);
    if(!strcmp(which_pause_menu, " Save and Quit ")){
        wclear(menu_win);
        for(int i = 0; i < 3; i++){
            wattron(menu_win, COLOR_PAIR(20));
            mvwprintw(menu_win, height / 2, width / 2 - 5, "Loading.  ");
            wattroff(menu_win, COLOR_PAIR(20));
            wrefresh(menu_win);
            usleep(300000);
            wattron(menu_win, COLOR_PAIR(21));
            mvwprintw(menu_win,  height / 2, width / 2 - 5, "Loading.. ");
            wattroff(menu_win, COLOR_PAIR(21));
            wrefresh(menu_win);
            usleep(300000);
            wattron(menu_win, COLOR_PAIR(10));
            mvwprintw(menu_win,  height / 2, width / 2 - 5, "Loading...");
            wattroff(menu_win, COLOR_PAIR(10));
            wrefresh(menu_win);
            usleep(300000);
        }
        return;
    }
    clrtoeol();
    refresh();
    // getch();
    delwin(menu_win);
}