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
#include "user.h"
#include "map_generator.h"
#include "music.h"
#include "game.h"
#include "before_game_menu.h"
#include "guest_before_game_menu.h"
#include "pause_menu.h"
#include "change_character.h"
#include "color_change.h"
#include "ui_theme.h"
static void legacy_draw_non_flashlight_room(WINDOW *game_win, Room *room, int room_number);
static void legacy_draw_room(WINDOW *game_win, Room *room, int room_number);
static void legacy_draw_last_room(WINDOW *game_win, Room *room, int room_number);
static void legacy_draw_nightmare_room(WINDOW *game_win, Room *room, int room_number);
static void legacy_draw_attacked_room(WINDOW *game_win, Room *room, int room_number);
static void legacy_draw_ranged_attacked_room(WINDOW *game_win, Room *room, int room_number, int dx, int dy);
static void legacy_print_map(WINDOW *game_win);
static void legacy_print_all_map(WINDOW *game_win);
static void legacy_guid(WINDOW *game_win);
static void legacy_quoting(WINDOW *game_win);
static void legacy_messaging(WINDOW *game_win);
static int victory_results_closed(void);
static void victory_results_start(int gold,int play_time);
static void victory_results_screen(WINDOW *canvas,time_t began);

// https://www.rapidtables.com/code/text/unicode-characters.html
// Characters1: @ ♚ L'\U0000265A' ♕ L'\U00002655' ♙ L'\U00002659' ⛐ L'\U000026D0' ඞ L'\U00000D9E' ⛇ L'\U000026C7'
// Characters2: ⚛ L'\U0000269B' ✺ L'\U0000273A' ✾ L'\U0000273E' ☀ L'\U00002600 ❂ L'\U00002742 ✡ L'\U00002721 
// Characters3: ⛂ L'\U000026C2' ⛃ L'\U000026C3' ⛀ L'\U000026C0' ⛁ L'\U000026C1' ⏣ L'\U000023E3' ⌘ L'\U00002318'
// Characters4: ☠ L'\U00002620' ☣ L'\U00002623' ✯ L'\U0000272F' ✠ L'\U00002720' ☫ L'\U0000262B' ⚙ L'\U00002699'
// Characters5: ◉ L'\U00002539' ⛒ L'\U000026D2' ㊝ L'\U0000329D' ☺ L'\U0000263A' ☻ L'\U0000263B' ◕ L'\U000025D5'
// Characters6: ℗ L'\U00002117' ♾ L'\U0000267E' ☉ L'\U00002609 ⍟ L'\U0000235F' ☮ L'\U0000262E' ◎ L'\U000025CE'
// Wall and Door and Windows: ┌ ┐ └ ┘ ─ │   ┼   ├ ┬ ┴ ┤
// Trap and Stair: ^   ≡
// Corridors and Pillars: █ ▓
extern wchar_t character;
extern Mix_Music *music;
Mix_Music *effects;
Mix_Music *walk_effect;
Mix_Music *eating_effect;
Mix_Music *glass_effect;
Mix_Music *magic_effect;
Mix_Music *magicfood_effect;
Mix_Music *weaponpickup_effect;
Mix_Music *shieldpickup_effect;
Mix_Music *shieldbreak_effect;
Mix_Music *forge_effect;
Mix_Music *coin_effect;
Mix_Music *black_coin_effect;
Mix_Music *mace_effect;
Mix_Music *dagger_effect;
Mix_Music *wand_effect;
Mix_Music *arrow_effect;
Mix_Music *sword_effect;
Mix_Music *damage_effect;
Mix_Music *loosing_effect;
Mix_Music *winning_effect;
Level *levels[6];
Game *game;
bool game_session_ready;

void release_room(Room *r) {
    free(r->doors); free(r->pillars); free(r->foods); free(r->shields);
    free(r->specialfoods); free(r->golds); free(r->weapons); free(r->spells);
    free(r->traps); free(r->enemies); free(r->newWeapons);
    memset(r, 0, sizeof(*r));
}
void release_game(void) {
    if (!game) return;
    for (int i=0;i<MAX_LEVELS;i++) {
        if (!game->levels[i]) continue;
        for (int j=0;j<MAX_ROOMS;j++) release_room(game->levels[i]->rooms[j]);
        free(game->levels[i]->rooms[0]); free(game->levels[i]->rooms); free(game->levels[i]);
    }
    free(game->levels); free(game->player->weapons); free(game->player->spells);
    free(game->player->specialfoods); free(game->player); free(game); game=NULL;
}

// Map whole_map[6][200][200];
Corridor *corridors[50];
Player *player;
extern User current_user;
MEVENT mouse_event;
int dx = 0, dy = 0, side = 0;
Point mouse_save;
extern int total_rooms;
int current_level;
int unlocked_level;
char which_pause_menu[50];
extern char which_user[50];
extern char which_color[50];
extern int new_game_check;
int color_pair_num;
int difficulty_efficiency;
int escape_check = 0;
int run_finished = 0;
char diff_from_map = 0;
int in_emoji = 0;
int lock_color = 6;
int password_disapear = 0;
int last_attemps = 0;
int back_front = 0;
int in_nightmare = 0;
int in_spell = 0;
int in_reinforce = 0;
int in_healing = 0;
int all_print = 0;
int t_check = 0;
int g_check = 0;
int f_check = 0;
int valid_f_check = 0;
int x_check = 0;
int m_check = 0;
int count_move = 1;
int poisonous_food_check = 0;
int last_item = '.';
int move_or_not = 0;
int is_enemy_or_not = 0;
int is_enemy_or_not2 = 0;
int is_last_room = 0;
int is_pass_lock = 0;
int which_pos = 1;
int last_one = 0;
int first_gold = 0;
int first_spell = 0;
int first_weapon = 0;
int first_shield = 0;
int first_food = 0;
int first_trap = 0;
int first_specialfood = 0;
int first_ancientkey = 0;
int first_password = 0;
int first_damage = 0;
int first_attack = 0;
int first_enemy_near = 0;
int first_lock_door = 0;
int first_stair = 0;
// int first_
long long pointss;
int play_timee;
int move_click;
int direction;
int move_direction;
extern char* selected_music;
int x_save, y_save, level_save, is_battle = 0, after_battle = 0, room_num_save;
time_t start_time;
time_t quote_time;
time_t quote_time2;
time_t move_time;
time_t move_time_2;
time_t move_time_lock;
time_t rotten_time;
time_t rotten_time_check;
time_t last_room1;
time_t last_room2;
time_t save_counter1;
time_t save_counter2;
time_t end_time;
clock_t drag1;
clock_t drag2;

void in_emoji_check(){
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '>'){
        in_emoji = 1;
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '<'){
        in_emoji = 1;
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '*'){
        in_emoji = 1;
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'E'){
        in_emoji = 1;
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'P'){
        in_emoji = 1;
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'H'){
        in_emoji = 1;
        return;
    }
    // if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '1'){
    //     in_emoji = 1;
    //     return;
    // }
    // if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '2'){
    //     in_emoji = 1;
    //     return;
    // }
    // if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '3'){
    //     in_emoji = 1;
    //     return;
    // } 
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '6'){
        in_emoji = 1;
        return;
    } 
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '7'){
        in_emoji = 1;
        return;
    } 
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '8'){
        in_emoji = 1;
        return;
    } 
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '9'){
        in_emoji = 1;
        return;
    } 
    // if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '!'){
    //     in_emoji = 1;
    //     return;
    // } 
    // if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '4'){
    //     in_emoji = 1;
    //     return;
    // }
    // if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '5'){
    //     in_emoji = 1;
    //     return;
    // }
}
int is_enemy(int y, int x){
    if(y<0 || y>=MAP_ROWS || x<0 || x>=MAP_COLS) return 0;
    if(game->whole_map[game->currentLevel][y][x].item == 'D' 
        || game->whole_map[game->currentLevel][y][x].item == 'F'
        || game->whole_map[game->currentLevel][y][x].item == 'G'
        || game->whole_map[game->currentLevel][y][x].item == 'S'
        || game->whole_map[game->currentLevel][y][x].item == 'U'
        || game->whole_map[game->currentLevel][y][x].item == 'I'
        || game->whole_map[game->currentLevel][y][x].item == 'i' - 32){
            return 1;
        }
    return 0;
}
void draw_non_flashlight_room(WINDOW *game_win, Room *room, int room_number) {
    if(ui_old_visuals){legacy_draw_non_flashlight_room(game_win, room, room_number);return;}
    // wattron(game_win, COLOR_PAIR(color_pair_num));
    if(!m_check){
        wattroff(game_win, COLOR_PAIR(3));
    }
    if(room->type == 's' && room->isVisible == true && room->doorCount == 1){
        wattron(game_win, COLOR_PAIR(15));
    }
    else if(room->type == 't' && room->isVisible == true){
        wattron(game_win, COLOR_PAIR(22));
    }
    else if(room->type == 'b' && room->isVisible == true){
        wattron(game_win, COLOR_PAIR(14));
    }
    else if(room->type == 'p' && room->isVisible == true){
        if(room_number == game->player->room_num){
            in_spell = 1;
        }
        wattron(game_win, COLOR_PAIR(17));
    }
    else if(room->type == 'r' && room->isVisible == true){
        if(room_number == game->player->room_num){
            in_reinforce = 1;
        }
        wattron(game_win, COLOR_PAIR(25));
    }
    else if(room->type == 's' && room->isVisible == true){
        if(room_number == game->player->room_num){
            in_healing = 1;
        }
        wattron(game_win, COLOR_PAIR(15));
    }
    if(room->type == 'm' && room->isVisible == true && all_print == 0){
        in_nightmare = 1;
        return;
    }
    if(room->type == 'm' && room->isVisible == true){
        wattron(game_win, COLOR_PAIR(6));
    }
    all_print = 1;
    in_nightmare = 0;
    room->lock_attemps = 0;
    // if(room->type == 'b'){
    //     for(int i = 0; i < room->pillarCount; i++){
    //         wattron(game_win, COLOR_PAIR(16));
    //         mvwprintw(game_win, room->pillars[i].y, room->pillars[i].x, "▒");
    //         wattroff(game_win, COLOR_PAIR(16));
    //     }
    //     wattron(game_win, COLOR_PAIR(14));
    //     for (int y = room->y; y < room->y + room->height; y++) {
    //         for (int x = room->x; x < room->x + room->width; x++) {
    //             if(y == room->y && x == room->x){
    //                 mvwprintw(game_win, y, x, "┌");
    //             }
    //             else if(y == room->y + room->height - 1 && x == room->x){
    //                 mvwprintw(game_win, y, x, "└");
    //             }
    //             else if(y == room->y && x == room->x + room->width - 1){
    //                 mvwprintw(game_win, y, x, "┐");
    //             }
    //             else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
    //                 mvwprintw(game_win, y, x, "┘");
    //             }
    //             else if (y == room->y) {
    //                 mvwprintw(game_win, y, x, "─");
    //             } else if (y == room->y + room->height - 1) {
    //                 mvwprintw(game_win, y, x, "─");
    //             } else if (x == room->x || x == room->x + room->width - 1) {
    //                 mvwprintw(game_win, y, x, "│");
    //             } else {
    //                 mvwprintw(game_win, y, x, ".");
    //             }
    //         }
    //     }
    //     wattroff(game_win, COLOR_PAIR(14));
    //     wattron(game_win, COLOR_PAIR(6));
    //     for(int i = 0; i < room->enemyCount; i++){
    //         if(room->enemies[i].isVisible == true){
    //             mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
    //         }
    //     }
    //     wattroff(game_win, COLOR_PAIR(6));
    //     wattron(game_win, COLOR_PAIR(color_pair_num));
    //     draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
    //     wattroff(game_win, COLOR_PAIR(color_pair_num));
    //     return;
    // }
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            if(y == room->y && x == room->x){
                // mvwprintw(game_win, y, x, "┌");
                mvwprintw(game_win, y, x, "┌");
            }
            else if(y == room->y + room->height - 1 && x == room->x){
                // mvwprintw(game_win, y, x, "└");
                mvwprintw(game_win, y, x, "└");
            }
            else if(y == room->y && x == room->x + room->width - 1){
                // mvwprintw(game_win, y, x, "┐");
                mvwprintw(game_win, y, x, "┐");
            }
            else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
                // mvwprintw(game_win, y, x, "┘");
                mvwprintw(game_win, y, x, "┘");
            }
            else if (y == room->y) {
                // mvwprintw(game_win, y, x, "─");
                mvwprintw(game_win, y, x, "─");
            } else if (y == room->y + room->height - 1) {
                // mvwprintw(game_win, y, x, "─");
                mvwprintw(game_win, y, x, "─");
            } else if (x == room->x || x == room->x + room->width - 1) {
                // mvwprintw(game_win, y, x, "│");
                mvwprintw(game_win, y, x, "│");
            } else {
                mvwprintw(game_win, y, x, ".");
            }
        }
    }
    for(int i = 0; i < room->pillarCount; i++){
        wattron(game_win, COLOR_PAIR(16));
        if(room->type == 'b'){
            wattron(game_win, COLOR_PAIR(20));
        }
        mvwprintw(game_win, room->pillars[i].y, room->pillars[i].x, "▒");
        if(room->type == 'b'){
            wattroff(game_win, COLOR_PAIR(20));
        }
        wattroff(game_win, COLOR_PAIR(16));
    }
    for(int i = 0; i < room->foodCount; i++){
        if(room->foods[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(7));
            mvwprintw(game_win, room->foods[i].cord.y, room->foods[i].cord.x, "♣");
            wattroff(game_win, COLOR_PAIR(7));
        }
    }
    for(int i = 0; i < room->shieldCount; i++){
        if(room->shields[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(19));
            mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x + 1, " ");
            mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x, "◇");
            wattroff(game_win, COLOR_PAIR(19));
        }
    }
    for(int i = 0; i < room->goldCount; i++){
        if(room->golds[i].isUsed == false){
            if(room->golds[i].type == 'n'){
                wattron(game_win, COLOR_PAIR(12));
                mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "¤");
                wattroff(game_win, COLOR_PAIR(12));
            }
            else if(room->golds[i].type == 'b'){
                mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "●");
            }
        }
    }
    for(int i = 0; i < room->spellCount; i++){
        if(room->spells[i].isUsed == false){
            if(room->spells[i].type == 's'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "✦");
            }
            else if(room->spells[i].type == 'p'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "◆");
            }
            else if(room->spells[i].type == 'h'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "✧");
            }
        }
    }
    for(int i = 0; i < room->specialfoodCount; i++){
        if(room->specialfoods[i].isUsed == false){
            if(room->specialfoods[i].type == 'n' || room->specialfoods[i].type == 'r'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "♣");
            }
            else if(room->specialfoods[i].type == 'p'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "♧");
            }
            else if(room->specialfoods[i].type == 's'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "♠");
            }
        }
    }
    for(int i = 0; i < room->weaponCount; i++){
        if(room->weapons[i].isUsed == false && room->weapons[i].count > 0){
            if(room->weapons[i].type == 'm'){
                wattron(game_win, COLOR_PAIR(16));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002692');
                wattroff(game_win, COLOR_PAIR(16));
            }
            else if(room->weapons[i].type == 'd'){
                wattron(game_win, COLOR_PAIR(10));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U0001F5E1');
                wattroff(game_win, COLOR_PAIR(10));
            }
            else if(room->weapons[i].type == 'w'){
                wattron(game_win, COLOR_PAIR(17));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "⚚");
                wattroff(game_win, COLOR_PAIR(17));
                //  L'\U0001FA84'
            }
            else if(room->weapons[i].type == 'a'){
                wattron(game_win, COLOR_PAIR(18));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U000027B3');
                wattroff(game_win, COLOR_PAIR(18));
            }
            else if(room->weapons[i].type == 's'){
                wattron(game_win, COLOR_PAIR(8));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002694');
                wattroff(game_win, COLOR_PAIR(8));
            }
        }
    }
    for(int i = 0; i < room->trapCount; i++){
        if(room->traps[i].isVisible == true || t_check){
            wattron(game_win, COLOR_PAIR(6));
            mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "^");
            wattroff(game_win, COLOR_PAIR(6));
        }
    }
    for(int i = 0; i < room->doorCount; i++){
        if(room->isVisible){
            game->whole_map[game->currentLevel][room->doors[i].cord.y][room->doors[i].cord.x].isVisible = true;
            if(room->type == 'p'){
                wattron(game_win, COLOR_PAIR(17));
            }
            else if(room->type == 's'){
                wattron(game_win, COLOR_PAIR(15));
            }
            else if(room->type == 'r'){
                wattron(game_win, COLOR_PAIR(25));
            }
        }
        if(room->doors[i].type == 'n'){
            // mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "+");
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "┼");
        }
        else if(room->doors[i].type == 'l' || room->doors[i].type == 'h'){
            if(room->doors[i].is_open == false){
                wattron(game_win, COLOR_PAIR(6));
            }
            else{
                wattron(game_win, COLOR_PAIR(7));
            }
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "☯");
            if(room->doors[i].is_open == false){
                wattroff(game_win, COLOR_PAIR(6));
            }
            else{
                wattroff(game_win, COLOR_PAIR(7));
            }
            wattron(game_win, COLOR_PAIR(color_pair_num));
            mvwprintw(game_win, room->password_generator.y, room->password_generator.x, "&");
            wattroff(game_win, COLOR_PAIR(color_pair_num));
            if(room->doors[i].type == 'h'){
                wattron(game_win, COLOR_PAIR(color_pair_num));
                mvwprintw(game_win, room->password_generator2.y, room->password_generator2.x, "&");
                wattroff(game_win, COLOR_PAIR(color_pair_num));
            }
        }
        else if((room->doors[i].type == 's' && room->doors[i].password == 1) || t_check){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "‽");
        }
        wattroff(game_win, COLOR_PAIR(25));
        wattroff(game_win, COLOR_PAIR(15));
        wattroff(game_win, COLOR_PAIR(17));
    }
    for(int i = 0; i < room->enemyCount; i++){
        if(room->enemies[i].isVisible == true){
            wattron(game_win, COLOR_PAIR(14));
            if((room->type == 't' && room->isVisible == true) || room->type == 'b'){
                wattron(game_win, COLOR_PAIR(6));
            }
            mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
            if((room->type == 't' && room->isVisible == true) || room->type == 'b'){
                wattroff(game_win, COLOR_PAIR(6));
            }
            wattroff(game_win, COLOR_PAIR(14));
        }
    }
    if(room->window.x != 0 && room->window.x != 0){
        if(room->type == 'p'){
            wattron(game_win, COLOR_PAIR(17));
        }
        else if(room->type == 's'){
            wattron(game_win, COLOR_PAIR(15));
        }
        else if(room->type == 'r'){
            wattron(game_win, COLOR_PAIR(25));
        }
        if(room->window.x == room->x){
            mvwprintw(game_win, room->window.y, room->window.x, "┤");
        }
        else if(room->window.y == room->y){
            mvwprintw(game_win, room->window.y, room->window.x, "┴");
        }
        else if(room->window.x == room->x + room->width - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "├");
        }
        else if(room->window.y == room->y + room->height - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "┬");
        }
    }
    if(room_number == 0 && game->currentLevel != 0){
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "≡ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    }
    else if(room->stairCount == 1){
        if(first_stair == 0 && room->isVisible){
            first_stair = 1;
        }
        // if(room->type == 'p'){
        //     wattron(game_win, COLOR_PAIR(17) | A_REVERSE);
        // }
        // else if(room->type == 'r'){
        //     wattron(game_win, COLOR_PAIR(25) | A_REVERSE);
        // }
        // else{
            wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        // }
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "≡ ");
        // if(room->type == 'p'){
        //     wattroff(game_win, COLOR_PAIR(17) | A_REVERSE);
        // }
        // else if(room->type == 'r'){
        //     wattroff(game_win, COLOR_PAIR(25) | A_REVERSE);
        // }
        // else{
            wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        // }
    }
    if(room->type == 's' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(15));
    }
    else if((room->type == 't' && room->isVisible == true) || room->type == 'b'){
        wattroff(game_win, COLOR_PAIR(22));
        in_emoji_check();
        if(in_emoji == 0){
            wattron(game_win, COLOR_PAIR(color_pair_num));
            draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
            wattroff(game_win, COLOR_PAIR(color_pair_num));
        }
    }
    else if(room->type == 'p' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(17));
    }
    else if(room->type == 'm' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(6));
    }
    if(room->keyCount == 1){
    
        wattron(game_win, COLOR_PAIR(12));
        mvwprintw(game_win, room->ancientkey.y, room->ancientkey.x, "Δ");
        wattroff(game_win, COLOR_PAIR(12));
    }
    // wattroff(game_win, COLOR_PAIR(color_pair_num));
}
// Change name above and below to change the showing way.
void draw_room(WINDOW *game_win, Room *room, int room_number) {
    if(ui_old_visuals){legacy_draw_room(game_win, room, room_number);return;}
    // wattron(game_win, COLOR_PAIR(color_pair_num));
    if(!m_check){
        wattroff(game_win, COLOR_PAIR(3));
    }
    if(room->type == 's' && room->isVisible == true && room->doorCount == 1){
        wattron(game_win, COLOR_PAIR(15));
    }
    else if(room->type == 't' && room->isVisible == true){
        draw_non_flashlight_room(game_win, room, room_number);
        return;
    }
    else if(room->type == 'b' && room->isVisible == true){
        draw_non_flashlight_room(game_win, room, room_number);
        return;
    }
    else if(room->type == 'p' && room->isVisible == true){
        draw_non_flashlight_room(game_win, room, room_number);
        return;
    }
    else if(room->type == 'r' && room->isVisible == true){
        if(room_number == game->player->room_num){
            in_reinforce = 1;
        }
        wattron(game_win, COLOR_PAIR(25));
    }
    else if(room->type == 's' && room->isVisible == true){
        if(room_number == game->player->room_num){
            in_healing = 1;
        }
        wattron(game_win, COLOR_PAIR(15));
    }
    if(room->type == 'm' && room->isVisible == true && all_print == 0){
        in_nightmare = 1;
        return;
    }
    if(room->type == 'm' && room->isVisible == true){
        wattron(game_win, COLOR_PAIR(6));
    }
    all_print = 1;
    in_nightmare = 0;
    room->lock_attemps = 0;
    if(m_check){
        draw_non_flashlight_room(game_win, room, room_number);
        return;
    }
    else if(room->type == 'm'){
        return;
    }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#' && room->isVisible == true){
        for (int y = room->y; y < room->y + room->height; y++) {
            for (int x = room->x; x < room->x + room->width; x++) {
                if(y == room->y && x == room->x){
                    mvwprintw(game_win, y, x, "┌");
                }
                else if(y == room->y + room->height - 1 && x == room->x){
                    mvwprintw(game_win, y, x, "└");
                }
                else if(y == room->y && x == room->x + room->width - 1){
                    mvwprintw(game_win, y, x, "┐");
                }
                else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
                    mvwprintw(game_win, y, x, "┘");
                }
                else if (y == room->y) {
                    mvwprintw(game_win, y, x, "─");
                } else if (y == room->y + room->height - 1) {
                    mvwprintw(game_win, y, x, "─");
                } else if (x == room->x || x == room->x + room->width - 1) {
                    mvwprintw(game_win, y, x, "│");
                }
                else{
                    mvwprintw(game_win, y, x, " ");
                }
            }
        }
        for(int i = 0; i < room->doorCount; i++){
            if(room->isVisible){
                game->whole_map[game->currentLevel][room->doors[i].cord.y][room->doors[i].cord.x].isVisible = true;
                if(room->type == 'p'){
                    wattron(game_win, COLOR_PAIR(17));
                }
                else if(room->type == 's'){
                    wattron(game_win, COLOR_PAIR(15));
                }
                else if(room->type == 'r'){
                    wattron(game_win, COLOR_PAIR(25));
                }
            }
            if(room->doors[i].type == 'n'){
                // mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "+");
                mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "┼");
            }
            else if(room->doors[i].type == 'l' || room->doors[i].type == 'h'){
                if(room->doors[i].is_open == false){
                    wattron(game_win, COLOR_PAIR(6));
                }
                else{
                    wattron(game_win, COLOR_PAIR(7));
                }
                mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "☯");
                if(room->doors[i].is_open == false){
                    wattroff(game_win, COLOR_PAIR(6));
                }
                else{
                    wattroff(game_win, COLOR_PAIR(7));
                }
                wattron(game_win, COLOR_PAIR(color_pair_num));
                mvwprintw(game_win, room->password_generator.y, room->password_generator.x, "&");
                wattroff(game_win, COLOR_PAIR(color_pair_num));
                if(room->doors[i].type == 'h'){
                    wattron(game_win, COLOR_PAIR(color_pair_num));
                    mvwprintw(game_win, room->password_generator2.y, room->password_generator2.x, "&");
                    wattroff(game_win, COLOR_PAIR(color_pair_num));
                }
            }
            else if((room->doors[i].type == 's' && room->doors[i].password == 1) || t_check){
                mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "‽");
            }
            wattroff(game_win, COLOR_PAIR(25));
            wattroff(game_win, COLOR_PAIR(15));
            wattroff(game_win, COLOR_PAIR(17));
        }
        if(room->window.x != 0 && room->window.x != 0){
            if(room->type == 'p'){
                wattron(game_win, COLOR_PAIR(17));
            }
            else if(room->type == 's'){
                wattron(game_win, COLOR_PAIR(15));
            }
            else if(room->type == 'r'){
                wattron(game_win, COLOR_PAIR(25));
            }
            if(room->window.x == room->x){
                mvwprintw(game_win, room->window.y, room->window.x, "┤");
            }
            else if(room->window.y == room->y){
                mvwprintw(game_win, room->window.y, room->window.x, "┴");
            }
            else if(room->window.x == room->x + room->width - 1){
                mvwprintw(game_win, room->window.y, room->window.x, "├");
            }
            else if(room->window.y == room->y + room->height - 1){
                mvwprintw(game_win, room->window.y, room->window.x, "┬");
            }
        }
        return;
    }
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            int dx1 = 0;
            int dy1 = 0;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker && room_number == game->player->room_num){
                if(y == room->y && x == room->x){
                    mvwprintw(game_win, y, x, "┌");
                }
                else if(y == room->y + room->height - 1 && x == room->x){
                    mvwprintw(game_win, y, x, "└");
                }
                else if(y == room->y && x == room->x + room->width - 1){
                    mvwprintw(game_win, y, x, "┐");
                }
                else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
                    mvwprintw(game_win, y, x, "┘");
                }
                else if (y == room->y) {
                    mvwprintw(game_win, y, x, "─");
                } else if (y == room->y + room->height - 1) {
                    mvwprintw(game_win, y, x, "─");
                } else if (x == room->x || x == room->x + room->width - 1) {
                    mvwprintw(game_win, y, x, "│");
                } else {
                    mvwprintw(game_win, y, x, ".");
                }
            }
            else{
                if(y == room->y && x == room->x){
                    mvwprintw(game_win, y, x, "┌");
                }
                else if(y == room->y + room->height - 1 && x == room->x){
                    mvwprintw(game_win, y, x, "└");
                }
                else if(y == room->y && x == room->x + room->width - 1){
                    mvwprintw(game_win, y, x, "┐");
                }
                else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
                    mvwprintw(game_win, y, x, "┘");
                }
                else if (y == room->y) {
                    mvwprintw(game_win, y, x, "─");
                } else if (y == room->y + room->height - 1) {
                    mvwprintw(game_win, y, x, "─");
                } else if (x == room->x || x == room->x + room->width - 1) {
                    mvwprintw(game_win, y, x, "│");
                }
                else{
                    wattron(game_win, COLOR_PAIR(20));
                    if(room_number == game->player->room_num){
                        mvwprintw(game_win, y, x, ".");
                    }
                    else{
                        mvwprintw(game_win, y, x, " ");
                    }
                    wattroff(game_win, COLOR_PAIR(20));
                    if(room->type == 'r' && room->isVisible == true){
                        wattron(game_win, COLOR_PAIR(25));
                    }
                    else if(room->type == 's' && room->isVisible == true){
                        wattron(game_win, COLOR_PAIR(15));
                    }
                    else if(room->type == 'm' && room->isVisible == true){
                        wattron(game_win, COLOR_PAIR(6));
                    }
                }
            }
        }
    }
    if(room->window.x != 0 && room->window.x != 0){
        if(room->type == 'p'){
            wattron(game_win, COLOR_PAIR(17));
        }
        else if(room->type == 's'){
            wattron(game_win, COLOR_PAIR(15));
        }
        else if(room->type == 'r'){
            wattron(game_win, COLOR_PAIR(25));
        }
        if(room->window.x == room->x){
            mvwprintw(game_win, room->window.y, room->window.x, "┤");
        }
        else if(room->window.y == room->y){
            mvwprintw(game_win, room->window.y, room->window.x, "┴");
        }
        else if(room->window.x == room->x + room->width - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "├");
        }
        else if(room->window.y == room->y + room->height - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "┬");
        }
    }
    for(int i = 0; i < room->doorCount; i++){
        if(room->isVisible){
            game->whole_map[game->currentLevel][room->doors[i].cord.y][room->doors[i].cord.x].isVisible = true;
            if(room->type == 'p'){
                wattron(game_win, COLOR_PAIR(17));
            }
            else if(room->type == 's'){
                wattron(game_win, COLOR_PAIR(15));
            }
            else if(room->type == 'r'){
                wattron(game_win, COLOR_PAIR(25));
            }
        }
        if(room->doors[i].type == 'n'){
            // mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "+");
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "┼");
        }
        else if(room->doors[i].type == 'l' || room->doors[i].type == 'h'){
            if(room->doors[i].is_open == false){
                wattron(game_win, COLOR_PAIR(6));
            }
            else{
                wattron(game_win, COLOR_PAIR(7));
            }
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "☯");
            if(room->doors[i].is_open == false){
                wattroff(game_win, COLOR_PAIR(6));
            }
            else{
                wattroff(game_win, COLOR_PAIR(7));
            }
            wattron(game_win, COLOR_PAIR(color_pair_num));
            if(game->player->room_num == room_number){
                mvwprintw(game_win, room->password_generator.y, room->password_generator.x, "&");
            }
            wattroff(game_win, COLOR_PAIR(color_pair_num));
            if(room->doors[i].type == 'h'){
                wattron(game_win, COLOR_PAIR(color_pair_num));
                if(game->player->room_num == room_number){
                    mvwprintw(game_win, room->password_generator2.y, room->password_generator2.x, "&");
                }
                wattroff(game_win, COLOR_PAIR(color_pair_num));
            }
        }
        else if((room->doors[i].type == 's' && room->doors[i].password == 1) || t_check){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "‽");
        }
        wattroff(game_win, COLOR_PAIR(25));
        wattroff(game_win, COLOR_PAIR(15));
        wattroff(game_win, COLOR_PAIR(17));
    }
    for(int i = 0; i < room->pillarCount; i++){
        wattron(game_win, COLOR_PAIR(16));
        if(room->type == 'b'){
            wattron(game_win, COLOR_PAIR(20));
        }
        int dx1 = 0, dy1 = 0;
        int x = room->pillars[i].x, y = room->pillars[i].y;
        if(x > game->player->cord.x){
            dx1 = 1;
        }
        else if(x < game->player->cord.y){
            dx1 = -1;
        }
        if(y > game->player->cord.y){
            dy1 = 1;
        }
        else if(y < game->player->cord.y){
            dy1 = -1;
        }
        int valid_checker = 0;
        if(side && (dx != 0 || dy != 0)){
            if(side == 1){
                if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                    valid_checker = 1;
                }
            }
            else if(side == 3){
                if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                    valid_checker = 1;
                }
            }
            else if(side == 2){
                if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                    valid_checker = 1;
                }
            }
            else if(side == 4){
                if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                    valid_checker = 1;
                }
            }
        }
        else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
            if(dx == 0 && dy == 0){
                valid_checker = 0;
            }
            else{
                valid_checker = 1;
            }
        }
        if(game->player->room_num == room_number){
            mvwprintw(game_win, room->pillars[i].y, room->pillars[i].x, "▒");
        }
        if(room->type == 'b'){
            wattroff(game_win, COLOR_PAIR(20));
        }
        wattroff(game_win, COLOR_PAIR(16));
    }
    if(room_number != game->player->room_num){
        if(room->stairCount == 1){
            if(first_stair == 0 && room->isVisible){
                first_stair = 1;
            }
            wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
            mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "≡ ");
            wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        }
        return;
    }
    for(int i = 0; i < room->foodCount; i++){
        if(room->foods[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(7));
            int dx1 = 0, dy1 = 0;
            int x = room->foods[i].cord.x, y = room->foods[i].cord.y;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker){
                mvwprintw(game_win, room->foods[i].cord.y, room->foods[i].cord.x, "♣");
            }
            wattroff(game_win, COLOR_PAIR(7));
        }
    }
    for(int i = 0; i < room->shieldCount; i++){
        if(room->shields[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(19));
            int dx1 = 0, dy1 = 0;
            int x = room->shields[i].cord.x, y = room->shields[i].cord.y;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker){
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x + 1, " ");
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x, "◇");
            }
            wattroff(game_win, COLOR_PAIR(19));
        }
    }
    for(int i = 0; i < room->goldCount; i++){
        if(room->golds[i].isUsed == false){
            int dx1 = 0, dy1 = 0;
            int x = room->golds[i].cord.x, y = room->golds[i].cord.y;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker){
                if(room->golds[i].type == 'n'){
                    wattron(game_win, COLOR_PAIR(12));
                    mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "¤");
                    wattroff(game_win, COLOR_PAIR(12));
                }
                else if(room->golds[i].type == 'b'){
                    mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "●");
                }
            }
        }
    }
    for(int i = 0; i < room->spellCount; i++){
        if(room->spells[i].isUsed == false){
            int dx1 = 0, dy1 = 0;
            int x = room->spells[i].cord.x, y = room->spells[i].cord.y;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker){
                if(room->spells[i].type == 's'){
                    mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "✦");
                }
                else if(room->spells[i].type == 'p'){
                    mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "◆");
                }
                else if(room->spells[i].type == 'h'){
                    mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "✧");
                }
            }
        }
    }
    for(int i = 0; i < room->specialfoodCount; i++){
        if(room->specialfoods[i].isUsed == false){
            int dx1 = 0, dy1 = 0;
            int x = room->specialfoods[i].cord.x, y = room->specialfoods[i].cord.y;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker){
                if(room->specialfoods[i].type == 'n' || room->specialfoods[i].type == 'r'){
                    mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "♣");
                }
                else if(room->specialfoods[i].type == 'p'){
                    mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "♧");
                }
                else if(room->specialfoods[i].type == 's'){
                    mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "♠");
                }
            }
        }
    }
    for(int i = 0; i < room->weaponCount; i++){
        if(room->weapons[i].isUsed == false && room->weapons[i].count > 0){
            int dx1 = 0, dy1 = 0;
            int x = room->weapons[i].cord.x, y = room->weapons[i].cord.y;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker){
                if(room->weapons[i].type == 'm'){
                    wattron(game_win, COLOR_PAIR(16));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002692');
                    wattroff(game_win, COLOR_PAIR(16));
                }
                else if(room->weapons[i].type == 'd'){
                    wattron(game_win, COLOR_PAIR(10));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U0001F5E1');
                    wattroff(game_win, COLOR_PAIR(10));
                }
                else if(room->weapons[i].type == 'w'){
                    wattron(game_win, COLOR_PAIR(17));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "⚚");
                    wattroff(game_win, COLOR_PAIR(17));
                    //  L'\U0001FA84'
                }
                else if(room->weapons[i].type == 'a'){
                    wattron(game_win, COLOR_PAIR(18));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U000027B3');
                    wattroff(game_win, COLOR_PAIR(18));
                }
                else if(room->weapons[i].type == 's'){
                    wattron(game_win, COLOR_PAIR(8));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002694');
                    wattroff(game_win, COLOR_PAIR(8));
                }
            }
        }
    }
    for(int i = 0; i < room->trapCount; i++){
        if(room->traps[i].isVisible == true || t_check){
            int dx1 = 0, dy1 = 0;
            int x = room->traps[i].cord.x, y = room->traps[i].cord.y;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker){
                wattron(game_win, COLOR_PAIR(6));
                mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "^");
                wattroff(game_win, COLOR_PAIR(6));
            }
        }
    }
    for(int i = 0; i < room->enemyCount; i++){
        if(room->enemies[i].isVisible == true){
            int dx1 = 0, dy1 = 0;
            int x = room->enemies[i].cord.x, y = room->enemies[i].cord.y;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker){
                wattron(game_win, COLOR_PAIR(14));
                if((room->type == 't' && room->isVisible == true) || room->type == 'b'){
                    wattron(game_win, COLOR_PAIR(6));
                }
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
                if((room->type == 't' && room->isVisible == true) || room->type == 'b'){
                    wattroff(game_win, COLOR_PAIR(6));
                }
                wattroff(game_win, COLOR_PAIR(14));
            }
        }
    }
    if(room_number == 0 && game->currentLevel != 0){
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "≡ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    }
    else if(room->stairCount == 1){
        if(first_stair == 0 && room->isVisible){
            first_stair = 1;
        }
        // if(room->type == 'p'){
        //     wattron(game_win, COLOR_PAIR(17) | A_REVERSE);
        // }
        // else if(room->type == 'r'){
        //     wattron(game_win, COLOR_PAIR(25) | A_REVERSE);
        // }
        // else{
            wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        // }
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "≡ ");
        // if(room->type == 'p'){
        //     wattroff(game_win, COLOR_PAIR(17) | A_REVERSE);
        // }
        // else if(room->type == 'r'){
        //     wattroff(game_win, COLOR_PAIR(25) | A_REVERSE);
        // }
        // else{
            wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        // }
    }
    if(room->type == 's' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(15));
    }
    else if((room->type == 't' && room->isVisible == true) || room->type == 'b'){
        wattroff(game_win, COLOR_PAIR(22));
        in_emoji_check();
        if(in_emoji == 0){
            wattron(game_win, COLOR_PAIR(color_pair_num));
            draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
            wattroff(game_win, COLOR_PAIR(color_pair_num));
        }
    }
    else if(room->type == 'p' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(17));
    }
    else if(room->type == 'm' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(6));
    }
    if(room->keyCount == 1){
        int dx1 = 0, dy1 = 0;
        int x = room->ancientkey.x, y = room->ancientkey.y;
        if(x > game->player->cord.x){
            dx1 = 1;
        }
        else if(x < game->player->cord.y){
            dx1 = -1;
        }
        if(y > game->player->cord.y){
            dy1 = 1;
        }
        else if(y < game->player->cord.y){
            dy1 = -1;
        }
        int valid_checker = 0;
        if(side && (dx != 0 || dy != 0)){
            if(side == 1){
                if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                    valid_checker = 1;
                }
            }
            else if(side == 3){
                if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                    valid_checker = 1;
                }
            }
            else if(side == 2){
                if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                    valid_checker = 1;
                }
            }
            else if(side == 4){
                if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                    valid_checker = 1;
                }
            }
        }
        else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
            if(dx == 0 && dy == 0){
                valid_checker = 0;
            }
            else{
                valid_checker = 1;
            }
        }
        if(valid_checker){
            wattron(game_win, COLOR_PAIR(12));
            mvwprintw(game_win, room->ancientkey.y, room->ancientkey.x, "Δ");
            wattroff(game_win, COLOR_PAIR(12));
        }
    }
    // wattroff(game_win, COLOR_PAIR(color_pair_num));
}
void draw_last_room(WINDOW *game_win, Room *room, int room_number) {
    if(ui_old_visuals){legacy_draw_last_room(game_win, room, room_number);boss_encounter_draw(game_win,room);return;}
    // wattron(game_win, COLOR_PAIR(color_pair_num));
    wattroff(game_win, COLOR_PAIR(3));
    wattron(game_win, COLOR_PAIR(14));
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            if(y == room->y && x == room->x){
                // mvwprintw(game_win, y, x, "╭");
                mvwprintw(game_win, y, x, "┌");
            }
            else if(y == room->y + room->height - 1 && x == room->x){
                // mvwprintw(game_win, y, x, "╰");
                mvwprintw(game_win, y, x, "└");
            }
            else if(y == room->y && x == room->x + room->width - 1){
                // mvwprintw(game_win, y, x, "╮");
                mvwprintw(game_win, y, x, "┐");
            }
            else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
                // mvwprintw(game_win, y, x, "╯");
                mvwprintw(game_win, y, x, "┘");
            }
            else if (y == room->y) {
                mvwprintw(game_win, y, x, "─");
            } else if (y == room->y + room->height - 1) {
                mvwprintw(game_win, y, x, "─");
            } else if (x == room->x || x == room->x + room->width - 1) {
                mvwprintw(game_win, y, x, "│");
            } else {
                game->whole_map[game->currentLevel][y][x].item = '.';
                mvwprintw(game_win, y, x, ".");
            }
        }
    }
    for(int i = 0; i < room->trapCount; i++){
        wattron(game_win, COLOR_PAIR(6));
        if(room->traps[i].isVisible){
            mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "^");
        }
        else{
            mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "⟂");
        }
        wattroff(game_win, COLOR_PAIR(6));
        game->whole_map[game->currentLevel][room->traps[i].cord.y][room->traps[i].cord.x].item = '^';
    }
    for(int i = 0; i < room->doorCount; i++){
        if(room->doors[i].type == 'n'){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "+");
        }
        else if(room->doors[i].type == 'l' || room->doors[i].type == 'h'){
            if(room->doors[i].is_open == false){
                wattron(game_win, COLOR_PAIR(6));
            }
            else{
                wattron(game_win, COLOR_PAIR(7));
            }
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "☯");
            if(room->doors[i].is_open == false){
                wattroff(game_win, COLOR_PAIR(6));
            }
            else{
                wattroff(game_win, COLOR_PAIR(7));
            }
            wattron(game_win, COLOR_PAIR(color_pair_num));
            mvwprintw(game_win, room->password_generator.y, room->password_generator.x, "&");
            wattroff(game_win, COLOR_PAIR(color_pair_num));
        }
        else if((room->doors[i].type == 's' && room->doors[i].password == 1) || t_check){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "‽");
        }
    }
    for(int i = 0; i < room->enemyCount; i++){
        // if(room->enemies[i].type == 'I'){
        //     game->whole_map[game->currentLevel][room->enemies[i].cord.y][room->enemies[i].cord.x].item = 'I';
        //     wattron(game_win, COLOR_PAIR(14));
        //     if(room->type == 't' && room->isVisible == true){
        //         wattron(game_win, COLOR_PAIR(6));
        //     }
        //     mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
        //     if(room->type == 't' && room->isVisible == true){
        //         wattroff(game_win, COLOR_PAIR(6));
        //     }
        //     wattroff(game_win, COLOR_PAIR(14));
        // }
        if(room->enemies[i].isAlive == true){
            wattron(game_win, COLOR_PAIR(17));
            game->whole_map[game->currentLevel][room->enemies[i].cord.y][room->enemies[i].cord.x].item = 'I';
            mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
            if(room->enemies[i].health > 150){
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x, "─");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x, "─");
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x + 1, "│");
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x - 1, "│");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x + 1, "╯");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x - 1, "╰");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x + 1, "╮");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x - 1, "╭");
            }
            else if(room->enemies[i].health > 100){
                wattron(game_win, COLOR_PAIR(14));
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x, "─");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x, "─");
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x + 1, "│");
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x - 1, "│");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x + 1, "╯");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x - 1, "╰");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x + 1, "╮");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x - 1, "╭");
            }
            else if(room->enemies[i].health > 50){
                wattron(game_win, COLOR_PAIR(6));
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x, "─");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x, "─");
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x + 1, "│");
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x - 1, "│");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x + 1, "╯");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x - 1, "╰");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x + 1, "╮");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x - 1, "╭");
            }
            else if(room->enemies[i].health <= 50){
                last_one = 1;
                wattron(game_win, COLOR_PAIR(21));
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x, "─");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x, "─");
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x + 1, "│");
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x - 1, "│");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x + 1, "╯");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x - 1, "╰");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x + 1, "╮");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x - 1, "╭");
            }
            wattroff(game_win, COLOR_PAIR(17));
            wattroff(game_win, COLOR_PAIR(14));
            wattroff(game_win, COLOR_PAIR(21));
            wattroff(game_win, COLOR_PAIR(6));
        }
        else{
            room->enemies[i].canMove = true;
            wattron(game_win, COLOR_PAIR(20));
            mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
            wattroff(game_win, COLOR_PAIR(20));
        }
    }
    if(room->window.x != 0 && room->window.x != 0){
        mvwprintw(game_win, room->window.y, room->window.x, "=");
    }
    if(room_number == 0 && game->currentLevel != 0){
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "<");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    }
    else if(room->stairCount == 1){
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, ">");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    }
    else if(room->type == 't' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(12));
    }
    if(room->keyCount == 1){
        wattron(game_win, COLOR_PAIR(12));
        game->whole_map[game->currentLevel][room->ancientkey.y][room->ancientkey.x].item = 'A';
        mvwprintw(game_win, room->ancientkey.y, room->ancientkey.x, "Δ");
        wattroff(game_win, COLOR_PAIR(12));
    }
    // for (int y = room->y; y < room->y + room->height; y++) {
    //     for (int x = room->x; x < room->x + room->width; x++) {
    //         mvwprintw(game_win, y, x, "%c", game->whole_map[game->currentLevel][y][x].item);
    //     }
    // }
    wattron(game_win, COLOR_PAIR(color_pair_num));
    draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
    wattroff(game_win, COLOR_PAIR(color_pair_num));
    // wattroff(game_win, COLOR_PAIR(color_pair_num));
    boss_encounter_draw(game_win,room);
}
void draw_nightmare_room(WINDOW *game_win, Room *room, int room_number) {
    if(ui_old_visuals){legacy_draw_nightmare_room(game_win, room, room_number);return;}
    if(room->type == 'm' && room->isVisible == true){
        wattron(game_win, COLOR_PAIR(6));
        in_nightmare = 1;
    }
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            if(y == room->y && x == room->x){
                mvwprintw(game_win, y, x, "┌");
            }
            else if(y == room->y + room->height - 1 && x == room->x){
                mvwprintw(game_win, y, x, "└");
            }
            else if(y == room->y && x == room->x + room->width - 1){
                mvwprintw(game_win, y, x, "┐");
            }
            else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
                mvwprintw(game_win, y, x, "┘");
            }
            else if (y == room->y) {
                mvwprintw(game_win, y, x, "─");
            } else if (y == room->y + room->height - 1) {
                mvwprintw(game_win, y, x, "─");
            } else if (x == room->x || x == room->x + room->width - 1) {
                mvwprintw(game_win, y, x, "│");
            } else {
                if((x - game->player->cord.x <= 2 && game->player->cord.x - x <= 2) && (y - game->player->cord.y <= 2 && game->player->cord.y - y <= 2)){
                    mvwprintw(game_win, y, x, ".");
                }
                else{
                    mvwprintw(game_win, y, x, " ");
                }
            }
        }
    }
    for(int i = 0; i < room->pillarCount; i++){
        wattron(game_win, COLOR_PAIR(16));
        if((room->pillars[i].x - game->player->cord.x <= 2 && game->player->cord.x - room->pillars[i].x <= 2)
        && (room->pillars[i].y - game->player->cord.y <= 2 && game->player->cord.y - room->pillars[i].y <= 2)){
            mvwprintw(game_win, room->pillars[i].y, room->pillars[i].x, "▒");
        }
        else{
            mvwprintw(game_win, room->pillars[i].y, room->pillars[i].x, " ");
        }
        wattroff(game_win, COLOR_PAIR(16));
    }
    for(int i = 0; i < room->foodCount; i++){
        if(room->foods[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(7));
            if((room->foods[i].cord.x - game->player->cord.x <= 2 && game->player->cord.x - room->foods[i].cord.x<= 2)
            && (room->foods[i].cord.y - game->player->cord.y <= 2 && game->player->cord.y - room->foods[i].cord.y <= 2)){
                mvwprintw(game_win, room->foods[i].cord.y, room->foods[i].cord.x, "♣");
            }
            else{
                mvwprintw(game_win, room->foods[i].cord.y, room->foods[i].cord.x, " ");
            }
            wattroff(game_win, COLOR_PAIR(7));
        }
    }
    for(int i = 0; i < room->shieldCount; i++){
        if(room->shields[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(19));
            if((room->shields[i].cord.x - game->player->cord.x <= 2 && game->player->cord.x - room->shields[i].cord.x <= 2)
            && (room->shields[i].cord.y - game->player->cord.y <= 2 && game->player->cord.y - room->shields[i].cord.y <= 2)){
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x, "◇");
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x + 1, " ");
            }
            else{
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x, " ");
            }
            wattroff(game_win, COLOR_PAIR(19));
        }
    }
    for(int i = 0; i < room->goldCount; i++){
        if(room->golds[i].isUsed == false){
            if((room->golds[i].cord.x - game->player->cord.x <= 2 && game->player->cord.x - room->golds[i].cord.x <= 2)
            && (room->golds[i].cord.y - game->player->cord.y <= 2 && game->player->cord.y - room->golds[i].cord.y <= 2)){
                if(room->golds[i].type == 'n'){
                    wattron(game_win, COLOR_PAIR(12));
                    mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "¤");
                    wattroff(game_win, COLOR_PAIR(12));
                }
                else if(room->golds[i].type == 'b'){
                    mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "●");
                }
            }
            else{
                mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "  ");
            }
        }
    }
    for(int i = 0; i < room->spellCount; i++){
        if(room->spells[i].isUsed == false){
            if((room->spells[i].cord.x - game->player->cord.x <= 2 && game->player->cord.x - room->spells[i].cord.x <= 2)
            && (room->spells[i].cord.y - game->player->cord.y <= 2 && game->player->cord.y - room->spells[i].cord.y <= 2)){
                if(room->spells[i].type == 's'){
                    mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "✦");
                }
                else if(room->spells[i].type == 'p'){
                    mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "◆");
                }
                else if(room->spells[i].type == 'h'){
                    mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "✧");
                }
            }
            else{
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, " ");
            }
        }
    }
    for(int i = 0; i < room->specialfoodCount; i++){
        if(room->specialfoods[i].isUsed == false){
            if((room->specialfoods[i].cord.x - game->player->cord.x <= 2 && game->player->cord.x - room->specialfoods[i].cord.x <= 2)
            && (room->specialfoods[i].cord.y - game->player->cord.y <= 2 && game->player->cord.y - room->specialfoods[i].cord.y <= 2)){
                if(room->specialfoods[i].type == 'n' || room->specialfoods[i].type == 'r'){
                    mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "♣");
                }
                else if(room->specialfoods[i].type == 'p'){
                    mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "♧");
                }
                else if(room->specialfoods[i].type == 's'){
                    mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "♠");
                }
            }
            else{
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, " ");
            }
        }
    }
    for(int i = 0; i < room->weaponCount; i++){
        if(room->weapons[i].isUsed == false){
            if((room->weapons[i].cord.x - game->player->cord.x <= 2 && game->player->cord.x - room->weapons[i].cord.x <= 2)
            && (room->weapons[i].cord.y - game->player->cord.y <= 2 && game->player->cord.y - room->weapons[i].cord.y <= 2)){
                if(room->weapons[i].type == 'm'){
                    wattron(game_win, COLOR_PAIR(16));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002692');
                    wattroff(game_win, COLOR_PAIR(16));
                }
                else if(room->weapons[i].type == 'd'){
                    wattron(game_win, COLOR_PAIR(10));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U0001F5E1');
                    wattroff(game_win, COLOR_PAIR(10));
                }
                else if(room->weapons[i].type == 'w'){
                    wattron(game_win, COLOR_PAIR(17));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "⚚");
                    wattroff(game_win, COLOR_PAIR(17));
                    //  L'\U0001FA84'
                }
                else if(room->weapons[i].type == 'a'){
                    wattron(game_win, COLOR_PAIR(18));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U000027B3');
                    wattroff(game_win, COLOR_PAIR(18));
                }
                else if(room->weapons[i].type == 's'){
                    wattron(game_win, COLOR_PAIR(8));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002694');
                    wattroff(game_win, COLOR_PAIR(8));
                }
            }
            else{
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, " ");
            }
        }
    }
    for(int i = 0; i < room->trapCount; i++){
        if(room->traps[i].isVisible == true){
            wattron(game_win, COLOR_PAIR(6));
            if((room->traps[i].cord.x - game->player->cord.x <= 2 && game->player->cord.x - room->traps[i].cord.x <= 2)
            && (room->traps[i].cord.y - game->player->cord.y <= 2 && game->player->cord.y - room->traps[i].cord.y <= 2)){
                mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "^");
            }
            else{
                mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, " ");
            }
            wattroff(game_win, COLOR_PAIR(6));
        }
    }
    for(int i = 0; i < room->enemyCount; i++){
        if(room->enemies[i].isVisible == true){
            wattron(game_win, COLOR_PAIR(14));
            if((room->enemies[i].cord.x - game->player->cord.x <= 2 && game->player->cord.x - room->enemies[i].cord.x <= 2)
            && (room->enemies[i].cord.y - game->player->cord.y <= 2 && game->player->cord.y - room->enemies[i].cord.y <= 2)){
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
            }
            else{
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, " ");
            }
            wattroff(game_win, COLOR_PAIR(14));
        }
    }
    for(int i = 0; i < room->doorCount; i++){
        if(room->doors[i].type == 'n'){
            wattron(game_win, COLOR_PAIR(5));
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "╂");
            wattroff(game_win, COLOR_PAIR(5));
        }
        else if(room->doors[i].type == 's' && room->doors[i].password == 1){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "‽");
        }
        else if(room->doors[i].type == 'l' || room->doors[i].type == 'h'){
            if(room->doors[i].is_open == false){
                wattron(game_win, COLOR_PAIR(6));
            }
            else{
                wattron(game_win, COLOR_PAIR(7));
            }
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "☯");
            if(room->doors[i].is_open == false){
                wattroff(game_win, COLOR_PAIR(6));
            }
            else{
                wattroff(game_win, COLOR_PAIR(7));
            }
            wattron(game_win, COLOR_PAIR(color_pair_num));
            if((room->password_generator.x - game->player->cord.x <= 2 && game->player->cord.x - room->password_generator.x <= 2)
            && (room->password_generator.y - game->player->cord.y <= 2 && game->player->cord.y - room->password_generator.y <= 2)){
                mvwprintw(game_win, room->password_generator.y, room->password_generator.x, "&");
            }
            else{
                mvwprintw(game_win, room->password_generator.y, room->password_generator.x, " ");
            }
            if(room->doors[i].type == 'h'){
            if((room->password_generator2.x - game->player->cord.x <= 2 && game->player->cord.x - room->password_generator2.x <= 2)
                && (room->password_generator2.y - game->player->cord.y <= 2 && game->player->cord.y - room->password_generator2.y <= 2)){
                    mvwprintw(game_win, room->password_generator2.y, room->password_generator2.x, "&");
                }
                else{
                    mvwprintw(game_win, room->password_generator2.y, room->password_generator2.x, " ");
                }
            }
            wattroff(game_win, COLOR_PAIR(color_pair_num));
        }
    }
    if(room->window.x != 0 && room->window.x != 0){
        wattron(game_win, COLOR_PAIR(5));
        if(room->window.x == room->x){
            mvwprintw(game_win, room->window.y, room->window.x, "╡");
        }
        else if(room->window.y == room->y){
            mvwprintw(game_win, room->window.y, room->window.x, "╨");
        }
        else if(room->window.x == room->x + room->width - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "╞");
        }
        else if(room->window.y == room->y + room->height - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "╥");
        }
        wattroff(game_win, COLOR_PAIR(5));
    }
    if(room_number == 0 && game->currentLevel != 0){
        wattron(game_win, COLOR_PAIR(color_pair_num));
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "≡ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num));
    }
    else if(room->stairCount == 1){
        wattron(game_win, COLOR_PAIR(color_pair_num));
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "≡ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num));
    }
    if(room->type == 'm' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(6));
    }
    if(room->keyCount == 1){
        wattron(game_win, COLOR_PAIR(12));
        if((room->ancientkey.x - game->player->cord.x <= 2 && game->player->cord.x - room->ancientkey.x <= 2)
        && (room->ancientkey.y - game->player->cord.y <= 2 && game->player->cord.y - room->ancientkey.y <= 2)){
            mvwprintw(game_win, room->ancientkey.y, room->ancientkey.x, "Δ");
        }
        else{
            mvwprintw(game_win, room->ancientkey.y, room->ancientkey.x, " ");
        }
        wattroff(game_win, COLOR_PAIR(12));
    }
}
void draw_attacked_room(WINDOW *game_win, Room *room, int room_number) {
    if(ui_old_visuals){legacy_draw_attacked_room(game_win, room, room_number);return;}
    if(is_last_room){
        draw_last_room(game_win, room, room_number);
        return;
    }
    if(room->type == 'm' || room->type == 's' || room->type == 'p' || room->type == 'r' || room->type == 's'){
        return;
    }
    int temp = color_pair_num;
    if(color_pair_num == 0){
        color_pair_num = 14;
    }
    else if(room->type == 't' && room->isVisible == true){
        wattron(game_win, COLOR_PAIR(12));
    }
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            if(y == room->y && x == room->x){
                mvwprintw(game_win, y, x, "┌");
            }
            else if(y == room->y + room->height - 1 && x == room->x){
                mvwprintw(game_win, y, x, "└");
            }
            else if(y == room->y && x == room->x + room->width - 1){
                mvwprintw(game_win, y, x, "┐");
            }
            else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
                mvwprintw(game_win, y, x, "┘");
            }
            else if (y == room->y) {
                mvwprintw(game_win, y, x, "─");
            } else if (y == room->y + room->height - 1) {
                mvwprintw(game_win, y, x, "─");
            } else if (x == room->x || x == room->x + room->width - 1) {
                mvwprintw(game_win, y, x, "│");
            } else {
                if((x - game->player->cord.x <= 1 && game->player->cord.x - x <= 1) && (y - game->player->cord.y <= 1 && game->player->cord.y - y <= 1)){
                    wattron(game_win, COLOR_PAIR(color_pair_num));
                    mvwprintw(game_win, y, x, ".");
                    wattroff(game_win, COLOR_PAIR(color_pair_num));
                }
                else{
                    if(room->type == 't'){
                        wattron(game_win, COLOR_PAIR(12));
                    }
                    mvwprintw(game_win, y, x, ".");
                    if(room->type == 't'){
                        wattroff(game_win, COLOR_PAIR(12));
                    }
                }
            }
        }
    }
    for(int i = 0; i < room->pillarCount; i++){
        wattron(game_win, COLOR_PAIR(16));
        mvwprintw(game_win, room->pillars[i].y, room->pillars[i].x, "▒");
        wattroff(game_win, COLOR_PAIR(16));
    }
    for(int i = 0; i < room->foodCount; i++){
        if(room->foods[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(7));
            if((room->foods[i].cord.x - game->player->cord.x <= 1 && game->player->cord.x - room->foods[i].cord.x<= 1)
            && (room->foods[i].cord.y - game->player->cord.y <= 1 && game->player->cord.y - room->foods[i].cord.y <= 1)){
                // wattron(game_win, COLOR_PAIR(color_pair_num));
                wattron(game_win, COLOR_PAIR(7));
                mvwprintw(game_win, room->foods[i].cord.y, room->foods[i].cord.x, "♣");
                wattroff(game_win, COLOR_PAIR(7));
                // wattroff(game_win, COLOR_PAIR(color_pair_num));
            }
            else{
                mvwprintw(game_win, room->foods[i].cord.y, room->foods[i].cord.x, "♣");
            }
            wattroff(game_win, COLOR_PAIR(7));
        }
    }
    for(int i = 0; i < room->shieldCount; i++){
        if(room->shields[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(19));
            if((room->shields[i].cord.x - game->player->cord.x <= 1 && game->player->cord.x - room->shields[i].cord.x<= 1)
            && (room->shields[i].cord.y - game->player->cord.y <= 1 && game->player->cord.y - room->shields[i].cord.y <= 1)){
                // wattron(game_win, COLOR_PAIR(color_pair_num));
                wattron(game_win, COLOR_PAIR(19));
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x + 1, " ");
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x, "◇");
                wattroff(game_win, COLOR_PAIR(19));
                // wattroff(game_win, COLOR_PAIR(color_pair_num));
            }
            else{
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x + 1, " ");
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x, "◇");
            }
            wattroff(game_win, COLOR_PAIR(19));
            wattron(game_win, COLOR_PAIR(color_pair_num));
            // mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x + 1, ".");
            wattroff(game_win, COLOR_PAIR(color_pair_num));
        }
    }
    for(int i = 0; i < room->goldCount; i++){
        if(room->golds[i].isUsed == false){
            if((room->golds[i].cord.x - game->player->cord.x <= 1 && game->player->cord.x - room->golds[i].cord.x <= 1)
            && (room->golds[i].cord.y - game->player->cord.y <= 1 && game->player->cord.y - room->golds[i].cord.y <= 1)){
                if(room->golds[i].type == 'n'){
                    wattron(game_win, COLOR_PAIR(12));
                    // wattron(game_win, COLOR_PAIR(color_pair_num));
                    mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "¤");
                    // wattron(game_win, COLOR_PAIR(color_pair_num));
                    wattroff(game_win, COLOR_PAIR(12));
                }
                else if(room->golds[i].type == 'b'){
                    mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "●");
                }
            }
            else{
                if(room->golds[i].type == 'n'){
                    wattron(game_win, COLOR_PAIR(12));
                    mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "¤");
                    wattroff(game_win, COLOR_PAIR(12));
                }
                else if(room->golds[i].type == 'b'){
                    mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "●");
                }
            }
        }
    }
    for(int i = 0; i < room->spellCount; i++){
        if(room->spells[i].isUsed == false){
            if(room->spells[i].type == 's'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "✦");
            }
            else if(room->spells[i].type == 'p'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "◆");
            }
            else if(room->spells[i].type == 'h'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "✧");
            }
        }
    }
    for(int i = 0; i < room->specialfoodCount; i++){
        if(room->specialfoods[i].isUsed == false){
            if(room->specialfoods[i].type == 'n' || room->specialfoods[i].type == 'r'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "♣");
            }
            else if(room->specialfoods[i].type == 'p'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "♧");
            }
            else if(room->specialfoods[i].type == 's'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "♠");
            }
        }
    }
    for(int i = 0; i < room->weaponCount; i++){
        if(room->weapons[i].isUsed == false){
            if(room->weapons[i].type == 'm'){
                wattron(game_win, COLOR_PAIR(16));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002692');
                wattroff(game_win, COLOR_PAIR(16));
            }
            else if(room->weapons[i].type == 'd'){
                wattron(game_win, COLOR_PAIR(10));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U0001F5E1');
                wattroff(game_win, COLOR_PAIR(10));
            }
            else if(room->weapons[i].type == 'w'){
                wattron(game_win, COLOR_PAIR(17));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "⚚");
                wattroff(game_win, COLOR_PAIR(17));
                //  L'\U0001FA84'
            }
            else if(room->weapons[i].type == 'a'){
                wattron(game_win, COLOR_PAIR(18));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U000027B3');
                wattroff(game_win, COLOR_PAIR(18));
            }
            else if(room->weapons[i].type == 's'){
                wattron(game_win, COLOR_PAIR(8));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002694');
                wattroff(game_win, COLOR_PAIR(8));
            }
        }
    }
    for(int i = 0; i < room->trapCount; i++){
        if(game->player->currentWeapon.type == 's' && game->player->weapons[4].count >= 2
        &&(room->traps[i].cord.x - game->player->cord.x <= 1 && game->player->cord.x - room->traps[i].cord.x <= 1)
        && (room->traps[i].cord.y - game->player->cord.y <= 1 && game->player->cord.y - room->traps[i].cord.y <= 1)){
            wattron(game_win, COLOR_PAIR(color_pair_num));
            mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "^");
            wattroff(game_win, COLOR_PAIR(color_pair_num));
            room->traps[i].isVisible = true;
        }
        if(room->traps[i].isVisible == true){
            wattron(game_win, COLOR_PAIR(6));
            if((room->traps[i].cord.x - game->player->cord.x <= 1 && game->player->cord.x - room->traps[i].cord.x <= 1)
            && (room->traps[i].cord.y - game->player->cord.y <= 1 && game->player->cord.y - room->traps[i].cord.y <= 1)){
                mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "^");
            }
            else{
                mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "^");
            }
            wattroff(game_win, COLOR_PAIR(6));
        }
    }
    for(int i = 0; i < room->doorCount; i++){
        if(room->doors[i].type == 'n'){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "┼");
        }
        else if(room->doors[i].type == 's' && room->doors[i].password == 1){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "‽");
        }
        else if(room->doors[i].type == 'l' || room->doors[i].type == 'h'){
            if(room->doors[i].is_open == false){
                wattron(game_win, COLOR_PAIR(6));
            }
            else{
                wattron(game_win, COLOR_PAIR(7));
            }
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "☯");
            if(room->doors[i].is_open == false){
                wattroff(game_win, COLOR_PAIR(6));
            }
            else{
                wattroff(game_win, COLOR_PAIR(7));
            }
            wattron(game_win, COLOR_PAIR(temp));
            mvwprintw(game_win, room->password_generator.y, room->password_generator.x, "&");
            wattroff(game_win, COLOR_PAIR(temp));
            if(room->doors[i].type == 'h'){
                wattron(game_win, COLOR_PAIR(temp));
                mvwprintw(game_win, room->password_generator2.y, room->password_generator2.x, "&");
                wattroff(game_win, COLOR_PAIR(temp));
            }
        }
    }
    if(room->window.x != 0 && room->window.x != 0){
        if(room->window.x == room->x){
            mvwprintw(game_win, room->window.y, room->window.x, "┤");
        }
        else if(room->window.y == room->y){
            mvwprintw(game_win, room->window.y, room->window.x, "┴");
        }
        else if(room->window.x == room->x + room->width - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "├");
        }
        else if(room->window.y == room->y + room->height - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "┬");
        }
    }
    for(int i = 0; i < room->enemyCount; i++){
        if(room->enemies[i].isVisible == true){
            wattron(game_win, COLOR_PAIR(14));
            mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
            wattroff(game_win, COLOR_PAIR(14));
        }
    }
    if(room_number == 0 && game->currentLevel != 0){
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "≡ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    }
    else if(room->stairCount == 1){
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "≡ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    }
    else if(room->type == 'm' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(6));
    }
    if(room->keyCount == 1){
        wattron(game_win, COLOR_PAIR(12));
        mvwprintw(game_win, room->ancientkey.y, room->ancientkey.x, "Δ");
        wattroff(game_win, COLOR_PAIR(12));
    }
    wattroff(game_win, COLOR_PAIR(color_pair_num));
    color_pair_num = temp;
}
void draw_ranged_attacked_room(WINDOW *game_win, Room *room, int room_number, int dx, int dy) {
    if(ui_old_visuals){legacy_draw_ranged_attacked_room(game_win, room, room_number, dx, dy);return;}
    // wattron(game_win, COLOR_PAIR(color_pair_num));
    if(is_last_room){
        draw_last_room(game_win, room, room_number);
        return;
    }
    if(room->type == 'p'){
       draw_room(game_win, room, room_number);
       return;
    }
    if(room->type == 't'){
       draw_room(game_win, room, room_number);
       return;
    }
    if(room->type == 'r'){
       draw_room(game_win, room, room_number);
       return;
    }
    if(room->type == 's'){
       draw_room(game_win, room, room_number);
       return;
    }
    int temp = color_pair_num;
    if(color_pair_num == 0){
        color_pair_num = 14;
    }
    else if(room->type == 't' && room->isVisible == true){
        wattron(game_win, COLOR_PAIR(12));
    }
    if(room->type == 'm' && room->isVisible == true && all_print == 0){
        in_nightmare = 1;
        return;
    }
    if(room->type == 'm' && room->isVisible == true){
        wattron(game_win, COLOR_PAIR(6));
    }
    all_print = 1;
    in_nightmare = 0;
    room->lock_attemps = 0;
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            if(y == room->y && x == room->x){
                mvwprintw(game_win, y, x, "┌");
            }
            else if(y == room->y + room->height - 1 && x == room->x){
                mvwprintw(game_win, y, x, "└");
            }
            else if(y == room->y && x == room->x + room->width - 1){
                mvwprintw(game_win, y, x, "┐");
            }
            else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
                mvwprintw(game_win, y, x, "┘");
            }
            else if (y == room->y) {
                mvwprintw(game_win, y, x, "─");
            } else if (y == room->y + room->height - 1) {
                mvwprintw(game_win, y, x, "─");
            } else if (x == room->x || x == room->x + room->width - 1) {
                mvwprintw(game_win, y, x, "│");
            } else {
                if(dx == 1){
                    if(x > game->player->cord.x && y == game->player->cord.y && x - game->player->cord.x <= game->player->currentWeapon.range){
                        wattron(game_win, COLOR_PAIR(color_pair_num));
                    }
                }
                else if(dx == -1 && y == game->player->cord.y && game->player->cord.x - x <= game->player->currentWeapon.range){
                    if(x < game->player->cord.x){
                        wattron(game_win, COLOR_PAIR(color_pair_num));
                    }
                }
                else if(dy == 1 && x == game->player->cord.x && y - game->player->cord.y <= game->player->currentWeapon.range){
                    if(y > game->player->cord.y){
                        wattron(game_win, COLOR_PAIR(color_pair_num));
                    }
                }
                else if(dy == -1 && x == game->player->cord.x && game->player->cord.y - y <= game->player->currentWeapon.range){
                    if(y < game->player->cord.y){
                        wattron(game_win, COLOR_PAIR(color_pair_num));
                    }
                }
                mvwprintw(game_win, y, x, ".");
                if(dx == 1 && y == game->player->cord.y){
                    if(x > game->player->cord.x && x - game->player->cord.x <= game->player->currentWeapon.range){
                        wattroff(game_win, COLOR_PAIR(color_pair_num));
                    }
                }
                else if(dx == -1 && y == game->player->cord.y){
                    if(x < game->player->cord.x && game->player->cord.x - x <= game->player->currentWeapon.range){
                        wattroff(game_win, COLOR_PAIR(color_pair_num));
                    }
                }
                else if(dy == 1 && x == game->player->cord.x){
                    if(y > game->player->cord.y && y - game->player->cord.y <= game->player->currentWeapon.range){
                        wattroff(game_win, COLOR_PAIR(color_pair_num));
                    }
                }
                else if(dy == -1 && x == game->player->cord.x && game->player->cord.y - y <= game->player->currentWeapon.range){
                    if(y < game->player->cord.y){
                        wattroff(game_win, COLOR_PAIR(color_pair_num));
                    }
                }
            }
        }
    }
    for(int i = 0; i < room->pillarCount; i++){
        wattron(game_win, COLOR_PAIR(16));
        mvwprintw(game_win, room->pillars[i].y, room->pillars[i].x, "▒");
        wattroff(game_win, COLOR_PAIR(16));
    }
    for(int i = 0; i < room->foodCount; i++){
        if(room->foods[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(7));
            mvwprintw(game_win, room->foods[i].cord.y, room->foods[i].cord.x, "♣");
            wattroff(game_win, COLOR_PAIR(7));
        }
    }
    for(int i = 0; i < room->shieldCount; i++){
        if(room->shields[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(19));
            mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x + 1, " ");
            mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x, "◇");
            wattroff(game_win, COLOR_PAIR(19));
        }
        wattron(game_win, COLOR_PAIR(color_pair_num));
        // mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x + 1, ".");
        wattroff(game_win, COLOR_PAIR(color_pair_num));
    }
    for(int i = 0; i < room->goldCount; i++){
        if(room->golds[i].isUsed == false){
            if(room->golds[i].type == 'n'){
                wattron(game_win, COLOR_PAIR(12));
                mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "¤");
                wattroff(game_win, COLOR_PAIR(12));
            }
            else if(room->golds[i].type == 'b'){
                mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "●");
            }
        }
    }
    for(int i = 0; i < room->spellCount; i++){
        if(room->spells[i].isUsed == false){
            if(room->spells[i].type == 's'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "✦");
            }
            else if(room->spells[i].type == 'p'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "◆");
            }
            else if(room->spells[i].type == 'h'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "✧");
            }
        }
    }
    for(int i = 0; i < room->specialfoodCount; i++){
        if(room->specialfoods[i].isUsed == false){
            if(room->specialfoods[i].type == 'n' || room->specialfoods[i].type == 'r'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "♣");
            }
            else if(room->specialfoods[i].type == 'p'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "♧");
            }
            else if(room->specialfoods[i].type == 's'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "♠");
            }
        }
    }
    for(int i = 0; i < room->weaponCount; i++){
        if(room->weapons[i].isUsed == false){
            if(room->weapons[i].type == 'm'){
                wattron(game_win, COLOR_PAIR(16));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002692');
                wattroff(game_win, COLOR_PAIR(16));
            }
            else if(room->weapons[i].type == 'd'){
                wattron(game_win, COLOR_PAIR(10));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U0001F5E1');
                wattroff(game_win, COLOR_PAIR(10));
            }
            else if(room->weapons[i].type == 'w'){
                wattron(game_win, COLOR_PAIR(17));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "⚚");
                wattroff(game_win, COLOR_PAIR(17));
                //  L'\U0001FA84'
            }
            else if(room->weapons[i].type == 'a'){
                wattron(game_win, COLOR_PAIR(18));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U000027B3');
                wattroff(game_win, COLOR_PAIR(18));
            }
            else if(room->weapons[i].type == 's'){
                wattron(game_win, COLOR_PAIR(8));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002694');
                wattroff(game_win, COLOR_PAIR(8));
            }
        }
    }
    for(int i = 0; i < room->trapCount; i++){
        if(room->traps[i].isVisible == true || t_check){
            wattron(game_win, COLOR_PAIR(6));
            mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "^");
            wattroff(game_win, COLOR_PAIR(6));
        }
    }
    for(int i = 0; i < room->doorCount; i++){
        if(room->doors[i].type == 'n'){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "┼");
        }
        else if(room->doors[i].type == 'l' || room->doors[i].type == 'h'){
            if(room->doors[i].is_open == false){
                wattron(game_win, COLOR_PAIR(6));
            }
            else{
                wattron(game_win, COLOR_PAIR(7));
            }
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "☯");
            if(room->doors[i].is_open == false){
                wattroff(game_win, COLOR_PAIR(6));
            }
            else{
                wattroff(game_win, COLOR_PAIR(7));
            }
            wattron(game_win, COLOR_PAIR(temp));
            mvwprintw(game_win, room->password_generator.y, room->password_generator.x, "&");
            wattroff(game_win, COLOR_PAIR(temp));
            if(room->doors[i].type == 'h'){
                wattron(game_win, COLOR_PAIR(temp));
                mvwprintw(game_win, room->password_generator2.y, room->password_generator2.x, "&");
                wattroff(game_win, COLOR_PAIR(temp));
            }
        }
        else if((room->doors[i].type == 's' && room->doors[i].password == 1) || t_check){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "‽");
        }
    }
    for(int i = 0; i < room->enemyCount; i++){
        if(room->enemies[i].isVisible == true){
            wattron(game_win, COLOR_PAIR(14));
            mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
            wattroff(game_win, COLOR_PAIR(14));
        }
    }
    if(room->window.x != 0 && room->window.x != 0){
        if(room->window.x == room->x){
            mvwprintw(game_win, room->window.y, room->window.x, "┤");
        }
        else if(room->window.y == room->y){
            mvwprintw(game_win, room->window.y, room->window.x, "┴");
        }
        else if(room->window.x == room->x + room->width - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "├");
        }
        else if(room->window.y == room->y + room->height - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "┬");
        }
    }
    if(room_number == 0 && game->currentLevel != 0){
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "≡ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    }
    else if(room->stairCount == 1){
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "≡ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    }
    if(room->type == 's' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(15));
    }
    else if(room->type == 'p' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(17));
    }
    else if(room->type == 'm' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(6));
    }
    if(room->keyCount == 1){
    
        wattron(game_win, COLOR_PAIR(12));
        mvwprintw(game_win, room->ancientkey.y, room->ancientkey.x, "Δ");
        wattroff(game_win, COLOR_PAIR(12));
    }
    color_pair_num = temp;
    // wattroff(game_win, COLOR_PAIR(color_pair_num));
}
void clear_room(WINDOW *game_win, Room *room, int room_number) {
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            mvwprintw(game_win, y, x, " ");
        }
    }
}
void print_map(WINDOW *game_win){
    if(ui_old_visuals){legacy_print_map(game_win);return;}
    int map_width, map_height;
    getmaxyx(stdscr, map_height, map_width);
    if (map_height > MAP_ROWS) map_height = MAP_ROWS;
    if (map_width > MAP_COLS) map_width = MAP_COLS;
    int start_x = BORDER_PADDING;
    int start_y = BORDER_PADDING;
    int end_x = map_width - BORDER_PADDING;
    int end_y = map_height - BORDER_PADDING;

    // keypad(game_win, TRUE);
    game->levels[game->currentLevel]->rooms[0]->isVisible = true;
    for(int i = start_y; i < map_height; i++){
        for(int j = start_x; j < map_width; j++){
            if(game->whole_map[game->currentLevel][i][j].isVisible == true && game->whole_map[game->currentLevel][i][j].item == '#'){
                wattron(game_win, COLOR_PAIR(3));
                // mvwprintw(game_win, i, j, "%lc", game->whole_map[game->currentLevel][i][j].item);
                mvwprintw(game_win, i, j, "░");
                wattroff(game_win, COLOR_PAIR(3));
            }
            else{
                mvwprintw(game_win, i, j, " ");
            }
        }
    }
    for(int i = 0; i < game->levels[game->currentLevel]->roomsCount; i++){
        if(game->levels[game->currentLevel]->rooms[i]->isVisible == true){
            draw_room(game_win, game->levels[game->currentLevel]->rooms[i], i);
            if(game->levels[game->currentLevel]->rooms[i]->type == 'm'){
                draw_nightmare_room(game_win, game->levels[game->currentLevel]->rooms[i], i);
                in_nightmare = 0;
            }
        }
    }
    int k = 0;
    // for(int i = start_y; i < map_height; i++){
    //     for(int j = start_x; j < map_width; j++){
    //         mvwprintw(game_win, game->whole_map[k]->cord.y, game->whole_map[k]->cord.x, "%lc", game->whole_map[k]->item);
    //         k++;
    //     }
    // }
    // for(int i = start_y; i < map_height; i++){
    //     for(int j = start_x; j < map_width; j++){
    //         mvwprintw(game_win, i, j, "%lc", game->whole_map[game->currentLevel][i][j]);
    //     }
    // }
    wattron(game_win, COLOR_PAIR(color_pair_num));
    draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
    box(game_win, 0, 0);
    if(color_pair_num == 0){
        wattron(game_win, COLOR_PAIR(14));
    }
    mvwprintw(game_win, map_height - 8, map_width - 22, "█████████████████");
    mvwprintw(game_win, map_height - 7, map_width - 22, "█████████████████");
    mvwprintw(game_win, map_height - 6, map_width - 22, "█████████████████");
    mvwprintw(game_win, map_height - 5, map_width - 22, "███  ████  ██████");
    mvwprintw(game_win, map_height - 4, map_width - 22, "█████    ████████");
    mvwprintw(game_win, map_height - 3, map_width - 22, "█████████████████");
    wattroff(game_win, COLOR_PAIR(color_pair_num));
    wattroff(game_win, COLOR_PAIR(14));
    mvwprintw(game_win, map_height-1, 3, " H: Field guide  |  Tab: Inventory  |  Esc: Pause ");
    wrefresh(game_win);
}
void print_all_map(WINDOW *game_win){
    if(ui_old_visuals){legacy_print_all_map(game_win);return;}
    int map_width, map_height;
    getmaxyx(stdscr, map_height, map_width);
    if (map_height > MAP_ROWS) map_height = MAP_ROWS;
    if (map_width > MAP_COLS) map_width = MAP_COLS;
    int start_x = BORDER_PADDING;
    int start_y = BORDER_PADDING;
    int end_x = map_width * 3 / 4 - BORDER_PADDING;
    int end_y = map_height * 3 / 4 - BORDER_PADDING;

    for(int i = start_y; i < map_height; i++){
        for(int j = start_x; j < map_width; j++){
            if(game->whole_map[game->currentLevel][i][j].isVisible == true){
                wattron(game_win, COLOR_PAIR(3));
            }
            if(game->whole_map[game->currentLevel][i][j].isVisible == false){
                wattron(game_win, COLOR_PAIR(10));
            }
            if(game->whole_map[game->currentLevel][i][j].item == '#'){
                // mvwprintw(game_win, i, j, "%lc", game->whole_map[game->currentLevel][i][j].item);
                mvwprintw(game_win, i, j, "░");
            }
            wattroff(game_win, COLOR_PAIR(3));
            wattroff(game_win, COLOR_PAIR(10));
        }
    }
    for(int i = 0; i < game->levels[game->currentLevel]->roomsCount; i++){
        if(game->levels[game->currentLevel]->rooms[i]->isVisible == false){
            wattron(game_win, COLOR_PAIR(20));
        }
        all_print = 1;
        draw_room(game_win, game->levels[game->currentLevel]->rooms[i], i);
        if(game->levels[game->currentLevel]->rooms[i]->isVisible == false){
            wattroff(game_win, COLOR_PAIR(20));
        }
    }
    wattron(game_win, COLOR_PAIR(color_pair_num));
    draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
    box(game_win, 0, 0);
    wattroff(game_win, COLOR_PAIR(color_pair_num));
    all_print = 0;
    wrefresh(game_win);
}
void continue_corridor(WINDOW *game_win, int y, int x, int a){
    Point next[4];
    next[0].x = x + 1;
    next[0].y = y;
    next[1].x = x;
    next[1].y = y + 1;
    next[2].x = x - 1;
    next[2].y = y;
    next[3].x = x;
    next[3].y = y - 1;
    if(a == 5){
        in_nightmare = 0;
        return;
    }
    else{
        for(int i = 0; i < 4; i++){
            if(game->whole_map[game->currentLevel][next[i].y][next[i].x].item == '#' 
            || game->whole_map[game->currentLevel][next[i].y][next[i].x].item == '+'
            || game->whole_map[game->currentLevel][next[i].y][next[i].x].item == 'L'
            ){
                if(game->whole_map[game->currentLevel][next[i].y][next[i].x].item == '#'){
                    wattron(game_win, COLOR_PAIR(3));
                }
                if(game->whole_map[game->currentLevel][next[i].y][next[i].x].isVisible == false){
                    if(game->whole_map[game->currentLevel][next[i].y][next[i].x].item == 'L'){
                        mvwprintw(game_win, next[i].y, next[i].x, "☯");
                    }
                    else{
                        mvwprintw(game_win, next[i].y, next[i].x, "┼");
                    }
                    game->whole_map[game->currentLevel][next[i].y][next[i].x].isVisible = true;
                }
                if(game->whole_map[game->currentLevel][next[i].y][next[i].x].item == '#'){
                    mvwprintw(game_win, next[i].y, next[i].x, "█");
                }
                // else{
                //     mvwprintw(game_win, next[i].y, next[i].x, "%lc", game->whole_map[game->currentLevel][next[i].y][next[i].x].item);
                // }
                game->whole_map[game->currentLevel][next[i].y][next[i].x].isVisible = true;
                if(game->whole_map[game->currentLevel][next[i].y][next[i].x].item == '#'){
                    wattroff(game_win, COLOR_PAIR(3));
                }
                continue_corridor(game_win, next[i].y, next[i].x, a + 1);
            }
        }
    }
}
void printf_diff_from_map(WINDOW *game_win){
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '+'){
        diff_from_map = '+';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "┼");
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'L'){
        diff_from_map = 'L';
        wattron(game_win, COLOR_PAIR(lock_color));
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "☯");
        wattroff(game_win, COLOR_PAIR(lock_color));
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '?'){
        diff_from_map = '?';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "‽");
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
        diff_from_map = '#';
        wattron(game_win, COLOR_PAIR(3));
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "█");
        wattroff(game_win, COLOR_PAIR(3));
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '%'){
        diff_from_map = '%';
        wattron(game_win, COLOR_PAIR(7));
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "♣");
        wattroff(game_win, COLOR_PAIR(7));
        return;
    }
    // if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '>'){
    //     diff_from_map = '>';
    //     wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x, ">");
    //     wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    //     return;
    // }
    // if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '<'){
    //     diff_from_map = '<';
    //     wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "<");
    //     wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    //     return;
    // }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '>' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '>'){
        diff_from_map = '>';
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "≡ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        return;
    }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '>' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '>'){
        diff_from_map = '>';
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, "≡ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '<' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '<'){
        diff_from_map = '<';
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "≡ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        return;
    }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '<' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '<'){
        diff_from_map = '<';
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, "≡ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'A'){
        diff_from_map = 'A';
        wattron(game_win, COLOR_PAIR(12));
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "Δ");
        wattroff(game_win, COLOR_PAIR(12));
        return;
    }
    if(is_enemy(game->player->cord.y, game->player->cord.x)){
        diff_from_map = 'A';
        wattron(game_win, COLOR_PAIR(14));
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "%c", game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item);
        wattroff(game_win, COLOR_PAIR(14));
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '$'){
        diff_from_map = '$';
        wattron(game_win, COLOR_PAIR(12));
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "¤");
        wattroff(game_win, COLOR_PAIR(12));
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '^'){
        diff_from_map = '^';
        wattron(game_win, COLOR_PAIR(6));
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "^");
        wattroff(game_win, COLOR_PAIR(6));
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '&'){
        diff_from_map = '&';
        wattron(game_win, COLOR_PAIR(color_pair_num));
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "&");
        wattroff(game_win, COLOR_PAIR(color_pair_num));
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '*' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '*'){
        diff_from_map = '*';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "●");
        return;
    }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '*' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '*'){
        diff_from_map = '*';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, "●");
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'E' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == 'E'){
        diff_from_map = 'E';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "✦");
        return;
    }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'E' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == 'E'){
        diff_from_map = 'E';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, "✦");
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'P' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == 'P'){
        diff_from_map = 'P';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "◆");
        return;
    }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'P' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == 'P'){
        diff_from_map = 'P';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, "◆");
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'H' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == 'H'){
        diff_from_map = 'H';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "✧");
        return;
    }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'H' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == 'H'){
        diff_from_map = 'H';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, "✧");
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '6' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '6'){
        diff_from_map = '6';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "♣");
        return;
    }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '6' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '6'){
        diff_from_map = '6';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, "♣");
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '9' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '9'){
        diff_from_map = '9';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "♣");
        return;
    }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '9' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '9'){
        diff_from_map = '9';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, "♣");
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '7' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '7'){
        diff_from_map = '7';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "♧");
        return;
    }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '7' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '7'){
        diff_from_map = '7';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, "♧");
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '8' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '8'){
        diff_from_map = '8';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "♠");
        return;
    }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '8' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '8'){
        diff_from_map = '9';
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, "♠");
        return;
    }
    // else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '1' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '1'){
    //     diff_from_map = '1';
    //     wattron(game_win, COLOR_PAIR(16));
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x + 1, "♠");
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x + 2, ".");
    //     wattroff(game_win, COLOR_PAIR(16));
    //     return;
    // }
    // else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '1' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '1'){
    //     diff_from_map = '1';
    //     wattron(game_win, COLOR_PAIR(16));
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, "♠");
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x, ".");
    //     wattroff(game_win, COLOR_PAIR(16));
    //     return;
    // }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '1'){
        diff_from_map = '1';
        wattron(game_win, COLOR_PAIR(16));
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "♠");
        wattroff(game_win, COLOR_PAIR(16));
        return;
    }
    // else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '2' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '2'){
    //     diff_from_map = '2';
    //     wattron(game_win, COLOR_PAIR(10));
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x + 1, "%lc", L'\U0001F5E1');
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x + 2, ".");
    //     wattroff(game_win, COLOR_PAIR(10));
    //     return;
    // }
    // else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '2' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '2'){
    //     diff_from_map = '2';
    //     wattron(game_win, COLOR_PAIR(10));
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, "%lc", L'\U0001F5E1');
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x, ".");
    //     wattroff(game_win, COLOR_PAIR(10));
    //     return;
    // }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '2'){
        diff_from_map = '2';
        wattron(game_win, COLOR_PAIR(10));
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "%lc", L'\U0001F5E1');
        wattroff(game_win, COLOR_PAIR(10));
        return;
    }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '3'){
        diff_from_map = '3';
        wattron(game_win, COLOR_PAIR(17));
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "⚚");
        wattroff(game_win, COLOR_PAIR(17));
        return;
    }
    // else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '3' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '3'){
    //     diff_from_map = '3';
    //     wattron(game_win, COLOR_PAIR(17));
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, "⚚");
    //     wattroff(game_win, COLOR_PAIR(17));
    //     return;
    // }
    // else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '4' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '4'){
    //     diff_from_map = '4';
    //     wattron(game_win, COLOR_PAIR(18));
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x + 1, "%lc", L'\U000027B3');
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x + 2, ".");
    //     wattroff(game_win, COLOR_PAIR(18));
    //     return;
    // }
    // else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '4' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '4'){
    //     diff_from_map = '4';
    //     wattron(game_win, COLOR_PAIR(18));
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, "%lc", L'\U000027B3');
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x, ".");
    //     wattroff(game_win, COLOR_PAIR(18));
    //     return;
    // }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '4'){
        diff_from_map = '4';
        wattron(game_win, COLOR_PAIR(18));
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "%lc", L'\U000027B3');
        wattroff(game_win, COLOR_PAIR(18));
        return;
    }
    // else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '5' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '5'){
    //     diff_from_map = '5';
    //     wattron(game_win, COLOR_PAIR(8));
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x + 1, "%lc", L'\U00002694');
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x + 2, ".");
    //     wattroff(game_win, COLOR_PAIR(8));
    //     return;
    // }
    // else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '5' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '5'){
    //     diff_from_map = '5';
    //     wattron(game_win, COLOR_PAIR(8));
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, "%lc", L'\U00002694');
    //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x, ".");
    //     wattroff(game_win, COLOR_PAIR(8));
    //     return;
    // }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '5'){
        diff_from_map = '5';
        wattron(game_win, COLOR_PAIR(8));
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "%lc", L'\U00002694');
        wattroff(game_win, COLOR_PAIR(8));
        return;
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '!' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '!'){
        diff_from_map = '!';
        wattron(game_win, COLOR_PAIR(19));
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "◇");
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x + 1, " ");
        wattroff(game_win, COLOR_PAIR(19));
        return;
    }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '!' && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '!'){
        diff_from_map = '!';
        wattron(game_win, COLOR_PAIR(19));
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, "◇");
        mvwprintw(game_win, game->player->cord.y, game->player->cord.x, " ");
        wattroff(game_win, COLOR_PAIR(19));
        return;
    }
}
void print_last_dragon(WINDOW *game_win){
    int map_width, map_height;
    getmaxyx(stdscr, map_height, map_width);
    int a = map_height - 32;
    int b = map_width - 60;
    for(int i = 0; i < 25; i++){
        wattron(game_win, COLOR_PAIR(2 * i));
        mvwprintw(game_win, a, b, "                                            ");
        mvwprintw(game_win, a + 1, b, "                                            ");
        mvwprintw(game_win, a + 2, b, "                                            ");
        mvwprintw(game_win, a + 3, b, "                                            ");
        mvwprintw(game_win, a + 4, b, "                                            ");
        mvwprintw(game_win, a + 5, b, "                                            ");
        mvwprintw(game_win, a + 6, b, "                                            ");
        mvwprintw(game_win, a + 7, b, "                                            ");
        mvwprintw(game_win, a + 8, b, "                                            ");
        mvwprintw(game_win, a + 9, b, "                                            ");
        mvwprintw(game_win, a + 10, b, "                                            ");
        mvwprintw(game_win, a + 11, b, "                                            ");
        mvwprintw(game_win, a + 12, b, "                                            ");
        mvwprintw(game_win, a + 13, b, "                                            ");
        mvwprintw(game_win, a + 14, b, "                                            ");
        mvwprintw(game_win, a, b, " <>=======() ");
        mvwprintw(game_win, a + 1, b, "(/\\___   /|\\          ()==========<>_");
        mvwprintw(game_win, a + 2, b, "      \\_/ | \\        //|\\   ______/ \\)");
        mvwprintw(game_win, a + 3, b, "        \\_|  \\      // | \\_/");
        mvwprintw(game_win, a + 4, b, "          \\|\\/|\\_   //  /\\/");
        mvwprintw(game_win, a + 5, b, "           (oo)\\ \\_//  /");
        mvwprintw(game_win, a + 6, b, "          //_/\\_\\/ /  |");
        mvwprintw(game_win, a + 7, b, "         @@/  |=\\  \\  |");
        mvwprintw(game_win, a + 8, b, "              \\_=\\_ \\ |");
        mvwprintw(game_win, a + 9, b, "                \\==\\ \\|\\_");
        mvwprintw(game_win, a + 10, b, "             __(\\===\\(  )\\");
        mvwprintw(game_win, a + 11, b, "            (((~) __(_/   |");
        mvwprintw(game_win, a + 12, b, "                 (((~) \\  /");
        mvwprintw(game_win, a + 13, b, "               ______/ /");
        mvwprintw(game_win, a + 14, b, "              '------'");
        wrefresh(game_win);
        wattroff(game_win, COLOR_PAIR(2 * i));
        wattron(game_win, COLOR_PAIR(2 * i + 1));
        usleep(60000);
        mvwprintw(game_win, a, b, "                                            ");
        mvwprintw(game_win, a + 1, b, "                                            ");
        mvwprintw(game_win, a + 2, b, "                                            ");
        mvwprintw(game_win, a + 3, b, "                                            ");
        mvwprintw(game_win, a + 4, b, "                                            ");
        mvwprintw(game_win, a + 5, b, "                                            ");
        mvwprintw(game_win, a + 6, b, "                                            ");
        mvwprintw(game_win, a + 7, b, "                                            ");
        mvwprintw(game_win, a + 8, b, "                                            ");
        mvwprintw(game_win, a + 9, b, "                                            ");
        mvwprintw(game_win, a + 10, b, "                                            ");
        mvwprintw(game_win, a + 11, b, "                                            ");
        mvwprintw(game_win, a + 12, b, "                                            ");
        mvwprintw(game_win, a + 13, b, "                                            ");
        mvwprintw(game_win, a + 14, b, "                                            ");
        mvwprintw(game_win, a + 3, b, "           _  _");
        mvwprintw(game_win, a + 4, b, "           |\\/|__    ___");
        mvwprintw(game_win, a + 5, b, "    ____   (oo)\\ \\_//   \\ ___");
        mvwprintw(game_win, a + 6, b, "   //  /\\_//_/\\_\\/ /  |\\_|  \\");
        mvwprintw(game_win, a + 7, b, "  // /\\/ @@/  |=\\  \\  | \\_|  \\_______ ");
        mvwprintw(game_win, a + 8, b, " // | \\_/  /   \\_=\\_ \\ |  (/\\___   /|\\");
        mvwprintw(game_win, a + 9, b, "//|\\______/ \\) \\==\\ \\|\\_  ()=========<>");
        mvwprintw(game_win, a + 10, b, "_<>======()     _(\\===\\(  )\\");
        mvwprintw(game_win, a + 11, b, "              (((~) __(_/   |");
        mvwprintw(game_win, a + 12, b, "                  (((~) \\  /");
        mvwprintw(game_win, a + 13, b, "                  ______/ /");
        mvwprintw(game_win, a + 14, b, "                  '------'");
        wrefresh(game_win);
        wattroff(game_win, COLOR_PAIR(2 * i + 1));
        usleep(60000);
    }
    effects = Mix_LoadMUS("./Base_Sounds/DeathSound.mp3");
    Mix_PlayMusic(effects, 1);
    mvwprintw(game_win, a, b, "                                            ");
    mvwprintw(game_win, a + 1, b, "                                            ");
    mvwprintw(game_win, a + 2, b, "                                            ");
    mvwprintw(game_win, a + 3, b, "                                            ");
    mvwprintw(game_win, a + 4, b, "                                            ");
    mvwprintw(game_win, a + 5, b, "                                            ");
    mvwprintw(game_win, a + 6, b, "                                            ");
    mvwprintw(game_win, a + 7, b, "                                            ");
    mvwprintw(game_win, a + 8, b, "                                            ");
    mvwprintw(game_win, a + 9, b, "                                            ");
    mvwprintw(game_win, a + 10, b, "                                            ");
    mvwprintw(game_win, a + 11, b, "                                            ");
    mvwprintw(game_win, a + 12, b, "                                            ");
    mvwprintw(game_win, a + 13, b, "                                            ");
    mvwprintw(game_win, a + 14, b, "                                            ");
    wrefresh(game_win);
}
void lost_game_screen(time_t start_time) {
    run_finished=1;
    { //Variable Reset
        escape_check = 1;
        diff_from_map = 0;
        in_emoji = 0;
        lock_color = 6;
        password_disapear = 0;
        last_attemps = 0;
        back_front = 0;
        in_nightmare = 0;
        in_spell = 0;
        in_reinforce = 0;
        in_healing = 0;
        all_print = 0;
        t_check = 0;
        g_check = 0;
        f_check = 0;
        valid_f_check = 0;
        x_check = 0;
        m_check = 0;
        count_move = 1;
        poisonous_food_check = 0;
        last_item = '.';
        move_or_not = 0;
        is_enemy_or_not = 0;
        is_enemy_or_not2 = 0;
        is_last_room = 0;
        is_pass_lock = 0;
        which_pos = 1;
        last_one = 0;
    }
    Mix_HaltMusic();
    music_stop_playlist();
    Mix_PlayMusic(loosing_effect, 1);
    game->player->can_be_saved = 0;
    game->player->games_count++;
    int win_height = LINES, win_width = COLS;
    int startx = (COLS - win_width) / 2;
    int starty = (LINES - win_height) / 2;
    WINDOW *lost_win = newwin(win_height, win_width, starty, startx);

    int y_kaka = 28;
    { // Grim Reaper
        int a1 = 3, b1 = 4;
        wattron(lost_win, COLOR_PAIR(124));
        mvwprintw(lost_win, a1, b1, "                                            .""--..__ ");
        mvwprintw(lost_win, a1 + 1, b1, "                     _                     []       ``-.._");
        mvwprintw(lost_win, a1 + 2, b1, "                  .'` `'.                  ||__           `-._");
        mvwprintw(lost_win, a1 + 3, b1, "                 /    ,-.\\                 ||_ ```---..__     `-.");
        mvwprintw(lost_win, a1 + 4, b1, "                /    /:::\\               /|//}          ``--._  `.");
        mvwprintw(lost_win, a1 + 5, b1, "                |    |:;:||              |////}                `-. \\");
        mvwprintw(lost_win, a1 + 6, b1, "                |    |:::||             //'///                    `.\\");
        mvwprintw(lost_win, a1 + 7, b1, "                |    |:::||            //  ||'                      `|");
        mvwprintw(lost_win, a1 + 8, b1, "                /    |:::|/        _,-//\\  ||");
        mvwprintw(lost_win, a1 + 9, b1, "               /`    |:::|`-,__,-'`  |/  \\ ||");
        mvwprintw(lost_win, a1 + 10, b1, "             /`  |   |'' ||           \\   |||");
        mvwprintw(lost_win, a1 + 11, b1, "           /`    \\   |   ||            |  /||");
        mvwprintw(lost_win, a1 + 12, b1, "         |`       |  |   |)            \\ | ||");
        mvwprintw(lost_win, a1 + 13, b1, "        |          \\ |   /      ,.__    \\| ||");
        mvwprintw(lost_win, a1 + 14, b1, "        /           `         /`    `\\   | ||");
        mvwprintw(lost_win, a1 + 15, b1, "       |                     /        \\  / ||");
        mvwprintw(lost_win, a1 + 16, b1, "       |                     |        | /  ||");
        mvwprintw(lost_win, a1 + 17, b1, "       /         /           |        `(   ||");
        mvwprintw(lost_win, a1 + 18, b1, "      /          .           /          )  ||");
        mvwprintw(lost_win, a1 + 19, b1, "     |            \\          |     ________||");
        mvwprintw(lost_win, a1 + 20, b1, "    /             |          /     `-------.|");
        mvwprintw(lost_win, a1 + 21, b1, "   |\\            /          |              ||");
        mvwprintw(lost_win, a1 + 22, b1, "   \\/`-._       |           /              ||");
        mvwprintw(lost_win, a1 + 23, b1, "    //   `.    /`           |              ||");
        mvwprintw(lost_win, a1 + 24, b1, "   //`.    `. |             \\              ||");
        mvwprintw(lost_win, a1 + 25, b1, "  ///\\ `-._  )/             |              ||");
        mvwprintw(lost_win, a1 + 26, b1, " //// )   .(/               |              ||");
        mvwprintw(lost_win, a1 + 27, b1, " ||||   ,'` )               /              //");
        mvwprintw(lost_win, a1 + 28, b1, " ||||  /                    /             || ");
        mvwprintw(lost_win, a1 + 29, b1, " `\\\\` /`                    |             // ");
        mvwprintw(lost_win, a1 + 30, b1, "     |`                     \\            ||  ");
        mvwprintw(lost_win, a1 + 31, b1, "    /                        |           //  ");
        mvwprintw(lost_win, a1 + 32, b1, "  /`                          \\         //   ");
        mvwprintw(lost_win, a1 + 33, b1, "/`                            |        ||    ");
        mvwprintw(lost_win, a1 + 34, b1, "`-.___,-.      .-.        ___,'        (/    ");
        mvwprintw(lost_win, a1 + 35, b1, "         `---'`   `'----'`");
        wattroff(lost_win, COLOR_PAIR(124));
    }
    wattron(lost_win, COLOR_PAIR(color_pair_num));
    box(lost_win, 0, 0);
    // for(int i = 1; i < LINES; i++){
    //     for(int j = 0; j < 5; j++){
    //         mvwprintw(lost_win, i, j, "__");
    //     }
    // }
    // for(int i = 1; i < LINES; i++){
    //     for(int j = COLS - 6; j < COLS - 2; j++){
    //         mvwprintw(lost_win, i, j, "__");
    //     }
    // }
    int a1 = 8;
    mvwprintw(lost_win, LINES / 2 - 15 + a1, (COLS - 45) / 2 - 4, "                    ███████████████                              ");
    mvwprintw(lost_win, LINES / 2 - 14 + a1, (COLS - 45) / 2 - 4, "                █████████ ████ ████                                       ");
    mvwprintw(lost_win, LINES / 2 - 13 + a1, (COLS - 45) / 2 - 4, "                    ████  ████  ███                                    ");
    mvwprintw(lost_win, LINES / 2 - 12 + a1, (COLS - 45) / 2 - 4, "                ███████████ ███████                                   ");
    mvwprintw(lost_win, LINES / 2 - 11 + a1, (COLS - 45) / 2 - 4, "                    ███████████████                                    ");
    mvwprintw(lost_win, LINES / 2 - 10 + a1, (COLS - 45) / 2 - 4, "                ██  ████      █████                                          ");
    mvwprintw(lost_win, LINES / 2 - 9 + a1, (COLS - 45) / 2 - 4, "              █████████ ██████ ███                                        ");
    mvwprintw(lost_win, LINES / 2 - 8 + a1, (COLS - 45) / 2 - 4, "              ██████                                                           ");
    mvwprintw(lost_win, LINES / 2 - 7 + a1, (COLS - 45) / 2 - 4, "              ████  █████████████                                  ");
    mvwprintw(lost_win, LINES / 2 - 6 + a1, (COLS - 45) / 2 - 4, "                ██  ██████  █████                                      ");
    mvwprintw(lost_win,  LINES / 2 - 5 + a1, (COLS - 45) / 2 - 4, "                ██  ██████  █████                                            ");
    wattroff(lost_win, COLOR_PAIR(color_pair_num));
    wattron(lost_win, COLOR_PAIR(6));
    mvwprintw(lost_win, LINES / 2 - 24 + y_kaka, (COLS - 45) / 2 - 12, "          __     ");
    mvwprintw(lost_win, LINES / 2 - 23 + y_kaka, (COLS - 45) / 2 - 12, "      |__|__     ");
    mvwprintw(lost_win, LINES / 2 - 22 + y_kaka, (COLS - 45) / 2 - 12, "       __|  |     ");
    wattroff(lost_win, COLOR_PAIR(6));
    wattron(lost_win, COLOR_PAIR(5));
    mvwprintw(lost_win, LINES / 2 - 21 + y_kaka, (COLS - 45) / 2 - 12, "      ___|__     ");
    mvwprintw(lost_win, LINES / 2 - 20 + y_kaka, (COLS - 45) / 2 - 12, "     /      \\   ");
    mvwprintw(lost_win, LINES / 2 - 19 + y_kaka, (COLS - 45) / 2 - 12, "    /  _  _  \\ ");
    mvwprintw(lost_win, LINES / 2 - 18 + y_kaka, (COLS - 45) / 2 - 12, "   |          |   ");
    mvwprintw(lost_win, LINES / 2 - 17 + y_kaka, (COLS - 45) / 2 - 12, "   |     |    |   ");
    mvwprintw(lost_win, LINES / 2 - 16 + y_kaka, (COLS - 45) / 2 - 12, "   |  ______  |   ");
    mvwprintw(lost_win, LINES / 2 - 15 + y_kaka, (COLS - 45) / 2 - 12, "  |   |____|  |  ");
    mvwprintw(lost_win, LINES / 2 - 14 + y_kaka, (COLS - 45) / 2 - 12, " |            | ");
    mvwprintw(lost_win, LINES / 2 - 13 + y_kaka, (COLS - 45) / 2 - 12, " \\           /  ");
    mvwprintw(lost_win, LINES / 2 - 12 + y_kaka, (COLS - 45) / 2 - 12, "  \\ ______ /  ");
    wattroff(lost_win, COLOR_PAIR(5));
    wattron(lost_win, COLOR_PAIR(6));
    mvwprintw(lost_win, LINES / 2 - 22 + y_kaka, (COLS - 45) / 2 + 39, "       ___      ");
    mvwprintw(lost_win, LINES / 2 - 21 + y_kaka, (COLS - 45) / 2 + 39, "      _|*|__     ");
    wattroff(lost_win, COLOR_PAIR(6));
    wattron(lost_win, COLOR_PAIR(5));
    mvwprintw(lost_win, LINES / 2 - 20 + y_kaka, (COLS - 45) / 2 + 39, "     /      \\   ");
    mvwprintw(lost_win, LINES / 2 - 19 + y_kaka, (COLS - 45) / 2 + 39, "    /  _  _  \\ ");
    mvwprintw(lost_win, LINES / 2 - 18 + y_kaka, (COLS - 45) / 2 + 39, "   |  |_||_|  |   ");
    mvwprintw(lost_win, LINES / 2 - 17 + y_kaka, (COLS - 45) / 2 + 39, "   |    _|    |   ");
    mvwprintw(lost_win, LINES / 2 - 16 + y_kaka, (COLS - 45) / 2 + 39, "   |  ______  |   ");
    mvwprintw(lost_win, LINES / 2 - 15 + y_kaka, (COLS - 45) / 2 + 39, "   |    |+|    |  ");
    mvwprintw(lost_win, LINES / 2 - 14 + y_kaka, (COLS - 45) / 2 + 39, "   |            | ");
    mvwprintw(lost_win, LINES / 2 - 13 + y_kaka, (COLS - 45) / 2 + 39, "    \\           /  ");
    mvwprintw(lost_win, LINES / 2 - 12 + y_kaka, (COLS - 45) / 2 + 39, "      \\ ______ /  ");
    wattroff(lost_win, COLOR_PAIR(5));
    wattron(lost_win, COLOR_PAIR(3));\
    int y_kaka1 = y_kaka - 1;
    mvwprintw(lost_win, LINES / 2 - 22 + y_kaka1, (COLS - 45) / 2 + 8, "You're out of hp and died,");
    time(&end_time);
    mvwprintw(lost_win, LINES / 2 - 20 + y_kaka1, (COLS - 45) / 2 + 9, " Better luck next time!");
    mvwprintw(lost_win, LINES / 2 - 18 + y_kaka1, (COLS - 45) / 2 + 10, "   Your point: %.0lf", (game->player->play_time + difftime(end_time, start_time)) * 5 + game->player->gold * 10);
    mvwprintw(lost_win, LINES / 2 - 16 + y_kaka1, (COLS - 45) / 2 + 9, "  Press Enter to exit.");
    wattroff(lost_win, COLOR_PAIR(3));
    // y_kaka1 += 4;
    wattron(lost_win, COLOR_PAIR(10));
    mvwprintw(lost_win, LINES / 2 - 15 + y_kaka1, (COLS - 45) / 2 + 6, "  ,-=-.       ______     _");
    mvwprintw(lost_win, LINES / 2 - 14 + y_kaka1, (COLS - 45) / 2 + 5, "  /  +  \\     />----->  _|1|_");
    mvwprintw(lost_win, LINES / 2 - 13 + y_kaka1, (COLS - 45) / 2 + 5, "  | ~~~ |    // -/- /  |_ H _|");
    mvwprintw(lost_win, LINES / 2 - 12 + y_kaka1, (COLS - 45) / 2 + 5, "  |R.I.P|   //  /  /     |S|");
    mvwprintw(lost_win, LINES / 2 - 11 + y_kaka1, (COLS - 45) / 2 + 2, "\vV,,|_____|V,//_____/VvV,v,|_|/,,vhjwv/,");
    wattroff(lost_win, COLOR_PAIR(10));
    wattron(lost_win, COLOR_PAIR(7));
    mvwprintw(lost_win, LINES / 2 - 11 + y_kaka1, (COLS - 45) / 2 + 2, "\vV,,");
    mvwprintw(lost_win, LINES / 2 - 11 + y_kaka1, (COLS - 45) / 2 + 14, "V,");
    mvwprintw(lost_win, LINES / 2 - 11 + y_kaka1, (COLS - 45) / 2 + 24, "VvV,v,");
    mvwprintw(lost_win, LINES / 2 - 11 + y_kaka1, (COLS - 45) / 2 + 33, "/,,vhjwv/,");
    wattroff(lost_win, COLOR_PAIR(7));
    wattron(lost_win, COLOR_PAIR(color_pair_num));
    int a = 22;
    if(color_pair_num == 0){
            wattron(lost_win, COLOR_PAIR(14));
    }
    mvwprintw(lost_win, LINES - 8 - a, COLS - 21, "█████████████████");
    mvwprintw(lost_win, LINES - 7 - a, COLS - 21, "█████████████████");
    mvwprintw(lost_win, LINES - 6 - a, COLS - 21, "█████████████████");
    mvwprintw(lost_win, LINES - 5 - a, COLS - 21, "███  ████  ██████");
    mvwprintw(lost_win, LINES - 4 - a, COLS - 21, "█████    ████████");
    mvwprintw(lost_win, LINES - 3 - a, COLS - 21, "█████████████████");
    wattroff(lost_win, COLOR_PAIR(color_pair_num));
    wattroff(lost_win, COLOR_PAIR(14));
    wattron(lost_win, COLOR_PAIR(16));
    mvwprintw(lost_win, LINES - 2 - a, COLS - 24, "╰────────────────────╯");
    mvwprintw(lost_win, LINES - 1 - a, COLS - 24, "⧹⧹;|_|;|_|;|;|_|;|_⧸⧸");
    mvwprintw(lost_win, LINES - 0 - a, COLS - 23, "\\\\.    .   ,   .  /");
    mvwprintw(lost_win, LINES + 1 - a, COLS - 23, " \\\\:  .   ,  . , /");
    mvwprintw(lost_win, LINES + 2 - a, COLS - 23, "  \\\\:  .    . , /");
    mvwprintw(lost_win, LINES + 3 - a, COLS - 23, "   \\\\:  .  . , /");
    mvwprintw(lost_win, LINES + 4 - a, COLS - 23, "    ||:  ,    |");
    mvwprintw(lost_win, LINES + 5 - a, COLS - 23, "    ||:    .  |");
    mvwprintw(lost_win, LINES + 6 - a, COLS - 23, "    ||:    ,  |");
    mvwprintw(lost_win, LINES + 7 - a, COLS - 23, "    ||: .    ,|");
    mvwprintw(lost_win, LINES + 8 - a, COLS - 23, "    ||:       |");
    mvwprintw(lost_win, LINES + 9 - a, COLS - 23, "    ||:   -   |");
    mvwprintw(lost_win, LINES + 10 - a, COLS - 23, "    ||: .     |");
    mvwprintw(lost_win, LINES + 11 - a, COLS - 23, "    ||:      ,|");
    mvwprintw(lost_win, LINES + 12 - a, COLS - 23, "    ||:   *   |");
    mvwprintw(lost_win, LINES + 13 - a, COLS - 23, "    ||:  `    |");
    mvwprintw(lost_win, LINES + 14 - a, COLS - 23, "    ||:   -   |");
    mvwprintw(lost_win, LINES + 15 - a, COLS - 23, "    ||: ,  '  |");
    mvwprintw(lost_win, LINES + 16 - a, COLS - 23, ".o_//     ,    \\_o.");
    wattroff(lost_win, COLOR_PAIR(16));
    wattron(lost_win, COLOR_PAIR(3));
    mvwprintw(lost_win, LINES - 11 - a, COLS - 76, "╭───────────────────────────────────────────────────╮");
    mvwprintw(lost_win, LINES - 10 - a, COLS - 76, "│ Hey my friend, I guess you had bad luck!          │");
    mvwprintw(lost_win, LINES - 9 - a, COLS - 76, UI_TEXT("│ No problem! You can surely make it next time.🔥   │","│ No problem! You can surely make it next time.✦   │"));
    mvwprintw(lost_win, LINES - 8 - a, COLS - 76, "│ After all those quotes I hope you enjoyed the game│");
    mvwprintw(lost_win, LINES - 7 - a, COLS - 76, "│ And most important thing is the friendship that   │");
    mvwprintw(lost_win, LINES - 6 - a, COLS - 76, "│ I made with you along this journey %s", game->player->name);
    mvwprintw(lost_win, LINES - 6 - a, COLS - 24, "│");
    mvwprintw(lost_win, LINES - 5 - a, COLS - 76, UI_TEXT("│ See you soon. 😉                                 _⧹","│ See you soon. ;)                                 _⧹"));
    mvwprintw(lost_win, LINES - 4 - a, COLS - 76, "╰─────────────────────────────────────────────────┘  ¯");
    wattroff(lost_win, COLOR_PAIR(3));
    mvwaddstr(lost_win,LINES-2,4,"Enter / Esc: return to the main menu");
    wrefresh(lost_win);
    sleep(3);
    while(1){
        int c = music_getch(lost_win);
        if(c == 10 || c == KEY_ENTER || c == 27){
            break;
        }
    }
    wclear(lost_win);
    wrefresh(lost_win);
    delwin(lost_win);
    clear();refresh();
    Mix_HaltMusic();
}
void win_game_screen(time_t start_time) {
    if(victory_results_closed())return;
    run_finished=1;
    { //Variable Reset
        escape_check = 1;
        diff_from_map = 0;
        in_emoji = 0;
        lock_color = 6;
        password_disapear = 0;
        last_attemps = 0;
        back_front = 0;
        in_nightmare = 0;
        in_spell = 0;
        in_reinforce = 0;
        in_healing = 0;
        all_print = 0;
        t_check = 0;
        g_check = 0;
        f_check = 0;
        valid_f_check = 0;
        x_check = 0;
        m_check = 0;
        count_move = 1;
        poisonous_food_check = 0;
        last_item = '.';
        move_or_not = 0;
        is_enemy_or_not = 0;
        is_enemy_or_not2 = 0;
        is_last_room = 0;
        is_pass_lock = 0;
        which_pos = 1;
        last_one = 0;
    }
    Mix_HaltMusic();
    music_stop_playlist();
    Mix_PlayMusic(winning_effect, 1);
    game->player->can_be_saved = 0;
    game->player->games_count++;
    int win_height = LINES, win_width = COLS;
    int startx = (COLS - win_width) / 2;
    int starty = (LINES - win_height) / 2;
    WINDOW *win_win = newwin(win_height, win_width, starty, startx);
    if(ui_old_visuals){
    { // Angel
        int a1 = 3, b1 = 8;
        wattron(win_win, COLOR_PAIR(117));
        mvwprintw(win_win, a1, b1, "       .-\"\"-.      _______      .-\"\"-.");
        mvwprintw(win_win, a1 + 1, b1, "     .'_.-.  |   ,*********,   |  .-._'.");
        mvwprintw(win_win, a1 + 2, b1, "    /    _/ /   **`       `**   \\ \\_    \\");
        mvwprintw(win_win, a1 + 3, b1, "   /.--.' | |    **,;;;;;,**    | | '.--.\\");
        mvwprintw(win_win, a1 + 4, b1, "  /   .-`-| |    ;//;/;/;\\;    | |-`-.   \\");
        mvwprintw(win_win, a1 + 5, b1, " ;.--':   | |   /;/;/;//\\\\;\\\\   | |   :'--.;");
        mvwprintw(win_win, a1 + 6, b1, "|    _\\.'-| |  ((;(/;/; \\;\\);)  | |-'./_    |");
        mvwprintw(win_win, a1 + 7, b1, ";_.-'/:   | |  );)) _   _ (;((  | |   :\\'-._;");
        mvwprintw(win_win, a1 + 8, b1, "|   | _:-'\\  \\((((    \\    );))/  /'-:_ |   |");
        mvwprintw(win_win, a1 + 9, b1, ";  .:` '._ \\  );))\\   \"   /((((  / _.' `:.  ;");
        mvwprintw(win_win, a1 + 10, b1, ";  .:` '._ \\  );))\\   \"   /((((  / _.' `:.  ;");
        mvwprintw(win_win, a1 + 11, b1, "; / .'\\ |`'\\ );));)/`---`\\((;(((./`'| /'. \\ ;");
        mvwprintw(win_win, a1 + 12, b1, "| .' / `'.\\-((((((\\       /))));) \\.'` \\ '. |");
        mvwprintw(win_win, a1 + 13, b1, ";/  /\\_/-`-/ ););)|   ,   |;(;(( \\` -\\_/\\  \\;");
        mvwprintw(win_win, a1 + 14, b1, " |.' .| `;/   (;(|'==/|\\=='|);)   \\;` |. '.|");
        mvwprintw(win_win, a1 + 15, b1, " |  / \\.'/      / _.` | `._ \\      \'./ \\  |");
        mvwprintw(win_win, a1 + 16, b1, "  \\| ; |;    _,.-` \\_/Y\\_/ `-.,_    ;| ; |/");
        mvwprintw(win_win, a1 + 17, b1, "   \\ | ;|   `       | | |       `   |. | /");
        mvwprintw(win_win, a1 + 18, b1, "    `\\ ||           | | |           || /`");
        mvwprintw(win_win, a1 + 19, b1, "      `:_\\         _\\/ \\/_         /_:'");
        mvwprintw(win_win, a1 + 20, b1, "          `\"----\"\"`       `\"\"----\"`");
        b1 -= 2;
        mvwprintw(win_win, a1 + 21, b1, "                 /             |              ");
        mvwprintw(win_win, a1 + 22, b1, "                |              /              ");
        mvwprintw(win_win, a1 + 23, b1, "               /`              |              ");
        mvwprintw(win_win, a1 + 24, b1, "              |                \\              ");
        mvwprintw(win_win, a1 + 25, b1, "             )/                |              ");
        mvwprintw(win_win, a1 + 26, b1, "          .(/                  |              ");
        mvwprintw(win_win, a1 + 27, b1, "        ,'`                    /              ");
        mvwprintw(win_win, a1 + 28, b1, "       /                       /              ");
        mvwprintw(win_win, a1 + 29, b1, "      /`                       |              ");
        mvwprintw(win_win, a1 + 30, b1, "     |`                        \\              ");
        mvwprintw(win_win, a1 + 31, b1, "    /                           |             ");
        mvwprintw(win_win, a1 + 32, b1, "  /`                             \\            ");
        mvwprintw(win_win, a1 + 33, b1, "/`                               |            ");
        mvwprintw(win_win, a1 + 34, b1, "`-.___,-.       .-.         ____,'            ");
        mvwprintw(win_win, a1 + 35, b1, "         `----'`   `\"'-----'`");
        wattroff(win_win, COLOR_PAIR(117));
    }
    int y_kaka = 28;
    wattron(win_win, COLOR_PAIR(color_pair_num));
    box(win_win, 0, 0);
    int a1 = 8;
    int b1 = -1;
    mvwprintw(win_win, LINES / 2 - 14 + a1, (COLS - 45) / 2 - 4 - b1, "                    ███████████████                              ");
    mvwprintw(win_win, LINES / 2 - 13 + a1, (COLS - 45) / 2 - 4 - b1, "                █████████ █████ ███                                       ");
    mvwprintw(win_win, LINES / 2 - 12 + a1, (COLS - 45) / 2 - 4 - b1, "                    ████  ████  ███                                    ");
    mvwprintw(win_win, LINES / 2 - 11 + a1, (COLS - 45) / 2 - 4 - b1, "                ███████████ ███████                                   ");
    mvwprintw(win_win, LINES / 2 - 10 + a1, (COLS - 45) / 2 - 4 - b1, "                    ███ ██████ ████                                    ");
    mvwprintw(win_win, LINES / 2 - 9 + a1, (COLS - 45) / 2 - 4 - b1, "                ██  ████       ████                                          ");
    mvwprintw(win_win, LINES / 2 - 8 + a1, (COLS - 45) / 2 - 4 - b1, "              ████████████████████                                        ");
    mvwprintw(win_win, LINES / 2 - 7 + a1, (COLS - 45) / 2 - 4 - b1, "              ██████                                                           ");
    mvwprintw(win_win, LINES / 2 - 6 + a1, (COLS - 45) / 2 - 4 - b1, "              ████  █████████████                                  ");
    mvwprintw(win_win, LINES / 2 - 5 + a1, (COLS - 45) / 2 - 4 - b1, "                ██  ██████  █████                                      ");
    mvwprintw(win_win,  LINES / 2 - 4 + a1, (COLS - 45) / 2 - 4 - b1, "                ██  ██████  █████                                            ");
    wattroff(win_win, COLOR_PAIR(color_pair_num));
    wattron(win_win, COLOR_PAIR(6));
    mvwprintw(win_win, LINES / 2 - 24 + y_kaka, (COLS - 45) / 2 - 7, "          __     ");
    mvwprintw(win_win, LINES / 2 - 23 + y_kaka, (COLS - 45) / 2 - 7, "      |__|__     ");
    mvwprintw(win_win, LINES / 2 - 22 + y_kaka, (COLS - 45) / 2 - 7, "       __|  |     ");
    wattroff(win_win, COLOR_PAIR(6));
    wattron(win_win, COLOR_PAIR(5));
    mvwprintw(win_win, LINES / 2 - 21 + y_kaka, (COLS - 45) / 2 - 7, "      ___|__     ");
    mvwprintw(win_win, LINES / 2 - 20 + y_kaka, (COLS - 45) / 2 - 7, "     /      \\   ");
    mvwprintw(win_win, LINES / 2 - 19 + y_kaka, (COLS - 45) / 2 - 7, "    /  _  _  \\ ");
    mvwprintw(win_win, LINES / 2 - 18 + y_kaka, (COLS - 45) / 2 - 7, "   |          |   ");
    mvwprintw(win_win, LINES / 2 - 17 + y_kaka, (COLS - 45) / 2 - 7, "   |     |    |   ");
    mvwprintw(win_win, LINES / 2 - 16 + y_kaka, (COLS - 45) / 2 - 7, "   |  ______  |   ");
    mvwprintw(win_win, LINES / 2 - 15 + y_kaka, (COLS - 45) / 2 - 7, "  |   |____|  |  ");
    mvwprintw(win_win, LINES / 2 - 14 + y_kaka, (COLS - 45) / 2 - 7, " |            | ");
    mvwprintw(win_win, LINES / 2 - 13 + y_kaka, (COLS - 45) / 2 - 7, " \\           /  ");
    mvwprintw(win_win, LINES / 2 - 12 + y_kaka, (COLS - 45) / 2 - 7, "  \\ ______ /  ");
    wattroff(win_win, COLOR_PAIR(5));
    wattron(win_win, COLOR_PAIR(6));
    mvwprintw(win_win, LINES / 2 - 22 + y_kaka, (COLS - 45) / 2 + 33, "       ___      ");
    mvwprintw(win_win, LINES / 2 - 21 + y_kaka, (COLS - 45) / 2 + 33, "      _|*|__     ");
    wattroff(win_win, COLOR_PAIR(6));
    wattron(win_win, COLOR_PAIR(5));
    mvwprintw(win_win, LINES / 2 - 20 + y_kaka, (COLS - 45) / 2 + 33, "     /      \\   ");
    mvwprintw(win_win, LINES / 2 - 19 + y_kaka, (COLS - 45) / 2 + 33, "    /  _  _  \\ ");
    mvwprintw(win_win, LINES / 2 - 18 + y_kaka, (COLS - 45) / 2 + 33, "   |  |_||_|  |   ");
    mvwprintw(win_win, LINES / 2 - 17 + y_kaka, (COLS - 45) / 2 + 33, "   |    _|    |   ");
    mvwprintw(win_win, LINES / 2 - 16 + y_kaka, (COLS - 45) / 2 + 33, "   |  ______  |   ");
    mvwprintw(win_win, LINES / 2 - 15 + y_kaka, (COLS - 45) / 2 + 33, "   |    |+|    |  ");
    mvwprintw(win_win, LINES / 2 - 14 + y_kaka, (COLS - 45) / 2 + 33, "   |            | ");
    mvwprintw(win_win, LINES / 2 - 13 + y_kaka, (COLS - 45) / 2 + 33, "    \\           /  ");
    mvwprintw(win_win, LINES / 2 - 12 + y_kaka, (COLS - 45) / 2 + 33, "      \\ ______ /  ");
    wattroff(win_win, COLOR_PAIR(5));
    wattron(win_win, COLOR_PAIR(3));\
    int y_kaka1 = y_kaka - 3;
    mvwprintw(win_win, LINES / 2 - 20 + y_kaka1, (COLS - 45) / 2 + 8, "        You made it!");
    time(&end_time);
    mvwprintw(win_win, LINES / 2 - 18 + y_kaka1, (COLS - 45) / 2 + 10, "     Your point: %.0lf", (game->player->play_time + difftime(end_time, start_time)) * 5 + game->player->gold * 10);
    mvwprintw(win_win, LINES / 2 - 16 + y_kaka1, (COLS - 45) / 2 + 11, "  Press enter to exit.");
    wattroff(win_win, COLOR_PAIR(3));
    wattron(win_win, COLOR_PAIR(14));
    mvwprintw(win_win, LINES / 2 - 15 + y_kaka1, (COLS - 45) / 2 + 15, "   _,     ,_");
    mvwprintw(win_win, LINES / 2 - 14 + y_kaka1, (COLS - 45) / 2 + 15, " .'/  ,_   \\'.");
    mvwprintw(win_win, LINES / 2 - 13 + y_kaka1, (COLS - 45) / 2 + 15, "|  \\__('>__/  |");
    mvwprintw(win_win, LINES / 2 - 12 + y_kaka1, (COLS - 45) / 2 + 15, "\\   - ,  -    /");
    mvwprintw(win_win, LINES / 2 - 11 + y_kaka1, (COLS - 45) / 2 + 15, " '-..__ __..-'");
    mvwprintw(win_win, LINES / 2 - 10 + y_kaka1, (COLS - 45) / 2 + 15, "      /_\\");
    mvwprintw(win_win, LINES / 2 - 9 + y_kaka1, (COLS - 45) / 2 + 15, "      m m");
    wattroff(win_win, COLOR_PAIR(14));
    wattron(win_win, COLOR_PAIR(color_pair_num));
    if(color_pair_num == 0){
        wattron(win_win, COLOR_PAIR(14));
    }
    else{
        wattron(win_win, COLOR_PAIR(color_pair_num));
    }
    int a = 22;
    mvwprintw(win_win, LINES - 8 - a, COLS - 21, "█████████████████");
    mvwprintw(win_win, LINES - 7 - a, COLS - 21, "█████████████████");
    mvwprintw(win_win, LINES - 6 - a, COLS - 21, "█████████████████");
    mvwprintw(win_win, LINES - 5 - a, COLS - 21, "███  ████  ██████");
    mvwprintw(win_win, LINES - 4 - a, COLS - 21, "█████    ████████");
    mvwprintw(win_win, LINES - 3 - a, COLS - 21, "█████████████████");
    wattroff(win_win, COLOR_PAIR(color_pair_num));
    wattroff(win_win, COLOR_PAIR(14));
    wattron(win_win, COLOR_PAIR(16));
    mvwprintw(win_win, LINES - 2 - a, COLS - 24, "╰────────────────────╯");
    mvwprintw(win_win, LINES - 1 - a, COLS - 24, "⧹⧹;|_|;|_|;|;|_|;|_⧸⧸");
    mvwprintw(win_win, LINES - 0 - a, COLS - 23, "\\\\.    .   ,   .  /");
    mvwprintw(win_win, LINES + 1 - a, COLS - 23, " \\\\:  .   ,  . , /");
    mvwprintw(win_win, LINES + 2 - a, COLS - 23, "  \\\\:  .    . , /");
    mvwprintw(win_win, LINES + 3 - a, COLS - 23, "   \\\\:  .  . , /");
    mvwprintw(win_win, LINES + 4 - a, COLS - 23, "    ||:  ,    |");
    mvwprintw(win_win, LINES + 5 - a, COLS - 23, "    ||:    .  |");
    mvwprintw(win_win, LINES + 6 - a, COLS - 23, "    ||:    ,  |");
    mvwprintw(win_win, LINES + 7 - a, COLS - 23, "    ||: .    ,|");
    mvwprintw(win_win, LINES + 8 - a, COLS - 23, "    ||:       |");
    mvwprintw(win_win, LINES + 9 - a, COLS - 23, "    ||:   -   |");
    mvwprintw(win_win, LINES + 10 - a, COLS - 23, "    ||: .     |");
    mvwprintw(win_win, LINES + 11 - a, COLS - 23, "    ||:      ,|");
    mvwprintw(win_win, LINES + 12 - a, COLS - 23, "    ||:   *   |");
    mvwprintw(win_win, LINES + 13 - a, COLS - 23, "    ||:  `    |");
    mvwprintw(win_win, LINES + 14 - a, COLS - 23, "    ||:   -   |");
    mvwprintw(win_win, LINES + 15 - a, COLS - 23, "    ||: ,  '  |");
    mvwprintw(win_win, LINES + 16 - a, COLS - 23, ".o_//     ,    \\_o.");
    wattroff(win_win, COLOR_PAIR(16));
    wattron(win_win, COLOR_PAIR(3));
    mvwprintw(win_win, LINES - 11 - a, COLS - 76, "╭───────────────────────────────────────────────────╮");
    mvwprintw(win_win, LINES - 10 - a, COLS - 76, "│ Hey my friend, I knew you could make it through!  │");
    mvwprintw(win_win, LINES - 9 - a, COLS - 76, UI_TEXT("│ Well done! You are that one worthy hero.🔥        │","│ Well done! You are that one worthy hero.✦        │"));
    mvwprintw(win_win, LINES - 8 - a, COLS - 76, "│ After all those quotes I hope you enjoyed the game│");
    mvwprintw(win_win, LINES - 7 - a, COLS - 76, "│ And most important thing is the friendship that   │");
    mvwprintw(win_win, LINES - 6 - a, COLS - 76, "│ I made with you along this journey %s =)", game->player->name);
    mvwprintw(win_win, LINES - 6 - a, COLS - 24, "│");
    mvwprintw(win_win, LINES - 5 - a, COLS - 76, UI_TEXT("│ See you soon. 😉                                 _⧹","│ See you soon. ;)                                 _⧹"));
    mvwprintw(win_win, LINES - 4 - a, COLS - 76, "╰─────────────────────────────────────────────────┘  ¯");
    wattroff(win_win, COLOR_PAIR(3));
    }
    victory_results_screen(win_win,start_time);
    wclear(win_win);
    wrefresh(win_win);
    delwin(win_win);
    clear();refresh();
    Mix_HaltMusic();
}
int valid_move_check(int y, int x){
    if (y < 0 || y >= MAP_ROWS || x < 0 || x >= MAP_COLS) return 0;
    if(game->whole_map[game->currentLevel][y][x].item != '|' 
    && game->whole_map[game->currentLevel][y][x].item != '-' 
    && game->whole_map[game->currentLevel][y][x].item != '_' 
    && game->whole_map[game->currentLevel][y][x].item != 'O' 
    && game->whole_map[game->currentLevel][y][x].item != '=' 
    && game->whole_map[game->currentLevel][y][x].item != 0
    && game->whole_map[game->currentLevel][y][x].item != ' '){
        if(valid_f_check && (game->whole_map[game->currentLevel][y][x].item == 'L' || game->whole_map[game->currentLevel][y][x].item ==  '+')){
            return 0;
        }
        for(int r=0;r<game->levels[game->currentLevel]->roomsCount;r++){
            Room *room=game->levels[game->currentLevel]->rooms[r];
            for(int i=0;i<room->doorCount;i++){
                Door *d=&room->doors[i];
                if((d->type=='l'||d->type=='h')&&!d->is_open&&d->cord.x==x&&d->cord.y==y)return 0;
            }
        }
        return 1;
    }
    else{
        return 0;
    }
}
/* A diagonal or sword lunge must traverse every tile and cannot clip a locked corner. */
int player_step_valid(int y,int x){
    int px=game->player->cord.x,py=game->player->cord.y;
    int dx=(x>px)-(x<px),dy=(y>py)-(y<py);
    while(px!=x||py!=y){
        int nx=px==x?px:px+dx,ny=py==y?py:py+dy;
        if(!valid_move_check(ny,nx))return 0;
        if(nx!=px&&ny!=py&&(!valid_move_check(py,nx)||!valid_move_check(ny,px)))return 0;
        px=nx;py=ny;
    }
    return 1;
}
void enemy_move_can_pass_throw_corridors(Enemy *enemy, WINDOW *game_win){
    if(enemy->canMove == false || enemy->isAlive == false){
        return;
    }
    int dx = 0, dy = 0;
    game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item = last_item;
    if(last_item == '#'){
        wattron(game_win, COLOR_PAIR(3));
        mvwprintw(game_win, enemy->cord.y, enemy->cord.x, "#");
        wattroff(game_win, COLOR_PAIR(3));
    }
    if(enemy->cord.x > game->player->cord.x){
        dx = -1;
    }
    if(enemy->cord.y > game->player->cord.y){
        dy = -1;
    }
    if(enemy->cord.x < game->player->cord.x){
        dx = 1;
    }
    if(enemy->cord.y < game->player->cord.y){
        dy = 1;
    }
    if(rand() % 2 && dx != 0){
        if(valid_move_check(enemy->cord.y, enemy->cord.x + dx)){
            enemy->cord.x += dx;
            last_item = game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item;
            game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item = enemy->type - 32;
            return;
        }
    }
    else{
        if(valid_move_check(enemy->cord.y + dy, enemy->cord.x)){
            if(dy == 0){
                if(valid_move_check(enemy->cord.y + dy, enemy->cord.x + dx)){
                    enemy->cord.x += dx;
                }
            }
            enemy->cord.y += dy;
            last_item = game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item;
            game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item = enemy->type - 32;
            return;
        }
    }
    if(rand() % 2 && dx != 0){
        if(valid_move_check(enemy->cord.y, enemy->cord.x - dx)){
            enemy->cord.x -= dx;
            last_item = game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item;
            game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item = enemy->type - 32;
            return;
        }
    }
    else{
        if(valid_move_check(enemy->cord.y - dy, enemy->cord.x)){
            if(dy == 0){
                if(valid_move_check(enemy->cord.y - dy, enemy->cord.x - dx)){
                    enemy->cord.x -= dx;
                }
            }
            enemy->cord.y -= dy;
            last_item = game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item;
            game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item = enemy->type - 32;
            return;
        }
    }
}
// Change name above and below to change the moving way.
void enemy_move(Enemy *enemy, WINDOW *game_win){
    if((enemy->canMove == false || enemy->isAlive == false) && !is_last_room){
        return;
    }
    int dx = 0, dy = 0;
    game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item = last_item;
    if(last_item == '#'){
        wattron(game_win, COLOR_PAIR(3));
        mvwprintw(game_win, enemy->cord.y, enemy->cord.x, "#");
        wattroff(game_win, COLOR_PAIR(3));
    }
    if(enemy->cord.x > game->player->cord.x){
        dx = -1;
    }
    if(enemy->cord.y > game->player->cord.y){
        dy = -1;
    }
    if(enemy->cord.x < game->player->cord.x){
        dx = 1;
    }
    if(enemy->cord.y < game->player->cord.y){
        dy = 1;
    }
    if(rand() % 2 && dx != 0){
        if(valid_move_check(enemy->cord.y, enemy->cord.x + dx)
        && game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x + dx].item != '+'
        && game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x + dx].item != 'L'
        && game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x + dx].item != '?'){
            enemy->cord.x += dx;
            last_item = game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item;
            game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item = enemy->type - 32;
            return;
        }
    }
    else{
        if(valid_move_check(enemy->cord.y + dy, enemy->cord.x)
        && game->whole_map[game->currentLevel][enemy->cord.y + dy][enemy->cord.x].item != '+'
        && game->whole_map[game->currentLevel][enemy->cord.y + dy][enemy->cord.x].item != 'L'
        && game->whole_map[game->currentLevel][enemy->cord.y + dy][enemy->cord.x].item != '?'){
            if(dy == 0){
                if(valid_move_check(enemy->cord.y + dy, enemy->cord.x + dx)
                && game->whole_map[game->currentLevel][enemy->cord.y + dy][enemy->cord.x + dx].item != '+'
                && game->whole_map[game->currentLevel][enemy->cord.y + dy][enemy->cord.x + dx].item != 'L'
                && game->whole_map[game->currentLevel][enemy->cord.y + dy][enemy->cord.x + dx].item != '?'){
                    enemy->cord.x += dx;
                }
            }
            enemy->cord.y += dy;
            last_item = game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item;
            game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item = enemy->type - 32;
            return;
        }
    }
    if(rand() % 2 && dx != 0){
        if(valid_move_check(enemy->cord.y, enemy->cord.x - dx)
        && game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x - dx].item != '+'
        && game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x - dx].item != 'L'
        && game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x - dx].item != '?'){
            enemy->cord.x -= dx;
            last_item = game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item;
            game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item = enemy->type - 32;
            return;
        }
    }
    else{
        if(valid_move_check(enemy->cord.y - dy, enemy->cord.x)
        && game->whole_map[game->currentLevel][enemy->cord.y - dy][enemy->cord.x].item != '+'
        && game->whole_map[game->currentLevel][enemy->cord.y - dy][enemy->cord.x].item != 'L'
        && game->whole_map[game->currentLevel][enemy->cord.y - dy][enemy->cord.x].item != '?'){
            if(dy == 0){
                if(valid_move_check(enemy->cord.y - dy, enemy->cord.x - dx)
                && game->whole_map[game->currentLevel][enemy->cord.y - dy][enemy->cord.x - dx].item != '+'
                && game->whole_map[game->currentLevel][enemy->cord.y - dy][enemy->cord.x - dx].item != 'L'
                && game->whole_map[game->currentLevel][enemy->cord.y - dy][enemy->cord.x - dx].item != '?'){
                    enemy->cord.x -= dx;
                }
            }
            enemy->cord.y -= dy;
            last_item = game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item;
            game->whole_map[game->currentLevel][enemy->cord.y][enemy->cord.x].item = enemy->type - 32;
            return;
        }
    }
}
void entering_password(void);
static int enter_door_password(int y,int x){
    int previous=game->player->room_num;
    for(int r=0;r<game->levels[game->currentLevel]->roomsCount;r++){
        Room *room=game->levels[game->currentLevel]->rooms[r];
        for(int d=0;d<room->doorCount;d++)if(room->doors[d].cord.x==x&&room->doors[d].cord.y==y&&(room->doors[d].type=='l'||room->doors[d].type=='h')){
            game->player->room_num=r;room->lock_door=d;last_attemps=room->lock_attemps;
            entering_password();room->lock_attemps=last_attemps;
            game->player->room_num=previous;return 1;
        }
    }return 0;
}
void entering_password() {
    if(first_lock_door == 0){
        first_lock_door = 1;
        return;
    }
    int correct_password = game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_door].password;
    int correct_password2;
    if(game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_door].type == 'h'){
        correct_password2 = game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_door].password2;
    }
    int input;
    int input2;
    int attempts = last_attemps;
    int choice;
    int highlight = 0;
    int win_height = 10, win_width = 42;
    int startx = (COLS - win_width) / 2;
    int starty = (LINES - win_height) / 2;
    WINDOW *password_win = newwin(win_height, win_width, starty, startx);
    wattron(password_win, COLOR_PAIR(color_pair_num));
    box(password_win, 0, 0);
    wattroff(password_win, COLOR_PAIR(color_pair_num));
    char *options[] = {"Use Ancient Key", "Enter Password", "Exit"};
    keypad(password_win, TRUE);
    int color = color_pair_num;
    if(attempts == 1){
        color = 8;
    }
    else if(attempts == 2){
        color = 14;
    }
    else if(attempts == 3){
        color = 6;
    }
    while (1) {
        werase(password_win);
        wattron(password_win, COLOR_PAIR(color));
        box(password_win, 0, 0);
        wattroff(password_win, COLOR_PAIR(color));
        
        for (int i = 0; i < 3; i++) {
            if (i == highlight || (game->currentLevel == 6 && highlight == 1 && i == 1))
                wattron(password_win, A_REVERSE);
            if(i == 0){
                wattron(password_win, COLOR_PAIR(12));
            }
            else if(i == 1 && game->currentLevel != 6){
                wattron(password_win, COLOR_PAIR(color));
            }
            if(game->currentLevel == 6 && i == 1){
                mvwprintw(password_win, i + 1, 1, "● %s", options[i + 1]);
                wattroff(password_win, A_REVERSE);
                wattroff(password_win, COLOR_PAIR(12));
                wattroff(password_win, COLOR_PAIR(color));
                break;
            }
            mvwprintw(password_win, i + 1, 1, "● %s", options[i]);
            wattroff(password_win, A_REVERSE);
            wattroff(password_win, COLOR_PAIR(12));
            wattroff(password_win, COLOR_PAIR(color));
        }
        wrefresh(password_win);

        choice = music_getch(password_win);

        switch (choice) {
            case KEY_UP:
                if(game->currentLevel == 6){
                    highlight = 1 - highlight;
                }
                else{
                    highlight = (highlight - 1 + 3) % 3;
                }
                break;
            case KEY_DOWN:
                if(game->currentLevel == 6){
                    highlight = 1 - highlight;
                }
                else{
                    highlight = (highlight + 1) % 3;
                }
                break;
            case 10:
                if (highlight == 0) {
                    if(game->player->acientKey <= 0){
                        mvwprintw(password_win, 5, 1, "You don't have any Ancient Keys!");
                    }
                    else{
                        game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_door].is_open = true;
                        game->player->acientKey--;
                        if(rand() % 20 == 0){
                            mvwprintw(password_win, 5, 1, "Aww, bad luck, you broke your key :(");
                            game->player->brokenAcientKey++;
                        }
                        else{
                            wattron(password_win, COLOR_PAIR(12));
                            mvwprintw(password_win, 5, 1, "You used the ancient key!");
                            wattroff(password_win, COLOR_PAIR(12));
                            if(game->currentLevel == 6){
                                game->levels[game->currentLevel]->rooms[0]->enemyLeft = 0;
                            }
                            wrefresh(password_win);
                            music_getch(password_win);
                            wclear(password_win);
                            wrefresh(password_win);
                            delwin(password_win);
                            // endwin();
                            last_attemps = 0;
                            return;
                        }
                    }
                    wrefresh(password_win);
                    music_getch(password_win);
                    break;
                } else if (highlight == 1 && game->currentLevel != 6) {
                    if(game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_door].type == 'l'){
                        while (attempts < 3) {
                            werase(password_win);
                            // wattron(password_win, COLOR_PAIR(color_pair_num));
                            wattron(password_win, COLOR_PAIR(color));
                            box(password_win, 0, 0);
                            // wattroff(password_win, COLOR_PAIR(color_pair_num));
                            mvwprintw(password_win, 1, 1, "Enter Password: ");
                            wrefresh(password_win);
                            echo();
                            char input_str[20];
                            mvwgetnstr(password_win, 2, 1, input_str, 20);
                            noecho();
                            input = atoi(input_str);
                            // mvwgetnstr(password_win, 2, 1, (char*) &input, 20);

                            if (input == correct_password || input == 1111) {
                                wattron(password_win, COLOR_PAIR(7));
                                mvwprintw(password_win, 3, 1, "Access Granted!");
                                wattroff(password_win, COLOR_PAIR(7));
                                game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_door].is_open = true;
                                wrefresh(password_win);
                                music_getch(password_win);
                                wclear(password_win);
                                wrefresh(password_win);
                                delwin(password_win);
                                // endwin();
                                // game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_attemps = 0;
                                last_attemps = 0;
                                return;
                            } else {
                                attempts++;
                                if(attempts == 1){
                                    color = 8;
                                }
                                else if(attempts == 2){
                                    color = 14;
                                }
                                else if(attempts == 3){
                                    color = 6;
                                    break;
                                }
                                last_attemps = attempts;
                                wattron(password_win, COLOR_PAIR(color));
                                mvwprintw(password_win, 3, 1, "Incorrect Password. Try again.");
                                mvwprintw(password_win, 4, 1, "It might be reversed!");
                                wattroff(password_win, COLOR_PAIR(color));
                                // music_getch(password_win);
                                break;
                            }
                        }

                        if (attempts == 3) {
                            last_attemps = 3;
                            wattron(password_win, COLOR_PAIR(6));
                            mvwprintw(password_win, 3, 1, "Security Mode Activated. Access Denied!");
                            wattroff(password_win, COLOR_PAIR(6));
                        }
                        
                        wrefresh(password_win);
                        music_getch(password_win);
                        break;
                    }
                    else{
                        while (attempts < 3) {
                            werase(password_win);
                            // wattron(password_win, COLOR_PAIR(color_pair_num));
                            wattron(password_win, COLOR_PAIR(color));
                            box(password_win, 0, 0);
                            // wattroff(password_win, COLOR_PAIR(color_pair_num));
                            mvwprintw(password_win, 1, 1, "Enter Password: ");
                            mvwprintw(password_win, 2, 1, "Enter Password: ");
                            wrefresh(password_win);
                            echo();
                            char input_str[20];
                            mvwgetnstr(password_win, 1, 17, input_str, 20);
                            char input2_str[20];
                            mvwgetnstr(password_win, 2, 17, input2_str, 20);
                            noecho();
                            input = atoi(input_str);
                            input2 = atoi(input2_str);
                            // mvwgetnstr(password_win, 2, 1, (char*) &input, 20);

                            if ((input == correct_password && input2 == correct_password2) || input == 1111) {
                                wattron(password_win, COLOR_PAIR(7));
                                mvwprintw(password_win, 3, 1, "Access Granted!");
                                wattroff(password_win, COLOR_PAIR(7));
                                game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_door].is_open = true;
                                wrefresh(password_win);
                                music_getch(password_win);
                                wclear(password_win);
                                wrefresh(password_win);
                                delwin(password_win);
                                // endwin();
                                last_attemps = 0;
                                return;
                            } else {
                                attempts++;
                                if(attempts == 1){
                                    color = 8;
                                }
                                else if(attempts == 2){
                                    color = 14;
                                }
                                else if(attempts == 3){
                                    color = 6;
                                    break;
                                }
                                last_attemps = attempts;
                                wattron(password_win, COLOR_PAIR(color));
                                mvwprintw(password_win, 3, 1, "Incorrect Password. Try again.");
                                wattroff(password_win, COLOR_PAIR(color));
                                // music_getch(password_win);
                                break;
                            }
                        }
                        if (attempts == 3) {
                            last_attemps = 3;
                            wattron(password_win, COLOR_PAIR(6));
                            mvwprintw(password_win, 3, 1, "Security Mode Activated. Access Denied!");
                            wattroff(password_win, COLOR_PAIR(6));
                        }
                        wrefresh(password_win);
                        music_getch(password_win);
                        break;
                    }
                }
                else if (highlight == 2 || (highlight == 1 && game->currentLevel == 6)) {
                    wclear(password_win);
                    wrefresh(password_win);
                    delwin(password_win);
                    // endwin();
                    // game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_attemps = 1;
                    last_attemps = attempts;
                    return;
                }
                break;
        }
        wattroff(password_win, COLOR_PAIR(color));
    }
    // game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_attemps = 1;
    last_attemps = attempts;
    wclear(password_win);
    wrefresh(password_win);
    delwin(password_win);
    // endwin();
    printf("\033[?1003l\n");
    printf("\033[?1003h\n");
}
void enemy_find(int y, int x, WINDOW *game_win){
    if(is_enemy(y, x)){
        for(int i = 0; i < game->levels[game->currentLevel]->rooms[game->player->room_num]->enemyCount; i++){
            if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.x == x 
            && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.y == y
            && (game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].isAlive == true || is_last_room)){
                is_enemy_or_not = game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].type;
                // if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].isVisible == false){
                    draw_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], game->player->room_num);
                    game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].isVisible = true;
                // }
                if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].type == 's'){
                    if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves != 1 || is_last_room){
                        game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves = 0;
                    }
                    // enemy_move(&game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i]);
                    // draw_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], i);
                }
                else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].type == 'g'){
                    if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves < 5
                    && (game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves != 1 || is_last_room)){
                        game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves++;
                        // enemy_move(&game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i]);
                        // draw_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], i);
                    }
                    else{
                        game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves = 0;
                    }
                }
                else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].type == 'u'){
                    if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves < 5 
                    && (game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves != 1 || is_last_room)){
                        game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves = 1;
                        // enemy_move(&game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i]);
                        // draw_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], i);
                    }
                    else{
                        game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves = 0;
                    }
                }
            }
        }
    }
}
void after_find_enemy(WINDOW *game_win){
    for(int i = 0; i < game->levels[game->currentLevel]->rooms[game->player->room_num]->enemyCount; i++){
        Enemy *current=&game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i];
        if(current->type=='i'&&(!is_last_room||!current->isAlive))continue;
        if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].type == 's'
        && (game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves == 0
        || game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves == 1)
        && (game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].isAlive == true || is_last_room)){
            if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves == 1 || is_last_room){
                enemy_move(&game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i], game_win);
                // draw_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], i);
            }
            else{
                game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves = 1;
            }
        }
        else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].type == 'i'){
            boss_encounter_tick(current, game_win);
            draw_last_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], i);
        }
        else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].type == 'g'){
            if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves <= 7 
            && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves > 0){
                game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves++;
                enemy_move(&game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i], game_win);
                // draw_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], i);
            }
            // else{
            //     game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves = 0;
            // }
        }
        else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].type == 'u'){
            if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves <= 5 
            && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves > 0){
                game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves++;
                enemy_move(&game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i], game_win);
                // draw_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], i);
            }
            // else{
            //     game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].moves = 0;
            // }
        }
        if(((game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.x == game->player->cord.x + 1
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.y == game->player->cord.y)
        || (game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.x == game->player->cord.x - 1
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.y == game->player->cord.y)
        || (game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.x == game->player->cord.x
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.y == game->player->cord.y - 1)
        || (game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.x == game->player->cord.x
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.y == game->player->cord.y + 1))
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].type == 'i'){
            if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].health > 75){
                int message = rand() % 4;
                int damage = 10;
                mvwprintw(game_win, 2, 82, "                                                                                                  ");
                is_enemy_or_not = 0;
                wattron(game_win, COLOR_PAIR(17));
                if(message == 0){
                    mvwprintw(game_win, 2, 82, "  Hey, you're near the Immortal and damaged by %d :<", damage);
                }
                else if(message == 1){
                    mvwprintw(game_win, 2, 82, "  Keep your distance! You've been damaged for %d✦", damage);
                }
                else if(message == 2){
                    mvwprintw(game_win, 2, 82, "  Oh crap, Immortal Monster damaged you by it zone for %d D=", damage);
                }
                else if(message == 3){
                    mvwprintw(game_win, 2, 82, "  Watch it! Final Monster hit %d damage to you with it's shield.", damage);
                }
                if(game->player->shield > 0){
                    if(game->player->shield - damage <= 0){
                        game->player->shield = 0;
                        mvwprintw(game_win, 3, 82, "                                                                                                ");
                        mvwprintw(game_win, 3, 82, "Your shield broke! From now on, you will get health damage.");
                    }
                    else{
                        game->player->shield -= damage;
                    }
                }
                else{
                    game->player->health -= damage;
                }
                wattroff(game_win, COLOR_PAIR(17));
            }
        }
        else if((game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.x == game->player->cord.x
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.y == game->player->cord.y
        || (game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.x == game->player->cord.x + 1
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.y == game->player->cord.y)
        || (game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.x == game->player->cord.x - 1
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.y == game->player->cord.y)
        || (game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.x == game->player->cord.x
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.y == game->player->cord.y - 1)
        || (game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.x == game->player->cord.x
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.y == game->player->cord.y + 1))
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].isAlive == true){
            // if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].isVisible == false){
                draw_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], game->player->room_num);
                game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].isVisible = true;
            // }
            int message = rand() % 4;
            int damage = game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].damage * ((difficulty_efficiency + 2) / 3);
            mvwprintw(game_win, 2, 82, "                                                                                                  ");
            is_enemy_or_not = 0;
            wattron(game_win, COLOR_PAIR(14));
            if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].type == 'i'){
                damage = 25;
            }
            if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].health <= 75 
            && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].type == 'i'){
                damage = 30;
            }
            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                Mix_PlayMusic(damage_effect, 1);
            }
            if(first_damage == 0){
                first_damage = 1;
            }
            if(message == 0){
                mvwprintw(game_win, 2, 82, "  Ouch, you've been hit by %s and recieved %d damage!", 
                game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].name, damage);
            }
            else if(message == 1){
                mvwprintw(game_win, 2, 82, "  Ah shit, you've been hit by %s and recieved %d damage♜",
                game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].name, damage);
            }
            else if(message == 2){
                mvwprintw(game_win, 2, 82, "  Damn it, you've been hit by %s and recieved %d damage D=", 
                game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].name, damage);
            }
            else if(message == 3){
                mvwprintw(game_win, 2, 82, "  Watch it! you've been hit by %s and recieved %d damage :<", 
                game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].name, damage);
            }
            if(game->player->shield > 0){
                if(game->player->shield - damage <= 0){
                    if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                        Mix_PlayMusic(shieldbreak_effect, 1);
                    }
                    game->player->shield = 0;
                    mvwprintw(game_win, 3, 82, "                                                                                                ");
                    mvwprintw(game_win, 3, 82, "Your shield broke! From now on, you will get health damage.");
                }
                else{
                    game->player->shield -= damage;
                }
            }
            else{
                game->player->health -= damage;
            }
            wattroff(game_win, COLOR_PAIR(14));
        }
        else if(((game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.x == game->player->cord.x + 1
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.y == game->player->cord.y + 1)
        || (game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.x == game->player->cord.x - 1
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.y == game->player->cord.y - 1)
        || (game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.x == game->player->cord.x + 1
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.y == game->player->cord.y - 1)
        || (game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.x == game->player->cord.x + 1
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.y == game->player->cord.y + 1))
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].type == 'i'
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].health > 75){
            int message = rand() % 4;
            int damage = 5;
            mvwprintw(game_win, 2, 82, "                                                                                                  ");
            is_enemy_or_not = 0;
            wattron(game_win, COLOR_PAIR(17));
            if(message == 0){
                mvwprintw(game_win, 2, 82, "  Hey, you're near the Immortal and damaged by %d :<", damage);
            }
            else if(message == 1){
                mvwprintw(game_win, 2, 82, "  Keep your distance! You've been damaged for %d✦", damage);
            }
            else if(message == 2){
                mvwprintw(game_win, 2, 82, "  Oh crap, Immortal Monster damaged you by it zone for %d D=", damage);
            }
            else if(message == 3){
                mvwprintw(game_win, 2, 82, "  Watch it! Final Monster hit %d damage to you with it's shield.", damage);
            }
            if(game->player->shield > 0){
                if(game->player->shield - damage <= 0){
                    game->player->shield = 0;
                    mvwprintw(game_win, 3, 82, "                                                                                                ");
                    mvwprintw(game_win, 3, 82, "Your shield broke by IMMORTAL! From now on, you will get health damage.");
                }
                else{
                    game->player->shield -= damage;
                }
            }
            else{
                game->player->health -= damage;
            }
            wattroff(game_win, COLOR_PAIR(17));
        }
    }
}
void after_move(WINDOW *game_win){
    if(escape_check||run_finished)return;
    int map_width, map_height;
    getmaxyx(stdscr, map_height, map_width);
    if (map_height > MAP_ROWS) map_height = MAP_ROWS;
    if (map_width > MAP_COLS) map_width = MAP_COLS;
    int start_x = BORDER_PADDING;
    int start_y = BORDER_PADDING;
    int end_x = map_width * 3 / 4 - BORDER_PADDING;
    int end_y = map_height * 3 / 4 - BORDER_PADDING + 5;
    wattron(game_win, COLOR_PAIR(color_pair_num));
    if(g_check == 0){
        if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '%' && game->player->foodCount + 15 <= 150){
            for(int j = 0; j < game->levels[game->currentLevel]->rooms[game->player->room_num]->foodCount; j++){
                if(game->levels[game->currentLevel]->rooms[game->player->room_num]->foods[j].cord.x == game->player->cord.x 
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->foods[j].cord.y == game->player->cord.y
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->foods[j].isUsed != true){
                    game->levels[game->currentLevel]->rooms[game->player->room_num]->foods[j].isUsed = true;
                    if(difficulty_efficiency >= 2 && rand() % 6 == 5){
                        poisonous_food_check = 1;
                        // for(int j = 0; j < game->player->foodCount; j++){
                        //     mvwprintw(game_win, map_height - 4 + j / 10, 5 + 2*j - 2*(j/10)*10, "♣");
                        // }
                        mvwprintw(game_win, 3, 5, "                                                                                ");
                        mvwprintw(game_win, 3, 5, "Ops! Poisonous food O_o");
                        game->player->health -= 5;
                    }
                    else{
                        if(first_food == 0){
                            first_food = 1;
                        }
                        mvwprintw(game_win, 3, 5, "                                                                            ");
                        mvwprintw(game_win, 3, 5, "Yummy, you've collected %d amount of food :3", game->levels[game->currentLevel]->rooms[game->player->room_num]->foods[j].health);
                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                            Mix_PlayMusic(eating_effect, 1);
                        }
                        game->player->foodCount += game->levels[game->currentLevel]->rooms[game->player->room_num]->foods[j].health;
                    }
                    // count_move = 1;
                    game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item = '.';
                }
            }
        }
        if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '!' && game->player->shield < 100){
            for(int j = 0; j < game->levels[game->currentLevel]->rooms[game->player->room_num]->shieldCount; j++){
                if((game->levels[game->currentLevel]->rooms[game->player->room_num]->shields[j].cord.x == game->player->cord.x
                || game->levels[game->currentLevel]->rooms[game->player->room_num]->shields[j].cord.x + 1 == game->player->cord.x)
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->shields[j].cord.y == game->player->cord.y
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->shields[j].isUsed != true){
                    game->levels[game->currentLevel]->rooms[game->player->room_num]->shields[j].isUsed = true;
                    if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                        Mix_PlayMusic(shieldpickup_effect, 1);
                    }
                    if(first_shield == 0){
                        first_shield = 1;
                    }
                    mvwprintw(game_win, 3, 5, "                                                                            ");
                    mvwprintw(game_win, 3, 5, "Great, you've collected %d amount of shield !^-^!", game->levels[game->currentLevel]->rooms[game->player->room_num]->shields[j].amount);
                    game->player->shield += game->levels[game->currentLevel]->rooms[game->player->room_num]->shields[j].amount;
                    game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item = '.';
                    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '!'){
                        game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item = '.';
                        mvwprintw(game_win, game->player->cord.y, game->player->cord.x + 1, ".");
                    }
                    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '!'){
                        game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item = '.';
                        mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, ".");
                    }
                }
            }
        }
        else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'A'){
            game->levels[game->currentLevel]->rooms[game->player->room_num]->keyCount = 0;
            game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item = '.';
            game->player->acientKey++;
            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                Mix_PlayMusic(forge_effect, 1);
            }
            if(first_ancientkey == 0){
                first_ancientkey = 1;
            }
            mvwprintw(game_win, 3, 5, "                                                                         ");
            mvwprintw(game_win, 3, 5, "Nice! You've collected an Ancient Key Δ");
        }
        else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '$' 
        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '*'){
            for(int j = 0; j < game->levels[game->currentLevel]->rooms[game->player->room_num]->goldCount; j++){
                if((game->levels[game->currentLevel]->rooms[game->player->room_num]->golds[j].cord.x == game->player->cord.x 
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->golds[j].cord.y == game->player->cord.y)
                || (game->levels[game->currentLevel]->rooms[game->player->room_num]->golds[j].cord.x + 1 == game->player->cord.x 
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->golds[j].cord.y == game->player->cord.y)
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->golds[j].isUsed != true){
                    game->levels[game->currentLevel]->rooms[game->player->room_num]->golds[j].isUsed = true;
                    game->player->gold += game->levels[game->currentLevel]->rooms[game->player->room_num]->golds[j].count * difficulty_efficiency;
                    game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item = '.';
                    if(first_gold == 0){
                        first_gold = 1;
                    }
                    mvwprintw(game_win, 3, 5, "                                                                           ");
                    if(game->levels[game->currentLevel]->rooms[game->player->room_num]->golds[j].type == 'b'){
                        // if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '*'){
                        //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x + 1, ".");
                        // }
                        // else{
                        //     mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, ".");
                        // }
                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                            Mix_PlayMusic(black_coin_effect, 1);
                        }
                        mvwprintw(game_win, 3, 5, "Wow! You've collected a black gold with amount of %d ¤",
                        game->levels[game->currentLevel]->rooms[game->player->room_num]->golds[j].count * difficulty_efficiency);
                        wattroff(game_win, COLOR_PAIR(color_pair_num));
                        if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '*'){
                            game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item = '.';
                            mvwprintw(game_win, game->player->cord.y, game->player->cord.x + 1, ".");
                        }
                        else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '*'){
                            game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item = '.';
                            mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, ".");
                        }
                        wattron(game_win, COLOR_PAIR(color_pair_num));
                    }
                    else{
                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                            Mix_PlayMusic(coin_effect, 1);
                        }
                        mvwprintw(game_win, 3, 5, "You've collected a coin with amount of %d gold!", 
                        game->levels[game->currentLevel]->rooms[game->player->room_num]->golds[j].count * difficulty_efficiency);
                    }
                }
            }
        }
        else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'E' 
        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'P'
        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'H'){
            for(int j = 0; j < game->levels[game->currentLevel]->rooms[game->player->room_num]->spellCount; j++){
                if((game->levels[game->currentLevel]->rooms[game->player->room_num]->spells[j].cord.x == game->player->cord.x 
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->spells[j].cord.y == game->player->cord.y)
                || (game->levels[game->currentLevel]->rooms[game->player->room_num]->spells[j].cord.x + 1 == game->player->cord.x 
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->spells[j].cord.y == game->player->cord.y)
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->spells[j].isUsed != true){
                    game->levels[game->currentLevel]->rooms[game->player->room_num]->spells[j].isUsed = true;
                    if(game->levels[game->currentLevel]->rooms[game->player->room_num]->spells[j].type == 's'){
                        game->player->spells[0].count++;
                    }
                    else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->spells[j].type == 'p'){
                        game->player->spells[1].count++;
                    }
                    else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->spells[j].type == 'h'){
                        game->player->spells[2].count++;
                    }
                    // strcpy(game->player->spells[game->player->spellCount].name, game->levels[game->currentLevel]->rooms[game->player->room_num]->spells[j].name);
                    game->player->spellCount++;
                    game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item = '.';
                    if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                        Mix_PlayMusic(glass_effect, 1);
                    }
                    if(first_spell == 0){
                        first_spell = 1;
                    }
                    mvwprintw(game_win, 3, 5, "                                                                              ");
                    mvwprintw(game_win, 3, 5, "You've collected a %s potion!", game->levels[game->currentLevel]->rooms[game->player->room_num]->spells[j].name);
                    wattroff(game_win, COLOR_PAIR(color_pair_num));
                    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == 'E'
                    || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == 'P'
                    || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == 'H'){
                        game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item = '.';
                        mvwprintw(game_win, game->player->cord.y, game->player->cord.x + 1, ".");
                    }
                    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == 'E'
                    || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == 'P'
                    || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == 'H'){
                        game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item = '.';
                        mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, ".");
                    }
                    wattron(game_win, COLOR_PAIR(color_pair_num));
                }
            }
        }
        else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '6' 
        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '7'
        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '8'
        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '9'){
            for(int j = 0; j < game->levels[game->currentLevel]->rooms[game->player->room_num]->specialfoodCount; j++){
                if((game->levels[game->currentLevel]->rooms[game->player->room_num]->specialfoods[j].cord.x == game->player->cord.x 
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->specialfoods[j].cord.y == game->player->cord.y)
                || (game->levels[game->currentLevel]->rooms[game->player->room_num]->specialfoods[j].cord.x + 1 == game->player->cord.x 
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->specialfoods[j].cord.y == game->player->cord.y)
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->specialfoods[j].isUsed != true){
                    game->levels[game->currentLevel]->rooms[game->player->room_num]->specialfoods[j].isUsed = true;
                    if(game->levels[game->currentLevel]->rooms[game->player->room_num]->specialfoods[j].type == 'n'){
                        game->player->specialfoods[0].count++;
                    }
                    else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->specialfoods[j].type == 'p'){
                        game->player->specialfoods[1].count++;
                    }
                    else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->specialfoods[j].type == 's'){
                        game->player->specialfoods[2].count++;
                    }
                    else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->specialfoods[j].type == 'r'){
                        game->player->specialfoods[3].count++;
                    }
                    game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item = '.';
                    if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                        Mix_PlayMusic(eating_effect, 1);
                    }
                    if(first_specialfood == 0){
                        first_specialfood = 1;
                    }
                    mvwprintw(game_win, 3, 5, "                                                                              ");
                    if(game->levels[game->currentLevel]->rooms[game->player->room_num]->specialfoods[j].type == 'n'){
                        mvwprintw(game_win, 3, 5, "You've collected a Special food!");
                    }
                    else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->specialfoods[j].type == 'p'){
                        mvwprintw(game_win, 3, 5, "You've collected a Power food!");
                    }
                    else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->specialfoods[j].type == 's'){
                        mvwprintw(game_win, 3, 5, "You've collected a Speed food!");
                    }
                    else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->specialfoods[j].type == 'r'){
                        mvwprintw(game_win, 3, 5, "You've collected a Rotten food :(");
                        game->player->health -= 5;
                    }
                    wattroff(game_win, COLOR_PAIR(color_pair_num));
                    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '6'
                    || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '7'
                    || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '8'
                    || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '9'){
                        game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item = '.';
                        mvwprintw(game_win, game->player->cord.y, game->player->cord.x + 1, ".");
                    }
                    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '6'
                    || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '7'
                    || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '8'
                    || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '9'){
                        game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item = '.';
                        mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, ".");
                    }
                    wattron(game_win, COLOR_PAIR(color_pair_num));
                }
            }
        }
        else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '1' 
        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '2'
        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '3'
        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '4'
        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '5'){
            for(int j = 0; j < game->levels[game->currentLevel]->rooms[game->player->room_num]->weaponCount; j++){
                if((game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].cord.x == game->player->cord.x 
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].cord.y == game->player->cord.y)
                || (game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].cord.x + 1 == game->player->cord.x 
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].cord.y == game->player->cord.y)
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].isUsed != true){
                    // game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].isUsed = true;
                    // if(game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].type == 'm'){
                    //     game->player->weapons[0].count++;
                    // }
                    if(first_weapon == 0){
                        first_weapon = 1;
                    }
                    if(game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].type == 'd'){
                        int num = 25 - game->player->weapons[1].count;
                        if(game->player->weapons[1].count >= 25){
                            mvwprintw(game_win, 3, 5, "                                                                           ");
                            mvwprintw(game_win, 3, 5, "You can't have more than 25 daggers!");
                        }
                        else if(num < 10){
                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                Mix_PlayMusic(weaponpickup_effect, 1);
                            }
                            mvwprintw(game_win, 3, 5, "                                                   ");
                            mvwprintw(game_win, 3, 5, "You've collected %d Daggers!", num);
                            game->player->weapons[1].count += num;
                            game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count -= num;
                            game->player->weaponCount += num;
                        }
                        else if(num >= 10){
                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                Mix_PlayMusic(weaponpickup_effect, 1);
                            }
                            mvwprintw(game_win, 3, 5, "                                                   ");
                            mvwprintw(game_win, 3, 5, "You've collected %d Daggers!", game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count);
                            game->player->weapons[1].count += game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count;
                            game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count = 0;
                            game->player->weaponCount += game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count;
                        }
                    }
                    else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].type == 'w'){
                        int num = 20 - game->player->weapons[2].count;
                        if(game->player->weapons[2].count >= 20){
                            mvwprintw(game_win, 3, 5, "                                                                               ");
                            mvwprintw(game_win, 3, 5, "You can't have more than 20 Magic Wands!");
                            draw_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], game->player->room_num);
                        }
                        else if(num < 8){
                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                Mix_PlayMusic(weaponpickup_effect, 1);
                            }
                            mvwprintw(game_win, 3, 5, "                                                   ");
                            mvwprintw(game_win, 3, 5, "You've collected %d Magic Wands!", num);
                            game->player->weapons[2].count += num;
                            game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count -= num;
                            game->player->weaponCount += num;
                        }
                        else if(num >= 8){
                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                Mix_PlayMusic(weaponpickup_effect, 1);
                            }
                            mvwprintw(game_win, 3, 5, "                                                   ");
                            mvwprintw(game_win, 3, 5, "You've collected %d Magic Wands!", game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count);
                            game->player->weapons[2].count += game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count;
                            game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count = 0;
                            game->player->weaponCount += game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count;
                        }
                    }
                    else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].type == 'a'){
                        int num = 50 - game->player->weapons[3].count;
                        if(game->player->weapons[3].count >= 50){
                            mvwprintw(game_win, 3, 5, "                                                                               ");
                            mvwprintw(game_win, 3, 5, "You can't have more than 50 Arrows!");
                        }
                        else if(num < 20){
                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                Mix_PlayMusic(weaponpickup_effect, 1);
                            }
                            mvwprintw(game_win, 3, 5, "                                                   ");
                            mvwprintw(game_win, 3, 5, "You've collected %d Arrows!", num);
                            game->player->weapons[3].count += num;
                            game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count -= num;
                            game->player->weaponCount += num;
                        }
                        else if(num >= 20){
                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                Mix_PlayMusic(weaponpickup_effect, 1);
                            }
                            mvwprintw(game_win, 3, 5, "                                                   ");
                            mvwprintw(game_win, 3, 5, "You've collected %d Arrows!", game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count);
                            game->player->weapons[3].count += game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count;
                            game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count = 0;
                            game->player->weaponCount += game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count;
                        }
                    }
                    else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].type == 's'){
                        if(game->player->weapons[4].count >= 5){
                            mvwprintw(game_win, 3, 5, "                                                                               ");
                            mvwprintw(game_win, 3, 5, "You can't have more than 5 sharpness on your Sword!");
                        }
                        else{
                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                Mix_PlayMusic(weaponpickup_effect, 1);
                            }
                            mvwprintw(game_win, 3, 5, "                                                   ");
                            game->player->weapons[4].count++;
                            mvwprintw(game_win, 3, 5, "You've upgraded your Sword to sharpness %d!", game->player->weapons[4].count);
                            game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count = 0;
                            game->player->weaponCount += 1;
                        }
                    }
                    // game->player->weaponCount++;
                    // mvwprintw(game_win, 3, 5, "                                                   ");
                    // mvwprintw(game_win, 3, 5, "You've collected a %s!", game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].name);
                    wattroff(game_win, COLOR_PAIR(color_pair_num));
                    if(game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].count <= 0){
                        if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '1' 
                        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '2'
                        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '3'
                        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '4'
                        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '5'){
                            game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item = '.';
                            mvwprintw(game_win, game->player->cord.y, game->player->cord.x + 1, ".");
                        }
                        else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '1' 
                        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '2'
                        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '3'
                        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '4'
                        || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '5'){
                            game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item = '.';
                            mvwprintw(game_win, game->player->cord.y, game->player->cord.x - 1, ".");
                        }
                        game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item = '.';
                        game->levels[game->currentLevel]->rooms[game->player->room_num]->weapons[j].isUsed = true;
                    }
                    wattron(game_win, COLOR_PAIR(color_pair_num));
                }
            }
        }
    }
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '^'){
        for(int j = 0; j < game->levels[game->currentLevel]->rooms[game->player->room_num]->trapCount; j++){
            if(game->levels[game->currentLevel]->rooms[game->player->room_num]->traps[j].cord.x == game->player->cord.x 
            && game->levels[game->currentLevel]->rooms[game->player->room_num]->traps[j].cord.y == game->player->cord.y
            && game->levels[game->currentLevel]->rooms[game->player->room_num]->traps[j].isVisible != true){
                game->levels[game->currentLevel]->rooms[game->player->room_num]->traps[j].isVisible = true;
                int damage = 5 * (rand() % (difficulty_efficiency + 1) + 1);
                if(rand() % 4 == 0 && game->currentLevel != game->levelCount - 1){
                    if(game->currentLevel != 6){
                        x_save = game->player->cord.x;
                        y_save = game->player->cord.y;
                        level_save = game->currentLevel;
                        room_num_save = game->player->room_num;
                        game->currentLevel = 6;
                        is_battle = 1;
                        back_front = 1;
                    }
                    return;
                }
                else{
                    game->player->health -= damage;
                }
                if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                    Mix_PlayMusic(damage_effect, 1);
                }
                if(first_trap == 0){
                    first_trap = 1;
                }
                mvwprintw(game_win, 3, 5, "                                                ");
                mvwprintw(game_win, 3, 5, "Ah, you've been hit by a trap and recieved %d damage :_(", damage);
            }
        }
    }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '&'){
        int password;
        int password_copy;
        while(1){
            password = rand() % 10000;
            password_copy = password;
            if(rand() % 3 && game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_door].type != 'h'){
                int a1 = password % 10;
                password /= 10;
                int a2 = password % 10;
                password /= 10;
                int a3 = password % 10;
                password /= 10;
                int a4 = password % 10;
                password /= 10;
                password = a1 * 1000 + a2 * 100 + a3 * 10 + a4 * 1;
                // password = 1313;
            }
            if(password >= 1000 && password_copy >= 1000){
                break;
            }      
        }
        if(first_password == 0){
            first_password = 1;
        }
        if(game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_door].type == 'h'){
            if(game->levels[game->currentLevel]->rooms[game->player->room_num]->password_generator.x == game->player->cord.x
            && game->levels[game->currentLevel]->rooms[game->player->room_num]->password_generator.y == game->player->cord.y){
                game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_door].password = password;
                mvwprintw(game_win, 2, 5, "                                                                                  ");
                mvwprintw(game_win, 2, 5, "Lock password 1 is %d ⚿", password_copy);
            }
            else{
                game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_door].password2 = password;
                mvwprintw(game_win, 2, 5, "                                                                                  ");
                mvwprintw(game_win, 2, 5, "Lock password 2 is %d ⚿", password_copy);
            }
        }
        else{
            game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_door].password = password;
            mvwprintw(game_win, 2, 5, "                                                                                  ");
            mvwprintw(game_win, 2, 5, "Lock password is %d ⚿", password_copy);
        }
        is_pass_lock = 1;
        password_disapear = 0;
        time(&move_time_lock);
    }
    if(move_or_not){
        if(game->player->Health_effect > 0){
            game->player->Health_effect--;
        }
        if(game->player->Speed_effect > 0){
            game->player->Speed_effect--;
        }
        if(game->player->Power_effect > 0){
            game->player->Power_effect--;
        }
        if(game->player->Speed_effect % 2 == 0){
            // enemy_find(game->player->cord.y, game->player->cord.x, game_win);
            enemy_find(game->player->cord.y, game->player->cord.x + 1, game_win);
            enemy_find(game->player->cord.y, game->player->cord.x - 1, game_win);
            enemy_find(game->player->cord.y + 1, game->player->cord.x, game_win);
            enemy_find(game->player->cord.y - 1, game->player->cord.x, game_win);
            enemy_find(game->player->cord.y + 1, game->player->cord.x + 1, game_win);
            enemy_find(game->player->cord.y + 1, game->player->cord.x - 1, game_win);
            enemy_find(game->player->cord.y - 1, game->player->cord.x + 1, game_win);
            enemy_find(game->player->cord.y - 1, game->player->cord.x - 1, game_win);
            after_find_enemy(game_win);
        }
    }
    { //Enemy Alert
        if(is_enemy_or_not){
            if(first_enemy_near == 0){
                first_enemy_near = 1;
            }
            wattron(game_win, COLOR_PAIR(14));
            mvwprintw(game_win, 2, 82, "                                                                                       ");
            mvwprintw(game_win, 2, 82, "  Hey, there is an enemy near you!");
            wattroff(game_win, COLOR_PAIR(14));
        }
    // if(is_enemy_or_not){
    //     mvwprintw(game_win, 2, 82, "                                                                                       ");
    //     is_enemy_or_not2 = 1 - is_enemy_or_not2;
    // }
    // if(is_enemy_or_not && is_enemy_or_not2 == 0){
    //     wattron(game_win, COLOR_PAIR(14));
    //     mvwprintw(game_win, 2, 82, "                                                                                       ");
    //     mvwprintw(game_win, 2, 82, "Hey, there is an enemy near you!");
    //     wattroff(game_win, COLOR_PAIR(14));
    //     // is_enemy_or_not2 = 1 - is_enemy_or_not2;
    // }
    }
    wattroff(game_win, COLOR_PAIR(3));
    wattroff(game_win, COLOR_PAIR(color_pair_num));
    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '+' 
    || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '?'
    || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'L'){
        for(int i = 0; i < game->levels[game->currentLevel]->roomsCount; i++){
            if(((game->player->cord.x == game->levels[game->currentLevel]->rooms[i]->x
            || game->player->cord.x == game->levels[game->currentLevel]->rooms[i]->x + game->levels[game->currentLevel]->rooms[i]->width - 1)
            &&(game->player->cord.y >= game->levels[game->currentLevel]->rooms[i]->y 
            && game->player->cord.y <= game->levels[game->currentLevel]->rooms[i]->y + game->levels[game->currentLevel]->rooms[i]->height))
            || ((game->player->cord.y == game->levels[game->currentLevel]->rooms[i]->y
            || game->player->cord.y == game->levels[game->currentLevel]->rooms[i]->y + game->levels[game->currentLevel]->rooms[i]->height - 1)
            && (game->player->cord.x >= game->levels[game->currentLevel]->rooms[i]->x 
            && game->player->cord.x <= game->levels[game->currentLevel]->rooms[i]->x + game->levels[game->currentLevel]->rooms[i]->width))){
                if(game->player->room_num != i){
                    // game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_attemps = 0;
                    last_attemps = 0;
                }
                in_spell = 0;
                in_reinforce = 0;
                in_healing = 0;
                in_nightmare = 0;
                game->player->room_num = i;
                lock_color = 7;
                if(game->levels[game->currentLevel]->rooms[i]->doors[game->levels[game->currentLevel]->rooms[i]->lock_door].is_open == false){
                    lock_color = 6;
                }
                // if(game->levels[game->currentLevel]->rooms[i]->isVisible == false || game->player->room_num == total_rooms - 1){
                    game->levels[game->currentLevel]->rooms[i]->isVisible = true;
                    draw_room(game_win, game->levels[game->currentLevel]->rooms[i], i);
                // }
                if(game->levels[game->currentLevel]->rooms[i]->type == 'm'){
                    in_nightmare = 1;
                }
                if(game->levels[game->currentLevel]->rooms[i]->type == 'p'){
                    in_spell = 1;
                }
                if(game->levels[game->currentLevel]->rooms[i]->type == 'r'){
                    in_reinforce = 1;
                }
                if(game->levels[game->currentLevel]->rooms[i]->type == 'r'){
                    in_healing = 1;
                }
                game->levels[game->currentLevel]->rooms[i]->isVisible = true;
                break;
                }
            }
        }
    if(!run_finished && !escape_check && game->currentLevel == game->levelCount - 1 && is_last_room == 0){
        if(game->levels[game->currentLevel]->rooms[0]->enemyLeft <= 0){
            for(int i = 0; i < game->levels[game->currentLevel]->rooms[0]->enemyCount + 1; i++){
                if(game->levels[game->currentLevel]->rooms[0]->enemies[i].type == 'i'){
                    game->levels[game->currentLevel]->rooms[0]->enemies[i].isAlive = true;
                    boss_encounter_begin(&game->levels[game->currentLevel]->rooms[0]->enemies[i]);
                }
            }
            // Mix_PauseMusic();
            game->levels[game->currentLevel]->rooms[0]->enemyCount++;
            is_last_room = 1;
            effects = Mix_LoadMUS("./Base_Sounds/BeforeBattle.mp3");
            Mix_PlayMusic(effects, 1);
            draw_last_room(game_win,game->levels[game->currentLevel]->rooms[0],0);
            int banner_y=LINES-35,banner_x=COLS-60;
            if(banner_y<4)banner_y=4;
            for(int seconds=3;seconds>0;seconds--){
                wattron(game_win,A_BOLD|COLOR_PAIR(12));
                mvwaddstr(game_win,banner_y,banner_x,"╭──────────────────────────────────────────╮");
                mvwprintw(game_win,banner_y+1,banner_x,"│       FINAL BATTLE  ·  begins in %d        │",seconds);
                mvwaddstr(game_win,banner_y+2,banner_x,"╰──────────────────────────────────────────╯");
                wattroff(game_win,A_BOLD|COLOR_PAIR(12));wrefresh(game_win);napms(1000);music_tick();
            }
            for(int y=0;y<3;y++)mvwhline(game_win,banner_y+y,banner_x,' ',44);
            effects = Mix_LoadMUS("./Base_Sounds/Invader.mp3");
            Mix_PlayMusic(effects, 1);
            draw_last_room(game_win, game->levels[game->currentLevel]->rooms[0], 0);
        }
    }
    if(game->currentLevel == 6 && game->levels[game->currentLevel]->rooms[0]->enemyLeft == 0){
        is_battle = 1;
        game->currentLevel = level_save;
        after_battle = 1;
        return;
    }
    if(is_last_room){
        draw_last_room(game_win, game->levels[game->currentLevel]->rooms[0], 0);
    }
    else if(game->currentLevel == game->levelCount - 1 || game->currentLevel == 6){
        draw_room(game_win, game->levels[game->currentLevel]->rooms[0], 0);
    }
}
void enemy_be_attaked(int y, int x, WINDOW *game_win){
    if(escape_check||run_finished)return;
    if(is_enemy(y, x)){
        for(int i = 0; i < game->levels[game->currentLevel]->rooms[game->player->room_num]->enemyCount; i++){
            if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.x == x 
            && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.y == y){
                if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].health - game->player->currentWeapon.damage <= 0
                && game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].isAlive == true){
                    game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].health = 0;
                    game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].isAlive = false;
                    game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].isVisible = false;
                    game->levels[game->currentLevel]->rooms[game->player->room_num]->enemyLeft--;
                    wattron(game_win, COLOR_PAIR(color_pair_num));
                    game->player->points += game->player->currentWeapon.damage;
                    mvwprintw(game_win, 3, 82, "                                                                      ");
                    if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].type == 'i'){
                        print_last_dragon(game_win);
                        mvwprintw(game_win, 3, 82, UI_TEXT("You dealt %d damage and killed the final boss🏆","You dealt %d damage and killed the final boss★"), game->player->currentWeapon.damage);
                        game->player->points += 40000 * difficulty_efficiency;
                        game->player->gold += 1000;
                        draw_last_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], 0);
                        wrefresh(game_win);
                        sleep(2);
                        win_game_screen(start_time);
                        escape_check = 1;
                        return;
                    }
                    else{
                        mvwprintw(game_win, 3, 82, UI_TEXT("You dealt %d damage and killed %s💥","You dealt %d damage and killed %s✧"), game->player->currentWeapon.damage, game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].name);
                    }
                    game->whole_map[game->currentLevel][game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.y]
                    [game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].cord.x].item = '.';
                    wattroff(game_win, COLOR_PAIR(color_pair_num));
                }
                else{
                    if(game->player->currentWeapon.type == 'w'){
                        if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].type != 'i'){
                            game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].canMove = false;
                        }
                    }
                    game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].health -= game->player->currentWeapon.damage;
                    game->player->points += game->player->currentWeapon.damage * 5;
                    wattron(game_win, COLOR_PAIR(color_pair_num));
                    if(first_attack == 0){
                        first_attack = 1;
                    }
                    mvwprintw(game_win, 3, 82, "                                                                          ");
                    mvwprintw(game_win, 3, 82, UI_TEXT("You dealt %d damage to %s🔥","You dealt %d damage to %s✦"), game->player->currentWeapon.damage, game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].name);
                    wattroff(game_win, COLOR_PAIR(color_pair_num));
                }
            }
        }
    }
}
int mouse_attack_event(const MEVENT *event);
static int mouse_aim_x=0,mouse_aim_y=-1;
int ranged_key_vector(int key,int *dx,int *dy){
    *dx=*dy=0;
    switch(key){
    case 'w':case 'W':case KEY_UP:*dy=-1;break;
    case 's':case 'S':case KEY_DOWN:*dy=1;break;
    case 'a':case 'A':case KEY_LEFT:*dx=-1;break;
    case 'd':case 'D':case KEY_RIGHT:*dx=1;break;
    case 'q':case 'Q':case KEY_HOME:*dx=-1;*dy=-1;break;
    case KEY_PPAGE:*dx=1;*dy=-1;break;
    case 'z':case 'Z':case KEY_END:*dx=-1;*dy=1;break;
    case 'c':case 'C':case KEY_NPAGE:*dx=1;*dy=1;break;
    default:return 0;
    }return 1;
}
static int ranged_passable(int y,int x){
    if(!valid_move_check(y,x))return 0;
    int c=game->whole_map[game->currentLevel][y][x].item;
    return c!='+'&&c!='?'&&c!='L';
}
static void fire_ranged(WINDOW *win){
    Player *p=game->player;Room *room=game->levels[game->currentLevel]->rooms[p->room_num];
    int index=p->currentWeapon.type=='d'?1:p->currentWeapon.type=='w'?2:3;
    if(p->currentWeapon.count<=0){mvwaddstr(win,3,82,"Out of ammo. Tab: choose a weapon.       ");move_or_not=0;return;}
    if(game->currentLevel==6){mvwaddstr(win,3,82,"No ranged attack in battle room!                  ");move_or_not=0;return;}
    if(game->whole_map[game->currentLevel][p->cord.y][p->cord.x].item=='#'){
        mvwaddstr(win,3,82,"No attack in corridors!                            ");move_or_not=0;return;
    }
    int dx=0,dy=0,key=direction;
    MEVENT target=mouse_event;
    if(move_click==' '){
        mvwaddstr(win,3,82,index==2?"Wand: arrows shoot (1); WASD ride magic (2). Esc cancels":"Aim: arrows / WASD / diagonal keys / click. Esc: cancel");wrefresh(win);
        for(;;){
            key=music_getch(win);
            if(key==27){move_or_not=0;mvwaddstr(win,3,82,"Aim cancelled.                                         ");return;}
            if(key==KEY_MOUSE){if(getmouse(&target)==OK&&mouse_attack_event(&target))break;}
            else if(ranged_key_vector(key,&dx,&dy))break;
        }
    }else if(move_click==KEY_MOUSE)key=KEY_MOUSE;
    if(key==KEY_MOUSE){
        dx=target.x-p->cord.x;dy=target.y-p->cord.y;
        if(!dx&&!dy){move_or_not=0;return;}
        mouse_aim_x=dx;mouse_aim_y=dy;direction=0;
    }else if(key==0){dx=mouse_aim_x;dy=mouse_aim_y;}
    else if(!ranged_key_vector(key,&dx,&dy)){move_or_not=0;return;}
    else direction=key;
    /* Letter aiming carries the wizard with the spell; arrow/mouse aiming stays ranged. */
    int ride=index==2&&((key>='a'&&key<='z')||(key>='A'&&key<='Z'));
    int cost=ride?2:1;
    if(p->currentWeapon.count<cost){
        mvwaddstr(win,3,82,"Magic ride needs 2 charges. Use arrows to shoot for 1. ");move_or_not=0;return;
    }
    int range=index==2?10:5,damage=index==1?12:index==2?15:5;
    if(p->Power_effect>0)damage*=2;
    p->currentWeapon.damage=damage;p->currentWeapon.range=range;p->currentWeapon.move=0;
    int x=p->cord.x,y=p->cord.y,ax=abs(dx),ay=abs(dy),sx=(dx>0)-(dx<0),sy=(dy>0)-(dy<0),err=ax-ay;
    int hit=0,travelled=0;
    /* Check the first step before spending a charge. */
    for(int step=0;step<range;step++){
        int nx=x,ny=y,e2=2*err;
        if(e2>-ay){err-=ay;nx+=sx;}if(e2<ax){err+=ax;ny+=sy;}
        if(!ranged_passable(ny,nx)||(nx!=x&&ny!=y&&(!ranged_passable(y,nx)||!ranged_passable(ny,x))))break;
        if(!travelled){
            p->currentWeapon.count-=cost;
            if(p->weapons[index].type==p->currentWeapon.type)p->weapons[index].count=p->currentWeapon.count;
            if(!strcmp(selected_music,"./Base_Sounds/silent.wav")&&game->currentLevel!=game->levelCount-1){
                Mix_Music *effect=index==1?dagger_effect:index==2?wand_effect:arrow_effect;
                if(effect)Mix_PlayMusic(effect,0);
            }
        }
        x=nx;y=ny;travelled++;p->currentWeapon.move=travelled;
        if(is_enemy(y,x)){enemy_be_attaked(y,x,win);if(escape_check||run_finished)return;hit=1;break;}
        if(ride)p->cord=(Point){x,y};
        wattron(win,A_BOLD|COLOR_PAIR(12));mvwaddstr(win,y,x,index==2?"✦":"•");wattroff(win,A_BOLD|COLOR_PAIR(12));
        wrefresh(win);napms(18);
        if(in_nightmare)draw_nightmare_room(win,room,p->room_num);
        else draw_ranged_attacked_room(win,room,p->room_num,sx,sy);
        draw_player_avatar(win,p->cord.y,p->cord.x);
    }
    if(!travelled){mvwaddstr(win,3,82,"Blocked shot: aim away from the wall.                ");move_or_not=0;return;}
    if(!hit){
        Weapon *grown=realloc(room->weapons,(room->weaponCount+1)*sizeof(Weapon));
        if(grown){room->weapons=grown;Weapon drop=p->currentWeapon;drop.count=1;drop.move=0;drop.cord=(Point){x,y};drop.isUsed=false;
            room->weapons[room->weaponCount++]=drop;
            /* Keep stairs, code panels and other loot beneath the recoverable projectile. */
            int c=game->whole_map[game->currentLevel][y][x].item;
            if(c=='.'||c=='#')game->whole_map[game->currentLevel][y][x].item=index==1?'2':index==2?'3':'4';
        }
    }
    if(in_nightmare)draw_nightmare_room(win,room,p->room_num);
    else draw_ranged_attacked_room(win,room,p->room_num,sx,sy);
    draw_player_avatar(win,p->cord.y,p->cord.x);wrefresh(win);
}
void attacking(WINDOW *game_win){
    if(game->levels[game->currentLevel]->rooms[game->player->room_num]->type == 's'){
        mvwprintw(game_win, 3, 82, "                                                                                 ");
        mvwprintw(game_win, 3, 82, "No attacking in Healing room!");
        move_or_not = 0;
        return;
    }
    else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->type == 'r'){
        mvwprintw(game_win, 3, 82, "                                                                                 ");
        mvwprintw(game_win, 3, 82, "No attacking in Reinforcing room!");
        move_or_not = 0;
        return;
    }
    if(game->player->currentWeapon.type == 'm' || game->player->currentWeapon.type == 's'){
        if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
            mvwprintw(game_win, 3, 82, "                                                                                 ");
            mvwprintw(game_win, 3, 82, "No attack in corridors!");
            move_or_not = 0;
            return;
        }
        if(game->player->currentWeapon.type == 's'){
            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                Mix_PlayMusic(sword_effect, 1);
            }
            game->player->currentWeapon.damage = game->player->currentWeapon.count * 5;
        }
        else if(game->player->currentWeapon.type == 'm'){
            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                Mix_PlayMusic(mace_effect, 1);
            }
            game->player->currentWeapon.damage = 5;
        }
        if(game->player->Power_effect > 0){
            game->player->currentWeapon.damage *= 2;
        }
        enemy_be_attaked(game->player->cord.y, game->player->cord.x, game_win);
        enemy_be_attaked(game->player->cord.y, game->player->cord.x + 1, game_win);
        enemy_be_attaked(game->player->cord.y, game->player->cord.x - 1, game_win);
        enemy_be_attaked(game->player->cord.y + 1, game->player->cord.x, game_win);
        enemy_be_attaked(game->player->cord.y - 1, game->player->cord.x, game_win);
        enemy_be_attaked(game->player->cord.y + 1, game->player->cord.x + 1, game_win);
        enemy_be_attaked(game->player->cord.y + 1, game->player->cord.x - 1, game_win);
        enemy_be_attaked(game->player->cord.y - 1, game->player->cord.x + 1, game_win);
        enemy_be_attaked(game->player->cord.y - 1, game->player->cord.x - 1, game_win);
        if(escape_check||run_finished)return;
        draw_attacked_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], game->player->room_num);
        if(game->player->currentWeapon.type == 's' && game->player->currentWeapon.count >= 4 && (move_click == 'E' || move_click == 'e' || move_click == KEY_MOUSE)){
            int dx = 0;
            int dy = 0;
            if(move_direction == KEY_UP || move_direction == 'w' || move_direction == 'W'){
                dy = -1;
            }
            else if(move_direction == KEY_DOWN || move_direction == 's' || move_direction == 'S'){
                dy = 1;
            }
            else if(move_direction == KEY_LEFT || move_direction == 'a' || move_direction == 'A'){
                dx = -1;
            }
            else if(move_direction == KEY_RIGHT || move_direction == 'd' || move_direction == 'D'){
                dx = 1;
            }
            if(player_step_valid(game->player->cord.y + 2 * dy, game->player->cord.x + 2 * dx)){
                game->player->cord.x += 2 * dx;
                game->player->cord.y += 2 * dy;
            }
        }
        else if(game->player->currentWeapon.type == 's' && game->player->currentWeapon.count >= 2 && (move_click == 'E' || move_click == 'e' || move_click == KEY_MOUSE)){
            int dx = 0;
            int dy = 0;
            if(move_direction == KEY_UP || move_direction == 'w' || move_direction == 'W'){
                dy = -1;
            }
            else if(move_direction == KEY_DOWN || move_direction == 's' || move_direction == 'S'){
                dy = 1;
            }
            else if(move_direction == KEY_LEFT || move_direction == 'a' || move_direction == 'A'){
                dx = -1;
            }
            else if(move_direction == KEY_RIGHT || move_direction == 'd' || move_direction == 'D'){
                dx = 1;
            }
            if(player_step_valid(game->player->cord.y + dy, game->player->cord.x + dx)){
                game->player->cord.x += dx;
                game->player->cord.y += dy;
            }
        }
    }
    else{
        fire_ranged(game_win);
    }
}

void guid(WINDOW *game_win){
    if(ui_old_visuals){legacy_guid(game_win);return;}
    int map_width, map_height;
    getmaxyx(stdscr, map_height, map_width);
    if (map_height > MAP_ROWS) map_height = MAP_ROWS;
    if (map_width > MAP_COLS) map_width = MAP_COLS;
    int start_x = BORDER_PADDING;
    int start_y = BORDER_PADDING;
    int end_x = map_width * 3 / 4 - BORDER_PADDING;
    int end_y = map_height * 3 / 4 - BORDER_PADDING + 5;
    if(first_ancientkey == 1 || first_attack == 1 || first_damage == 1 || first_enemy_near == 1 || first_food == 1 || first_gold == 1 || first_stair == 1
    || first_password == 1 || first_shield == 1 || first_specialfood == 1 || first_spell == 1 || first_trap == 1 || first_weapon == 1 || first_lock_door == 1){
        mvwprintw(game_win, map_height - 10 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 9 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 8 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 7 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 6 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 5 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 4 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 3 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 2 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 1 - 1, map_width - 54 - 23, "                                                                        ");
        wattron(game_win, COLOR_PAIR(color_pair_num));
        if(color_pair_num == 0){
            wattron(game_win, COLOR_PAIR(14));
        }
        mvwprintw(game_win, map_height - 8, map_width - 22, "█████████████████");
        mvwprintw(game_win, map_height - 7, map_width - 22, "█████████████████");
        mvwprintw(game_win, map_height - 6, map_width - 22, "█████████████████");
        mvwprintw(game_win, map_height - 5, map_width - 22, "███  ████  ██████");
        mvwprintw(game_win, map_height - 4, map_width - 22, "█████    ████████");
        mvwprintw(game_win, map_height - 3, map_width - 22, "█████████████████");
        // wattroff(game_win, COLOR_PAIR(color_pair_num));
        // wattroff(game_win, COLOR_PAIR(14));
    }
    // wattron(game_win, COLOR_PAIR(color_pair_num));
    if(first_ancientkey == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 14;
        int b = -1;
        first_ancientkey = -1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ Ancient Keys are old and magical tools  │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│    used to open all kind of doors.      │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│          Use them wisely!              _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰────────────────────────────────────────╯ \\_");
    }
    else if(first_attack == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_attack = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ Well done! You successfully hit your first │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ shot. You should kill monsters like this   │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ You can see the amount of damage that you  |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ dealt on combat report!                   _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_damage == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_damage = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ Hey, when you get close to a monster it    │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ will damage you with each move.            │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ Some of them also follow you for a few     |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ steps or while you are in the room        _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_enemy_near == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_enemy_near = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ Watch your steps! When you see the alert   │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ on the combat box, it means that there is  │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ an enemy near you at it will be revealed,  |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ but at first they are hidden.             _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_food == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_food = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ You should eat food to keep your hunger    │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ above 30 so you could regenerate you health│");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ and also when you are out of food you will |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ loose health. Check it out on bar box!    _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_gold == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_gold = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ Golds are kind of points for your XP and   │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ they will be saved forever when you collect│");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ them. Look for them!                       |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ You can check them on your bar.           _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_lock_door == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_lock_door = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ Some doors are locked and the need password│");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ to be unlocked!                            │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ password generators are shown with \"&\" in  |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ the room, look for them and step on them! _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_password == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_password = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ When you step on a password generator its  │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ password will be shown on messages box and │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ it will disapear after a small period of   |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ time. (Some doors need 2 passwords!!!)    _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_shield == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_shield = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ Shields are used to defend you against     │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ monsters damage. When you are out of shield│");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ the damage will be health damage. It won't |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ work on traps. You can see it on your bar._\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_specialfood == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 19;
        int b = -2;
        first_specialfood = -1;
        mvwprintw(game_win, map_height - 13 - b, map_width - 54 - a, "╭──────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 12 - b, map_width - 54 - a, "│ Special foods are known to be one of the     │");
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "│ most precious kind of food that have magical │");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ effects! They will be rotten after a few time|");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ and loose their magic. You can also combine  |");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ two rotten food for a chance of enchanting   |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ spells. BTW normal ones will give you 20HP. _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰─────────────────────────────────────────────╯ \\_");
    }
    else if(first_trap == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 19;
        int b = -2;
        first_trap = -1;
        mvwprintw(game_win, map_height - 13 - b, map_width - 54 - a, "╭──────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 12 - b, map_width - 54 - a, "│ Traps are hidden in each room and they can   │");
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "│ deal damage to you or move you to battle room│");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ which you have to defeat all of the monsters |");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ in that room to be able to return and        |");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ continue your game. When a trap is revealed  |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ It will loose its ability and stays visible._\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰─────────────────────────────────────────────╯ \\_");
    }
    else if(first_stair == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 19;
        int b = -1;
        first_stair = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭──────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ When you see a '<' or '>' sign; it means that│");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ you are in a stair room and you can go on the│");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ stair sign and press the same button of the  |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ stair and move to next/previous level. '≡' _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰─────────────────────────────────────────────╯ \\_");
    }
    else if(first_spell == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_spell = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ Spells are mysterious potions that will    │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ give you temporary effects!                │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ You can use them by pressing \"TAB\" button  |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ to open your inventory and choose them.   _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_weapon == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_weapon = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ RANGED: first press SPACE to aim.          │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ Then press an arrow / WASD key to FIRE.   │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ WAND: arrows shoot 1; WASD rides for 2.   │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ TAB: equipment. E: repeat. Esc: cancel.   _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    wattroff(game_win, COLOR_PAIR(color_pair_num));
    wattroff(game_win, COLOR_PAIR(14));
}
void quoting(WINDOW *game_win){
    if(ui_old_visuals){legacy_quoting(game_win);return;}
    int map_width, map_height;
    getmaxyx(stdscr, map_height, map_width);
    if (map_height > MAP_ROWS) map_height = MAP_ROWS;
    if (map_width > MAP_COLS) map_width = MAP_COLS;
    int start_x = BORDER_PADDING;
    int start_y = BORDER_PADDING;
    int end_x = map_width * 3 / 4 - BORDER_PADDING;
    int end_y = map_height * 3 / 4 - BORDER_PADDING + 5;
    int message = rand() % 31;
    mvwprintw(game_win, map_height - 10 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 9 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 8 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 7 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 6 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 5 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 4 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 3 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 2 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 1 - 1, map_width - 54 - 23, "                                                                        ");
    // mvwprintw(game_win, map_height - 0 - 1, map_width - 54 - 23, "                                                                        ");
    wattron(game_win, COLOR_PAIR(color_pair_num));
    if(color_pair_num == 0){
        wattron(game_win, COLOR_PAIR(14));
    }
    mvwprintw(game_win, map_height - 8, map_width - 22, "█████████████████");
    mvwprintw(game_win, map_height - 7, map_width - 22, "█████████████████");
    mvwprintw(game_win, map_height - 6, map_width - 22, "█████████████████");
    mvwprintw(game_win, map_height - 5, map_width - 22, "███  ████  ██████");
    mvwprintw(game_win, map_height - 4, map_width - 22, "█████    ████████");
    mvwprintw(game_win, map_height - 3, map_width - 22, "█████████████████");
    wattroff(game_win, COLOR_PAIR(color_pair_num));
    wattroff(game_win, COLOR_PAIR(14));
    wattron(game_win, COLOR_PAIR(3));
    if(message == 0){
        int a = 9;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│You could live a better life, if you⧹");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│had a better mind and a better body _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰───────────────────────────────────┘");
    }
    else if(message == 1){
        int a = 12;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭───────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│You are deeply attached to your friends⧹");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│and acquaintances. Just leave it!      _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰──────────────────────────────────────┘");
    }
    else if(message == 2){
        int a = -1;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭──────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│It was all so different   │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│before everything changed._⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰─────────────────────────┘  ¯");
    }
    else if(message == 3){
        int a = 8;
        int b = 1;
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, " /───────────────────────────────────\\");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "< Excellent day to have a rotten day._⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, " \\__________________________________/  ¯");
    }
    else if(message == 4){
        int a = 14;
        int b = 0;
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│Q: What's the difference between Bell    │");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ Labs and the Boy Scouts of America?     │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│A: The Boy Scouts have adult supervision _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰────────────────────────────────────────┘  ¯");
    }
    else if(message == 5){
        int a = 13;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ Q: Why did the WASP cross the road?    │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│ A: To get to the middle. :>           _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰───────────────────────────────────────┘  ¯");
    }
    else if(message == 6){
        int a = 14;
        int b = 0;
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "╭────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│Perilous to all of us are the devices of│");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│an art deeper than we ourselves possess.│");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ -- Gandalf the Grey                    │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│ [J.R.R. Tolkien,\"Lord of the Rings\"]  _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰───────────────────────────────────────┘  ¯");
    }
    else if(message == 7){
        int a = 14;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ The human race has one really effective │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│ weapon, and that is laughter. =D        _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰────────────────────────────────────────┘  ¯");
    }
    else if(message == 8){
        int a = 14;
        int b = 1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ Q: How do you shoot a blue elephant?    │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ A: With a blue-elephant gun.            │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ Q: How do you shoot a pink elephant?    │");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ A: Twist its trunk until it turns blue, │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│  then shoot it with a blue-elephant gun._⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰────────────────────────────────────────┘  ¯");
    }
    else if(message == 9){
        int a = 1;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭────────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ Beware of a tall black man │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│ With one blond shoe!       _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰───────────────────────────┘  ¯");
    }
    else if(message == 10){
        int a = 10;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭─────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ Don't tell any big lies today.      │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│ Small ones can be just as effective._⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰────────────────────────────────────┘  ¯");
    }
    else if(message == 11){
        int a = 23;
        int b = 1;
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╭──────────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│You'll feel much better once you've given up hope.=\\");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰─────────────────────────────────────────────────┘  ¯¯");
    }
    else if(message == 12){
        int a = 9;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│You are sick, twisted and perverted.│");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│I like that in a person.           _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰───────────────────────────────────┘  ¯");
    }
    else if(message == 13){
        int a = 5;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭────────────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ A tall, dark and creep stranger │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│ will have more fun than you O_o _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰────────────────────────────────┘  ¯");
    }
    else if(message == 14){
        int a = 12;
        int b = 1;
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "╭───────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ Anyone who has had a bull by the tail │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ knows five or six more things         │");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ than someone who hasn't.              │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│  'Mark Twain'                        _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰─────────────────────────────────────┘  ¯");
    }
    else if(message == 15){
    int a = 13;
    int b = -1;
    mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭────────────────────────────────────────╮");
    mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ \"The only limit to our realization of  │");
    mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│  tomorrow is our doubts of today.\"     │");
    mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ - Franklin D. Roosevelt                _⧹");
    mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰────────────────────────────────────────╯ ¯");
    }
    else if(message == 16){
        int a = 14;
        int b = -1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ \"You miss 100%% of the shots you don't   │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│  take.\"                                 │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ - Wayne Gretzky                        _⧹");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰────────────────────────────────────────╯ ¯");
    }
    else if(message == 17){
        int a = 14;
        int b = -1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ \"In the end, it's not the years in      │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│  your life that count. It's the life    │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│  in your years.\"                        │");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│  \'Abraham Lincoln\'                      _\\_");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "╰────────────────────────────────────────╯");
    }
    else if(message == 18){
        int a = 14;
        int b = -1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ \"Do not go where the path may lead,     │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│  go instead where there is no path and  │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│  leave a trail.\"                        │");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ - Ralph Waldo Emerson                   _\\_");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "╰────────────────────────────────────────╯");
    }
    else if(message == 19){
        int a = 14;
        int b = -1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ \"Life is what happens when you're busy  │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│  making other plans.\"                   │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ - John Lennon                           _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰────────────────────────────────────────╯  \\");
    }
    else if(message == 20){
        int a = 14;
        int b = 0;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ Two antennas met on a roof, fell in     │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ love, and got married. The ceremony     │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ wasn't much, but the reception was      │");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ excellent. <$_$>                        \\");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "╰─────────────────────────────────────────╯\\_");
    }
    else if(message == 21){
        int a = 14;
        int b = -2;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ I told my wife she was drawing her      │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ eyebrows too high. She looked surprised._⧹");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╰─────────────────────────────────────────╯ ¯");
    }
    else if(message == 22){
        int a = 14;
        int b = -2;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ I'm reading a book about anti-gravity.  │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ It's impossible to put down.            _⧹");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╰─────────────────────────────────────────╯ ¯");
    }
    else if(message == 23){
        int a = 14;
        int b = -2;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ Did you hear about the guy who          │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ invented Lifesavers? He made a mint!    _\\");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╰─────────────────────────────────────────╯ \\");
    }
    else if(message == 24){
        int a = 14;
        int b = -1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ Parallel lines have so much in common.  │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ It’s a shame they’ll never meet. =_(    _\\");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╰────────────────────────────────────────╯  \\");
    }
    else if(message == 25){
        int a = 14;
        int b = -1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ My wife told me I had to stop acting    │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ like a flamingo. So I had to put my     │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ foot down.                             _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰────────────────────────────────────────╯ \\_");
    }
    else if(message == 26){
        int a = 14;
        int b = -2;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ Why did the chicken go to the seance?   │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ To talk to the other side.              │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╰───────────────────────────────────────/\\_\\_");
    }
    else if(message == 27){
        int a = 14;
        int b = -2;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ I used to be addicted to the hokey      │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ pokey, but then I turned myself around. │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╰───────────────────────────────────────~\\_\\_");
    }
    else if(message == 28){
        int a = 13;
        int b = 0;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ \"The greatest glory in living lies not │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│  in never falling, but in rising every │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│  time we fall.\"                        │");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ - Nelson Mandela                       │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "╰──────────────────────────────────────`\\_\\_");
    }
    else if(message == 29){
        int a = 14;
        int b = -2;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ Did you hear about the claustrophobic   │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ astronaut? He needed a little space.    │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╰───────────────────────────────────────¯\\_\\_");
    }
    else if(message == 30){
        int a = 14;
        int b = -1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ I told my computer I needed a break,    │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ and now it won't stop sending me        │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ Kit Kat ads.                            │");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────¯\\_\\_");
    }
    wattroff(game_win, COLOR_PAIR(3));
}
static void draw_status_hud(WINDOW *win) {
    if(ui_old_visuals){legacy_messaging(win);return;}
    int h=getmaxy(win), y=h-11, x=4, width=77;
    attr_t attrs;short pair;wattr_get(win,&attrs,&pair,NULL);
    wattrset(win,COLOR_PAIR(10));
    for(int r=0;r<10;r++) mvwhline(win,y+r,x,' ',width);
    wattron(win,COLOR_PAIR(3));
    mvwaddstr(win,y,x,"┌");mvwhline(win,y,x+1,ACS_HLINE,width-2);mvwaddstr(win,y,x+width-1,"┐");
    mvwaddstr(win,y+9,x,"└");mvwhline(win,y+9,x+1,ACS_HLINE,width-2);mvwaddstr(win,y+9,x+width-1,"┘");
    for(int r=1;r<9;r++){mvwaddstr(win,y+r,x,"│");mvwaddstr(win,y+r,x+width-1,"│");}
    mvwaddstr(win,y,x+3," ADVENTURER ");wattroff(win,COLOR_PAIR(3));
    const char *weapon="Mace";
    switch(game->player->currentWeapon.type){case 'd':weapon="Dagger";break;case 'w':weapon="Magic Wand";break;case 'a':weapon="Bow";break;case 's':weapon="Sword";break;}
    wattron(win,COLOR_PAIR(12));
    mvwprintw(win,y+2,x+3,"Weapon  %-12s   Stock / rank  %d",weapon,game->player->currentWeapon.count);
    wattroff(win,COLOR_PAIR(12));
    const char *labels[]={"HEALTH","SHIELD","FOOD"};
    int values[]={game->player->health,game->player->shield,game->player->foodCount};
    short colors[]={6,3,7};
    for(int r=0;r<3;r++) {
        int value=values[r],fill=value<0?0:value>100?20:value/5;
        wattron(win,COLOR_PAIR(colors[r]));
        mvwprintw(win,y+4+r,x+3,"%-7s %3d  [",labels[r],value);
        for(int c=0;c<20;c++)mvwaddstr(win,y+4+r,x+17+c,c<fill?"━":"·");
        mvwaddstr(win,y+4+r,x+37,"]");wattroff(win,COLOR_PAIR(colors[r]));
    }
    wattron(win,COLOR_PAIR(12));mvwprintw(win,y+4,x+43,"Gold   %d",game->player->gold);
    mvwprintw(win,y+5,x+43,"Keys   %d",game->player->acientKey);wattroff(win,COLOR_PAIR(12));
    wattron(win,COLOR_PAIR(17));
    if(game->player->currentWeapon.type=='w')
        mvwaddstr(win,y+7,x+3,"WAND: SPACE, then arrows: shoot 1 | WASD: magic ride 2");
    else if(game->player->currentWeapon.type=='d'||game->player->currentWeapon.type=='a')
        mvwaddstr(win,y+7,x+3,"RANGED: 1 SPACE to aim  2 Arrow/WASD to fire  | E repeat");
    mvwprintw(win,y+8,x+3,"Effects (turns)   Speed %d   Power %d   Healing %d",game->player->Speed_effect,game->player->Power_effect,game->player->Health_effect);
    wattroff(win,COLOR_PAIR(17));
    wattr_set(win,attrs,pair,NULL);
}

void messaging(WINDOW *game_win){
    if(ui_old_visuals){legacy_messaging(game_win);return;}
    { //Updating weapon
        if(game->player->currentWeapon.type == 'd'){
            game->player->currentWeapon = game->player->weapons[1];
        }
        else if(game->player->currentWeapon.type == 'w'){
            game->player->currentWeapon = game->player->weapons[2];
        }
        else if(game->player->currentWeapon.type == 'a'){
            game->player->currentWeapon = game->player->weapons[3];
        }
        else if(game->player->currentWeapon.type == 's'){
            game->player->currentWeapon = game->player->weapons[4];
        }
    }
    time(&move_time_2);
    if(difftime(move_time_2, move_time) >= 4){
        time(&move_time);
        mvwprintw(game_win, 3, 5, "                                                                                 ");
        mvwprintw(game_win, 3, 82, "                                                                                 ");
        mvwprintw(game_win, 2, 82, "                                                                                 ");
    }
    if(password_disapear % 20 == 19 || difftime(move_time_2, move_time_lock) >= 15){
        time(&move_time_lock);
        is_pass_lock = 0;
    }
    if(is_pass_lock == 0){
        time_t mid_time;
        time(&mid_time);
        long long pt = difftime(mid_time, start_time) * 5 * difficulty_efficiency;
        mvwprintw(game_win, 2, 5, "                                                                               ");
        wattron(game_win, COLOR_PAIR(7));
        mvwprintw(game_win, 2, 5, "Floor %d   |   Deepest %d   |   XP %lld", game->currentLevel + 1, game->unlockedLevel, pointss + game->player->gold * 10 + pt);
        wattroff(game_win, COLOR_PAIR(7));
        if(game->unlockedLevel == -1){
            mvwprintw(game_win, 2, 5, "Floor %d   |   Deepest %d   |   XP %lld", game->currentLevel + 1, 0, pointss + game->player->gold * 10 + pt);
        }
        else{
            if((game->currentLevel == game->levelCount - 1 && !is_last_room) || game->currentLevel == 6){
                mvwprintw(game_win, 2, 5, "Floor %d   |   Deepest %d   |   XP %lld Enemy Left: %d❌", game->currentLevel + 1, game->unlockedLevel, pointss + game->player->gold * 10 + pt, game->levels[game->currentLevel]->rooms[0]->enemyLeft);
            }
            else{
                mvwprintw(game_win, 2, 5, "Floor %d   |   Deepest %d   |   XP %lld", game->currentLevel + 1, game->unlockedLevel, pointss + game->player->gold * 10 + pt);
            }
        }
    }
    int map_width, map_height;
    getmaxyx(stdscr, map_height, map_width);
    if (map_height > MAP_ROWS) map_height = MAP_ROWS;
    if (map_width > MAP_COLS) map_width = MAP_COLS;
    int start_x = BORDER_PADDING;
    int start_y = BORDER_PADDING;
    int end_x = map_width * 3 / 4 - BORDER_PADDING;
    int end_y = map_height * 3 / 4 - BORDER_PADDING + 5;
    int effects_count = 0;
    int a = (game->player->foodCount - 1) / 100;
    int spacing = 1;
    { // Effects
        if(game->player->Power_effect > 0){
            effects_count++;
        }
        if(game->player->Health_effect > 0){
            effects_count++;
        }
        if(game->player->Speed_effect > 0){
            effects_count++;
        }
    }
    in_emoji_check();
    if(in_emoji == 0){
        if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
            wattron(game_win, A_REVERSE | COLOR_PAIR(color_pair_num));
            if(character != L'\U00000040' && character != L'\U00002117'
            && character != L'\U0000263A' && character != L'\U0000263B'
            && character != L'\U0000235F' && character != L'\U0000058D'
            && character != L'\U0000272F'
            && player_step_valid(game->player->cord.y, game->player->cord.x + 1)){
                draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
            }
            else{
                draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
            }
        }
        else{
            wattron(game_win, COLOR_PAIR(color_pair_num));
            draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
            wattroff(game_win, COLOR_PAIR(color_pair_num));
        }
        if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
            wattroff(game_win, A_REVERSE | COLOR_PAIR(color_pair_num));
        }
    }
    draw_status_hud(game_win);
    { //Borders
        wattron(game_win, COLOR_PAIR(15));
        mvwprintw(game_win, 1, 4, "┌───────Messages────────────────────────────────────────────────────────────┐");
        mvwprintw(game_win, 2, 80, "│");
        mvwprintw(game_win, 3, 80, "│");
        mvwprintw(game_win, 2, 4, "│");
        mvwprintw(game_win, 3, 4, "│");
        mvwprintw(game_win, 4, 4, "└───────────────────────────────────────────────────────────────────────────┘");
        mvwprintw(game_win, 1, 81, "┌───────Combat-Report───────────────────────────────────────────────────────┐");
        mvwprintw(game_win, 2, 80 + 77, "│");
        mvwprintw(game_win, 3, 80 + 77, "│");
        mvwprintw(game_win, 2, 81, "│");
        mvwprintw(game_win, 3, 81, "│");
        mvwprintw(game_win, 4, 81, "└───────────────────────────────────────────────────────────────────────────┘");
        mvwprintw(game_win, 2, 159, "☰");
        mvwprintw(game_win, 3, 158, "\u2009▣");
        mvwhline(game_win, start_y - 1, start_x - 1, ACS_HLINE, map_width - 11);
        mvwhline(game_win, end_y, start_x - 1, ACS_HLINE, map_width - 11);
        mvwprintw(game_win, start_y - 1, start_x - 2, "╭");
        mvwprintw(game_win, start_y - 1, map_width - 6, "╮");
        mvwprintw(game_win, end_y, start_x - 2, "╰");
        mvwprintw(game_win, end_y, map_width - 6, "╯");
        for(int i = start_y; i < end_y; i++){
            mvwprintw(game_win, i, start_x - 2, "│");
            mvwprintw(game_win, i, map_width - 6, "│");
        }
        wattroff(game_win, COLOR_PAIR(15));
        time(&last_room2);
        last_room2 = time(NULL);
        drag2 = clock();
        if(game->currentLevel == game->levelCount - 1){
            if((double)(drag2 - drag1) / CLOCKS_PER_SEC >= 0.03){
                which_pos = 1 - which_pos;
                time(&last_room1);
                drag1 = clock();
                int a = map_height - 32;
                int b = map_width - 60;
                mvwprintw(game_win, a, b, "                                            ");
                mvwprintw(game_win, a + 1, b, "                                            ");
                mvwprintw(game_win, a + 2, b, "                                            ");
                mvwprintw(game_win, a + 3, b, "                                            ");
                mvwprintw(game_win, a + 4, b, "                                            ");
                mvwprintw(game_win, a + 5, b, "                                            ");
                mvwprintw(game_win, a + 6, b, "                                            ");
                mvwprintw(game_win, a + 7, b, "                                            ");
                mvwprintw(game_win, a + 8, b, "                                            ");
                mvwprintw(game_win, a + 9, b, "                                            ");
                mvwprintw(game_win, a + 10, b, "                                            ");
                mvwprintw(game_win, a + 11, b, "                                            ");
                mvwprintw(game_win, a + 12, b, "                                            ");
                mvwprintw(game_win, a + 13, b, "                                            ");
                mvwprintw(game_win, a + 14, b, "                                            ");
                b -= 41;
                a -= 5;
                mvwprintw(game_win, a, b, "                                                  ");
                mvwprintw(game_win, a + 1, b, "                                                  ");
                mvwprintw(game_win, a + 2, b, "                                                  ");
                mvwprintw(game_win, a + 3, b, "                                                  ");
                mvwprintw(game_win, a + 4, b, "                                                  ");
                mvwprintw(game_win, a + 5, b, "                                                  ");
                mvwprintw(game_win, a + 6, b, "                                                  ");
                mvwprintw(game_win, a + 7, b, "                                                  ");
                mvwprintw(game_win, a + 8, b, "                                                  ");
                mvwprintw(game_win, a + 9, b, "                                                  ");
                mvwprintw(game_win, a + 10, b, "                                                  ");
                mvwprintw(game_win, a + 11, b, "                                                  ");
                mvwprintw(game_win, a + 12, b, "                                                  ");
                mvwprintw(game_win, a + 13, b, "                                                  ");
                mvwprintw(game_win, a + 14, b, "                                                  ");
                b += 41;
                a += 5;
                wattron(game_win, COLOR_PAIR(14));
                if(last_one){
                    b -= 41;
                    a -= 5;
                    wattron(game_win, COLOR_PAIR(17));
                    mvwprintw(game_win, a + 3, b, "╭────────────────────────────────────────────╮");
                    mvwprintw(game_win, a + 4, b, "│          What is happening?!!!             │");
                    mvwprintw(game_win, a + 5, b, "│   I think I have underestimated you...     │");
                    mvwprintw(game_win, a + 5, b, "│After all... Finally! A mighty HERO appeared│");
                    mvwprintw(game_win, a + 6, b, "│ I always knew I have a glorious death...  _\\");
                    mvwprintw(game_win, a + 7, b, "╰───────────────────────────────────────────╯ \\");
                    mvwprintw(game_win, a + 8, b, "                                               \\");
                    mvwprintw(game_win, a + 9, b, "                                                \\");
                    mvwprintw(game_win, a + 10, b, "                                                \\_____");
                    b += 41;
                    a += 5;
                }
                else if(is_last_room){
                    b -= 40;
                    a -= 5;
                    wattron(game_win, COLOR_PAIR(6));
                    if(game->levels[game->currentLevel]->rooms[0]->enemies[game->levels[game->currentLevel]->rooms[0]->enemyCount - 1].health >= 120){
                        mvwprintw(game_win, a + 3, b, "╭───────────────────────────────────────────╮");
                        mvwprintw(game_win, a + 4, b, "│                IMMORTAL!!!                │");
                        mvwprintw(game_win, a + 5, b, "│      The GREATEST MoNSteR of all time     │");
                        mvwprintw(game_win, a + 6, b, "│Lets see how you can come up with this one_\\");
                        mvwprintw(game_win, a + 7, b, "╰──────────────────────────────────────────╯ \\");
                        mvwprintw(game_win, a + 8, b, "                                              \\");
                        mvwprintw(game_win, a + 9, b, "                                               \\");
                        mvwprintw(game_win, a + 10, b, "                                               \\_____");
                    }
                    if(game->levels[game->currentLevel]->rooms[0]->enemies[game->levels[game->currentLevel]->rooms[0]->enemyCount - 1].health <= 100){
                        mvwprintw(game_win, a + 3, b, "╭───────────────────────────────────────────╮");
                        mvwprintw(game_win, a + 4, b, "│ Well maybe you are not that bad little one│");
                        mvwprintw(game_win, a + 5, b, "│ But still not good ENOUGH!!!              │");
                        mvwprintw(game_win, a + 6, b, "│ Catch it Immortalll...                   _\\");
                        mvwprintw(game_win, a + 7, b, "╰──────────────────────────────────────────╯ \\");
                        mvwprintw(game_win, a + 8, b, "                                              \\");
                        mvwprintw(game_win, a + 9, b, "                                               \\");
                        mvwprintw(game_win, a + 10, b, "                                               \\_____");
                    }
                    b += 40;
                    a += 5;
                }
                else{
                    b -= 40;
                    a -= 5;
                    if(game->levels[game->currentLevel]->rooms[0]->enemyLeft > 15){
                        mvwprintw(game_win, a + 3, b, "╭───────────────────────────────────────────╮");
                        mvwprintw(game_win, a + 4, b, "│ Welcome to Treasure Level you little hero │");
                        mvwprintw(game_win, a + 5, b, "│ Lets see if you can handle all of my      │");
                        mvwprintw(game_win, a + 6, b, "│ monsters in this room!                   _\\");
                        mvwprintw(game_win, a + 7, b, "╰──────────────────────────────────────────╯ \\");
                        mvwprintw(game_win, a + 8, b, "                                              \\");
                        mvwprintw(game_win, a + 9, b, "                                               \\");
                        mvwprintw(game_win, a + 10, b, "                                               \\_____");
                    }
                    else if(game->levels[game->currentLevel]->rooms[0]->enemyLeft <= 10 && game->levels[game->currentLevel]->rooms[0]->enemyLeft >= 4){
                        mvwprintw(game_win, a + 3, b, "╭───────────────────────────────────────────╮");
                        mvwprintw(game_win, a + 4, b, "│ Ha Ha HAAA looking at you on this filed   │");
                        mvwprintw(game_win, a + 5, b, "│ is joyously and also funny how you suffer!│");
                        mvwprintw(game_win, a + 6, b, "│ I still have a BIG suprise for you       _\\");
                        mvwprintw(game_win, a + 7, b, "╰──────────────────────────────────────────╯ \\");
                        mvwprintw(game_win, a + 8, b, "                                              \\");
                        mvwprintw(game_win, a + 9, b, "                                               \\");
                        mvwprintw(game_win, a + 10, b, "                                               \\_____");
                    }
                    else if(game->levels[game->currentLevel]->rooms[0]->enemyLeft <= 1){
                        mvwprintw(game_win, a + 4, b, "╭───────────────────────────────────────────╮");
                        mvwprintw(game_win, a + 5, b, "│ I guess you are ready for my suprise...   │");
                        mvwprintw(game_win, a + 6, b, "│ And now... RISE of the---                _\\");
                        mvwprintw(game_win, a + 7, b, "╰──────────────────────────────────────────╯ \\");
                        mvwprintw(game_win, a + 8, b, "                                              \\");
                        mvwprintw(game_win, a + 9, b, "                                               \\");
                        mvwprintw(game_win, a + 10, b, "                                               \\_____");
                    }
                    b += 40;
                    a += 5;
                }
                if(which_pos){
                    mvwprintw(game_win, a, b, " <>=======() ");
                    mvwprintw(game_win, a + 1, b, "(/\\___   /|\\          ()==========<>_");
                    mvwprintw(game_win, a + 2, b, "      \\_/ | \\        //|\\   ______/ \\)");
                    mvwprintw(game_win, a + 3, b, "        \\_|  \\      // | \\_/");
                    mvwprintw(game_win, a + 4, b, "          \\|\\/|\\_   //  /\\/");
                    mvwprintw(game_win, a + 5, b, "           (oo)\\ \\_//  /");
                    mvwprintw(game_win, a + 6, b, "          //_/\\_\\/ /  |");
                    mvwprintw(game_win, a + 7, b, "         @@/  |=\\  \\  |");
                    mvwprintw(game_win, a + 8, b, "              \\_=\\_ \\ |");
                    mvwprintw(game_win, a + 9, b, "                \\==\\ \\|\\_");
                    mvwprintw(game_win, a + 10, b, "             __(\\===\\(  )\\");
                    mvwprintw(game_win, a + 11, b, "            (((~) __(_/   |");
                    mvwprintw(game_win, a + 12, b, "                 (((~) \\  /");
                    mvwprintw(game_win, a + 13, b, "               ______/ /");
                    mvwprintw(game_win, a + 14, b, "              '------'");
                }
                else{
                    mvwprintw(game_win, a + 3, b, "           _  _");
                    mvwprintw(game_win, a + 4, b, "           |\\/|__    ___");
                    mvwprintw(game_win, a + 5, b, "    ____   (oo)\\ \\_//   \\ ___");
                    mvwprintw(game_win, a + 6, b, "   //  /\\_//_/\\_\\/ /  |\\_|  \\");
                    mvwprintw(game_win, a + 7, b, "  // /\\/ @@/  |=\\  \\  | \\_|  \\_______ ");
                    mvwprintw(game_win, a + 8, b, " // | \\_/  /   \\_=\\_ \\ |  (/\\___   /|\\");
                    mvwprintw(game_win, a + 9, b, "//|\\______/ \\) \\==\\ \\|\\_  ()=========<>");
                    mvwprintw(game_win, a + 10, b, "_<>======()     _(\\===\\(  )\\");
                    mvwprintw(game_win, a + 11, b, "              (((~) __(_/   |");
                    mvwprintw(game_win, a + 12, b, "                  (((~) \\  /");
                    mvwprintw(game_win, a + 13, b, "                  ______/ /");
                    mvwprintw(game_win, a + 14, b, "                  '------'");
                }
            }
            wattroff(game_win, COLOR_PAIR(14));
            wattroff(game_win, COLOR_PAIR(17));
            wattroff(game_win, COLOR_PAIR(6));
        }
        time_t time_to_quote;
        time(&time_to_quote);
        if(difftime(time_to_quote, quote_time) >= 3){
            guid(game_win);
        }
        if(difftime(time_to_quote, quote_time2) >= 10){
            time(&quote_time2);
            mvwprintw(game_win, map_height - 10 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 9 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 8 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 7 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 6 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 5 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 4 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 3 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 2 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 1 - 1, map_width - 54 - 23, "                                                                        ");
            wattron(game_win, COLOR_PAIR(color_pair_num));
            if(color_pair_num == 0){
                wattron(game_win, COLOR_PAIR(14));
            }
            mvwprintw(game_win, map_height - 8, map_width - 22, "█████████████████");
            mvwprintw(game_win, map_height - 7, map_width - 22, "█████████████████");
            mvwprintw(game_win, map_height - 6, map_width - 22, "█████████████████");
            mvwprintw(game_win, map_height - 5, map_width - 22, "███  ████  ██████");
            mvwprintw(game_win, map_height - 4, map_width - 22, "█████    ████████");
            mvwprintw(game_win, map_height - 3, map_width - 22, "█████████████████");
            wattroff(game_win, COLOR_PAIR(color_pair_num));
            wattroff(game_win, COLOR_PAIR(14));
        }
        if(difftime(time_to_quote, quote_time) >= 15){
            time(&quote_time);
            quoting(game_win);
        }
    }
}
void inventory_bar_menu() {
    WINDOW *inventory_bar_win;
    int map_width, map_height;
    getmaxyx(stdscr, map_height, map_width);
    if (map_height > MAP_ROWS) map_height = MAP_ROWS;
    if (map_width > MAP_COLS) map_width = MAP_COLS;
    // int max_height = game->player->spellCount > game->player->weaponCount ? game->player->spellCount : game->player->weaponCount;
    int max_height = 3;
    inventory_bar_win = newwin(20 + max_height, 34 + 12, map_height / 2 - 14, map_width / 2 - 17);
    int x_start = map_width / 2 - 17, y_start = map_height / 2 - 14;
    keypad(inventory_bar_win, TRUE);
    box(inventory_bar_win, 0, 0);

    char *inventory_items[] = {
        "Health",
        "Food",
        "Gold",
        "Weapons",
        "Spells",
        "AncientKey"
    };
    int inventory_size = sizeof(inventory_items) / sizeof(inventory_items[0]);

    char *options[] = {
        UI_TEXT("Consume Food🌱 ","Consume Food♣ "),
        UI_TEXT("Choose Weapon🏹 ","Choose Weapon➳ "),
        UI_TEXT("Use Spell🍷","Use Spell◆"),
        "Forge broken Ancientkey⚠ "
    };
    int options_size = sizeof(options) / sizeof(options[0]);

    int selected_option = 0;
    int selected_item = 0;
    int items_size = 0;
    int ch;
    int mouse_checker = 1;

    while (1) {
        mouse_save.x = mouse_event.x;
        mouse_save.y = mouse_event.y;
        getmouse(&mouse_event);
        werase(inventory_bar_win);
        wattron(inventory_bar_win, COLOR_PAIR(color_pair_num));
        box(inventory_bar_win, 0, 0);
        wattroff(inventory_bar_win, COLOR_PAIR(color_pair_num));
        wattron(inventory_bar_win, COLOR_PAIR(3));
        mvwprintw(inventory_bar_win, 1, 1, UI_TEXT("%s: %d💗 Shield: %d💙","%s: %d♥ Shield: %d♡"), inventory_items[0], game->player->health, game->player->shield);
        mvwprintw(inventory_bar_win, 2, 1, UI_TEXT("%s: %d🍓","%s: %d♣"), inventory_items[1], game->player->foodCount);
        mvwprintw(inventory_bar_win, 3, 1, UI_TEXT("%s: %d📀","%s: %d¤"), inventory_items[2], game->player->gold);
        mvwprintw(inventory_bar_win, 4, 1, "%s: ", inventory_items[3]);
        int k = 11;
        for(int i = 0; i < 5; i++){
            if(i == 0){
                mvwprintw(inventory_bar_win, 4, k, UI_TEXT("Mace %d⚒","Mace %d♠"), game->player->weapons[i].count);
                k += 9;
            }
            else if(i == 1){
                mvwprintw(inventory_bar_win, 4, k, UI_TEXT("Dagger %d🗡","Dagger %d†"), game->player->weapons[i].count);
            }
            else if(i == 2){
                mvwprintw(inventory_bar_win, 5, k, "Magic Wand %d⚚", game->player->weapons[i].count);
                k += 15;
            }
            else if(i == 3){
                mvwprintw(inventory_bar_win, 5, k, "Normal Arrow %d➳", game->player->weapons[i].count);
                k += 17;
            }
            else if(i == 4){
                mvwprintw(inventory_bar_win, 5, k, UI_TEXT("Sword %d⚔","Sword %d†"), game->player->weapons[i].count);
            }
            int a = 0;
            if(game->player->weapons[i].count >= 10){
                a = 1;
            }
            k += a;
            if(i == 1){
                k = 1;
            }
        }
        // mvwprintw(inventory_bar_win, 4, k - 2, "  ➳");
        k = 9;
        mvwprintw(inventory_bar_win, 6, 1, "%s: ", inventory_items[4]);
        for(int i = 0; i < 3; i++){
            if(i == 0){
                mvwprintw(inventory_bar_win, 6, k, UI_TEXT("Speed %d🍾","Speed %d✦"), game->player->spells[i].count);
            }
            else if(i == 1){
                mvwprintw(inventory_bar_win, 6, k, UI_TEXT("Power %d🍷","Power %d◆"), game->player->spells[i].count);
            }
            else if(i == 2){
                mvwprintw(inventory_bar_win, 6, k, UI_TEXT("Healing %d🍸","Healing %d✧"), game->player->spells[i].count);
            }
            int a = 0;
            if(game->player->spells[i].count >= 10){
                a = 1;
            }
            k += 10 + a; 
        }
        // mvwprintw(inventory_bar_win, 5, k - 1, "  ♧");
        mvwprintw(inventory_bar_win, 7, 1, "Ancient Key: %d", game->player->acientKey);
        mvwprintw(inventory_bar_win, 8, 1, "Broken Ancient Key: %d", game->player->brokenAcientKey);
        wattroff(inventory_bar_win, COLOR_PAIR(3));
        wattron(inventory_bar_win, COLOR_PAIR(12));
        int a = 0;
        if(game->player->brokenAcientKey >= 10){
            a = 1;
        }
        mvwprintw(inventory_bar_win, 7, 15 + a, "Δ");
        a = 0;
        if(game->player->brokenAcientKey >= 10){
            a = 1;
        }
        mvwprintw(inventory_bar_win, 8, 22 + a, "δ");
        wattroff(inventory_bar_win, COLOR_PAIR(12));

        for (int i = 0; i < options_size; i++) {
            if (i == 0) wattron(inventory_bar_win, COLOR_PAIR(7));
            if (i == 1) wattron(inventory_bar_win, COLOR_PAIR(19));
            if (i == 2) wattron(inventory_bar_win, COLOR_PAIR(17));
            if (i == 3) wattron(inventory_bar_win, COLOR_PAIR(12));
            
            if (i == selected_option) {
                wattron(inventory_bar_win, A_REVERSE);
            }
            mvwprintw(inventory_bar_win, i + inventory_size + 4, 1, "%s", options[i]);
            wattroff(inventory_bar_win, A_REVERSE);

            if (i == 0) wattroff(inventory_bar_win, COLOR_PAIR(7));
            if (i == 1) wattroff(inventory_bar_win, COLOR_PAIR(19));
            if (i == 2) wattroff(inventory_bar_win, COLOR_PAIR(17));
            if (i == 3) wattroff(inventory_bar_win, COLOR_PAIR(12));
        }

        wrefresh(inventory_bar_win);
        ch = music_getch(inventory_bar_win);

        switch (ch) {
            case KEY_UP:
                selected_option = (selected_option - 1 + options_size) % options_size;
                break;
            case KEY_DOWN:
                selected_option = (selected_option + 1) % options_size;
                break;
            case 27:
            case '\t':
                delwin(inventory_bar_win);
                return;
            case KEY_MOUSE:
                if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y && mouse_checker == 0){
                    mouse_checker = 1;
                    for (int i = 0; i < options_size; i++) {
                        if(mouse_event.y == i + inventory_size + 4 + y_start && mouse_event.x >= x_start + 1 && mouse_event.x <= x_start + 1 + strlen(options[i])){
                            selected_option = i;
                        }
                    }
                    for (int i = 0; i < options_size; i++) {
                        if (i == 0) wattron(inventory_bar_win, COLOR_PAIR(7));
                        if (i == 1) wattron(inventory_bar_win, COLOR_PAIR(19));
                        if (i == 2) wattron(inventory_bar_win, COLOR_PAIR(17));
                        if (i == 3) wattron(inventory_bar_win, COLOR_PAIR(12));
                        
                        if (i == selected_option) {
                            wattron(inventory_bar_win, A_REVERSE);
                        }
                        mvwprintw(inventory_bar_win, i + inventory_size + 4, 1, "%s", options[i]);
                        wattroff(inventory_bar_win, A_REVERSE);
            
                        if (i == 0) wattroff(inventory_bar_win, COLOR_PAIR(7));
                        if (i == 1) wattroff(inventory_bar_win, COLOR_PAIR(19));
                        if (i == 2) wattroff(inventory_bar_win, COLOR_PAIR(17));
                        if (i == 3) wattroff(inventory_bar_win, COLOR_PAIR(12));
                    }
                    wrefresh(inventory_bar_win);
                    usleep(100000);
                    wattron(inventory_bar_win, COLOR_PAIR(color_pair_num));
                    box(inventory_bar_win, 0, 0);
                    wattroff(inventory_bar_win, COLOR_PAIR(color_pair_num));
                    if(selected_option == 0){
                        mvwprintw(inventory_bar_win, inventory_size + options_size + 4, 1, "Choose your food:");
                        items_size = 4;
                        int tab_check = 0;
                        mouse_checker = 1;
                        while(tab_check != 1){
                            mouse_save.x = mouse_event.x;
                            mouse_save.y = mouse_event.y;
                            if (ch == KEY_MOUSE) getmouse(&mouse_event);
                            wattron(inventory_bar_win, COLOR_PAIR(7));
                            for(int i = 0; i < 4; i++){
                                if (i == selected_item) {
                                    wattron(inventory_bar_win, A_REVERSE);
                                }
                                if(i == 0){
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Special Food %d🍓 ","● Special Food %d♣ "), game->player->specialfoods[i].count);
                                }
                                if(i == 1){
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Power Food %d🍘 ","● Power Food %d♧ "), game->player->specialfoods[i].count);
                                }
                                if(i == 2){
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Speed Food %d🍟 ","● Speed Food %d♠ "), game->player->specialfoods[i].count);
                                }
                                if(i == 3){
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Enchant spell with Rotten Food %d☣💱 ","● Enchant spell with Rotten Food %d☣¤ "), game->player->specialfoods[i].count);
                                }
                                wattroff(inventory_bar_win, A_REVERSE);
                            }
                            wrefresh(inventory_bar_win);
                            ch = music_getch(inventory_bar_win);
                            switch (ch) {
                                case KEY_UP:
                                    selected_item = (selected_item - 1 + items_size) % items_size;
                                    break;
                                case KEY_DOWN:
                                    selected_item = (selected_item + 1) % items_size;
                                    break;
                                case KEY_MOUSE:
                                    if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y && mouse_checker == 0){
                                        mouse_checker = 1;
                                        for(int i = 0; i < 4; i++){
                                            if(mouse_event.y == inventory_size + options_size + 5 + i + y_start && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 20){
                                                selected_item = i;
                                            }
                                            else if(mouse_event.y == inventory_size + options_size + 5 + i + y_start && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 46 && i == 3){
                                                selected_item = i;
                                            }
                                        }
                                        for(int i = 0; i < 4; i++){
                                            if (i == selected_item) {
                                                wattron(inventory_bar_win, A_REVERSE);
                                            }
                                            if(i == 0){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Special Food %d🍓 ","● Special Food %d♣ "), game->player->specialfoods[i].count);
                                            }
                                            if(i == 1){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Power Food %d🍘 ","● Power Food %d♧ "), game->player->specialfoods[i].count);
                                            }
                                            if(i == 2){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Speed Food %d🍟 ","● Speed Food %d♠ "), game->player->specialfoods[i].count);
                                            }
                                            if(i == 3){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Enchant spell with Rotten Food %d☣💱 ","● Enchant spell with Rotten Food %d☣¤ "), game->player->specialfoods[i].count);
                                            }
                                            wattroff(inventory_bar_win, A_REVERSE);
                                        }
                                        wrefresh(inventory_bar_win);
                                        if(selected_item == 0){
                                            if(game->player->specialfoods[0].count <= 0){
                                                wattroff(inventory_bar_win, COLOR_PAIR(7));
                                                wattron(inventory_bar_win, COLOR_PAIR(3));
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Your Hunger is %d.           ", game->player->foodCount);
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 2, "And you don't have any Special Food :b");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 2, "You need to eat more!   ");
                                                wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                wattron(inventory_bar_win, COLOR_PAIR(7));
                                            }
                                            else if(game->player->health > 80){
                                                if(game->player->foodCount <= 130){
                                                    game->player->foodCount += 20;
                                                    game->player->specialfoods[0].count--;
                                                    if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                        Mix_PlayMusic(magicfood_effect, 1);
                                                    }
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You gained 20 food");
                                                    wattron(inventory_bar_win, COLOR_PAIR(3));
                                                    mvwprintw(inventory_bar_win, 2, 1, UI_TEXT("%s: %d🍓","%s: %d♣"), inventory_items[1], game->player->foodCount);
                                                    wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                }
                                                else{
                                                    wattroff(inventory_bar_win, COLOR_PAIR(7));
                                                    wattron(inventory_bar_win, COLOR_PAIR(3));
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Your Health is %d.", game->player->health);
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 2, "And it can't be more than 100 :b   ");
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 2, "Get back to the field!   ");
                                                    wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                    wattron(inventory_bar_win, COLOR_PAIR(7));
                                                }
                                            }
                                            else{
                                                game->player->health += 20;
                                                game->player->specialfoods[0].count--;
                                                if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                    Mix_PlayMusic(magicfood_effect, 1);
                                                }
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You gained 20 health      ");
                                                wattron(inventory_bar_win, COLOR_PAIR(3));
                                                mvwprintw(inventory_bar_win, 1, 1, UI_TEXT("%s: %d💗","%s: %d♥"), inventory_items[0], game->player->health);
                                                mvwprintw(inventory_bar_win, 2, 1, UI_TEXT("%s: %d🍓","%s: %d♣"), inventory_items[1], game->player->foodCount);
                                                wattroff(inventory_bar_win, COLOR_PAIR(3));
                                            }
                                        }
                                        else if(selected_item == 1){
                                            if(game->player->specialfoods[1].count <= 0){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Not enough foods!         ");
                                            }
                                            else{
                                                if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                    Mix_PlayMusic(magicfood_effect, 1);
                                                }
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You used a Power Food.      ");
                                                game->player->specialfoods[1].count--;
                                                game->player->Power_effect += 10;
                                                game->player->foodCount += 10;
                                            }
                                        }
                                        else if(selected_item == 2){
                                            if(game->player->specialfoods[2].count <= 0){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Not enough foods!         ");
                                            }
                                            else{
                                                if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                    Mix_PlayMusic(magicfood_effect, 1);
                                                }
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You used a Speed Food.     ");
                                                game->player->specialfoods[2].count--;
                                                game->player->Speed_effect += 10;
                                                game->player->foodCount += 10;
                                            }
                                        }
                                        else if(selected_item == 3){
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "                                           ");
                                        if(game->player->specialfoods[3].count <= 1){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Not enough Rotten foods!    ");
                                        }
                                        else{
                                            if(rand() % 2){
                                                if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                    Mix_PlayMusic(magic_effect, 1);
                                                }
                                                if(rand() % 2){
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You've enchanted a Speed Spell!");
                                                    game->player->spells[0].count++;
                                                }
                                                else{
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You've enchanted a Power Spell!");
                                                    game->player->spells[1].count++;
                                                }
                                            }
                                            else{
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Ah, bad luck, hope you get it next time =(");
                                            }
                                            game->player->specialfoods[3].count -= 2;
                                        }
                                    }
                                    }
                                    mouse_checker = 0;
                                    break;
                                case '\n':
                                    if(selected_item == 0){
                                        if(game->player->specialfoods[0].count <= 0){
                                            wattroff(inventory_bar_win, COLOR_PAIR(7));
                                            wattron(inventory_bar_win, COLOR_PAIR(3));
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Your Hunger is %d.   ", game->player->foodCount);
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 2, "And you don't have any Special Food :b");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 2, "You need to eat more!   ");
                                            wattroff(inventory_bar_win, COLOR_PAIR(3));
                                            wattron(inventory_bar_win, COLOR_PAIR(7));
                                        }
                                        else if(game->player->health > 80){
                                            if(game->player->foodCount <= 130){
                                                game->player->foodCount += 20;
                                                game->player->specialfoods[0].count--;
                                                if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                    Mix_PlayMusic(magicfood_effect, 1);
                                                }
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You gained 20 food");
                                                wattron(inventory_bar_win, COLOR_PAIR(3));
                                                mvwprintw(inventory_bar_win, 2, 1, UI_TEXT("%s: %d🍓","%s: %d♣"), inventory_items[1], game->player->foodCount);
                                                wattroff(inventory_bar_win, COLOR_PAIR(3));
                                            }
                                            else{
                                                wattroff(inventory_bar_win, COLOR_PAIR(7));
                                                wattron(inventory_bar_win, COLOR_PAIR(3));
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Your Health is %d.", game->player->health);
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 2, "And it can't be more than 100 :b   ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 2, "Get back to the field!   ");
                                                wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                wattron(inventory_bar_win, COLOR_PAIR(7));
                                            }
                                        }
                                        else{
                                            game->player->health += 20;
                                            game->player->specialfoods[0].count--;
                                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                Mix_PlayMusic(magicfood_effect, 1);
                                            }
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You gained 20 health");
                                            wattron(inventory_bar_win, COLOR_PAIR(3));
                                            mvwprintw(inventory_bar_win, 1, 1, UI_TEXT("%s: %d💗","%s: %d♥"), inventory_items[0], game->player->health);
                                            mvwprintw(inventory_bar_win, 2, 1, UI_TEXT("%s: %d🍓","%s: %d♣"), inventory_items[1], game->player->foodCount);
                                            wattroff(inventory_bar_win, COLOR_PAIR(3));
                                        }
                                    }
                                    else if(selected_item == 1){
                                        if(game->player->specialfoods[1].count <= 0){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Not enough foods!");
                                        }
                                        else{
                                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                Mix_PlayMusic(magicfood_effect, 1);
                                            }
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You used a Power Food.");
                                            game->player->specialfoods[1].count--;
                                            game->player->Power_effect += 10;
                                        }
                                    }
                                    else if(selected_item == 2){
                                        if(game->player->specialfoods[2].count <= 0){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Not enough foods!");
                                        }
                                        else{
                                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                Mix_PlayMusic(magicfood_effect, 1);
                                            }
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You used a Speed Food.");
                                            game->player->specialfoods[2].count--;
                                            game->player->Speed_effect += 10;
                                        }
                                    }
                                    else if(selected_item == 3){
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "                                           ");
                                    if(game->player->specialfoods[3].count <= 1){
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Not enough Rotten foods!");
                                    }
                                    else{
                                        if(rand() % 2){
                                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                Mix_PlayMusic(magic_effect, 1);
                                            }
                                            if(rand() % 2){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You've enchanted a Speed Spell!");
                                                game->player->spells[0].count++;
                                            }
                                            else{
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You've enchanted a Power Spell!");
                                                game->player->spells[1].count++;
                                            }
                                        }
                                        else{
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Ah, bad luck, hope you get it next time =(");
                                        }
                                        game->player->specialfoods[3].count -= 2;
                                    }
                                }
                                    break;
                                case 27:
            case '\t':
                                    tab_check = 1;
                                    break;
                            }
                            wattroff(inventory_bar_win, COLOR_PAIR(7));
                        }
                        selected_item = 0;
                    }
                    else if(selected_option == 1){
                        mvwprintw(inventory_bar_win, inventory_size + options_size + 4, 1, "Choose your weapon type:");
                        char *weapon_types[] = {"Melee Weapon", "Ranged Weapon"};
                        int weapon_types_size = 2;
                        int selected_type = 0;
                        int tab_check1 = 0;
                        while (tab_check1 != 1) {
                            mouse_save.x = mouse_event.x;
                            mouse_save.y = mouse_event.y;
                            if (ch == KEY_MOUSE) getmouse(&mouse_event);
                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                           ");
                            mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "                                       ");
                            // mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 1, "                                           ");
                            // mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "                                       ");
                            // mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 2, "                              ");
                            // mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "                              ");
                            // mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 2, "                              ");
                            // mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 2, "                              ");
                            wattron(inventory_bar_win, COLOR_PAIR(19));
                            for (int i = 0; i < weapon_types_size; i++) {
                                if (i == selected_type) {
                                    wattron(inventory_bar_win, A_REVERSE);
                                }
                                if(i == 0){
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, UI_TEXT("%s🪓 ","%s♠ "), weapon_types[i]);
                                }
                                else if(i == 1){
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1 + i * 29, "%s➳ ", weapon_types[i]);
                                }
                                wattroff(inventory_bar_win, A_REVERSE);
                            }
                            wrefresh(inventory_bar_win);
                            ch = music_getch(inventory_bar_win);
    
                            switch (ch) {
                                case KEY_RIGHT:
                                    selected_type = (selected_type - 1 + weapon_types_size) % weapon_types_size;
                                    break;
                                case KEY_LEFT:
                                    selected_type = (selected_type + 1) % weapon_types_size;
                                    break;
                                case KEY_MOUSE:
                                    if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y){
                                        for (int i = 0; i < weapon_types_size; i++) {
                                            if(mouse_event.y == y_start + inventory_size + options_size + 5 && mouse_event.x >= x_start + 1 + i * 29 && mouse_event.x <= x_start + 3 + i * 29 + strlen(weapon_types[i])){
                                                selected_type = i;
                                            }
                                        }
                                        for (int i = 0; i < weapon_types_size; i++) {
                                            if (i == selected_type) {
                                                wattron(inventory_bar_win, A_REVERSE);
                                            }
                                            if(i == 0){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, UI_TEXT("%s🪓 ","%s♠ "), weapon_types[i]);
                                            }
                                            else if(i == 1){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1 + i * 29, "%s➳ ", weapon_types[i]);
                                            }
                                            wattroff(inventory_bar_win, A_REVERSE);
                                        }
                                        wrefresh(inventory_bar_win);
                                        usleep(100000);
                                        if (selected_type == 0) { // Melee Weapon
                                            char *melee_weapons[] = {"Mace", "Sword"};
                                            int items_size = 2;
                                            int selected_item = 0;
                                            int tab_check = 0;
                                            while (tab_check != 1) {
                                                mouse_save.x = mouse_event.x;
                                                mouse_save.y = mouse_event.y;
                                                if (ch == KEY_MOUSE) getmouse(&mouse_event);
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                            ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 1, "                                            ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 1, "                                            ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 1, "                                            ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 1, "                                            ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 1, "                                            ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 1, "                                            ");
                                                for (int i = 0; i < items_size; i++) {
                                                    if (i == selected_item) {
                                                        wattron(inventory_bar_win, A_REVERSE);
                                                    }
                                                    if(i == 0){
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Mace⚒ Damage: 5","● Mace♠ Damage: 5"));
                                                    }
                                                    else if(i == 1){
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Sword⚔ Sharpness: %d Damage: %d","● Sword† Sharpness: %d Damage: %d"), game->player->weapons[4].count, game->player->weapons[4].count * 5);
                                                    }
                                                    wattroff(inventory_bar_win, A_REVERSE);
                                                }
                                                wrefresh(inventory_bar_win);
                                                ch = music_getch(inventory_bar_win);
        
                                                switch (ch) {
                                                    case KEY_UP:
                                                        selected_item = (selected_item - 1 + items_size) % items_size;
                                                        break;
                                                    case KEY_DOWN:
                                                        selected_item = (selected_item + 1) % items_size;
                                                        break;
                                                    case KEY_MOUSE:
                                                        if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y){
                                                            for (int i = 0; i < items_size; i++) {
                                                                if(mouse_event.y == y_start + inventory_size + options_size + 5 + i && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 20){
                                                                    selected_item = i;
                                                                }
                                                                else if(mouse_event.y == y_start + inventory_size + options_size + 5 + i && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 38 && i == 1){
                                                                    selected_item = i;
                                                                }
                                                            }
                                                            for (int i = 0; i < items_size; i++) {
                                                                if (i == selected_item) {
                                                                    wattron(inventory_bar_win, A_REVERSE);
                                                                }
                                                                if(i == 0){
                                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Mace⚒ Damage: 5","● Mace♠ Damage: 5"));
                                                                }
                                                                else if(i == 1){
                                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Sword⚔ Sharpness: %d Damage: %d","● Sword† Sharpness: %d Damage: %d"), game->player->weapons[4].count, game->player->weapons[4].count * 5);
                                                                }
                                                                wattroff(inventory_bar_win, A_REVERSE);
                                                            }
                                                            wrefresh(inventory_bar_win);
                                                            usleep(100000);
                                                            wattroff(inventory_bar_win, COLOR_PAIR(19));
                                                            wattron(inventory_bar_win, COLOR_PAIR(3));
                                                            if (game->player->weapons[4].count <= 0 && selected_item == 1) {
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You don't have that weapon!                ");
                                                            } else {
                                                                if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                                    Mix_PlayMusic(weaponpickup_effect, 1);
                                                                }
                                                                if(selected_item == 0){
                                                                    game->player->currentWeapon = game->player->weapons[0];
                                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You choosed %s           ", game->player->weapons[0].name);
                                                                }
                                                                if(selected_item == 1){
                                                                    game->player->currentWeapon = game->player->weapons[4];
                                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You choosed %s           ", game->player->weapons[4].name);
                                                                }
                                                            }
                                                            wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                            wattron(inventory_bar_win, COLOR_PAIR(19));
                                                            tab_check = 1;
                                                        }
                                                        break;
                                                    case '\n':
                                                        wattroff(inventory_bar_win, COLOR_PAIR(19));
                                                        wattron(inventory_bar_win, COLOR_PAIR(3));
                                                        if (game->player->weapons[4].count <= 0 && selected_item == 1) {
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You don't have that weapon!                ");
                                                        } else {
                                                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                                Mix_PlayMusic(weaponpickup_effect, 1);
                                                            }
                                                            if(selected_item == 0){
                                                                game->player->currentWeapon = game->player->weapons[0];
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You choosed %s           ", game->player->weapons[0].name);
                                                            }
                                                            if(selected_item == 1){
                                                                game->player->currentWeapon = game->player->weapons[4];
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You choosed %s           ", game->player->weapons[4].name);
                                                            }
                                                        }
                                                        wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                        wattron(inventory_bar_win, COLOR_PAIR(19));
                                                        tab_check = 1;
                                                        break;
                                                    case 27:
            case '\t':
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                           ");
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 2, "                                       ");
                                                        tab_check = 1;
                                                        break;
                                                }
                                            }
                                        } else if (selected_type == 1) { // Ranged Weapon
                                            char *ranged_weapons[] = {"Dagger", "Magic Wand", "Normal Arrow"};
                                            int items_size = 3;
                                            int selected_item = 0;
                                            int tab_check = 0;
                                            while (tab_check != 1) {
                                                mouse_save.x = mouse_event.x;
                                                mouse_save.y = mouse_event.y;
                                                if (ch == KEY_MOUSE) getmouse(&mouse_event);
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                            ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 1, "                                            ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 1, "                                            ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 1, "                                            ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 1, "                                            ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 1, "                                            ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 1, "                                            ");
                                                for (int i = 0; i < items_size; i++) {
                                                    if (i == selected_item) {
                                                        wattron(inventory_bar_win, A_REVERSE);
                                                    }
                                                    if(i == 0){
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Dagger🗡 Damage: 12 Range: 5 Count: %d","● Dagger† Damage: 12 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                    }
                                                    else if(i == 1){
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, "● MagicWand⚚ Damage: 15 Range: 10 Count: %d", game->player->weapons[i + 1].count);
                                                    }
                                                    else if(i == 2){
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Bow🏹 Damage: 5 Range: 5 Count: %d","● Bow➳ Damage: 5 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                    }
                                                    wattroff(inventory_bar_win, A_REVERSE);
                                                }
                                                wrefresh(inventory_bar_win);
                                                ch = music_getch(inventory_bar_win);
        
                                                switch (ch) {
                                                    case KEY_UP:
                                                        selected_item = (selected_item - 1 + items_size) % items_size;
                                                        break;
                                                    case KEY_DOWN:
                                                        selected_item = (selected_item + 1) % items_size;
                                                        break;
                                                    case KEY_MOUSE:
                                                    if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y){
                                                        for (int i = 0; i < items_size; i++) {
                                                            if(mouse_event.y == y_start + inventory_size + options_size + 5 + i && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 35){
                                                                selected_item = i;
                                                            }
                                                            if(i == 0){
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Dagger🗡 Damage: 12 Range: 5 Count: %d","● Dagger† Damage: 12 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                            }
                                                            else if(i == 1){
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, "● MagicWand⚚ Damage: 15 Range: 10 Count: %d", game->player->weapons[i + 1].count);
                                                            }
                                                            else if(i == 2){
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Bow🏹 Damage: 5 Range: 5 Count: %d","● Bow➳ Damage: 5 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                            }
                                                            wattroff(inventory_bar_win, A_REVERSE);
                                                        }
                                                        for (int i = 0; i < items_size; i++) {
                                                            if (i == selected_item) {
                                                                wattron(inventory_bar_win, A_REVERSE);
                                                            }
                                                            if(i == 0){
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Dagger🗡 Damage: 12 Range: 5 Count: %d","● Dagger† Damage: 12 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                            }
                                                            else if(i == 1){
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, "● MagicWand⚚ Damage: 15 Range: 10 Count: %d", game->player->weapons[i + 1].count);
                                                            }
                                                            else if(i == 2){
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Bow🏹 Damage: 5 Range: 5 Count: %d","● Bow➳ Damage: 5 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                            }
                                                            wattroff(inventory_bar_win, A_REVERSE);
                                                        }
                                                        wrefresh(inventory_bar_win);
                                                        usleep(100000);
                                                        wattroff(inventory_bar_win, COLOR_PAIR(19));
                                                        wattron(inventory_bar_win, COLOR_PAIR(3));
                                                        if (game->player->weapons[selected_item + 1].count <= 0) {
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 1, "                                            ");
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "You don't have that projectile!         ");
                                                        } else {
                                                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                                Mix_PlayMusic(weaponpickup_effect, 1);
                                                            }
                                                            game->player->currentWeapon = game->player->weapons[selected_item + 1];
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "                                           ");
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "You choosed %s              ", game->player->weapons[selected_item + 1].name);
                                                        }
                                                        tab_check = 1;
                                                        wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                        wattron(inventory_bar_win, COLOR_PAIR(19));
                                                    }
                                                        break;
                                                    case '\n':
                                                        wattroff(inventory_bar_win, COLOR_PAIR(19));
                                                        wattron(inventory_bar_win, COLOR_PAIR(3));
                                                        if (game->player->weapons[selected_item + 1].count <= 0) {
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 1, "                                            ");
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "You don't have that projectile!         ");
                                                        } else {
                                                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                                Mix_PlayMusic(weaponpickup_effect, 1);
                                                            }
                                                            game->player->currentWeapon = game->player->weapons[selected_item + 1];
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "                                           ");
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "You choosed %s              ", game->player->weapons[selected_item + 1].name);
                                                        }
                                                        tab_check = 1;
                                                        wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                        wattron(inventory_bar_win, COLOR_PAIR(19));
                                                        break;
                                                    case 27:
            case '\t':
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                           ");
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 2, "                                           ");
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "                                       ");
                                                        tab_check = 1;
                                                        break;
                                                }
                                            }
                                        }
                                    }
                                    break;
                                case '\n':
                                    for (int i = 0; i < weapon_types_size; i++) {
                                        if(mouse_event.y == y_start + inventory_size + options_size + 5 && mouse_event.x >= x_start + 1 + i * 29 && mouse_event.x <= x_start + 3 + i * 29 + strlen(weapon_types[i])){
                                            selected_type = i;
                                        }
                                    }
                                    for (int i = 0; i < weapon_types_size; i++) {
                                        if (i == selected_type) {
                                            wattron(inventory_bar_win, A_REVERSE);
                                        }
                                        if(i == 0){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, UI_TEXT("%s🪓 ","%s♠ "), weapon_types[i]);
                                        }
                                        else if(i == 1){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1 + i * 29, "%s➳ ", weapon_types[i]);
                                        }
                                        wattroff(inventory_bar_win, A_REVERSE);
                                    }
                                    wrefresh(inventory_bar_win);
                                    usleep(100000);
                                    if (selected_type == 0) { // Melee Weapon
                                        char *melee_weapons[] = {"Mace", "Sword"};
                                        int items_size = 2;
                                        int selected_item = 0;
                                        int tab_check = 0;
                                        while (tab_check != 1) {
                                            mouse_save.x = mouse_event.x;
                                            mouse_save.y = mouse_event.y;
                                            if (ch == KEY_MOUSE) getmouse(&mouse_event);
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 1, "                                            ");
                                            for (int i = 0; i < items_size; i++) {
                                                if (i == selected_item) {
                                                    wattron(inventory_bar_win, A_REVERSE);
                                                }
                                                if(i == 0){
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Mace⚒ Damage: 5","● Mace♠ Damage: 5"));
                                                }
                                                else if(i == 1){
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Sword⚔ Sharpness: %d Damage: %d","● Sword† Sharpness: %d Damage: %d"), game->player->weapons[4].count, game->player->weapons[4].count * 5);
                                                }
                                                wattroff(inventory_bar_win, A_REVERSE);
                                            }
                                            wrefresh(inventory_bar_win);
                                            ch = music_getch(inventory_bar_win);
    
                                            switch (ch) {
                                                case KEY_UP:
                                                    selected_item = (selected_item - 1 + items_size) % items_size;
                                                    break;
                                                case KEY_DOWN:
                                                    selected_item = (selected_item + 1) % items_size;
                                                    break;
                                                case KEY_MOUSE:
                                                    if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y){
                                                        for (int i = 0; i < items_size; i++) {
                                                            if(mouse_event.y == y_start + inventory_size + options_size + 5 + i && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 20){
                                                                selected_item = i;
                                                            }
                                                            else if(mouse_event.y == y_start + inventory_size + options_size + 5 + i && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 38 && i == 1){
                                                                selected_item = i;
                                                            }
                                                        }
                                                        for (int i = 0; i < items_size; i++) {
                                                            if (i == selected_item) {
                                                                wattron(inventory_bar_win, A_REVERSE);
                                                            }
                                                            if(i == 0){
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Mace⚒ Damage: 5","● Mace♠ Damage: 5"));
                                                            }
                                                            else if(i == 1){
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Sword⚔ Sharpness: %d Damage: %d","● Sword† Sharpness: %d Damage: %d"), game->player->weapons[4].count, game->player->weapons[4].count * 5);
                                                            }
                                                            wattroff(inventory_bar_win, A_REVERSE);
                                                        }
                                                        wrefresh(inventory_bar_win);
                                                        usleep(100000);
                                                        wattroff(inventory_bar_win, COLOR_PAIR(19));
                                                        wattron(inventory_bar_win, COLOR_PAIR(3));
                                                        if (game->player->weapons[4].count <= 0 && selected_item == 1) {
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You don't have that weapon!                ");
                                                        } else {
                                                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                                Mix_PlayMusic(weaponpickup_effect, 1);
                                                            }
                                                            if(selected_item == 0){
                                                                game->player->currentWeapon = game->player->weapons[0];
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You choosed %s           ", game->player->weapons[0].name);
                                                            }
                                                            if(selected_item == 1){
                                                                game->player->currentWeapon = game->player->weapons[4];
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You choosed %s           ", game->player->weapons[4].name);
                                                            }
                                                        }
                                                        wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                        wattron(inventory_bar_win, COLOR_PAIR(19));
                                                        tab_check = 1;
                                                    }
                                                    break;
                                                case '\n':
                                                    wattroff(inventory_bar_win, COLOR_PAIR(19));
                                                    wattron(inventory_bar_win, COLOR_PAIR(3));
                                                    if (game->player->weapons[4].count <= 0 && selected_item == 1) {
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You don't have that weapon!                ");
                                                    } else {
                                                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                            Mix_PlayMusic(weaponpickup_effect, 1);
                                                        }
                                                        if(selected_item == 0){
                                                            game->player->currentWeapon = game->player->weapons[0];
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You choosed %s           ", game->player->weapons[0].name);
                                                        }
                                                        if(selected_item == 1){
                                                            game->player->currentWeapon = game->player->weapons[4];
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You choosed %s           ", game->player->weapons[4].name);
                                                        }
                                                    }
                                                    wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                    wattron(inventory_bar_win, COLOR_PAIR(19));
                                                    tab_check = 1;
                                                    break;
                                                case 27:
            case '\t':
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                           ");
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 2, "                                       ");
                                                    tab_check = 1;
                                                    break;
                                            }
                                        }
                                    } else if (selected_type == 1) { // Ranged Weapon
                                        char *ranged_weapons[] = {"Dagger", "Magic Wand", "Normal Arrow"};
                                        int items_size = 3;
                                        int selected_item = 0;
                                        int tab_check = 0;
                                        while (tab_check != 1) {
                                            mouse_save.x = mouse_event.x;
                                            mouse_save.y = mouse_event.y;
                                            if (ch == KEY_MOUSE) getmouse(&mouse_event);
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 1, "                                            ");
                                            for (int i = 0; i < items_size; i++) {
                                                if (i == selected_item) {
                                                    wattron(inventory_bar_win, A_REVERSE);
                                                }
                                                if(i == 0){
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Dagger🗡 Damage: 12 Range: 5 Count: %d","● Dagger† Damage: 12 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                }
                                                else if(i == 1){
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, "● MagicWand⚚ Damage: 15 Range: 10 Count: %d", game->player->weapons[i + 1].count);
                                                }
                                                else if(i == 2){
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Bow🏹 Damage: 5 Range: 5 Count: %d","● Bow➳ Damage: 5 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                }
                                                wattroff(inventory_bar_win, A_REVERSE);
                                            }
                                            wrefresh(inventory_bar_win);
                                            ch = music_getch(inventory_bar_win);
    
                                            switch (ch) {
                                                case KEY_UP:
                                                    selected_item = (selected_item - 1 + items_size) % items_size;
                                                    break;
                                                case KEY_DOWN:
                                                    selected_item = (selected_item + 1) % items_size;
                                                    break;
                                                case KEY_MOUSE:
                                                if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y){
                                                    for (int i = 0; i < items_size; i++) {
                                                        if(mouse_event.y == y_start + inventory_size + options_size + 5 + i && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 35){
                                                            selected_item = i;
                                                        }
                                                        if(i == 0){
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Dagger🗡 Damage: 12 Range: 5 Count: %d","● Dagger† Damage: 12 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                        }
                                                        else if(i == 1){
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, "● MagicWand⚚ Damage: 15 Range: 10 Count: %d", game->player->weapons[i + 1].count);
                                                        }
                                                        else if(i == 2){
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Bow🏹 Damage: 5 Range: 5 Count: %d","● Bow➳ Damage: 5 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                        }
                                                        wattroff(inventory_bar_win, A_REVERSE);
                                                    }
                                                    for (int i = 0; i < items_size; i++) {
                                                        if (i == selected_item) {
                                                            wattron(inventory_bar_win, A_REVERSE);
                                                        }
                                                        if(i == 0){
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Dagger🗡 Damage: 12 Range: 5 Count: %d","● Dagger† Damage: 12 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                        }
                                                        else if(i == 1){
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, "● MagicWand⚚ Damage: 15 Range: 10 Count: %d", game->player->weapons[i + 1].count);
                                                        }
                                                        else if(i == 2){
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Bow🏹 Damage: 5 Range: 5 Count: %d","● Bow➳ Damage: 5 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                        }
                                                        wattroff(inventory_bar_win, A_REVERSE);
                                                    }
                                                    wrefresh(inventory_bar_win);
                                                    usleep(100000);
                                                    wattroff(inventory_bar_win, COLOR_PAIR(19));
                                                    wattron(inventory_bar_win, COLOR_PAIR(3));
                                                    if (game->player->weapons[selected_item + 1].count <= 0) {
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 1, "                                            ");
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "You don't have that projectile!         ");
                                                    } else {
                                                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                            Mix_PlayMusic(weaponpickup_effect, 1);
                                                        }
                                                        game->player->currentWeapon = game->player->weapons[selected_item + 1];
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "                                           ");
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "You choosed %s              ", game->player->weapons[selected_item + 1].name);
                                                    }
                                                    tab_check = 1;
                                                    wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                    wattron(inventory_bar_win, COLOR_PAIR(19));
                                                }
                                                    break;
                                                case '\n':
                                                    wattroff(inventory_bar_win, COLOR_PAIR(19));
                                                    wattron(inventory_bar_win, COLOR_PAIR(3));
                                                    if (game->player->weapons[selected_item + 1].count <= 0) {
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 1, "                                            ");
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "You don't have that projectile!         ");
                                                    } else {
                                                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                            Mix_PlayMusic(weaponpickup_effect, 1);
                                                        }
                                                        game->player->currentWeapon = game->player->weapons[selected_item + 1];
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "                                           ");
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "You choosed %s              ", game->player->weapons[selected_item + 1].name);
                                                    }
                                                    tab_check = 1;
                                                    wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                    wattron(inventory_bar_win, COLOR_PAIR(19));
                                                    break;
                                                case 27:
            case '\t':
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                           ");
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 2, "                                           ");
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "                                       ");
                                                    tab_check = 1;
                                                    break;
                                            }
                                        }
                                    }
                                    break;
                                case 27:
            case '\t':
                                    tab_check1 = 1;
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "                                       ");
                                    break;
                            }
                            wattroff(inventory_bar_win, COLOR_PAIR(19));
                        }
                    }
                    else if(selected_option == 2){
                        mvwprintw(inventory_bar_win, inventory_size + options_size + 4, 1, "Choose your spell:");
                        items_size = 3;
                        int tab_check = 0;
                        while(tab_check != 1){
                            mouse_save.x = mouse_event.x;
                            mouse_save.y = mouse_event.y;
                            if (ch == KEY_MOUSE) getmouse(&mouse_event);
                            wattron(inventory_bar_win, COLOR_PAIR(17));
                            for(int i = 0; i < items_size; i++){
                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, "                            ");
                                if (i == selected_item) {
                                    wattron(inventory_bar_win, A_REVERSE);
                                }
                                if(i == 0){
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● %s🍾 %d","● %s✦ %d"), game->player->spells[i].name, game->player->spells[i].count);
                                }
                                if(i == 1){
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● %s🍷 %d","● %s◆ %d"), game->player->spells[i].name, game->player->spells[i].count);
                                }
                                if(i == 2){
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● %s🍸 %d","● %s✧ %d"), game->player->spells[i].name, game->player->spells[i].count);
                                }
                                wattroff(inventory_bar_win, A_REVERSE);
                            }
                            wrefresh(inventory_bar_win);
                            ch = music_getch(inventory_bar_win);
                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "                                    ");
                            switch (ch) {
                                case KEY_UP:
                                    selected_item = (selected_item - 1 + items_size) % items_size;
                                    break;
                                case KEY_DOWN:
                                    selected_item = (selected_item + 1) % items_size;
                                    break;
                                case KEY_MOUSE:
                                    if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y){
                                        for(int i = 0; i < items_size; i++){
                                            if(mouse_event.y == y_start + inventory_size + options_size + 5 + i && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 10 + strlen(game->player->spells[i].name)){
                                                selected_item = i;
                                            }
                                        }
                                        for(int i = 0; i < items_size; i++){
                                            if (i == selected_item) {
                                                wattron(inventory_bar_win, A_REVERSE);
                                            }
                                            if(i == 0){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● %s🍾 %d   ","● %s✦ %d   "), game->player->spells[i].name, game->player->spells[i].count);
                                            }
                                            if(i == 1){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● %s🍷 %d   ","● %s◆ %d   "), game->player->spells[i].name, game->player->spells[i].count);
                                            }
                                            if(i == 2){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● %s🍸 %d   ","● %s✧ %d   "), game->player->spells[i].name, game->player->spells[i].count);
                                            }
                                            wattroff(inventory_bar_win, A_REVERSE);
                                        }
                                        wrefresh(inventory_bar_win);
                                        wattroff(inventory_bar_win, COLOR_PAIR(17));
                                        wattron(inventory_bar_win, COLOR_PAIR(3));
                                        if(game->player->spells[selected_item].count <= 0){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "                                    ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "Not enough spells");  
                                            wattroff(inventory_bar_win, COLOR_PAIR(3));
                                            wattron(inventory_bar_win, COLOR_PAIR(17));
                                            break;
                                        }
                                        else{
                                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                Mix_PlayMusic(magic_effect, 1);
                                            }
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "                                    ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "You used %s", game->player->spells[selected_item].name);
                                            game->player->spells[selected_item].count--;
                                            game->player->spellCount--;
                                            if(game->player->spells[selected_item].type == 's'){
                                                game->player->Speed_effect += 15;
                                            }
                                            if(game->player->spells[selected_item].type == 'p'){
                                                game->player->Power_effect += 15;
                                            }
                                            if(game->player->spells[selected_item].type == 'h'){
                                                game->player->Health_effect += 15;
                                            }
                                            k = 9;
                                            for(int i = 0; i < 3; i++){
                                                if(i == 0){
                                                    mvwprintw(inventory_bar_win, 6, k, UI_TEXT("Speed %d🍾","Speed %d✦"), game->player->spells[i].count);
                                                }
                                                else if(i == 1){
                                                    mvwprintw(inventory_bar_win, 6, k, UI_TEXT("Power %d🍷","Power %d◆"), game->player->spells[i].count);
                                                }
                                                else if(i == 2){
                                                    mvwprintw(inventory_bar_win, 6, k, UI_TEXT("Healing %d🍸","Healing %d✧"), game->player->spells[i].count);
                                                }
                                                int a = 0;
                                                if(game->player->spells[i].count >= 10){
                                                    a = 1;
                                                }
                                                k += 10 + a; 
                                            }
                                            wattroff(inventory_bar_win, COLOR_PAIR(3));
                                            wattron(inventory_bar_win, COLOR_PAIR(17));
                                            break;
                                        }
                                    }
                                    break;
                                case '\n':
                                    wattroff(inventory_bar_win, COLOR_PAIR(17));
                                    wattron(inventory_bar_win, COLOR_PAIR(3));
                                    if(game->player->spells[selected_item].count <= 0){
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "                                    ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "Not enough spells");  
                                        wattroff(inventory_bar_win, COLOR_PAIR(3));
                                        wattron(inventory_bar_win, COLOR_PAIR(17));
                                        break;
                                    }
                                    else{
                                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                            Mix_PlayMusic(magic_effect, 1);
                                        }
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "                                    ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "You used %s", game->player->spells[selected_item].name);
                                        game->player->spells[selected_item].count--;
                                        game->player->spellCount--;
                                        if(game->player->spells[selected_item].type == 's'){
                                            game->player->Speed_effect += 15;
                                        }
                                        if(game->player->spells[selected_item].type == 'p'){
                                            game->player->Power_effect += 15;
                                        }
                                        if(game->player->spells[selected_item].type == 'h'){
                                            game->player->Health_effect += 15;
                                        }
                                        k = 9;
                                        for(int i = 0; i < 3; i++){
                                            if(i == 0){
                                                mvwprintw(inventory_bar_win, 6, k, UI_TEXT("Speed %d🍾","Speed %d✦"), game->player->spells[i].count);
                                            }
                                            else if(i == 1){
                                                mvwprintw(inventory_bar_win, 6, k, UI_TEXT("Power %d🍷","Power %d◆"), game->player->spells[i].count);
                                            }
                                            else if(i == 2){
                                                mvwprintw(inventory_bar_win, 6, k, UI_TEXT("Healing %d🍸","Healing %d✧"), game->player->spells[i].count);
                                            }
                                            int a = 0;
                                            if(game->player->spells[i].count >= 10){
                                                a = 1;
                                            }
                                            k += 10 + a; 
                                        }
                                        wattroff(inventory_bar_win, COLOR_PAIR(3));
                                        wattron(inventory_bar_win, COLOR_PAIR(17));
                                        break;
                                    }
                                case 27:
            case '\t':
                                    tab_check = 1;
                                    break;
                            }
                            wattroff(inventory_bar_win, COLOR_PAIR(17));
                        }
                        selected_item = 0;
                    }
                    else if(selected_option == 3){
                        mvwprintw(inventory_bar_win, inventory_size + options_size + 4, 1, "You have %d broken Ancientkeys:  ", game->player->brokenAcientKey);
                        items_size = 1;
                        int tab_check = 0;
                        while(tab_check != 1){
                            mouse_save.x = mouse_event.x;
                            mouse_save.y = mouse_event.y;
                            if (ch == KEY_MOUSE) getmouse(&mouse_event);
                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4, 1, "You have %d broken Ancientkeys:  ", game->player->brokenAcientKey);
                            wattron(inventory_bar_win, COLOR_PAIR(12));
                            for(int i = 0; i < 1; i++){
                                if (i == selected_item) {
                                    wattron(inventory_bar_win, A_REVERSE);
                                }
                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, "● Forge");
                                wattroff(inventory_bar_win, A_REVERSE);
                            }
                            wrefresh(inventory_bar_win);
                            ch = music_getch(inventory_bar_win);
                            switch (ch) {
                                case KEY_UP:
                                    selected_item = (selected_item - 1 + items_size) % items_size;
                                    break;
                                case KEY_DOWN:
                                    selected_item = (selected_item + 1) % items_size;
                                    break;
                                case KEY_MOUSE:
                                    if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y){
                                        if(mouse_event.y == y_start + inventory_size + options_size + 5 && mouse_event.x >= x_start + 2 && mouse_event.x >= x_start + 9){
                                            selected_item = 0;
                                        }
                                        wattron(inventory_bar_win, A_REVERSE);
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 2, "● Forge");
                                        wattroff(inventory_bar_win, A_REVERSE);
                                        wrefresh(inventory_bar_win);
                                        if(game->player->brokenAcientKey <= 1){
                                            wattroff(inventory_bar_win, COLOR_PAIR(12));
                                            wattron(inventory_bar_win, COLOR_PAIR(3));
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "Not enough broken Ancientkeys.  ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 2, "                              ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "                              ");
                                            wattroff(inventory_bar_win, COLOR_PAIR(3));
                                            wattron(inventory_bar_win, COLOR_PAIR(12));
                                        }
                                        else{
                                            wattroff(inventory_bar_win, COLOR_PAIR(12));
                                            wattron(inventory_bar_win, COLOR_PAIR(3));
                                            game->player->acientKey++;
                                            game->player->brokenAcientKey -= 2;
                                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                Mix_PlayMusic(forge_effect, 1);
                                            }
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "Forged Successfully.     ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 2, "                        ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "                    ");
                                            wattroff(inventory_bar_win, COLOR_PAIR(3));
                                            wattron(inventory_bar_win, COLOR_PAIR(12));
                                        }
                                        wattron(inventory_bar_win, COLOR_PAIR(3));
                                        mvwprintw(inventory_bar_win, 7, 1, "Ancient Key: %d", game->player->acientKey);
                                        mvwprintw(inventory_bar_win, 8, 1, "Broken Ancient Key: %d", game->player->brokenAcientKey);
                                        wattroff(inventory_bar_win, COLOR_PAIR(3));
                                    }
                                    break;
                                case '\n':
                                    if(game->player->brokenAcientKey <= 1){
                                        wattroff(inventory_bar_win, COLOR_PAIR(12));
                                        wattron(inventory_bar_win, COLOR_PAIR(3));
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "Not enough broken Ancientkeys.  ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 2, "                              ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "                              ");
                                        wattroff(inventory_bar_win, COLOR_PAIR(3));
                                        wattron(inventory_bar_win, COLOR_PAIR(12));
                                    }
                                    else{
                                        wattroff(inventory_bar_win, COLOR_PAIR(12));
                                        wattron(inventory_bar_win, COLOR_PAIR(3));
                                        game->player->acientKey++;
                                        game->player->brokenAcientKey -= 2;
                                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                            Mix_PlayMusic(forge_effect, 1);
                                        }
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "Forged Successfully.     ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 2, "                        ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "                    ");
                                        wattroff(inventory_bar_win, COLOR_PAIR(3));
                                        wattron(inventory_bar_win, COLOR_PAIR(12));
                                    }
                                    wattron(inventory_bar_win, COLOR_PAIR(3));
                                    mvwprintw(inventory_bar_win, 7, 1, "Ancient Key: %d", game->player->acientKey);
                                    mvwprintw(inventory_bar_win, 8, 1, "Broken Ancient Key: %d", game->player->brokenAcientKey);
                                    wattroff(inventory_bar_win, COLOR_PAIR(3));
                                    break;
                                case 27:
            case '\t':
                                    tab_check = 1;
                                    break;
                            }
                            wattroff(inventory_bar_win, COLOR_PAIR(12));
                        }
                        selected_item = 0;
                    }
                    wrefresh(inventory_bar_win);
                }
                mouse_checker = 0;
                break;
            case '\n':
                // werase(inventory_bar_win);
                wattron(inventory_bar_win, COLOR_PAIR(color_pair_num));
                box(inventory_bar_win, 0, 0);
                wattroff(inventory_bar_win, COLOR_PAIR(color_pair_num));
                if(selected_option == 0){
                    mvwprintw(inventory_bar_win, inventory_size + options_size + 4, 1, "Choose your food:");
                    items_size = 4;
                    int tab_check = 0;
                    mouse_checker = 1;
                    while(tab_check != 1){
                        mouse_save.x = mouse_event.x;
                        mouse_save.y = mouse_event.y;
                        if (ch == KEY_MOUSE) getmouse(&mouse_event);
                        wattron(inventory_bar_win, COLOR_PAIR(7));
                        for(int i = 0; i < 4; i++){
                            if (i == selected_item) {
                                wattron(inventory_bar_win, A_REVERSE);
                            }
                            if(i == 0){
                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Special Food %d🍓 ","● Special Food %d♣ "), game->player->specialfoods[i].count);
                            }
                            if(i == 1){
                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Power Food %d🍘 ","● Power Food %d♧ "), game->player->specialfoods[i].count);
                            }
                            if(i == 2){
                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Speed Food %d🍟 ","● Speed Food %d♠ "), game->player->specialfoods[i].count);
                            }
                            if(i == 3){
                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Enchant spell with Rotten Food %d☣💱 ","● Enchant spell with Rotten Food %d☣¤ "), game->player->specialfoods[i].count);
                            }
                            wattroff(inventory_bar_win, A_REVERSE);
                        }
                        wrefresh(inventory_bar_win);
                        ch = music_getch(inventory_bar_win);
                        switch (ch) {
                            case KEY_UP:
                                selected_item = (selected_item - 1 + items_size) % items_size;
                                break;
                            case KEY_DOWN:
                                selected_item = (selected_item + 1) % items_size;
                                break;
                            case KEY_MOUSE:
                                if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y && mouse_checker == 0){
                                    mouse_checker = 1;
                                    for(int i = 0; i < 4; i++){
                                        if(mouse_event.y == inventory_size + options_size + 5 + i + y_start && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 20){
                                            selected_item = i;
                                        }
                                        else if(mouse_event.y == inventory_size + options_size + 5 + i + y_start && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 46 && i == 3){
                                            selected_item = i;
                                        }
                                    }
                                    for(int i = 0; i < 4; i++){
                                        if (i == selected_item) {
                                            wattron(inventory_bar_win, A_REVERSE);
                                        }
                                        if(i == 0){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Special Food %d🍓 ","● Special Food %d♣ "), game->player->specialfoods[i].count);
                                        }
                                        if(i == 1){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Power Food %d🍘 ","● Power Food %d♧ "), game->player->specialfoods[i].count);
                                        }
                                        if(i == 2){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Speed Food %d🍟 ","● Speed Food %d♠ "), game->player->specialfoods[i].count);
                                        }
                                        if(i == 3){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Enchant spell with Rotten Food %d☣💱 ","● Enchant spell with Rotten Food %d☣¤ "), game->player->specialfoods[i].count);
                                        }
                                        wattroff(inventory_bar_win, A_REVERSE);
                                    }
                                    wrefresh(inventory_bar_win);
                                    if(selected_item == 0){
                                        if(game->player->specialfoods[0].count <= 0){
                                            wattroff(inventory_bar_win, COLOR_PAIR(7));
                                            wattron(inventory_bar_win, COLOR_PAIR(3));
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Your Hunger is %d.           ", game->player->foodCount);
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 2, "And you don't have any Special Food :b");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 2, "You need to eat more!   ");
                                            wattroff(inventory_bar_win, COLOR_PAIR(3));
                                            wattron(inventory_bar_win, COLOR_PAIR(7));
                                        }
                                        else if(game->player->health > 80){
                                            if(game->player->foodCount <= 130){
                                                game->player->foodCount += 20;
                                                game->player->specialfoods[0].count--;
                                                if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                    Mix_PlayMusic(magicfood_effect, 1);
                                                }
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You gained 20 food");
                                                wattron(inventory_bar_win, COLOR_PAIR(3));
                                                mvwprintw(inventory_bar_win, 2, 1, UI_TEXT("%s: %d🍓","%s: %d♣"), inventory_items[1], game->player->foodCount);
                                                wattroff(inventory_bar_win, COLOR_PAIR(3));
                                            }
                                            else{
                                                wattroff(inventory_bar_win, COLOR_PAIR(7));
                                                wattron(inventory_bar_win, COLOR_PAIR(3));
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Your Health is %d.", game->player->health);
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 2, "And it can't be more than 100 :b   ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 2, "Get back to the field!   ");
                                                wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                wattron(inventory_bar_win, COLOR_PAIR(7));
                                            }
                                        }
                                        else{
                                            game->player->health += 20;
                                            game->player->specialfoods[0].count--;
                                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                Mix_PlayMusic(magicfood_effect, 1);
                                            }
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You gained 20 health      ");
                                            wattron(inventory_bar_win, COLOR_PAIR(3));
                                            mvwprintw(inventory_bar_win, 1, 1, UI_TEXT("%s: %d💗","%s: %d♥"), inventory_items[0], game->player->health);
                                            mvwprintw(inventory_bar_win, 2, 1, UI_TEXT("%s: %d🍓","%s: %d♣"), inventory_items[1], game->player->foodCount);
                                            wattroff(inventory_bar_win, COLOR_PAIR(3));
                                        }
                                    }
                                    else if(selected_item == 1){
                                        if(game->player->specialfoods[1].count <= 0){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Not enough foods!         ");
                                        }
                                        else{
                                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                Mix_PlayMusic(magicfood_effect, 1);
                                            }
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You used a Power Food.      ");
                                            game->player->specialfoods[1].count--;
                                            game->player->Power_effect += 10;
                                            game->player->foodCount += 10;
                                        }
                                    }
                                    else if(selected_item == 2){
                                        if(game->player->specialfoods[2].count <= 0){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Not enough foods!         ");
                                        }
                                        else{
                                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                Mix_PlayMusic(magicfood_effect, 1);
                                            }
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You used a Speed Food.     ");
                                            game->player->specialfoods[2].count--;
                                            game->player->Speed_effect += 10;
                                            game->player->foodCount += 10;
                                        }
                                    }
                                    else if(selected_item == 3){
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "                                           ");
                                    if(game->player->specialfoods[3].count <= 1){
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Not enough Rotten foods!    ");
                                    }
                                    else{
                                        if(rand() % 2){
                                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                Mix_PlayMusic(magic_effect, 1);
                                            }
                                            if(rand() % 2){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You've enchanted a Speed Spell!");
                                                game->player->spells[0].count++;
                                            }
                                            else{
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You've enchanted a Power Spell!");
                                                game->player->spells[1].count++;
                                            }
                                        }
                                        else{
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Ah, bad luck, hope you get it next time =(");
                                        }
                                        game->player->specialfoods[3].count -= 2;
                                    }
                                }
                                }
                                mouse_checker = 0;
                                break;
                            case '\n':
                                if(selected_item == 0){
                                    if(game->player->specialfoods[0].count <= 0){
                                        wattroff(inventory_bar_win, COLOR_PAIR(7));
                                        wattron(inventory_bar_win, COLOR_PAIR(3));
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Your Hunger is %d.   ", game->player->foodCount);
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 2, "And you don't have any Special Food :b");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 2, "You need to eat more!   ");
                                        wattroff(inventory_bar_win, COLOR_PAIR(3));
                                        wattron(inventory_bar_win, COLOR_PAIR(7));
                                    }
                                    else if(game->player->health > 80){
                                        if(game->player->foodCount <= 130){
                                            game->player->foodCount += 20;
                                            game->player->specialfoods[0].count--;
                                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                Mix_PlayMusic(magicfood_effect, 1);
                                            }
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You gained 20 food");
                                            wattron(inventory_bar_win, COLOR_PAIR(3));
                                            mvwprintw(inventory_bar_win, 2, 1, UI_TEXT("%s: %d🍓","%s: %d♣"), inventory_items[1], game->player->foodCount);
                                            wattroff(inventory_bar_win, COLOR_PAIR(3));
                                        }
                                        else{
                                            wattroff(inventory_bar_win, COLOR_PAIR(7));
                                            wattron(inventory_bar_win, COLOR_PAIR(3));
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Your Health is %d.", game->player->health);
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 2, "And it can't be more than 100 :b   ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 2, "Get back to the field!   ");
                                            wattroff(inventory_bar_win, COLOR_PAIR(3));
                                            wattron(inventory_bar_win, COLOR_PAIR(7));
                                        }
                                    }
                                    else{
                                        game->player->health += 20;
                                        game->player->specialfoods[0].count--;
                                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                            Mix_PlayMusic(magicfood_effect, 1);
                                        }
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You gained 20 health");
                                        wattron(inventory_bar_win, COLOR_PAIR(3));
                                        mvwprintw(inventory_bar_win, 1, 1, UI_TEXT("%s: %d💗","%s: %d♥"), inventory_items[0], game->player->health);
                                        mvwprintw(inventory_bar_win, 2, 1, UI_TEXT("%s: %d🍓","%s: %d♣"), inventory_items[1], game->player->foodCount);
                                        wattroff(inventory_bar_win, COLOR_PAIR(3));
                                    }
                                }
                                else if(selected_item == 1){
                                    if(game->player->specialfoods[1].count <= 0){
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Not enough foods!");
                                    }
                                    else{
                                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                            Mix_PlayMusic(magicfood_effect, 1);
                                        }
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You used a Power Food.");
                                        game->player->specialfoods[1].count--;
                                        game->player->Power_effect += 10;
                                        game->player->foodCount += 10;
                                    }
                                }
                                else if(selected_item == 2){
                                    if(game->player->specialfoods[2].count <= 0){
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Not enough foods!");
                                    }
                                    else{
                                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                            Mix_PlayMusic(magicfood_effect, 1);
                                        }
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You used a Speed Food.");
                                        game->player->specialfoods[2].count--;
                                        game->player->Speed_effect += 10;
                                        game->player->foodCount += 10;
                                    }
                                }
                                else if(selected_item == 3){
                                mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "                                           ");
                                if(game->player->specialfoods[3].count <= 1){
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Not enough Rotten foods!");
                                }
                                else{
                                    if(rand() % 2){
                                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                            Mix_PlayMusic(magic_effect, 1);
                                        }
                                        if(rand() % 2){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You've enchanted a Speed Spell!");
                                            game->player->spells[0].count++;
                                        }
                                        else{
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "You've enchanted a Power Spell!");
                                            game->player->spells[1].count++;
                                        }
                                    }
                                    else{
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "Ah, bad luck, hope you get it next time =(");
                                    }
                                    game->player->specialfoods[3].count -= 2;
                                }
                            }
                                break;
                            case 27:
            case '\t':
                                tab_check = 1;
                                break;
                        }
                        wattroff(inventory_bar_win, COLOR_PAIR(7));
                    }
                    selected_item = 0;
                }
                else if(selected_option == 1){
                    mvwprintw(inventory_bar_win, inventory_size + options_size + 4, 1, "Choose your weapon type:");
                    char *weapon_types[] = {"Melee Weapon", "Ranged Weapon"};
                    int weapon_types_size = 2;
                    int selected_type = 0;
                    int tab_check1 = 0;
                    while (tab_check1 != 1) {
                        mouse_save.x = mouse_event.x;
                        mouse_save.y = mouse_event.y;
                        if (ch == KEY_MOUSE) getmouse(&mouse_event);
                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                           ");
                        mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "                                       ");
                        wattron(inventory_bar_win, COLOR_PAIR(19));
                        for (int i = 0; i < weapon_types_size; i++) {
                            if (i == selected_type) {
                                wattron(inventory_bar_win, A_REVERSE);
                            }
                            if(i == 0){
                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, UI_TEXT("%s🪓 ","%s♠ "), weapon_types[i]);
                            }
                            else if(i == 1){
                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1 + i * 29, "%s➳ ", weapon_types[i]);
                            }
                            wattroff(inventory_bar_win, A_REVERSE);
                        }
                        wrefresh(inventory_bar_win);
                        ch = music_getch(inventory_bar_win);

                        switch (ch) {
                            case KEY_RIGHT:
                                selected_type = (selected_type - 1 + weapon_types_size) % weapon_types_size;
                                break;
                            case KEY_LEFT:
                                selected_type = (selected_type + 1) % weapon_types_size;
                                break;
                            case KEY_MOUSE:
                                if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y){
                                    for (int i = 0; i < weapon_types_size; i++) {
                                        if(mouse_event.y == y_start + inventory_size + options_size + 5 && mouse_event.x >= x_start + 1 + i * 29 && mouse_event.x <= x_start + 3 + i * 29 + strlen(weapon_types[i])){
                                            selected_type = i;
                                        }
                                    }
                                    for (int i = 0; i < weapon_types_size; i++) {
                                        if (i == selected_type) {
                                            wattron(inventory_bar_win, A_REVERSE);
                                        }
                                        if(i == 0){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, UI_TEXT("%s🪓 ","%s♠ "), weapon_types[i]);
                                        }
                                        else if(i == 1){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1 + i * 29, "%s➳ ", weapon_types[i]);
                                        }
                                        wattroff(inventory_bar_win, A_REVERSE);
                                    }
                                    wrefresh(inventory_bar_win);
                                    usleep(100000);
                                    if (selected_type == 0) { // Melee Weapon
                                        char *melee_weapons[] = {"Mace", "Sword"};
                                        int items_size = 2;
                                        int selected_item = 0;
                                        int tab_check = 0;
                                        while (tab_check != 1) {
                                            mouse_save.x = mouse_event.x;
                                            mouse_save.y = mouse_event.y;
                                            if (ch == KEY_MOUSE) getmouse(&mouse_event);
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 1, "                                            ");
                                            for (int i = 0; i < items_size; i++) {
                                                if (i == selected_item) {
                                                    wattron(inventory_bar_win, A_REVERSE);
                                                }
                                                if(i == 0){
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Mace⚒ Damage: 5","● Mace♠ Damage: 5"));
                                                }
                                                else if(i == 1){
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Sword⚔ Sharpness: %d Damage: %d","● Sword† Sharpness: %d Damage: %d"), game->player->weapons[4].count, game->player->weapons[4].count * 5);
                                                }
                                                wattroff(inventory_bar_win, A_REVERSE);
                                            }
                                            wrefresh(inventory_bar_win);
                                            ch = music_getch(inventory_bar_win);
    
                                            switch (ch) {
                                                case KEY_UP:
                                                    selected_item = (selected_item - 1 + items_size) % items_size;
                                                    break;
                                                case KEY_DOWN:
                                                    selected_item = (selected_item + 1) % items_size;
                                                    break;
                                                case KEY_MOUSE:
                                                    if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y){
                                                        for (int i = 0; i < items_size; i++) {
                                                            if(mouse_event.y == y_start + inventory_size + options_size + 5 + i && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 20){
                                                                selected_item = i;
                                                            }
                                                            else if(mouse_event.y == y_start + inventory_size + options_size + 5 + i && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 38 && i == 1){
                                                                selected_item = i;
                                                            }
                                                        }
                                                        for (int i = 0; i < items_size; i++) {
                                                            if (i == selected_item) {
                                                                wattron(inventory_bar_win, A_REVERSE);
                                                            }
                                                            if(i == 0){
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Mace⚒ Damage: 5","● Mace♠ Damage: 5"));
                                                            }
                                                            else if(i == 1){
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Sword⚔ Sharpness: %d Damage: %d","● Sword† Sharpness: %d Damage: %d"), game->player->weapons[4].count, game->player->weapons[4].count * 5);
                                                            }
                                                            wattroff(inventory_bar_win, A_REVERSE);
                                                        }
                                                        wrefresh(inventory_bar_win);
                                                        usleep(100000);
                                                        wattroff(inventory_bar_win, COLOR_PAIR(19));
                                                        wattron(inventory_bar_win, COLOR_PAIR(3));
                                                        if (game->player->weapons[4].count <= 0 && selected_item == 1) {
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You don't have that weapon!                ");
                                                        } else {
                                                            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                                Mix_PlayMusic(weaponpickup_effect, 1);
                                                            }
                                                            if(selected_item == 0){
                                                                game->player->currentWeapon = game->player->weapons[0];
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You choosed %s           ", game->player->weapons[0].name);
                                                            }
                                                            if(selected_item == 1){
                                                                game->player->currentWeapon = game->player->weapons[4];
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You choosed %s           ", game->player->weapons[4].name);
                                                            }
                                                        }
                                                        wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                        wattron(inventory_bar_win, COLOR_PAIR(19));
                                                        tab_check = 1;
                                                    }
                                                    break;
                                                case '\n':
                                                    wattroff(inventory_bar_win, COLOR_PAIR(19));
                                                    wattron(inventory_bar_win, COLOR_PAIR(3));
                                                    if (game->player->weapons[4].count <= 0 && selected_item == 1) {
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You don't have that weapon!                ");
                                                    } else {
                                                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                            Mix_PlayMusic(weaponpickup_effect, 1);
                                                        }
                                                        if(selected_item == 0){
                                                            game->player->currentWeapon = game->player->weapons[0];
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You choosed %s           ", game->player->weapons[0].name);
                                                        }
                                                        if(selected_item == 1){
                                                            game->player->currentWeapon = game->player->weapons[4];
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You choosed %s           ", game->player->weapons[4].name);
                                                        }
                                                    }
                                                    wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                    wattron(inventory_bar_win, COLOR_PAIR(19));
                                                    tab_check = 1;
                                                    break;
                                                case 27:
            case '\t':
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                           ");
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 2, "                                       ");
                                                    tab_check = 1;
                                                    break;
                                            }
                                        }
                                    } else if (selected_type == 1) { // Ranged Weapon
                                        char *ranged_weapons[] = {"Dagger", "Magic Wand", "Normal Arrow"};
                                        int items_size = 3;
                                        int selected_item = 0;
                                        int tab_check = 0;
                                        while (tab_check != 1) {
                                            mouse_save.x = mouse_event.x;
                                            mouse_save.y = mouse_event.y;
                                            if (ch == KEY_MOUSE) getmouse(&mouse_event);
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 1, "                                            ");
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 1, "                                            ");
                                            for (int i = 0; i < items_size; i++) {
                                                if (i == selected_item) {
                                                    wattron(inventory_bar_win, A_REVERSE);
                                                }
                                                if(i == 0){
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Dagger🗡 Damage: 12 Range: 5 Count: %d","● Dagger† Damage: 12 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                }
                                                else if(i == 1){
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, "● MagicWand⚚ Damage: 15 Range: 10 Count: %d", game->player->weapons[i + 1].count);
                                                }
                                                else if(i == 2){
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Bow🏹 Damage: 5 Range: 5 Count: %d","● Bow➳ Damage: 5 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                }
                                                wattroff(inventory_bar_win, A_REVERSE);
                                            }
                                            wrefresh(inventory_bar_win);
                                            ch = music_getch(inventory_bar_win);
    
                                            switch (ch) {
                                                case KEY_UP:
                                                    selected_item = (selected_item - 1 + items_size) % items_size;
                                                    break;
                                                case KEY_DOWN:
                                                    selected_item = (selected_item + 1) % items_size;
                                                    break;
                                                case KEY_MOUSE:
                                                if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y){
                                                    for (int i = 0; i < items_size; i++) {
                                                        if(mouse_event.y == y_start + inventory_size + options_size + 5 + i && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 35){
                                                            selected_item = i;
                                                        }
                                                        if(i == 0){
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Dagger🗡 Damage: 12 Range: 5 Count: %d","● Dagger† Damage: 12 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                        }
                                                        else if(i == 1){
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, "● MagicWand⚚ Damage: 15 Range: 10 Count: %d", game->player->weapons[i + 1].count);
                                                        }
                                                        else if(i == 2){
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Bow🏹 Damage: 5 Range: 5 Count: %d","● Bow➳ Damage: 5 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                        }
                                                        wattroff(inventory_bar_win, A_REVERSE);
                                                    }
                                                    for (int i = 0; i < items_size; i++) {
                                                        if (i == selected_item) {
                                                            wattron(inventory_bar_win, A_REVERSE);
                                                        }
                                                        if(i == 0){
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Dagger🗡 Damage: 12 Range: 5 Count: %d","● Dagger† Damage: 12 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                        }
                                                        else if(i == 1){
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, "● MagicWand⚚ Damage: 15 Range: 10 Count: %d", game->player->weapons[i + 1].count);
                                                        }
                                                        else if(i == 2){
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Bow🏹 Damage: 5 Range: 5 Count: %d","● Bow➳ Damage: 5 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                        }
                                                        wattroff(inventory_bar_win, A_REVERSE);
                                                    }
                                                    wrefresh(inventory_bar_win);
                                                    usleep(100000);
                                                    wattroff(inventory_bar_win, COLOR_PAIR(19));
                                                    wattron(inventory_bar_win, COLOR_PAIR(3));
                                                    if (game->player->weapons[selected_item + 1].count <= 0) {
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 1, "                                            ");
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "You don't have that projectile!         ");
                                                    } else {
                                                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                            Mix_PlayMusic(weaponpickup_effect, 1);
                                                        }
                                                        game->player->currentWeapon = game->player->weapons[selected_item + 1];
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "                                           ");
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "You choosed %s              ", game->player->weapons[selected_item + 1].name);
                                                    }
                                                    tab_check = 1;
                                                    wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                    wattron(inventory_bar_win, COLOR_PAIR(19));
                                                }
                                                    break;
                                                case '\n':
                                                    wattroff(inventory_bar_win, COLOR_PAIR(19));
                                                    wattron(inventory_bar_win, COLOR_PAIR(3));
                                                    if (game->player->weapons[selected_item + 1].count <= 0) {
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 1, "                                            ");
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "You don't have that projectile!         ");
                                                    } else {
                                                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                            Mix_PlayMusic(weaponpickup_effect, 1);
                                                        }
                                                        game->player->currentWeapon = game->player->weapons[selected_item + 1];
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "                                           ");
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "You choosed %s              ", game->player->weapons[selected_item + 1].name);
                                                    }
                                                    tab_check = 1;
                                                    wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                    wattron(inventory_bar_win, COLOR_PAIR(19));
                                                    break;
                                                case 27:
            case '\t':
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                           ");
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 2, "                                           ");
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "                                       ");
                                                    tab_check = 1;
                                                    break;
                                            }
                                        }
                                    }
                                }
                                break;
                            case '\n':
                                for (int i = 0; i < weapon_types_size; i++) {
                                    if(mouse_event.y == y_start + inventory_size + options_size + 5 && mouse_event.x >= x_start + 1 + i * 29 && mouse_event.x <= x_start + 3 + i * 29 + strlen(weapon_types[i])){
                                        selected_type = i;
                                    }
                                }
                                for (int i = 0; i < weapon_types_size; i++) {
                                    if (i == selected_type) {
                                        wattron(inventory_bar_win, A_REVERSE);
                                    }
                                    if(i == 0){
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, UI_TEXT("%s🪓 ","%s♠ "), weapon_types[i]);
                                    }
                                    else if(i == 1){
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1 + i * 29, "%s➳ ", weapon_types[i]);
                                    }
                                    wattroff(inventory_bar_win, A_REVERSE);
                                }
                                wrefresh(inventory_bar_win);
                                usleep(100000);
                                if (selected_type == 0) { // Melee Weapon
                                    char *melee_weapons[] = {"Mace", "Sword"};
                                    int items_size = 2;
                                    int selected_item = 0;
                                    int tab_check = 0;
                                    while (tab_check != 1) {
                                        mouse_save.x = mouse_event.x;
                                        mouse_save.y = mouse_event.y;
                                        if (ch == KEY_MOUSE) getmouse(&mouse_event);
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                            ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 1, "                                            ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 1, "                                            ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 1, "                                            ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 1, "                                            ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 1, "                                            ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 1, "                                            ");
                                        for (int i = 0; i < items_size; i++) {
                                            if (i == selected_item) {
                                                wattron(inventory_bar_win, A_REVERSE);
                                            }
                                            if(i == 0){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Mace⚒ Damage: 5","● Mace♠ Damage: 5"));
                                            }
                                            else if(i == 1){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Sword⚔ Sharpness: %d Damage: %d","● Sword† Sharpness: %d Damage: %d"), game->player->weapons[4].count, game->player->weapons[4].count * 5);
                                            }
                                            wattroff(inventory_bar_win, A_REVERSE);
                                        }
                                        wrefresh(inventory_bar_win);
                                        ch = music_getch(inventory_bar_win);

                                        switch (ch) {
                                            case KEY_UP:
                                                selected_item = (selected_item - 1 + items_size) % items_size;
                                                break;
                                            case KEY_DOWN:
                                                selected_item = (selected_item + 1) % items_size;
                                                break;
                                            case KEY_MOUSE:
                                                if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y){
                                                    for (int i = 0; i < items_size; i++) {
                                                        if(mouse_event.y == y_start + inventory_size + options_size + 5 + i && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 20){
                                                            selected_item = i;
                                                        }
                                                        else if(mouse_event.y == y_start + inventory_size + options_size + 5 + i && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 38 && i == 1){
                                                            selected_item = i;
                                                        }
                                                    }
                                                    for (int i = 0; i < items_size; i++) {
                                                        if (i == selected_item) {
                                                            wattron(inventory_bar_win, A_REVERSE);
                                                        }
                                                        if(i == 0){
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Mace⚒ Damage: 5","● Mace♠ Damage: 5"));
                                                        }
                                                        else if(i == 1){
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Sword⚔ Sharpness: %d Damage: %d","● Sword† Sharpness: %d Damage: %d"), game->player->weapons[4].count, game->player->weapons[4].count * 5);
                                                        }
                                                        wattroff(inventory_bar_win, A_REVERSE);
                                                    }
                                                    wrefresh(inventory_bar_win);
                                                    usleep(100000);
                                                    wattroff(inventory_bar_win, COLOR_PAIR(19));
                                                    wattron(inventory_bar_win, COLOR_PAIR(3));
                                                    if (game->player->weapons[4].count <= 0 && selected_item == 1) {
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You don't have that weapon!                ");
                                                    } else {
                                                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                            Mix_PlayMusic(weaponpickup_effect, 1);
                                                        }
                                                        if(selected_item == 0){
                                                            game->player->currentWeapon = game->player->weapons[0];
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You choosed %s           ", game->player->weapons[0].name);
                                                        }
                                                        if(selected_item == 1){
                                                            game->player->currentWeapon = game->player->weapons[4];
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You choosed %s           ", game->player->weapons[4].name);
                                                        }
                                                    }
                                                    wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                    wattron(inventory_bar_win, COLOR_PAIR(19));
                                                    tab_check = 1;
                                                }
                                                break;
                                            case '\n':
                                                wattroff(inventory_bar_win, COLOR_PAIR(19));
                                                wattron(inventory_bar_win, COLOR_PAIR(3));
                                                if (game->player->weapons[4].count <= 0 && selected_item == 1) {
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You don't have that weapon!                ");
                                                } else {
                                                    if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                        Mix_PlayMusic(weaponpickup_effect, 1);
                                                    }
                                                    if(selected_item == 0){
                                                        game->player->currentWeapon = game->player->weapons[0];
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You choosed %s           ", game->player->weapons[0].name);
                                                    }
                                                    if(selected_item == 1){
                                                        game->player->currentWeapon = game->player->weapons[4];
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 1, "                                            ");
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 4 + items_size, 2, "You choosed %s           ", game->player->weapons[4].name);
                                                    }
                                                }
                                                wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                wattron(inventory_bar_win, COLOR_PAIR(19));
                                                tab_check = 1;
                                                break;
                                            case 27:
            case '\t':
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                           ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 2, "                                       ");
                                                tab_check = 1;
                                                break;
                                        }
                                    }
                                } else if (selected_type == 1) { // Ranged Weapon
                                    char *ranged_weapons[] = {"Dagger", "Magic Wand", "Normal Arrow"};
                                    int items_size = 3;
                                    int selected_item = 0;
                                    int tab_check = 0;
                                    while (tab_check != 1) {
                                        mouse_save.x = mouse_event.x;
                                        mouse_save.y = mouse_event.y;
                                        if (ch == KEY_MOUSE) getmouse(&mouse_event);
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                            ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 1, "                                            ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 1, "                                            ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 1, "                                            ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 1, "                                            ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 10, 1, "                                            ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 11, 1, "                                            ");
                                        for (int i = 0; i < items_size; i++) {
                                            if (i == selected_item) {
                                                wattron(inventory_bar_win, A_REVERSE);
                                            }
                                            if(i == 0){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Dagger🗡 Damage: 12 Range: 5 Count: %d","● Dagger† Damage: 12 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                            }
                                            else if(i == 1){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, "● MagicWand⚚ Damage: 15 Range: 10 Count: %d", game->player->weapons[i + 1].count);
                                            }
                                            else if(i == 2){
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Bow🏹 Damage: 5 Range: 5 Count: %d","● Bow➳ Damage: 5 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                            }
                                            wattroff(inventory_bar_win, A_REVERSE);
                                        }
                                        wrefresh(inventory_bar_win);
                                        ch = music_getch(inventory_bar_win);

                                        switch (ch) {
                                            case KEY_UP:
                                                selected_item = (selected_item - 1 + items_size) % items_size;
                                                break;
                                            case KEY_DOWN:
                                                selected_item = (selected_item + 1) % items_size;
                                                break;
                                            case KEY_MOUSE:
                                            if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y){
                                                for (int i = 0; i < items_size; i++) {
                                                    if(mouse_event.y == y_start + inventory_size + options_size + 5 + i && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 35){
                                                        selected_item = i;
                                                    }
                                                    if(i == 0){
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Dagger🗡 Damage: 12 Range: 5 Count: %d","● Dagger† Damage: 12 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                    }
                                                    else if(i == 1){
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, "● MagicWand⚚ Damage: 15 Range: 10 Count: %d", game->player->weapons[i + 1].count);
                                                    }
                                                    else if(i == 2){
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Bow🏹 Damage: 5 Range: 5 Count: %d","● Bow➳ Damage: 5 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                    }
                                                    wattroff(inventory_bar_win, A_REVERSE);
                                                }
                                                for (int i = 0; i < items_size; i++) {
                                                    if (i == selected_item) {
                                                        wattron(inventory_bar_win, A_REVERSE);
                                                    }
                                                    if(i == 0){
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Dagger🗡 Damage: 12 Range: 5 Count: %d","● Dagger† Damage: 12 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                    }
                                                    else if(i == 1){
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, "● MagicWand⚚ Damage: 15 Range: 10 Count: %d", game->player->weapons[i + 1].count);
                                                    }
                                                    else if(i == 2){
                                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● Bow🏹 Damage: 5 Range: 5 Count: %d","● Bow➳ Damage: 5 Range: 5 Count: %d"), game->player->weapons[i + 1].count);
                                                    }
                                                    wattroff(inventory_bar_win, A_REVERSE);
                                                }
                                                wrefresh(inventory_bar_win);
                                                usleep(100000);
                                                wattroff(inventory_bar_win, COLOR_PAIR(19));
                                                wattron(inventory_bar_win, COLOR_PAIR(3));
                                                if (game->player->weapons[selected_item + 1].count <= 0) {
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 1, "                                            ");
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "You don't have that projectile!         ");
                                                } else {
                                                    if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                        Mix_PlayMusic(weaponpickup_effect, 1);
                                                    }
                                                    game->player->currentWeapon = game->player->weapons[selected_item + 1];
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "                                           ");
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "You choosed %s              ", game->player->weapons[selected_item + 1].name);
                                                }
                                                tab_check = 1;
                                                wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                wattron(inventory_bar_win, COLOR_PAIR(19));
                                            }
                                                break;
                                            case '\n':
                                                wattroff(inventory_bar_win, COLOR_PAIR(19));
                                                wattron(inventory_bar_win, COLOR_PAIR(3));
                                                if (game->player->weapons[selected_item + 1].count <= 0) {
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 1, "                                            ");
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "You don't have that projectile!         ");
                                                } else {
                                                    if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                                        Mix_PlayMusic(weaponpickup_effect, 1);
                                                    }
                                                    game->player->currentWeapon = game->player->weapons[selected_item + 1];
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "                                           ");
                                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 3 + items_size, 2, "You choosed %s              ", game->player->weapons[selected_item + 1].name);
                                                }
                                                tab_check = 1;
                                                wattroff(inventory_bar_win, COLOR_PAIR(3));
                                                wattron(inventory_bar_win, COLOR_PAIR(19));
                                                break;
                                            case 27:
            case '\t':
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 1, "                                           ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 6, 2, "                                           ");
                                                mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "                                       ");
                                                tab_check = 1;
                                                break;
                                        }
                                    }
                                }
                                break;
                            case 27:
            case '\t':
                                tab_check1 = 1;
                                mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "                                       ");
                                break;
                        }
                        wattroff(inventory_bar_win, COLOR_PAIR(19));
                    }
                }
                else if(selected_option == 2){
                    mvwprintw(inventory_bar_win, inventory_size + options_size + 4, 1, "Choose your spell:");
                    items_size = 3;
                    int tab_check = 0;
                    while(tab_check != 1){
                        mouse_save.x = mouse_event.x;
                        mouse_save.y = mouse_event.y;
                        if (ch == KEY_MOUSE) getmouse(&mouse_event);
                        wattron(inventory_bar_win, COLOR_PAIR(17));
                        for(int i = 0; i < items_size; i++){
                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, "                            ");
                            if (i == selected_item) {
                                wattron(inventory_bar_win, A_REVERSE);
                            }
                            if(i == 0){
                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● %s🍾 %d","● %s✦ %d"), game->player->spells[i].name, game->player->spells[i].count);
                            }
                            if(i == 1){
                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● %s🍷 %d","● %s◆ %d"), game->player->spells[i].name, game->player->spells[i].count);
                            }
                            if(i == 2){
                                mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● %s🍸 %d","● %s✧ %d"), game->player->spells[i].name, game->player->spells[i].count);
                            }
                            wattroff(inventory_bar_win, A_REVERSE);
                        }
                        wrefresh(inventory_bar_win);
                        ch = music_getch(inventory_bar_win);
                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "                                    ");
                        switch (ch) {
                            case KEY_UP:
                                selected_item = (selected_item - 1 + items_size) % items_size;
                                break;
                            case KEY_DOWN:
                                selected_item = (selected_item + 1) % items_size;
                                break;
                            case KEY_MOUSE:
                                if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y){
                                    for(int i = 0; i < items_size; i++){
                                        if(mouse_event.y == y_start + inventory_size + options_size + 5 + i && mouse_event.x >= x_start + 2 && mouse_event.x <= x_start + 10 + strlen(game->player->spells[i].name)){
                                            selected_item = i;
                                        }
                                    }
                                    for(int i = 0; i < items_size; i++){
                                        if (i == selected_item) {
                                            wattron(inventory_bar_win, A_REVERSE);
                                        }
                                        if(i == 0){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● %s🍾 %d","● %s✦ %d"), game->player->spells[i].name, game->player->spells[i].count);
                                        }
                                        if(i == 1){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● %s🍷 %d","● %s◆ %d"), game->player->spells[i].name, game->player->spells[i].count);
                                        }
                                        if(i == 2){
                                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, UI_TEXT("● %s🍸 %d","● %s✧ %d"), game->player->spells[i].name, game->player->spells[i].count);
                                        }
                                        wattroff(inventory_bar_win, A_REVERSE);
                                    }
                                    wrefresh(inventory_bar_win);
                                    wattroff(inventory_bar_win, COLOR_PAIR(17));
                                    wattron(inventory_bar_win, COLOR_PAIR(3));
                                    if(game->player->spells[selected_item].count <= 0){
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "                                    ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "Not enough spells");  
                                        wattroff(inventory_bar_win, COLOR_PAIR(3));
                                        wattron(inventory_bar_win, COLOR_PAIR(17));
                                        break;
                                    }
                                    else{
                                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                            Mix_PlayMusic(magic_effect, 1);
                                        }
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "                                    ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "You used %s", game->player->spells[selected_item].name);
                                        game->player->spells[selected_item].count--;
                                        game->player->spellCount--;
                                        if(game->player->spells[selected_item].type == 's'){
                                            game->player->Speed_effect += 15;
                                        }
                                        if(game->player->spells[selected_item].type == 'p'){
                                            game->player->Power_effect += 15;
                                        }
                                        if(game->player->spells[selected_item].type == 'h'){
                                            game->player->Health_effect += 15;
                                        }
                                        k = 9;
                                        mvwprintw(inventory_bar_win, 6, 9, "                             ");
                                        for(int i = 0; i < 3; i++){
                                            if(i == 0){
                                                mvwprintw(inventory_bar_win, 6, k, UI_TEXT("Speed %d🍾","Speed %d✦"), game->player->spells[i].count);
                                            }
                                            else if(i == 1){
                                                mvwprintw(inventory_bar_win, 6, k, UI_TEXT("Power %d🍷","Power %d◆"), game->player->spells[i].count);
                                            }
                                            else if(i == 2){
                                                mvwprintw(inventory_bar_win, 6, k, UI_TEXT("Healing %d🍸","Healing %d✧"), game->player->spells[i].count);
                                            }
                                            int a = 0;
                                            if(game->player->spells[i].count >= 10){
                                                a = 1;
                                            }
                                            k += 10 + a; 
                                        }
                                        wattroff(inventory_bar_win, COLOR_PAIR(3));
                                        wattron(inventory_bar_win, COLOR_PAIR(17));
                                        break;
                                    }
                                }
                                break;
                            case '\n':
                                wattroff(inventory_bar_win, COLOR_PAIR(17));
                                wattron(inventory_bar_win, COLOR_PAIR(3));
                                if(game->player->spells[selected_item].count <= 0){
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "                                    ");
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "Not enough spells");  
                                    wattroff(inventory_bar_win, COLOR_PAIR(3));
                                    wattron(inventory_bar_win, COLOR_PAIR(17));
                                    break;
                                }
                                else{
                                    if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                        Mix_PlayMusic(magic_effect, 1);
                                    }
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "                                    ");
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + items_size, 2, "You used %s", game->player->spells[selected_item].name);
                                    game->player->spells[selected_item].count--;
                                    game->player->spellCount--;
                                    if(game->player->spells[selected_item].type == 's'){
                                        game->player->Speed_effect += 15;
                                    }
                                    if(game->player->spells[selected_item].type == 'p'){
                                        game->player->Power_effect += 15;
                                    }
                                    if(game->player->spells[selected_item].type == 'h'){
                                        game->player->Health_effect += 15;
                                    }
                                    k = 9;
                                    mvwprintw(inventory_bar_win, 6, 9, "                             ");
                                    for(int i = 0; i < 3; i++){
                                        if(i == 0){
                                            mvwprintw(inventory_bar_win, 6, k, UI_TEXT("Speed %d🍾","Speed %d✦"), game->player->spells[i].count);
                                        }
                                        else if(i == 1){
                                            mvwprintw(inventory_bar_win, 6, k, UI_TEXT("Power %d🍷","Power %d◆"), game->player->spells[i].count);
                                        }
                                        else if(i == 2){
                                            mvwprintw(inventory_bar_win, 6, k, UI_TEXT("Healing %d🍸","Healing %d✧"), game->player->spells[i].count);
                                        }
                                        int a = 0;
                                        if(game->player->spells[i].count >= 10){
                                            a = 1;
                                        }
                                        k += 10 + a; 
                                    }
                                    wattroff(inventory_bar_win, COLOR_PAIR(3));
                                    wattron(inventory_bar_win, COLOR_PAIR(17));
                                    break;
                                }
                            case 27:
            case '\t':
                                tab_check = 1;
                                break;
                        }
                        wattroff(inventory_bar_win, COLOR_PAIR(17));
                    }
                    selected_item = 0;
                }
                else if(selected_option == 3){
                    mvwprintw(inventory_bar_win, inventory_size + options_size + 4, 1, "You have %d broken Ancientkeys:  ", game->player->brokenAcientKey);
                    items_size = 1;
                    int tab_check = 0;
                    while(tab_check != 1){
                        mouse_save.x = mouse_event.x;
                        mouse_save.y = mouse_event.y;
                        if (ch == KEY_MOUSE) getmouse(&mouse_event);
                        mvwprintw(inventory_bar_win, inventory_size + options_size + 4, 1, "You have %d broken Ancientkeys:  ", game->player->brokenAcientKey);
                        wattron(inventory_bar_win, COLOR_PAIR(12));
                        for(int i = 0; i < 1; i++){
                            if (i == selected_item) {
                                wattron(inventory_bar_win, A_REVERSE);
                            }
                            mvwprintw(inventory_bar_win, inventory_size + options_size + 5 + i, 2, "● Forge");
                            wattroff(inventory_bar_win, A_REVERSE);
                        }
                        wrefresh(inventory_bar_win);
                        ch = music_getch(inventory_bar_win);
                        switch (ch) {
                            case KEY_UP:
                                selected_item = (selected_item - 1 + items_size) % items_size;
                                break;
                            case KEY_DOWN:
                                selected_item = (selected_item + 1) % items_size;
                                break;
                            case KEY_MOUSE:
                                if(mouse_save.x == mouse_event.x && mouse_save.y == mouse_event.y){
                                    if(mouse_event.y == y_start + inventory_size + options_size + 5 && mouse_event.x >= x_start + 2 && mouse_event.x >= x_start + 9){
                                        selected_item = 0;
                                    }
                                    wattron(inventory_bar_win, A_REVERSE);
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 5, 2, "● Forge");
                                    wattroff(inventory_bar_win, A_REVERSE);
                                    wrefresh(inventory_bar_win);
                                    if(game->player->brokenAcientKey <= 1){
                                        wattroff(inventory_bar_win, COLOR_PAIR(12));
                                        wattron(inventory_bar_win, COLOR_PAIR(3));
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "Not enough broken Ancientkeys.  ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 2, "                              ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "                              ");
                                        wattroff(inventory_bar_win, COLOR_PAIR(3));
                                        wattron(inventory_bar_win, COLOR_PAIR(12));
                                    }
                                    else{
                                        wattroff(inventory_bar_win, COLOR_PAIR(12));
                                        wattron(inventory_bar_win, COLOR_PAIR(3));
                                        game->player->acientKey++;
                                        game->player->brokenAcientKey -= 2;
                                        if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                            Mix_PlayMusic(forge_effect, 1);
                                        }
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "Forged Successfully.     ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 2, "                        ");
                                        mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "                    ");
                                        wattroff(inventory_bar_win, COLOR_PAIR(3));
                                        wattron(inventory_bar_win, COLOR_PAIR(12));
                                    }
                                    wattron(inventory_bar_win, COLOR_PAIR(3));
                                    mvwprintw(inventory_bar_win, 7, 1, "Ancient Key: %d", game->player->acientKey);
                                    mvwprintw(inventory_bar_win, 8, 1, "Broken Ancient Key: %d", game->player->brokenAcientKey);
                                    wattroff(inventory_bar_win, COLOR_PAIR(3));
                                }
                                break;
                            case '\n':
                                if(game->player->brokenAcientKey <= 1){
                                    wattroff(inventory_bar_win, COLOR_PAIR(12));
                                    wattron(inventory_bar_win, COLOR_PAIR(3));
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "Not enough broken Ancientkeys.  ");
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 2, "                              ");
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "                              ");
                                    wattroff(inventory_bar_win, COLOR_PAIR(3));
                                    wattron(inventory_bar_win, COLOR_PAIR(12));
                                }
                                else{
                                    wattroff(inventory_bar_win, COLOR_PAIR(12));
                                    wattron(inventory_bar_win, COLOR_PAIR(3));
                                    game->player->acientKey++;
                                    game->player->brokenAcientKey -= 2;
                                    if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                                        Mix_PlayMusic(forge_effect, 1);
                                    }
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 7, 2, "Forged Successfully.     ");
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 8, 2, "                        ");
                                    mvwprintw(inventory_bar_win, inventory_size + options_size + 9, 2, "                    ");
                                    wattroff(inventory_bar_win, COLOR_PAIR(3));
                                    wattron(inventory_bar_win, COLOR_PAIR(12));
                                }
                                wattron(inventory_bar_win, COLOR_PAIR(3));
                                mvwprintw(inventory_bar_win, 7, 1, "Ancient Key: %d", game->player->acientKey);
                                mvwprintw(inventory_bar_win, 8, 1, "Broken Ancient Key: %d", game->player->brokenAcientKey);
                                wattroff(inventory_bar_win, COLOR_PAIR(3));
                                break;
                            case 27:
            case '\t':
                                tab_check = 1;
                                break;
                        }
                        wattroff(inventory_bar_win, COLOR_PAIR(12));
                    }
                    selected_item = 0;
                }
                // mvwprintw(inventory_bar_win, inventory_size + options_size + 2, 1, "Option Under %s:", options[selected_option]);
                // mvwprintw(inventory_bar_win, inventory_size + options_size + 3, 1, " [Sample Choice 1]");
                // mvwprintw(inventory_bar_win, inventory_size + options_size + 4, 1, " [Sample Choice 2]");
                wrefresh(inventory_bar_win);
                // music_getch(inventory_bar_win);
                break;
        }
    }
    werase(inventory_bar_win);
    wrefresh(inventory_bar_win);
    delwin(inventory_bar_win);
}
void x_print(WINDOW *game_win, int x, int y){
    if(game->whole_map[game->currentLevel][y][x].item == '^'){
        wattron(game_win, COLOR_PAIR(10));
        mvwprintw(game_win, y, x, "^");
        wattroff(game_win, COLOR_PAIR(10));
    }
    else if(game->whole_map[game->currentLevel][y][x].item == '?'){
        wattron(game_win, COLOR_PAIR(10));
        mvwprintw(game_win, y, x, "‽");
        wattroff(game_win, COLOR_PAIR(10));
    }
}
static void controls_help(WINDOW *game_win) {
    int h,w; getmaxyx(game_win,h,w);
    WINDOW *help=newwin(23,76,(h-23)/2,(w-76)/2);
    if (!help) return;
    ui_frame(help,"FIELD GUIDE");
    const char *lines[]={"MOVEMENT & COMBAT", "WASD / arrows    Move; Q / Z / C / Page Up move diagonally",
        "Ranged: 1) SPACE to aim  2) Arrow / WASD to FIRE. Esc cancels.",
        "E / left click  Repeat shot / shoot toward mouse (8-way keys too)",
        "Tab             Inventory, equipment, food and spells",
        "Enter / < / >   Use the staircase under your feet",
        "Esc             Pause, change character / colour, save and quit", "",
        "THE DUNGEON", "@ You    D/F/G/S/U Enemies    ^ Trap    ≡ Stairs    ¤ Gold",
        "♣ Food   ◇ Shield   † Blade   ➳ Arrow   ✦ Spell   Δ Ancient key",
        "Cyan passages connect rooms. Search walls for hidden entrances.",
        "Green sanctuary: recover health. Cyan forge: recover shields.",
        "Some rooms restrict combat. Darkness hides what lies ahead.",
        "WAND: arrows shoot (1 charge); WASD rides the spell (2).",
        "Timmy teaches new discoveries in the lower-right corner.",
        "Guest runs are temporary; create a hero to save your journey.", "", "Press any key to return"};
    for(int i=0;i<19;i++) {
        wattron(help,COLOR_PAIR(i==0||i==8?12:10));mvwaddstr(help,i+2,3,lines[i]);
        wattroff(help,COLOR_PAIR(i==0||i==8?12:10));
    }
    wrefresh(help);music_getch(help);delwin(help);touchwin(game_win);wrefresh(game_win);
}
int mouse_attack_event(const MEVENT *event) {
    return event && event->x>=0 && event->x<MAP_COLS && event->y>=0 && event->y<MAP_ROWS
        && (event->bstate & (BUTTON1_PRESSED|BUTTON1_CLICKED));
}
void player_move(WINDOW *game_win, time_t start_time){
    time(&move_time);
    time(&move_time_2);
    time(&move_time_lock);
    time(&quote_time);
    time(&last_room1);
    count_move = 1;
    int map_width, map_height;
    getmaxyx(stdscr, map_height, map_width);
    if (map_height > MAP_ROWS) map_height = MAP_ROWS;
    if (map_width > MAP_COLS) map_width = MAP_COLS;
    int start_x = BORDER_PADDING;
    int start_y = BORDER_PADDING;
    int end_x = map_width * 3 / 4 - BORDER_PADDING;
    int end_y = map_height * 3 / 4 - BORDER_PADDING + 5;
    game->map_height = map_height - 1;
    game->map_width = map_width - 1;
    poisonous_food_check = 0;
    int window_check = 0;
    direction = 'w';
    g_check = 0;
    f_check = 0;
    x_check = 0;
    m_check = 0;
    time(&move_time);
    time(&rotten_time);
    time(&quote_time2);
    { //First printings.
        wattron(game_win, COLOR_PAIR(15));
        mvwprintw(game_win, 1, 4, "┌───────Messages────────────────────────────────────────────────────────────┐");
        mvwprintw(game_win, 2, 4, "│                                                                           │");
        mvwprintw(game_win, 3, 4, "│                                                                           │");
        mvwprintw(game_win, 4, 4, "└───────────────────────────────────────────────────────────────────────────┘");
        mvwprintw(game_win, 1, 81, "┌───────Combat-Report───────────────────────────────────────────────────────┐");
        mvwprintw(game_win, 2, 81, "│                                                                           │");
        mvwprintw(game_win, 3, 81, "│                                                                           │");
        mvwprintw(game_win, 4, 81, "└───────────────────────────────────────────────────────────────────────────┘");
        draw_status_hud(game_win);
        mvwhline(game_win, start_y - 1, start_x - 1, ACS_HLINE, map_width - 11);
        mvwhline(game_win, end_y, start_x - 1, ACS_HLINE, map_width - 11);
        mvwprintw(game_win, start_y - 1, start_x - 2, "╭");
        mvwprintw(game_win, start_y - 1, map_width - 6, "╮");
        mvwprintw(game_win, end_y, start_x - 2, "╰");
        mvwprintw(game_win, end_y, map_width - 6, "╯");
        for(int i = start_y; i < end_y; i++){
            mvwprintw(game_win, i, start_x - 2, "│");
            mvwprintw(game_win, i, map_width - 6, "│");
        }
        wattroff(game_win, COLOR_PAIR(15));
        if(color_pair_num == 0){
            wattron(game_win, COLOR_PAIR(14));
        }
        else{
            wattron(game_win, COLOR_PAIR(color_pair_num));
        }
        mvwprintw(game_win, map_height - 8, map_width - 22, "█████████████████");
        mvwprintw(game_win, map_height - 7, map_width - 22, "█████████████████");
        mvwprintw(game_win, map_height - 6, map_width - 22, "█████████████████");
        mvwprintw(game_win, map_height - 5, map_width - 22, "███  ████  ██████");
        mvwprintw(game_win, map_height - 4, map_width - 22, "█████    ████████");
        mvwprintw(game_win, map_height - 3, map_width - 22, "█████████████████");
        if(color_pair_num == 0){
            wattroff(game_win, COLOR_PAIR(14));
        }
        if(game->currentLevel == 0){
            wattron(game_win, COLOR_PAIR(color_pair_num));
            mvwprintw(game_win, map_height - 11, map_width - 77, "╭──────────────────────────────────────────────────╮");
            mvwprintw(game_win, map_height - 10, map_width - 77, UI_TEXT("│ Hi, welcome to AID Rouge Dungeon🎆🎇             │","│ Hi, welcome to AID Rouge Dungeon✦✧             │"));
            mvwprintw(game_win, map_height - 9, map_width - 77, "│ My name is Timmy and I'm Arad's best friend :D   │");
            mvwprintw(game_win, map_height - 8, map_width - 77, "│ I'm your guide through this adventurous journey. │");
            mvwprintw(game_win, map_height - 7, map_width - 77, "│ And I'll entertain you with my quotes =)         │");
            mvwprintw(game_win, map_height - 6, map_width - 77, "│ Important messages are show with your color!     │");
            mvwprintw(game_win, map_height - 5, map_width - 77, UI_TEXT("│ Hope you enjoy this game and also survive it😃  _⧹","│ Hope you enjoy this game and also survive it:)  _⧹"));
            mvwprintw(game_win, map_height - 4, map_width - 77, "╰─────────────────────────────────────────────────┘  ¯");
            wattroff(game_win, COLOR_PAIR(color_pair_num));
        }
        if(game->currentLevel == game->levelCount - 1){
            int a = map_height - 32;
            int b = map_width - 60;
            mvwprintw(game_win, a, b, " <>=======() ");
            mvwprintw(game_win, a + 1, b, "(/\\___   /|\\          ()==========<>_");
            mvwprintw(game_win, a + 2, b, "      \\_/ | \\        //|\\   ______/ \\)");
            mvwprintw(game_win, a + 3, b, "        \\_|  \\      // | \\_/");
            mvwprintw(game_win, a + 4, b, "          \\|\\/|\\_   //  /\\/");
            mvwprintw(game_win, a + 5, b, "           (oo)\\ \\_//  /");
            mvwprintw(game_win, a + 6, b, "          //_/\\_\\/ /  |");
            mvwprintw(game_win, a + 7, b, "         @@/  |=\\  \\  |");
            mvwprintw(game_win, a + 8, b, "              \\_=\\_ \\ |");
            mvwprintw(game_win, a + 9, b, "                \\==\\ \\|\\_");
            mvwprintw(game_win, a + 10, b, "             __(\\===\\(  )\\");
            mvwprintw(game_win, a + 11, b, "            (((~) __(_/   |");
            mvwprintw(game_win, a + 12, b, "               ______/ /");
            mvwprintw(game_win, a + 13, b, "              '------'");
        }
        wattron(game_win, COLOR_PAIR(7));
        mvwprintw(game_win, 2, 5, UI_TEXT("Current level: %d🟢 Completed level: %d✅ Total points: %lldXP","Floor %d   |   Deepest %d   |   XP %lld"), game->currentLevel + 1, game->unlockedLevel + 1, game->player->points);
        wattroff(game_win, COLOR_PAIR(7));
        mvwprintw(game_win, 2, 5, UI_TEXT("Current level: %d🟢 Completed level: %d✅ Total points: %lld","Floor %d   |   Deepest %d   |   XP %lld"), game->currentLevel + 1, game->unlockedLevel + 1, game->player->points);
    }
    while(1){
        {
            if(game->levels[game->currentLevel]->rooms[game->player->room_num]->type == 'r'){
                in_reinforce = 1;
            }
            if(game->levels[game->currentLevel]->rooms[game->player->room_num]->type == 's'){
                in_healing = 1;
            }
            if(in_spell){
                wattron(game_win, COLOR_PAIR(17));
            }
            if(in_reinforce){
                wattron(game_win, COLOR_PAIR(25));
            }
            if(in_healing){
                wattron(game_win, COLOR_PAIR(15));
            }
            if(game->currentLevel == game->levelCount - 1){
                wattron(game_win, COLOR_PAIR(12));
            }
            if(game->currentLevel == 6){
                wattron(game_win, COLOR_PAIR(14));
            }
        }
        { // Rottening
            time(&rotten_time_check);
            if(difftime(rotten_time_check, rotten_time) >= 30){
                time(&rotten_time);
                if(game->player->specialfoods[0].count > 0){
                    game->player->specialfoods[0].count--;
                    game->player->specialfoods[3].count++;
                }
                if(game->player->specialfoods[1].count > 0){
                    game->player->specialfoods[1].count--;
                    game->player->specialfoods[3].count++;
                }
                if(game->player->specialfoods[2].count > 0){
                    game->player->specialfoods[2].count--;
                    game->player->specialfoods[3].count++;
                }
            }
        }
        valid_f_check = 0;
        in_emoji = 0;
        diff_from_map = 0;
        move_or_not = 0;
        is_enemy_or_not = 0;
        time(&save_counter2);
        if(difftime(save_counter2, save_counter1) >= 5){
            time(&save_counter1);
            game->player->play_time = play_timee + difftime(save_counter1, start_time);
            if(game->player->points >= 0){
                game->player->points = pointss + play_timee * 5 * difficulty_efficiency + game->player->gold * 10;
            }
            else{
                game->player->points = pointss + play_timee * 5 * difficulty_efficiency + game->player->gold * 10;
            }
            if(strcmp(game->player->name, "guest") && strcmp(which_user, "guest")){
                if (game->currentLevel != 6 && game->player->can_be_saved) save_user_game(game);
            }
        }
        // wattron(game_win, COLOR_PAIR(3));
        // // mvwprintw(game_win, 2, 5, "Current level: %d Completed level: %d", game->currentLevel + 1, game->unlockedLevel + 1);
        // wattroff(game_win, COLOR_PAIR(3));
        if(is_last_room){
            draw_last_room(game_win, game->levels[game->currentLevel]->rooms[0], 0);
        }
        else if(game->currentLevel == game->levelCount - 1 || game->currentLevel == 6){
            draw_room(game_win, game->levels[game->currentLevel]->rooms[0], 0);
        }
        move_click = music_getch(game_win);
        { // Mouse
            mouse_save.x = mouse_event.x;
            mouse_save.y = mouse_event.y;
            if (move_click == KEY_MOUSE) getmouse(&mouse_event);
            if(mouse_event.x != 0 && mouse_event.y != 0){
                side = 0;
                dx = 0;
                dy = 0;
                if(mouse_event.x > game->player->cord.x){
                    dx = 1;
                }
                else if(mouse_event.x < game->player->cord.x){
                    dx = -1;
                }
                if(mouse_event.y > game->player->cord.y){
                    dy = 1;
                }
                else if(mouse_event.y < game->player->cord.y){
                    dy = -1;
                }
                if(game->player->cord.y - mouse_event.y >= 0 && (game->player->cord.y - mouse_event.y > (mouse_event.x - game->player->cord.x)*dx)){
                    if((mouse_event.x - game->player->cord.x)*dx <= (game->player->cord.y - mouse_event.y)/2){
                        side = 1;
                    }
                }
                else if(game->player->cord.y - mouse_event.y <= 0 && (mouse_event.y - game->player->cord.y > (mouse_event.x - game->player->cord.x)*dx)){
                    if((mouse_event.x - game->player->cord.x)*dx <= (mouse_event.y - game->player->cord.y)/2){
                        side = 3;
                    }
                }
                else if(game->player->cord.x - mouse_event.x >= 0 && (game->player->cord.x - mouse_event.x > (mouse_event.y - game->player->cord.y)*dy)){
                    if((mouse_event.y - game->player->cord.y)*dy <= (game->player->cord.x - mouse_event.x)/2){
                        side = 2;
                    }
                }
                else if(game->player->cord.x- mouse_event.x <= 0 && (mouse_event.x - game->player->cord.x > (mouse_event.y - game->player->cord.y)*dy)){
                    if((mouse_event.y - game->player->cord.y)*dy <= (mouse_event.x - game->player->cord.x)/2){
                        side = 4;
                    }
                }
            }
            draw_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], game->player->room_num);
            in_emoji_check();
            if(in_emoji == 0){
                wattron(game_win, COLOR_PAIR(color_pair_num));
                draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
                wattroff(game_win, COLOR_PAIR(color_pair_num));
            }
        }
        if(is_last_room){
            draw_last_room(game_win, game->levels[game->currentLevel]->rooms[0], 0);
        }
        else if(game->currentLevel == game->levelCount - 1 || game->currentLevel == 6){
            draw_room(game_win, game->levels[game->currentLevel]->rooms[0], 0);
        }
        if (move_click == 'h' || move_click == 'H') { controls_help(game_win); continue; }
        if (move_click == KEY_RESIZE) {
            int rows,cols;getmaxyx(stdscr,rows,cols);
            while(rows<MIN_ROWS || cols<MIN_COLS) {
                erase();mvprintw(1,1,"Resize Terminal to at least %d x %d; gameplay is paused.",MIN_COLS,MIN_ROWS);refresh();
                music_getch(stdscr);getmaxyx(stdscr,rows,cols);
            }
            wresize(game_win,rows<MAP_ROWS?rows:MAP_ROWS,cols<MAP_COLS?cols:MAP_COLS);
            print_map(game_win);continue;
        }
        if(move_click == 'f' || move_click == 'F'){
            move_click = music_getch(game_win);
            if(move_click == 'o' || move_click == 'O' || move_click == 10){
                move_click = music_getch(game_win);
                f_check = 1 - f_check;
            }
        }
        else if(move_click == 'g' || move_click == 'G'){ // if(password_disapear % 20 == 19){
        //     mvwprintw(game_win, 2, 2, "                                                                      ");
        // }
            g_check = 1 - g_check;
        }
        else if(move_click == 'x' || move_click == 'X'){
            x_check = 1 - x_check;
        }
        else if(move_click == 'm' || move_click == 'M'){
            move_click = music_getch(game_win);
            if(move_click == 'o' || move_click == 'O' || move_click == 10){
                m_check = 1 - m_check;
                if(m_check){
                    print_all_map(game_win);
                }
                else{
                    print_map(game_win);
                }
            }
        }
        else if(move_click == 't' || move_click == 'T'){
            move_click = music_getch(game_win);
            if(move_click == 'o' || move_click == 'O' || move_click == 10){
                t_check = 1 - t_check;
                if(m_check){
                    print_all_map(game_win);
                }
                else{
                    print_map(game_win);
                }
            }
        }
        if(x_check){
            x_print(game_win, game->player->cord.x + 1, game->player->cord.y);
            x_print(game_win, game->player->cord.x - 1, game->player->cord.y);
            x_print(game_win, game->player->cord.x, game->player->cord.y + 1);
            x_print(game_win, game->player->cord.x, game->player->cord.y - 1);
            x_print(game_win, game->player->cord.x + 1, game->player->cord.y + 1);
            x_print(game_win, game->player->cord.x - 1, game->player->cord.y + 1);
            x_print(game_win, game->player->cord.x + 1, game->player->cord.y - 1);
            x_print(game_win, game->player->cord.x - 1, game->player->cord.y - 1);
        }
        // if(count_move_click % 5 == 0){
        //     mvwprintw(game_win, map_height - 7, 5, "                                                              ");
        //     mvwprintw(game_win, map_height - 5 + (game->player->foodCount) / 100, 5 + 2*(game->player->foodCount / 10) - 2*((game->player->foodCount)/100)*10, "  ");
        // }
        if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
            wattron(game_win, COLOR_PAIR(3));
        }
        if(move_click == 27 || (move_click == KEY_MOUSE && (mouse_event.bstate & (BUTTON1_PRESSED|BUTTON1_CLICKED)) && mouse_event.y == 2 && mouse_event.x >= 159 && mouse_event.x <= 160)){
            wclear(game_win);
            wrefresh(game_win);
            strcpy(which_pause_menu, " ");
            pause_menu();
            if(!strcmp(which_pause_menu, " Save and Quit ")){
                Mix_HaltMusic();
                escape_check = 1;
                game->player->can_be_saved = 1;
                if(game->currentLevel==6) {
                    game->currentLevel=level_save;game->player->cord=(Point){x_save,y_save};
                    game->player->room_num=room_num_save;game->newLevel=0;
                }
                return;
            }
            else if(!strcmp(which_pause_menu, " Pause Music ")){
                Mix_PauseMusic();
            }
            else if(!strcmp(which_pause_menu, " Resume Music ")){
                Mix_ResumeMusic();
            }
            else if(!strcmp(which_pause_menu, " Select Music ")){
                Mix_PauseMusic();
                char *chosen_music = display_music_menu();
                if (chosen_music) selected_music = chosen_music;
                play_music(selected_music);
            }
            else if(!strcmp(which_pause_menu, " Change Character ")){
                wclear(game_win);
                wrefresh(game_win);
                clear();
                refresh();
                character_change();
            }
            else if(!strcmp(which_pause_menu, " Change Color ")){
                wclear(game_win);
                wrefresh(game_win);
                clear();
                refresh();
                color_change();
                strcpy(game->player->color, which_color);
                if(!strcmp(game->player->color, " Yellow ")){
                    color_pair_num = 1;
                }
                else if(!strcmp(game->player->color, " Red ")){
                    color_pair_num = 6;
                }
                else if(!strcmp(game->player->color, " Blue ")){
                    color_pair_num = 9;
                }
                else if(!strcmp(game->player->color, " Green ")){
                    color_pair_num = 7;
                }
            }
            if(m_check){
                print_all_map(game_win);
            }
            else{
                print_map(game_win);
            }
        }
        if(game->levels[game->currentLevel]->rooms[game->player->room_num]->type == 's'){
            wattron(game_win, COLOR_PAIR(15));
        }
        if(window_check){
            clear_room(game_win, game->levels[game->currentLevel]->rooms[window_check - 1], window_check - 1);
            window_check = 0;
        }
        else if(move_click == '\t'|| (move_click == KEY_MOUSE && (mouse_event.bstate & (BUTTON1_PRESSED|BUTTON1_CLICKED)) && mouse_event.y == 3 && mouse_event.x >= 159 && mouse_event.x <= 160)){
            if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
                Mix_PlayMusic(weaponpickup_effect, 1);
            }
            inventory_bar_menu();
            if(m_check){
                print_all_map(game_win);
            }
            else{
                print_map(game_win);
            }
        }
        else if(move_click == KEY_UP || move_click == 'w' || move_click == 'W'){
            move_direction = 'w';
            if(player_step_valid(game->player->cord.y - 1,game->player->cord.x)){
                mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "%lc", game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item);
                printf_diff_from_map(game_win);
                game->player->cord.y -= 1;
                count_move++;
                password_disapear++;
                move_or_not = 1;
            }
            else if(game->whole_map[game->currentLevel][game->player->cord.y - 1][game->player->cord.x].item == '=' && m_check == 0){
                window_check = 0;
                for(int j = game->player->cord.y - 2; j > start_y + 1; j--){
                    for(int k = game->player->cord.x - 10; k < game->player->cord.x + 10; k++){
                        if(j >= start_x && j <= end_x){
                            for(int i = 0; i < game->levels[game->currentLevel]->roomsCount; i++){
                                if((((k == game->levels[game->currentLevel]->rooms[i]->x
                                || k == game->levels[game->currentLevel]->rooms[i]->x + game->levels[game->currentLevel]->rooms[i]->width - 1)
                                &&(j >= game->levels[game->currentLevel]->rooms[i]->y 
                                && j <= game->levels[game->currentLevel]->rooms[i]->y + game->levels[game->currentLevel]->rooms[i]->height))
                                || ((j == game->levels[game->currentLevel]->rooms[i]->y
                                || j == game->levels[game->currentLevel]->rooms[i]->y + game->levels[game->currentLevel]->rooms[i]->height - 1)
                                && (k >= game->levels[game->currentLevel]->rooms[i]->x 
                                && k <= game->levels[game->currentLevel]->rooms[i]->x + game->levels[game->currentLevel]->rooms[i]->width)))
                                && game->levels[game->currentLevel]->rooms[i]->isVisible == false){
                                    draw_room(game_win, game->levels[game->currentLevel]->rooms[i], i);
                                    if(game->levels[game->currentLevel]->rooms[i]->isVisible)
                                    game->levels[game->currentLevel]->rooms[i]->isVisible = false;
                                    // clear_room(game_win, game->levels[game->currentLevel]->rooms[i], i);
                                    window_check = i + 1;
                                    break;
                                }
                            }
                        }
                        if(window_check){
                            break;
                        }
                    }
                    if(window_check){
                        // sleep(1);
                        break;
                    }
                }
            }
            else if(game->whole_map[game->currentLevel][game->player->cord.y - 1][game->player->cord.x].item == 'L'){
                // last_attemps = game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_attemps;
                enter_door_password(game->player->cord.y-1,game->player->cord.x);
                printf("\033[?1003h\n");
                mvwprintw(game_win, 3, 2, "                                                                      ");
                if(m_check){
                    print_all_map(game_win);
                }
                else{
                    print_map(game_win);
                }
            }
            int speed_check = 0;
            if(f_check){
                valid_f_check = 1;
            }
            // if(game->player->Speed_effect > 0){
            //     speed_check = 1;
            // }
            while((f_check || speed_check) && player_step_valid(game->player->cord.y - 1,game->player->cord.x) && player_step_valid(game->player->cord.y, game->player->cord.x)){
                game->player->cord.y -= 1;
                speed_check = 0;
                move_or_not = 1;
                if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
                    continue_corridor(game_win, game->player->cord.y, game->player->cord.x, 3);
                }
                else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '+'
                || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '?'
                || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'L'){
                    break;
                }
            }
        }
        else if(move_click == KEY_DOWN || move_click == 's' || move_click == 'S'){
            move_direction = 's';
            if(player_step_valid(game->player->cord.y + 1,game->player->cord.x)){
                mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "%lc", game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item);
                printf_diff_from_map(game_win);
                game->player->cord.y += 1;
                count_move++;
                password_disapear++;
                move_or_not = 1;
            }
            else if(game->whole_map[game->currentLevel][game->player->cord.y + 1][game->player->cord.x].item == '=' && m_check == 0){
                window_check = 0;
                for(int j = game->player->cord.y + 2; j < end_y; j++){
                    for(int k = game->player->cord.x - 15; k < game->player->cord.x + 15; k++){
                        if(j >= start_x && j <= end_x){
                            for(int i = 0; i < game->levels[game->currentLevel]->roomsCount; i++){
                                if((((k == game->levels[game->currentLevel]->rooms[i]->x
                                || k == game->levels[game->currentLevel]->rooms[i]->x + game->levels[game->currentLevel]->rooms[i]->width - 1)
                                &&(j >= game->levels[game->currentLevel]->rooms[i]->y 
                                && j <= game->levels[game->currentLevel]->rooms[i]->y + game->levels[game->currentLevel]->rooms[i]->height))
                                || ((j == game->levels[game->currentLevel]->rooms[i]->y
                                || j == game->levels[game->currentLevel]->rooms[i]->y + game->levels[game->currentLevel]->rooms[i]->height - 1)
                                && (k >= game->levels[game->currentLevel]->rooms[i]->x 
                                && k <= game->levels[game->currentLevel]->rooms[i]->x + game->levels[game->currentLevel]->rooms[i]->width)))
                                && game->levels[game->currentLevel]->rooms[i]->isVisible == false){
                                    draw_room(game_win, game->levels[game->currentLevel]->rooms[i], i);
                                    game->levels[game->currentLevel]->rooms[i]->isVisible = false;
                                    // clear_room(game_win, game->levels[game->currentLevel]->rooms[i], i);
                                    window_check = i + 1;
                                    break;
                                }
                            }
                        }
                        if(window_check){
                            break;
                        }
                    }
                    if(window_check){
                        // sleep(1);
                        break;
                    }
                }
            }
            else if(game->whole_map[game->currentLevel][game->player->cord.y + 1][game->player->cord.x].item == 'L'){
                // last_attemps = game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_attemps;
                enter_door_password(game->player->cord.y+1,game->player->cord.x);
                printf("\033[?1003h\n");
                mvwprintw(game_win, 3, 2, "                                                                      ");
                if(m_check){
                    print_all_map(game_win);
                }
                else{
                    print_map(game_win);
                }
            }
            int speed_check = 0;
            if(f_check){
                valid_f_check = 1;
            }
            // if(game->player->Speed_effect > 0){
            //     speed_check = 1;
            // }
            while((f_check || speed_check) && player_step_valid(game->player->cord.y + 1,game->player->cord.x) && player_step_valid(game->player->cord.y, game->player->cord.x)){
                game->player->cord.y += 1;
                speed_check = 0;
                move_or_not = 1;
                if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
                    continue_corridor(game_win, game->player->cord.y, game->player->cord.x, 3);
                }
                else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '+'
                || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '?'
                || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'L'){
                    break;
                }
            }
        }
        else if(move_click == KEY_RIGHT || move_click == 'd' || move_click == 'D'){
            move_direction = 'd';
            if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '?' && game->player->room_num == game->levels[game->currentLevel]->secret_room){
                mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "‽");
                game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[game->levels[game->currentLevel]->rooms[game->player->room_num]->doorCount - 1].password = 1;
                game->player->room_num = game->levels[game->currentLevel]->roomsCount - 1;
                game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[0].password = 1;
                game->player->cord.x = game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[0].cord.x;
                game->player->cord.y = game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[0].cord.y;
            }
            else if(player_step_valid(game->player->cord.y,game->player->cord.x + 1)){
                mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "%lc", game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item);
                printf_diff_from_map(game_win);
                game->player->cord.x += 1;
                count_move++;
                password_disapear++;
                move_or_not = 1;
            }
            else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == '=' && m_check == 0){
                window_check = 0;
                for(int k = game->player->cord.x + 2; k < end_x - 4; k++){
                    for(int j = game->player->cord.y - 15; j < game->player->cord.y + 15; j++){
                        if(j >= start_y && j <= end_y){
                            for(int i = 0; i < game->levels[game->currentLevel]->roomsCount; i++){
                                if((((k == game->levels[game->currentLevel]->rooms[i]->x
                                || k == game->levels[game->currentLevel]->rooms[i]->x + game->levels[game->currentLevel]->rooms[i]->width - 1)
                                &&(j >= game->levels[game->currentLevel]->rooms[i]->y 
                                && j <= game->levels[game->currentLevel]->rooms[i]->y + game->levels[game->currentLevel]->rooms[i]->height))
                                || ((j == game->levels[game->currentLevel]->rooms[i]->y
                                || j == game->levels[game->currentLevel]->rooms[i]->y + game->levels[game->currentLevel]->rooms[i]->height - 1)
                                && (k >= game->levels[game->currentLevel]->rooms[i]->x 
                                && k <= game->levels[game->currentLevel]->rooms[i]->x + game->levels[game->currentLevel]->rooms[i]->width)))
                                && game->levels[game->currentLevel]->rooms[i]->isVisible == false){
                                    draw_room(game_win, game->levels[game->currentLevel]->rooms[i], i);
                                    game->levels[game->currentLevel]->rooms[i]->isVisible = false;
                                    // clear_room(game_win, game->levels[game->currentLevel]->rooms[i], i);
                                    window_check = i + 1;
                                    break;
                                }
                            }
                        }
                        if(window_check){
                            break;
                        }
                    }
                    if(window_check){
                        // sleep(1);
                        break;
                    }
                }
            }
            else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x + 1].item == 'L'){
                // game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_attemps = 1;
                // last_attemps = game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_attemps;
                enter_door_password(game->player->cord.y,game->player->cord.x+1);
                printf("\033[?1003h\n");
                mvwprintw(game_win, 3, 2, "                                                                      ");
                if(m_check){
                    print_all_map(game_win);
                }
                else{
                    print_map(game_win);
                }
            }
            int speed_check = 0;
            if(f_check){
                valid_f_check = 1;
            }
            // if(game->player->Speed_effect > 0){
            //     speed_check = 1;
            // }
            while((f_check || speed_check) && player_step_valid(game->player->cord.y,game->player->cord.x + 1) && player_step_valid(game->player->cord.y, game->player->cord.x)){
                game->player->cord.x += 1;
                speed_check = 0;
                if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
                    continue_corridor(game_win, game->player->cord.y, game->player->cord.x, 3);
                }
                else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '+' 
                || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '?'
                || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'L'){
                    if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '?'){
                        game->player->cord.x -= 1;
                    }
                    break;
                }
            }
        }
        else if(move_click == KEY_LEFT || move_click == 'a' || move_click == 'A'){
            move_direction = 'a';
            if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '?' && game->player->room_num == total_rooms - 1){
                mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "‽");
                game->player->room_num = game->levels[game->currentLevel]->secret_room;
                game->player->cord.x = game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[game->levels[game->currentLevel]->rooms[game->player->room_num]->doorCount - 1].cord.x;
                game->player->cord.y = game->levels[game->currentLevel]->rooms[game->player->room_num]->doors[game->levels[game->currentLevel]->rooms[game->player->room_num]->doorCount - 1].cord.y;
            }
            else if(player_step_valid(game->player->cord.y,game->player->cord.x - 1)){
                mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "%lc", game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item);
                printf_diff_from_map(game_win);
                game->player->cord.x -= 1;
                count_move++;
                password_disapear++;
                move_or_not = 1;
            }
            else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == '=' && m_check == 0){
                window_check = 0;
                for(int k = game->player->cord.x - 1; k < start_x + 4; k++){
                    for(int j = game->player->cord.y - 15; j > game->player->cord.y + 15; j--){
                        if(j >= start_y && j <= end_y){
                            for(int i = 0; i < game->levels[game->currentLevel]->roomsCount; i++){
                                if((((k == game->levels[game->currentLevel]->rooms[i]->x
                                || k == game->levels[game->currentLevel]->rooms[i]->x + game->levels[game->currentLevel]->rooms[i]->width - 1)
                                &&(j >= game->levels[game->currentLevel]->rooms[i]->y 
                                && j <= game->levels[game->currentLevel]->rooms[i]->y + game->levels[game->currentLevel]->rooms[i]->height))
                                || ((j == game->levels[game->currentLevel]->rooms[i]->y
                                || j == game->levels[game->currentLevel]->rooms[i]->y + game->levels[game->currentLevel]->rooms[i]->height - 1)
                                && (k >= game->levels[game->currentLevel]->rooms[i]->x 
                                && k <= game->levels[game->currentLevel]->rooms[i]->x + game->levels[game->currentLevel]->rooms[i]->width)))
                                && game->levels[game->currentLevel]->rooms[i]->isVisible == false){
                                    draw_room(game_win, game->levels[game->currentLevel]->rooms[i], i);
                                    game->levels[game->currentLevel]->rooms[i]->isVisible = false;
                                    // clear_room(game_win, game->levels[game->currentLevel]->rooms[i], i);
                                    window_check = i + 1;
                                    break;
                                }
                            }
                        }
                        if(window_check){
                            break;
                        }
                    }
                    if(window_check){
                        // sleep(1);
                        break;
                    }
                }
            }
            else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x - 1].item == 'L'){
                // last_attemps = game->levels[game->currentLevel]->rooms[game->player->room_num]->lock_attemps;
                enter_door_password(game->player->cord.y,game->player->cord.x-1);
                printf("\033[?1003h\n");
                mvwprintw(game_win, 3, 2, "                                                                      ");
                if(m_check){
                    print_all_map(game_win);
                }
                else{
                    print_map(game_win);
                }
            }
            int speed_check = 0;
            if(f_check){
                valid_f_check = 1;
            }
            // if(game->player->Speed_effect > 0){
            //     speed_check = 1;
            // }
            while((f_check || speed_check) && player_step_valid(game->player->cord.y,game->player->cord.x - 1) && player_step_valid(game->player->cord.y, game->player->cord.x)){
                game->player->cord.x -= 1;
                speed_check = 0;
                if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
                    continue_corridor(game_win, game->player->cord.y, game->player->cord.x, 3);
                }
                else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '+'
                || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '?'
                || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == 'L'){
                    break;
                }
            }
        }
        else if(move_click == KEY_HOME || move_click == 'q' || move_click == 'Q'){
            if(player_step_valid(game->player->cord.y - 1,game->player->cord.x - 1)){
                mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "%lc", game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item);
                printf_diff_from_map(game_win);
                game->player->cord.x -= 1;
                game->player->cord.y -= 1;
                count_move++;
                password_disapear++;
                move_or_not = 1;
            }
            while(f_check && player_step_valid(game->player->cord.y - 1,game->player->cord.x - 1)){
                game->player->cord.y -= 1;
                game->player->cord.x -= 1;
                if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
                    continue_corridor(game_win, game->player->cord.y, game->player->cord.x, 3);
                }
                else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '+'){
                    break;
                }
            }
        }
        else if(move_click == KEY_PPAGE){
            if(player_step_valid(game->player->cord.y - 1,game->player->cord.x + 1)){
                mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "%lc", game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item);
                printf_diff_from_map(game_win);
                game->player->cord.x += 1;
                game->player->cord.y -= 1;
                count_move++;
                password_disapear++;
                move_or_not = 1;
            }
            while(f_check && player_step_valid(game->player->cord.y - 1,game->player->cord.x + 1)){
                game->player->cord.y -= 1;
                game->player->cord.x += 1;
                if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
                    continue_corridor(game_win, game->player->cord.y, game->player->cord.x, 3);
                }
                else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '+'){
                    break;
                }
            }
        }
        else if(move_click == KEY_NPAGE || move_click == 'c' || move_click == 'C'){
            if(player_step_valid(game->player->cord.y + 1,game->player->cord.x + 1)){
                mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "%lc", game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item);
                printf_diff_from_map(game_win);
                game->player->cord.x += 1;
                game->player->cord.y += 1;
                count_move++;
                password_disapear++;
                move_or_not = 1;
            }
            while(f_check && player_step_valid(game->player->cord.y + 1,game->player->cord.x + 1)){
                game->player->cord.y += 1;
                game->player->cord.x += 1;
                if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
                    continue_corridor(game_win, game->player->cord.y, game->player->cord.x, 3);
                }
                else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '+'){
                    break;
                }
            }
        }
        else if(move_click == KEY_END || move_click == 'z' || move_click == 'Z'){
            if(player_step_valid(game->player->cord.y + 1,game->player->cord.x - 1)){
                mvwprintw(game_win, game->player->cord.y, game->player->cord.x, "%lc", game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item);
                printf_diff_from_map(game_win);
                game->player->cord.x -= 1;
                game->player->cord.y += 1;
                count_move++;
                password_disapear++;
                move_or_not = 1;
            }
            while(f_check && player_step_valid(game->player->cord.y + 1,game->player->cord.x - 1)){
                game->player->cord.y += 1;
                game->player->cord.x -= 1;
                if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
                    continue_corridor(game_win, game->player->cord.y, game->player->cord.x, 3);
                }
                else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '+'){
                    break;
                }
            }
        }
        // if(move_or_not){
        //     if(!strcmp(selected_music, "./Base_Sounds/silent.wav") && game->currentLevel != game->levelCount - 1){
        //         Mix_PlayMusic(walk_effect, 1);
        //     }
        // }
        else if((move_click == '>' || move_click == '.' || move_click == 10)
        && game->currentLevel < game->levelCount - 1 && game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '>'){
            // if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '>'){
                game->currentLevel++;
                if(game->unlockedLevel < game->currentLevel){
                    game->unlockedLevel = game->currentLevel;
                    unlocked_level = game->unlockedLevel;
                    game->newLevel = 1;
                }
                total_rooms = game->levels[game->currentLevel]->roomsCount;
                current_level = game->currentLevel;
                back_front = 1;
                return;
            // }
        }
        else if((move_click == '<' || move_click == ',' || move_click == 10) && game->currentLevel > 0){
            if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '<'){
                game->currentLevel--;
                total_rooms = game->levels[game->currentLevel]->roomsCount;
                current_level = game->currentLevel;
                back_front = -1;
                return;
            }
        }
        else if(move_click == 'P' || move_click == 'p'){
            move_click = music_getch(game_win);
            if((move_click == '>' || move_click == '.' || move_click == 10) && game->currentLevel < game->levelCount - 1){
                game->currentLevel++;
                if(game->unlockedLevel < game->currentLevel){
                    game->unlockedLevel = game->currentLevel;
                    unlocked_level = game->unlockedLevel;
                    game->newLevel = 1;
                }
                total_rooms = game->levels[game->currentLevel]->roomsCount;
                current_level = game->currentLevel;
                back_front = 1;
                return;
            }
            else if((move_click == '<' || move_click == ',') && game->currentLevel > 0){
                game->currentLevel--;
                total_rooms = game->levels[game->currentLevel]->roomsCount;
                current_level = game->currentLevel;
                back_front = -1;
                return;
            }
        }
        else if(move_click == 'i' || move_click == 'I'){
            move_click = music_getch(game_win);
            if(move_click == 'o' || move_click == 'O' || move_click == 10){
                if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemyLeft > 0){
                    for(int i = 0; i < game->levels[game->currentLevel]->rooms[game->player->room_num]->enemyCount; i++){
                        int i1 = rand() % game->levels[game->currentLevel]->rooms[game->player->room_num]->enemyCount;
                        if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i1].isAlive){
                            game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i1].isAlive = false;
                            game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i1].isVisible = false;
                            // game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i1].canMove = false;
                            game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i1].health = 0;
                            game->levels[game->currentLevel]->rooms[game->player->room_num]->enemyLeft--;
                            break;
                        }
                        else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].isAlive){
                            game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].isAlive = false;
                            game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].isVisible = false;
                            // game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].canMove = false;
                            game->levels[game->currentLevel]->rooms[game->player->room_num]->enemies[i].health = 0;
                            game->levels[game->currentLevel]->rooms[game->player->room_num]->enemyLeft--;
                            break;
                        }
                    }
                    draw_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], game->player->room_num);
                }
                }
        }
        else if(move_click == ' ' || move_click == 'e' || move_click == 'E' || (move_click == KEY_MOUSE && mouse_attack_event(&mouse_event))){
            move_or_not = 1;
            attacking(game_win);
            if(escape_check||run_finished)return;
            count_move++;
        }
        if(escape_check||run_finished)return;
        after_move(game_win);
        if(is_battle){
            is_battle = 0;
            if(!after_battle){
                game->newLevel = 1;
            }
            return;
        }
        if(game->levels[game->currentLevel]->rooms[game->player->room_num]->type == 's'){
            wattroff(game_win, COLOR_PAIR(15));
        }
        if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '+' || game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
            continue_corridor(game_win, game->player->cord.y, game->player->cord.x, 0);
        }
        if(game->levels[game->currentLevel]->rooms[game->player->room_num]->type == 'm'){
            in_nightmare = 1;
        }
        if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
            in_spell = 0;
        }
        if(game->levels[game->currentLevel]->rooms[game->player->room_num]->type == 's' && count_move % 16 == 2 && game->player->health <= 95 && move_or_not){
            game->player->health += 5;
        }
        if(in_spell && move_or_not){
            if(count_move % (100 / (difficulty_efficiency * 2 + 1)) == 0){
                game->player->health -= 5;
                // count_move = 1;
            }
        }
        if(in_reinforce && move_or_not){
            if(count_move % 20 == 0 && game->player->shield <= 95){
                game->player->shield += 5;
                // count_move = 1;
            }
        }
        else if(move_or_not){
            int a = 1;
            if(game->player->Speed_effect > 0){
                a = 2;
            }
            if(count_move % (150 / (difficulty_efficiency * 2 + 1) / a) == 0 && game->player->foodCount == 0){
                game->player->health -= 5;
                // count_move = 1;
            }
            else if(count_move % (90 / (difficulty_efficiency * 2 + 1) / a) == 0 && game->player->foodCount > 0){
                game->player->foodCount -= 5;
                // count_move = 1;
            }
        }
        int is_healing = 1;
        if(game->player->Health_effect > 0){
            is_healing = 5;
        }
        int a = 1;
        if(game->player->Speed_effect > 0){
            a = 2;
        }
        if(count_move % (25 / is_healing / a) == 0 && (game->player->foodCount > 30 || game->player->Health_effect > 0) && game->player->health <= 95 && in_spell == 0 && in_reinforce == 0
        && game->levels[game->currentLevel]->rooms[game->player->room_num]->type != 's' && move_or_not){
            game->player->health += 5;
            // count_move = 1;
        }
        // if(count_move % 20 == 2 && game->player->foodCount > 30 && game->player->health <= 90 && in_spell == 0 
        // && game->levels[game->currentLevel]->rooms[game->player->room_num]->type != 's'){
        //     game->player->foodCount -= 5;
        // }
        if(in_nightmare){
            draw_nightmare_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], game->player->room_num);
        }
        else if(in_spell){
            draw_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], game->player->room_num);
        }
        else if(in_reinforce){
            draw_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], game->player->room_num);
        }
        else if(in_healing){
            draw_room(game_win, game->levels[game->currentLevel]->rooms[game->player->room_num], game->player->room_num);
        }
        if(in_spell){
            wattroff(game_win, COLOR_PAIR(17));
        }
        else if(in_reinforce){
            wattroff(game_win, COLOR_PAIR(25));
        }
        else if(in_healing){
            wattroff(game_win, COLOR_PAIR(15));
        }
        else if(game->currentLevel == game->levelCount - 1){
            wattroff(game_win, COLOR_PAIR(12));
        }
        else if(game->currentLevel == 6){
            wattroff(game_win, COLOR_PAIR(14));
        }
        messaging(game_win);
        if(is_last_room)boss_encounter_draw(game_win,game->levels[game->currentLevel]->rooms[0]);
        if(game->player->health <= 0){
            lost_game_screen(start_time);
            escape_check = 1;
            game->new_game = true;
            return;
        }
        if(count_move >= 1000){
            count_move = 1;
        }
        wrefresh(game_win);
    }
}
void game_play(WINDOW *game_win){
    printf("\033[?1003h\n");
    while (!escape_check) {
    if(game->currentLevel == game->levelCount - 1){
        if(rand() % 2 == 0){
            effects = Mix_LoadMUS("./Base_Sounds/PiratesoftheCaribbean.mp3");
        }
        else{
            effects = Mix_LoadMUS("./Base_Sounds/GameOfThrones.mp3");
        }
        Mix_PlayMusic(effects, 1);
    }
    if(game->newLevel){
        generate_map();
        game->newLevel = 0;
    }
    total_rooms = game->levels[game->currentLevel]->roomsCount;
    current_level = game->currentLevel;
    unlocked_level = game->unlockedLevel;
    if(after_battle){
        after_battle = 0;
        game->player->cord.x = x_save;
        game->player->cord.y = y_save;
        game->player->room_num = room_num_save;
    }
    else if(back_front == 1){
        if(game->currentLevel == game->levelCount - 1 || game->currentLevel == 6){
            game->player->cord.x = game->levels[game->currentLevel]->rooms[0]->x + 1;
            game->player->cord.y = game->levels[game->currentLevel]->rooms[0]->y + 1;
        }
        else{
            game->player->cord.x = game->levels[game->currentLevel]->rooms[0]->stair.cord.x;
            game->player->cord.y = game->levels[game->currentLevel]->rooms[0]->stair.cord.y;
        }
        game->player->room_num = 0;
    }
    else if(back_front == -1){
        int a = 0;
        if(game->levels[game->currentLevel]->is_secret_room == true){
            a = -1;
        } 
        game->player->cord.x = game->levels[game->currentLevel]->rooms[game->levels[game->currentLevel]->roomsCount - 1 + a]->stair.cord.x;
        game->player->cord.y = game->levels[game->currentLevel]->rooms[game->levels[game->currentLevel]->roomsCount - 1 + a]->stair.cord.y;
        game->player->room_num = game->levels[game->currentLevel]->roomsCount - 1 + a;
    }
    print_map(game_win);
    player_move(game_win, start_time);
    in_spell = 0;
    in_nightmare = 0;
    all_print = 0;
    if(escape_check){
        return;
    }
    }
}
void start_game(){
    boss_encounter_reset();
    victory_results_reset();
    run_finished=0;
    game_session_ready = false;
    // strcpy(which_pause_menu, " Save and Quit ");
    // pause_menu();
    int screen_rows, screen_cols;
    getmaxyx(stdscr, screen_rows, screen_cols);
    if (screen_rows < MIN_ROWS || screen_cols < MIN_COLS) {
        clear(); mvprintw(2, 2, "Please enlarge Terminal to at least %d columns x %d rows, then try New Game again.", MIN_COLS, MIN_ROWS);
        mvprintw(4, 2, "Current size: %d x %d. Press any key to return.", screen_cols, screen_rows);
        refresh(); music_getch(stdscr); return;
    }
    release_game();
    back_front = after_battle = is_battle = is_last_room = escape_check = 0;
    in_nightmare = in_spell = m_check = x_check = f_check = g_check = t_check = 0;
    in_reinforce = in_healing = valid_f_check = 0;
    first_gold = first_spell = first_weapon = first_shield = first_food = first_trap = 0;
    first_specialfood = first_ancientkey = first_password = first_damage = first_attack = 0;
    first_enemy_near = first_lock_door = first_stair = 0;
    count_move = 1;
    time(&start_time);
    time(&save_counter1);
    drag2 = clock();
    Mix_HaltMusic();
    if (loosing_effect) Mix_FreeMusic(loosing_effect);
    if (winning_effect) Mix_FreeMusic(winning_effect);
    { // Sounds
        if (eating_effect) Mix_FreeMusic(eating_effect);
        eating_effect = Mix_LoadMUS("./Base_Sounds/Eating.mp3");
        if (walk_effect) Mix_FreeMusic(walk_effect);
        walk_effect = Mix_LoadMUS("./Base_Sounds/Walk.mp3");
        if (damage_effect) Mix_FreeMusic(damage_effect);
        damage_effect = Mix_LoadMUS("./Base_Sounds/Damage.mp3");
        if (glass_effect) Mix_FreeMusic(glass_effect);
        glass_effect = Mix_LoadMUS("./Base_Sounds/Glass.mp3");
        if (magic_effect) Mix_FreeMusic(magic_effect);
        magic_effect = Mix_LoadMUS("./Base_Sounds/Magic.wav");
        if (magicfood_effect) Mix_FreeMusic(magicfood_effect);
        magicfood_effect = Mix_LoadMUS("./Base_Sounds/MagicFood.mp3");
        if (weaponpickup_effect) Mix_FreeMusic(weaponpickup_effect);
        weaponpickup_effect = Mix_LoadMUS("./Base_Sounds/WeaponPickup.mp3");
        if (shieldpickup_effect) Mix_FreeMusic(shieldpickup_effect);
        shieldpickup_effect = Mix_LoadMUS("./Base_Sounds/ShieldPickup.mp3");
        if (forge_effect) Mix_FreeMusic(forge_effect);
        forge_effect = Mix_LoadMUS("./Base_Sounds/Forge.mp3");
        if (shieldbreak_effect) Mix_FreeMusic(shieldbreak_effect);
        shieldbreak_effect = Mix_LoadMUS("./Base_Sounds/ShieldBreak.mp3");
        if (coin_effect) Mix_FreeMusic(coin_effect);
        coin_effect = Mix_LoadMUS("./Base_Sounds/Coin1.mp3");
        if (black_coin_effect) Mix_FreeMusic(black_coin_effect);
        black_coin_effect = Mix_LoadMUS("./Base_Sounds/Coin2.mp3");
        if (mace_effect) Mix_FreeMusic(mace_effect);
        mace_effect = Mix_LoadMUS("./Base_Sounds/Mace.mp3");
        if (dagger_effect) Mix_FreeMusic(dagger_effect);
        dagger_effect = Mix_LoadMUS("./Base_Sounds/Dagger.mp3");
        if (wand_effect) Mix_FreeMusic(wand_effect);
        wand_effect = Mix_LoadMUS("./Base_Sounds/Wand.mp3");
        if (arrow_effect) Mix_FreeMusic(arrow_effect);
        arrow_effect = Mix_LoadMUS("./Base_Sounds/Arrow.mp3");
        if (sword_effect) Mix_FreeMusic(sword_effect);
        sword_effect = Mix_LoadMUS("./Base_Sounds/Sword.mp3");
        int random_music = rand() % 4;
        switch(random_music){
            case 0:
                loosing_effect = Mix_LoadMUS("./Base_Sounds/Loosing1.mp3");
                break;
            case 1:
                loosing_effect = Mix_LoadMUS("./Base_Sounds/Loosing2.mp3");
                break;
            case 2:
                loosing_effect = Mix_LoadMUS("./Base_Sounds/Loosing3.mp3");
                break;
            case 3:
                loosing_effect = Mix_LoadMUS("./Base_Sounds/Loosing4.mp3");
                break;
        }
        random_music = rand() % 4;
        switch(random_music){
            case 0:
                winning_effect = Mix_LoadMUS("./Base_Sounds/Winning1.mp3");
                break;
            case 1:
                winning_effect = Mix_LoadMUS("./Base_Sounds/Winning2.mp3");
                break;
            case 2:
                winning_effect = Mix_LoadMUS("./Base_Sounds/Winning3.mp3");
                break;
            case 3:
                winning_effect = Mix_LoadMUS("./Base_Sounds/Winning4.mp3");
                break;
        }
    }
    if (music && strcmp(selected_music,"./Base_Sounds/silent.wav")) Mix_PlayMusic(music, 0);
    player = NULL;
    game = calloc(1, sizeof(Game));
    game->player = calloc(1, sizeof(Player));
    game->player->name = "guest";
    game->levelCount = 3 + rand() % 2;
    game->levels = calloc(MAX_LEVELS, sizeof(Level *));
    game->currentLevel = 0;
    game->unlockedLevel = 0;
    game->newLevel = 1;
    unlocked_level = -1;
    // game->levelCount = 2;
    game->player->Speed_effect = 0;
    game->player->Health_effect = 0;
    game->player->Power_effect = 0;
    game->player->can_be_saved = 1;
    /* Room pointers must remain valid across level transitions. */
    for (int i = 0; i < 7; i++)
    {
        game->levels[i] = calloc(1, sizeof(Level));
        game->levels[i]->rooms = calloc(MAX_ROOMS, sizeof(Room *));
        Room *storage = calloc(MAX_ROOMS, sizeof(Room));
        game->levels[i]->level = i;
        game->levels[i]->key.x = 0;
        game->levels[i]->key.y = 0;
        for(int j = 0; j < 9; j++){
            game->levels[i]->rooms[j] = storage + j;
            // game->levels[i]->rooms[j]->doors = malloc(12 * sizeof(Door));
            // game->levels[i]->rooms[j]->pillars = malloc(5 * sizeof(Point));
        }
    }
    WINDOW *game_win;
    int map_width, map_height;
    getmaxyx(stdscr, map_height, map_width);
    if (map_height > MAP_ROWS) map_height = MAP_ROWS;
    if (map_width > MAP_COLS) map_width = MAP_COLS;
    int start_x = BORDER_PADDING;
    int start_y = BORDER_PADDING;
    int end_x = map_width * 3 / 4 - BORDER_PADDING;
    int end_y = map_height * 3 / 4 - BORDER_PADDING;

    start_color();
    ui_theme();
    game->map_height = map_height - 1;
    game->map_width = map_width - 1;
    game_win = newwin(map_height, map_width, 0, 0);
    { //Jsut a shit
    // if(game->currentLevel != 0){
    //     for(int i = 0; i < rooms[0].height; i++){
    //         int k = 0;
    //         for(int j = 0; j < rooms[0].width; j++){
    //             if(game->whole_map[game->currentLevel][i][j].item == '.'){
    //                 k = 1;
    //                 game->player->cord.x = rooms[0].x + i;
    //                 game->player->cord.y = rooms[0].y + j;
    //                 break;
    //             }
    //         }
    //         if(k){
    //             break;
    //         }
    //     }
    // }
    // struct timeval tv;
    // gettimeofday(&tv, NULL);
    // milliseconds = tv.tv_sec * 1000LL + tv.tv_usec / 1000;
    // // u = user;
    // // printf("%d , %d\n", maxX, maxY);
    // noecho();
    // damageTime -= (user->setting.level - 1) * 5;
    // time_t a = time(NULL);
    // initilizing the game
    // game->currentLevel = currentLevel;

    // initialize player
    // game->player->cord.x = game->levels[game->currentLevel]->rooms[0]->cord.x + 2;
    // game->player->cord.y = game->levels[game->currentLevel]->rooms[0]->cord.y + 2;
    // game->player->state = 1;
    // game->player->room = game->levels[game->currentLevel]->rooms[0];
    }
    if(new_game_check == 0){
        // generate_map();
        game->player->health = 100;
        game->player->shield = 0;
        game->player->foodCount = current_user.food;
        game->player->weaponCount = current_user.weaponsCount;
        game->player->spellCount = current_user.spellsCount;
        game->player->weapons = calloc(5, sizeof(Weapon));
        game->player->weapons[0].count = current_user.weaponsCount_each[0]; game->player->weapons[0].type = 'm'; strcpy(game->player->weapons[0].name, "Mace"); game->player->weapons[0].damage = 5;
        game->player->weapons[1].count = current_user.weaponsCount_each[1]; game->player->weapons[1].type = 'd'; strcpy(game->player->weapons[1].name, "Dagger"); game->player->weapons[1].damage = 12;
        game->player->weapons[2].count = current_user.weaponsCount_each[2]; game->player->weapons[2].type = 'w'; strcpy(game->player->weapons[2].name, "MagicWand"); game->player->weapons[2].damage = 15;
        game->player->weapons[3].count = current_user.weaponsCount_each[3]; game->player->weapons[3].type = 'a'; strcpy(game->player->weapons[3].name, "NormalArrow"); game->player->weapons[3].damage = 5;
        game->player->weapons[4].count = current_user.weaponsCount_each[4]; game->player->weapons[4].type = 's'; strcpy(game->player->weapons[4].name, "Sword"); game->player->weapons[4].damage = 10;
        game->player->spells = calloc(3, sizeof(Spell));
        game->player->spells[0].count = current_user.spellsCount_each[0]; game->player->spells[0].type = 's'; strcpy(game->player->spells[0].name, "Speed");
        game->player->spells[1].count = current_user.spellsCount_each[1]; game->player->spells[1].type = 'p'; strcpy(game->player->spells[1].name, "Power");
        game->player->spells[2].count = current_user.spellsCount_each[2]; game->player->spells[2].type = 'h'; strcpy(game->player->spells[2].name, "Healing");
        game->player->specialfoods = calloc(4, sizeof(Food));
        game->player->specialfoods[0].count = 0; game->player->specialfoods[0].type = 'n';
        game->player->specialfoods[1].count = 0; game->player->specialfoods[1].type = 'p';
        game->player->specialfoods[2].count = 0; game->player->specialfoods[2].type = 's';
        game->player->specialfoods[3].count = 0; game->player->specialfoods[3].type = 'r';
        game->player->currentWeapon = game->player->weapons[0];
        // for(int i = 0; i < 5; i++){
        //     strcpy(game->player->weapons[i].name, current_user.weapons[i]);
        // }
        // for(int i = 0; i < 3; i++){
        //     strcpy(game->player->spells[i].name, current_user.spells[i]);
        // }
        game->player->level = current_user.level;
        game->player->acientKey = current_user.ancientkey;
        game->player->brokenAcientKey = current_user.brokenancientkey;
        game->player->gold = current_user.golds;
        game->player->name = current_user.username;
        game->player->room_num = 0;
        game->player->points = current_user.points;
        game->player->games_count = current_user.games_count;
        game->player->play_time = current_user.play_time;
        game->new_game = false;
        if (!load_user_game()) {
            delwin(game_win); clear();
            mvprintw(2,2,"This save is incomplete or uses the old unsafe format. It has been kept unchanged.");
            mvprintw(4,2,"Start a New Game, or keep the old file for recovery. Press any key.");
            refresh(); music_getch(stdscr); return;
        }
    }
    else{
        game->new_game = false;
        game->player->health = 100;
        game->player->foodCount = 100;
        game->player->shield = 0;
        game->player->weaponCount = 0;
        game->player->spellCount = 0;
        game->player->weapons = calloc(5, sizeof(Weapon));
        game->player->weapons[0].count = 1; game->player->weapons[0].type = 'm'; strcpy(game->player->weapons[0].name, "Mace"); game->player->weapons[0].damage = 5;
        game->player->weapons[1].count = 0; game->player->weapons[1].type = 'd'; strcpy(game->player->weapons[1].name, "Dagger"); game->player->weapons[1].damage = 12;
        game->player->weapons[2].count = 0; game->player->weapons[2].type = 'w'; strcpy(game->player->weapons[2].name, "MagicWand"); game->player->weapons[2].damage = 15;
        game->player->weapons[3].count = 0; game->player->weapons[3].type = 'a'; strcpy(game->player->weapons[3].name, "NormalArrow"); game->player->weapons[3].damage = 5;
        game->player->weapons[4].count = 0; game->player->weapons[4].type = 's'; strcpy(game->player->weapons[4].name, "Sword"); game->player->weapons[4].damage = 10;
        game->player->spells = calloc(3, sizeof(Spell));
        game->player->spells[0].count = 0; game->player->spells[0].type = 's'; strcpy(game->player->spells[0].name, "Speed");
        game->player->spells[1].count = 0; game->player->spells[1].type = 'p'; strcpy(game->player->spells[1].name, "Power");
        game->player->spells[2].count = 0; game->player->spells[2].type = 'h'; strcpy(game->player->spells[2].name, "Healing");
        game->player->specialfoods = calloc(4, sizeof(Food));
        game->player->specialfoods[0].count = 0; game->player->specialfoods[0].type = 'n';
        game->player->specialfoods[1].count = 0; game->player->specialfoods[1].type = 'p';
        game->player->specialfoods[2].count = 0; game->player->specialfoods[2].type = 's';
        game->player->specialfoods[3].count = 0; game->player->specialfoods[3].type = 'r';
        game->player->currentWeapon = game->player->weapons[0];
        game->player->level = 0;
        game->currentLevel = 0;
        game->player->acientKey = 0;
        game->player->brokenAcientKey = 0;
        game->player->gold = current_user.golds;
        game->player->acientKey = current_user.ancientkey;
        game->player->brokenAcientKey = current_user.brokenancientkey;
        game->player->name = current_user.username;
        game->player->room_num = 0;
        game->player->points = current_user.points;
        game->player->games_count = current_user.games_count;
        game->player->play_time = current_user.play_time;
    }
    { // Player Color
        strcpy(game->player->color, which_color);
        if(!strcmp(game->player->color, " Yellow ")){
            color_pair_num = 1;
        }
        else if(!strcmp(game->player->color, " Red ")){
            color_pair_num = 6;
        }
        else if(!strcmp(game->player->color, " Blue ")){
            color_pair_num = 9;
        }
        else if(!strcmp(game->player->color, " Green ")){
            color_pair_num = 7;
        }
    }
    new_game_check = 0;
    game->player->can_be_saved = 1;
    pointss = 0;
    play_timee = 0;
    if(game->player->points >= 0){
        pointss = game->player->points;
    }
    else{
        game->player->points = 0;
    }
    if(game->player->play_time >= 0){
        play_timee = game->player->play_time;
    }
    else{
        game->player->play_time = 0;
    }
    victory_results_start(game->player->gold,play_timee);
    keypad(game_win, TRUE);
    mousemask(ALL_MOUSE_EVENTS| REPORT_MOUSE_POSITION, NULL);
    timeout(-1);
    printf("\033[?1003h\n");
    mouseinterval(0);
    clear();
    game_session_ready = true;
    game_play(game_win);
    if(escape_check){
        delwin(game_win);
        if(victory_results_ready()){
            printf("\033[?1003l\n");return;
        }
        time(&end_time);
        game->player->play_time = play_timee + difftime(end_time, start_time);
        if(game->player->points >= 0){
            game->player->points = pointss + difftime(end_time, start_time) * 5 * difficulty_efficiency + game->player->gold * 10;
        }
        else{
            game->player->points = difftime(end_time, start_time) * 5 * difficulty_efficiency + game->player->gold * 10;
        }
        printf("\033[?1003l\n");
        return;
    }
    wclear(game_win); 
    werase(game_win);
    wrefresh(game_win); 
    delwin(game_win);
    // music_getch(stdscr);
}
/* Original visual renderer, sharing the repaired game state. */
#include "game_legacy_visuals.inc"

/* Final boss telegraphs and presentation share the existing weapon/combat model. */
#include "boss_encounter.inc"

/* Victory reward selection and a frozen end-of-run summary. */
#include "victory_results.inc"
