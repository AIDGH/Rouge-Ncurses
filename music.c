#include "ui_theme.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <dirent.h>
#include <string.h>
#include <strings.h>
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
    ui_connected_art(music_win);
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
            if (n_choices < 100 && (strstr(ent->d_name, ".mp3") || strstr(ent->d_name, ".flac") || strstr(ent->d_name, ".wav"))) {
                choices[n_choices] = strdup(ent->d_name);
                n_choices++;
            }
        }
        closedir(dir);
    } else {
        perror("Could not open music directory");
        return NULL;
    }
    if(n_choices == 0) {
        clear();mvprintw(LINES/2,COLS/2-29,"No music yet. Add MP3, WAV or FLAC files to music/.");
        mvprintw(LINES/2+2,COLS/2-17,"Press any key to go back.");refresh();getch();return NULL;
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
        c = music_getch(music_win);
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
            case 27:
                for(int i=0;i<n_choices;i++)free(choices[i]);
                delwin(music_win);return NULL;
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
    delwin(music_win);
    return result;
}


#define PLAYLIST_MAX 100
static char tracks[PLAYLIST_MAX][256],current_track[256];
static int bag[PLAYLIST_MAX],track_count,bag_count,last_track=-1,playlist_active;
extern char *selected_music;
static int track_file(const char *name){
    const char *ext=strrchr(name,'.');
    return ext&&(!strcasecmp(ext,".mp3")||!strcasecmp(ext,".wav")||!strcasecmp(ext,".flac"));
}
static void scan_playlist(void){
    track_count=bag_count=0;last_track=-1;
    DIR *dir=opendir("music");if(!dir)return;struct dirent *ent;
    while((ent=readdir(dir))&&track_count<PLAYLIST_MAX)
        if(track_file(ent->d_name)&&strlen(ent->d_name)<248)
            snprintf(tracks[track_count++],256,"./music/%s",ent->d_name);
    closedir(dir);
}
static void shuffle_bag(void){
    bag_count=track_count;
    for(int i=0;i<track_count;i++)bag[i]=i;
    for(int i=track_count-1;i>0;i--){int j=rand()%(i+1),tmp=bag[i];bag[i]=bag[j];bag[j]=tmp;}
    if(track_count>1&&bag[bag_count-1]==last_track){int tmp=bag[0];bag[0]=bag[bag_count-1];bag[bag_count-1]=tmp;}
}
static int start_track(const char *path){
    Mix_Music *next=Mix_LoadMUS(path);if(!next)return 0;
    Mix_HaltMusic();if(music)Mix_FreeMusic(music);music=next;
    if(Mix_PlayMusic(music,0)<0){Mix_FreeMusic(music);music=NULL;return 0;}
    snprintf(current_track,sizeof(current_track),"%s",path);selected_music=current_track;return 1;
}
void music_stop_playlist(void){playlist_active=0;}
void music_tick(void){
    if(!playlist_active||Mix_PlayingMusic()||Mix_PausedMusic())return;
    for(int attempt=0;attempt<track_count;attempt++){
        if(!bag_count)shuffle_bag();int next=bag[--bag_count];
        if(start_track(tracks[next])){last_track=next;return;}
    }
    playlist_active=0;
}
/* Poll only audio while waiting; a timeout never becomes a game turn. */
int music_getch(WINDOW *win){
    int delay=wgetdelay(win);music_tick();
    if(delay>=0)return wgetch(win);
    wtimeout(win,100);int key;
    do{key=wgetch(win);music_tick();}while(key==ERR);
    wtimeout(win,delay);return key;
}
void play_music(const char *filepath){
    playlist_active=0;
    if(!filepath)return;
    char chosen[256];snprintf(chosen,sizeof(chosen),"%s",filepath);
    if(!Mix_QuerySpec(NULL,NULL,NULL)){
        if(SDL_Init(SDL_INIT_AUDIO)<0||Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048)<0){
            fprintf(stderr,"Audio unavailable: %s\n",SDL_GetError());return;
        }
    }
    scan_playlist();
    if(!strcmp(chosen,"./Base_Sounds/silent.wav")){
        Mix_HaltMusic();if(music)Mix_FreeMusic(music);music=NULL;return;
    }
    playlist_active=track_count>0;
    if(start_track(chosen)){
        for(int i=0;i<track_count;i++)if(!strcmp(chosen,tracks[i]))last_track=i;
        shuffle_bag();
        /* The selected opening track counts as played in this cycle. */
        for(int i=0;i<bag_count;i++)if(bag[i]==last_track){bag[i]=bag[--bag_count];break;}
    }else music_tick();
}
