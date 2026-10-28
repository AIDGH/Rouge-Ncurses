/* Headless memory tests: no audio device, SDL constructor or game music. */
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
int SDL_Init(Uint32 f) { return 0; }
const char *SDL_GetError(void) { return "headless test"; }
int Mix_OpenAudio(int f, Uint16 fmt, int ch, int n) { return 0; }
Mix_Music *Mix_LoadMUS(const char *p) { return NULL; }
int Mix_PlayMusic(Mix_Music *m,int loops) { return 0; }
int Mix_HaltMusic(void) { return 0; }
void Mix_PauseMusic(void) {}
void Mix_ResumeMusic(void) {}

void Mix_FreeMusic(Mix_Music *m) {}

int Mix_PlayingMusic(void){return 0;}
int Mix_PausedMusic(void){return 0;}
int Mix_QuerySpec(int *f,Uint16 *fmt,int *ch){return 1;}
