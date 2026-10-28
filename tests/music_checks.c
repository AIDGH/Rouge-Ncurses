#include "../music.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
Mix_Music *music;char *selected_music="./Base_Sounds/silent.wav";
static int playing,paused,plays;static char loaded[256];
int SDL_Init(Uint32 f){return 0;}const char *SDL_GetError(void){return "test";}
int Mix_QuerySpec(int *f,Uint16 *fmt,int *ch){return 1;}
int Mix_OpenAudio(int f,Uint16 fmt,int ch,int n){return 0;}
Mix_Music *Mix_LoadMUS(const char *p){snprintf(loaded,sizeof(loaded),"%s",p);return (Mix_Music *)malloc(1);}
void Mix_FreeMusic(Mix_Music *m){free(m);}
int Mix_HaltMusic(void){playing=0;return 0;}
int Mix_PlayMusic(Mix_Music *m,int loops){assert(m&&loops==0);playing=1;paused=0;plays++;return 0;}
int Mix_PlayingMusic(void){return playing;}int Mix_PausedMusic(void){return paused;}
int main(void){
    srand(14);mkdir("music",0700);
    for(int i=0;i<3;i++){char p[64];snprintf(p,sizeof(p),"music/%d.mp3",i);FILE *f=fopen(p,"w");assert(f);fclose(f);}
    play_music("./music/0.mp3");assert(plays==1&&!strcmp(selected_music,"./music/0.mp3"));
    int seen=1;char previous[256];snprintf(previous,sizeof(previous),"%s",selected_music);
    for(int i=0;i<2;i++){
        playing=0;music_tick();int n=atoi(strrchr(selected_music,'/')+1);assert(!(seen&(1<<n)));seen|=1<<n;
        assert(strcmp(previous,selected_music));snprintf(previous,sizeof(previous),"%s",selected_music);
    }assert(seen==7);
    playing=0;music_tick();assert(strcmp(previous,selected_music));
    int before=plays;playing=0;paused=1;music_tick();assert(plays==before);
    paused=0;music_tick();assert(plays==before+1);
    music_stop_playlist();playing=0;music_tick();assert(plays==before+1);
    play_music("./Base_Sounds/silent.wav");music_tick();assert(!music&&plays==before+1);
    puts("PASS shuffle without repeats, seamless cycle boundary, one playback per track, pause, stop and silent mode");
}
