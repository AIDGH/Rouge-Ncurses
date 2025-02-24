#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "menu.h"
#include "outro.h"
#include "music.h"
#include "login.h"
#include "sign_up.h"
#include "before_game_menu.h"
#include "guest_before_game_menu.h"
#include "map_generator.h"
#include "settings.h"
#include "color_change.h"
#include "difficulty.h"
#include "user.h"
#include "scoreboard.h"
#include "profile.h"
#include "game.h"
#include "pause_menu.h"
#include "change_character.h"

Mix_Music *music;
User current_user;
extern Game *game;
wchar_t character;
char our_user[50] = "guest";
char which_menu[50] = "menu";
char which_user[50] = "guest";
char which_color[50] = " White ";
char which_difficulty[50] = " Easy ";
extern int current_level;
extern int unlocked_level;
int new_game_check;
extern int escape_check;
extern int difficulty_efficiency;
char* selected_music;

int main() {
    int current_level = 0;
    int unlocked_level_level = -1;
    new_game_check = 1;
    difficulty_efficiency = 1;
    escape_check = 0;
    character = L'\U00000040';
    selected_music = "./Base_Sounds/silent.wav";
    srand(time(NULL));
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    curs_set(0);
    noecho();
    cbreak();
    while (1) {
        if (!strcmp(which_menu, "exit")) {
            clear();
            refresh();
            display_message();
            break;
        } else if (!strcmp(which_menu, "menu") || !strcmp(which_menu, " Menu ") || !strcmp(which_menu, " Main menu ")) {
            clear();
            refresh();
            strcpy(which_menu, display_menu());
        } else if (!strcmp(which_menu, " MUSIC ") || !strcmp(which_menu, " Music ")) {
            clear();
            refresh();
            selected_music = display_music_menu();
            strcpy(which_menu, " Settings ");
        } else if (!strcmp(which_menu, " SIGN UP ")) {
            clear();
            refresh();
            user_input_screen();
            strcpy(which_menu, "menu");
        } else if (!strcmp(which_menu, " LOGIN ")) {
            clear();
            refresh();
            login_screen();
        } else if (!strcmp(which_menu, "Before game menu") || !strcmp(which_menu, " Before game menu ") || !strcmp(which_menu, " GAME MENU ")) {
            if(!strcmp(which_user, "guest")){
                strcpy(which_menu, "Guest before game menu");
                continue;
            }
            clear();
            refresh();
            before_game_menu();
        } else if (!strcmp(which_menu, "Guest before game menu")) {
            clear();
            refresh();
            guest_before_game_menu();
        } else if (!strcmp(which_menu, " Settings ") || !strcmp(which_menu, " SETTINGS ")) {
            clear();
            refresh();
            setting_menu();
        } else if (!strcmp(which_menu, " Color Change ")) {
            clear();
            refresh();
            color_change();
            strcpy(which_menu, " Settings ");
        } else if (!strcmp(which_menu, " Difficulty ")) {
            clear();
            refresh();
            difficulty_change();
            strcpy(which_menu, " Settings ");
        } else if (!strcmp(which_menu, " Character Change ")) {
            clear();
            refresh();
            character_change();
            strcpy(which_menu, " Settings ");
        } else if (!strcmp(which_menu, " SCOREBOARD ")) {
            clear();
            refresh();
            display_scoreboard();
            strcpy(which_menu, "menu");
        } else if (!strcmp(which_menu, " Scoreboard ")) {
            clear();
            refresh();
            display_scoreboard();
            strcpy(which_menu, " Before game menu ");
        } else if (!strcmp(which_menu, " Profile ")) {
            clear();
            refresh();
            display_profile();
            strcpy(which_menu, "Before game menu");
        }else if (!strcmp(which_menu, " New Game ") || !strcmp(which_menu, " Continue Previous Game ")) {
            new_game_check = 0;
            if(!strcmp(which_menu, " New Game ")){
                new_game_check = 1;
            }
            play_music(selected_music);
            current_level = 0;
            unlocked_level = -1;
            start_game();
            escape_check = 0;
            if(strcmp(game->player->name, "guest") && strcmp(which_user, "guest")){
                if(game->player->can_be_saved == 0){
                    save_just_user_info(game->player);
                    load_user_info(game->player->name, &current_user);
                    current_user.have_game = 0;
                }
                else{
                    save_just_user_info(game->player);
                    save_user_game(game);
                    load_user_info(game->player->name, &current_user);
                    current_user.have_game = 1;
                }
            }
            else if(!strcmp(which_user, "guest")){
                game->player->name = "You";
                save_just_user_info(game->player);
            }
            strcpy(which_menu, "menu");
        }
    }
    endwin();
    return 0;
}
