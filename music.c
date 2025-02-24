#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <dirent.h>
#include <string.h>
#include "music.h"

extern Mix_Music *music;

wchar_t notes[7] = {L'\U00002669', L'\U0000266A', L'\U0000266B', L'\U0000266C', L'\U0000266D', L'\U0000266E', L'\U0000266F'};

void print_music_menu(WINDOW *music_win, int highlight, char **choices, int n_choices, wchar_t *choices_for_each) {
    int x, y, i;
    x = 2;
    y = 2;
    box(music_win, 0, 0);
    mvwprintw(music_win, 0, (40 - strlen("[Music Menu]")) / 2, "[Music Menu]");
    for (i = 0; i < n_choices; ++i) {
        if (highlight == i + 1) {
            wattron(music_win, A_BOLD | A_REVERSE);
            mvwprintw(music_win, y, x, "%s %lc", choices[i], choices_for_each[i]);
            wattroff(music_win, A_BOLD | A_REVERSE);
        } else {
            mvwprintw(music_win, y, x, "%s %lc", choices[i], choices_for_each[i]);
        }
        y++;
    }
    y += 2;
    x += 2;
    wattron(music_win, COLOR_PAIR(1));
    mvwprintw(music_win, y - 1, x, "  ♩   _____     ♫       ___ ♬");
    mvwprintw(music_win, y, x, "♫     |    |      |  ♪ /   \\  ♭");
    mvwprintw(music_win, y + 1, x - 1, " ♪    _|   _|  ♮  _|   -|---|-  ♯");
    mvwprintw(music_win, y + 2, x - 1, "  ♯  |_|  |_|    |_| ♭  \\___/ ♮");
    box(stdscr, '|', '-');
    wattroff(music_win, COLOR_PAIR(1));
    wrefresh(music_win);
}

char* display_music_menu() {
    WINDOW *music_win;
    int highlight = 1;
    int choice = 0;
    int c;
    static char result[256];

    char *music_dir = "./music";
    DIR *dir;
    struct dirent *ent;
    char *choices[100];
    int n_choices = 0;

    if ((dir = opendir(music_dir)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strstr(ent->d_name, ".mp3") || strstr(ent->d_name, ".flac") || strstr(ent->d_name, ".wav")) {
                choices[n_choices] = strdup(ent->d_name);
                n_choices++;
            }
        }
        closedir(dir);
    } else {
        perror("Could not open music directory");
        return NULL;
    }
    wchar_t choices_for_each[n_choices];
    for(int i = 0; i < n_choices; i++){
        int which = rand() % 7;
        choices_for_each[i] = notes[which];
    }
    initscr();
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    curs_set(0);
    clear();
    noecho();
    cbreak();
    int startx, starty, width, height;
    height = n_choices + 9;
    width = 40;
    starty = (LINES - height) / 2;
    startx = (COLS - width) / 2;
    attron(COLOR_PAIR(1));
    box(stdscr, '|', '-');
    // mvprintw(27, 110, "Press Again");
    attroff(COLOR_PAIR(1));
    music_win = newwin(height, width, starty, startx);
    keypad(music_win, TRUE);
    mvprintw(LINES / 2 - n_choices / 2 - 7, COLS / 2 - 33, "Use arrow keys to go up and down, press enter to select a music file");
    refresh();
    print_music_menu(music_win, highlight, choices, n_choices, choices_for_each);
    while (1) {
        c = wgetch(music_win);
        switch (c) {
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
                choice = highlight;
                break;
            default:
                break;
        }
        print_music_menu(music_win, highlight, choices, n_choices, choices_for_each);
        if (choice != 0)
            break;
    }
    snprintf(result, sizeof(result), "%s/%s", music_dir, choices[choice - 1]);
    for (int i = 0; i < n_choices; i++) {
        free(choices[i]);
    }
    clrtoeol();
    refresh();
    endwin();
    return result;
}

void play_music(const char *filepath) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "SDL_mixer could not initialize: %s\n", Mix_GetError());
        return;
    }

    music = Mix_LoadMUS(filepath);
    // if (!music) {
    //     fprintf(stderr, "Failed to load music: %s\n", Mix_GetError());
    //     return;
    // }

    Mix_PlayMusic(music, 1);

    // printf("Playing %s...\nPress Enter to stop the music.\n", filepath);
    // getchar();

    // Mix_FreeMusic(music);
    // Mix_CloseAudio();
    // SDL_Quit();
}
