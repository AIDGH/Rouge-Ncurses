#include "map_generator.h"
#include "game.h"
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <string.h>

extern char which_color[50];
extern Player *player;
extern Corridor **corridors;
int k = 0, k1 = 0;
int total_rooms;
int is_secret = 0;
int is_battle1 = 0;
extern Level *levels[6];
extern Game *game;
extern int current_level;
extern int unlocked_level;
extern int difficulty_efficiency;
int is_spell;
int is_reinforce;
int is_healing;
int is_nightmare;
int is_treasure = 0;
int is_stair;
int counter = 0;
int inter = 0;

void create_room(Room *room) {
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            if (y == room->y) {
                mvaddch(y, x, '_');
            } else if (y == room->y + room->height - 1) {
                mvaddch(y, x, '-');
                // if(x == room->x || x == room->x + room->width - 1){
                //     mvaddch(y, x, '|');
                // }
            } else if (x == room->x || x == room->x + room->width - 1) {
                mvaddch(y, x, '|');
            } else {
                mvaddch(y, x, '.');
            }
        }
    }
    room->type = 'n';
    // int type = rand() % (difficulty_efficiency + 1) * 2;
    int aa = rand() % (4 + difficulty_efficiency / 2);
    if(is_stair && rand() % 2){
        room->type = 'p';
    }
    else if(rand() % (25 / (difficulty_efficiency / 2 + 3)) == 0 && room->stairCount < 1 && room->doorCount != 1){
        room->type = 'm';
    }
    else if(aa == 0 && room->stairCount < 1 && room->doorCount != 1){
        room->type = 's';
    }
    else if(aa == 1 && room->stairCount < 1 && room->doorCount != 1){
        room->type = 'r';
    }
    room->isVisible = false;
    room->lock_attemps = 0;
    if(room->stairCount == 0){
        room->stair.cord.x = -1;
        room->stair.cord.y = -1;
    }
    // room->newWeapons = (Weapon *)malloc((room->width * room->height) * sizeof(Weapon));
    // room->newWeaponsCount = 0;
}
void create_secret_door(Room *room, Room *rooms) {
    int a = rand() % total_rooms - 1;
    // a = total_rooms - 2;
    int y1 = rooms[a].y + 1 + rand() % (rooms[a].height - 2);
    int y2 = room->y + 1 + rand() % (room->height - 2);
    if(mvwinch(stdscr, y1, rooms[a].x + rooms[a].width - 1) == '|'){
        room->doors = malloc(2 * sizeof(Door));
        room->doorCount = 1;
        room->doors[0].cord.x = room->x;
        room->doors[0].cord.y = y2;
        rooms[a].doors[rooms[a].doorCount].type = 's';
        rooms[a].doors[rooms[a].doorCount].password = 0;
        rooms[a].doors[rooms[a].doorCount].password2 = 0;
        rooms[a].doors[rooms[a].doorCount].cord.x = rooms[a].x + rooms[a].width - 1;
        rooms[a].doors[rooms[a].doorCount].cord.y = y1;
        rooms[a].doorCount++;
        mvaddch(y1, rooms[a].x + rooms[a].width - 1, '?');
        mvaddch(y2, room->x, '?');
        game->levels[game->currentLevel]->secret_room = a;
    }
    else{
        if(counter == 100){
            counter = 0;
            return;
        }
        create_secret_door(room, rooms);
    }
}
void correct_room(Room *room) {
    int count = 0;
    int check1 = 0, check2 = 0, check3 = 0, check4 = 0;
    room->doors = malloc(12 * sizeof(Door));
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            if(y == room->y && (mvwinch(stdscr, y - 1, x) == '#' || mvwinch(stdscr, y - 2, x) == '#') && check1 != 2){
                mvaddch(y - 1, x, '#');
                room->doors[count].type = 'n';
                room->doors[count].cord.x = x;
                room->doors[count].cord.y = y;
                room->doors[count].password = 0;
                room->doors[count].password2 = 0;
                count++;
                mvaddch(y, x, '+');
                check1 += 1;
            }
            else if(y == room->y + room->height - 1 && (mvwinch(stdscr, y + 1, x) == '#' || mvwinch(stdscr, y + 2, x) == '#') && check2 != 2){
                mvaddch(y + 1, x, '#');
                room->doors[count].type = 'n';
                room->doors[count].cord.x = x;
                room->doors[count].cord.y = y;
                room->doors[count].password = 0;
                room->doors[count].password2 = 0;
                count++;
                mvaddch(y, x, '+');
                check2 += 1;
            }
            else if(x == room->x && (mvwinch(stdscr, y, x - 1) == '#' || mvwinch(stdscr, y, x - 2) == '#') && check3 != 2){
                mvaddch(y, x - 1, '#');
                room->doors[count].type = 'n';
                room->doors[count].cord.x = x;
                room->doors[count].cord.y = y;
                room->doors[count].password = 0;
                room->doors[count].password2 = 0;
                count++;
                mvaddch(y, x, '+');
                check3 += 1;
            }
            else if(x == room->x + room->width - 1 && (mvwinch(stdscr, y, x + 1) == '#' || mvwinch(stdscr, y, x + 2) == '#') && check4 != 2){
                mvaddch(y, x + 1, '#');
                room->doors[count].type = 'n';
                room->doors[count].cord.x = x;
                room->doors[count].cord.y = y;
                room->doors[count].password = 0;
                room->doors[count].password2 = 0;
                count++;
                mvaddch(y, x, '+');
                check4 += 1;
            }
            else if (y == room->y && mvwinch(stdscr, y, x) == '#') {
                room->doors[count].type = 'n';
                room->doors[count].cord.x = x;
                room->doors[count].cord.y = y;
                room->doors[count].password = 0;
                room->doors[count].password2 = 0;
                count++;
                mvaddch(y, x, '+');
            } else if (y == room->y + room->height - 1 && mvwinch(stdscr, y, x) == '#') {
                room->doors[count].type = 'n';
                room->doors[count].cord.x = x;
                room->doors[count].cord.y = y;
                room->doors[count].password = 0;
                room->doors[count].password2 = 0;
                count++;
                mvaddch(y, x, '+');
            } else if ((x == room->x || x == room->x + room->width - 1) && mvwinch(stdscr, y, x) == '#') {
                room->doors[count].type = 'n';
                room->doors[count].cord.x = x;
                room->doors[count].cord.y = y;
                room->doors[count].password = 0;
                room->doors[count].password2 = 0;
                count++;
                mvaddch(y, x, '+');
            } else {
                if(mvwinch(stdscr, y, x) == '#')
                mvaddch(y, x, '.');
            }
        }
    }
    if(mvwinch(stdscr, room->y, room->x) == '+' && (mvwinch(stdscr, room->y, room->x + 1) != '+' && mvwinch(stdscr, room->y + 1, room->x) != '+')){
        mvaddch(room->y + 1, room->x, '+');
        room->doors[count].type = 'n';
        room->doors[count].cord.x = room->x;
        room->doors[count].cord.y = room->y;
        count++;
    }
    if(mvwinch(stdscr, room->y + room->height - 1, room->x) == '+' && 
    (mvwinch(stdscr, room->y + room->height - 1, room->x + 1) != '+' && mvwinch(stdscr, room->y + room->height - 2, room->x) != '+')){
        mvaddch(room->y  + room->height - 1, room->x + 1, '+');
        room->doors[count].type = 'n';
        room->doors[count].cord.x = room->x + 1;
        room->doors[count].cord.y = room->y  + room->height - 1;
        count++;
    }
    if(mvwinch(stdscr, room->y + room->height - 1, room->x + room->width - 1) == '+' && 
    (mvwinch(stdscr, room->y + room->height - 1, room->x + room->width - 2) != '+' && 
    mvwinch(stdscr, room->y + room->height - 2, room->x + room->width - 1) != '+')){
        mvaddch(room->y  + room->height - 1, room->x  + room->width - 2, '+');
        room->doors[count].type = 'n';
        room->doors[count].cord.x = room->x + room->width - 2;
        room->doors[count].cord.y = room->y + room->height - 1;
        count++;
    }
    if(mvwinch(stdscr, room->y, room->x + room->width - 1) == '+' && (mvwinch(stdscr, room->y, room->x + room->width - 2) != '+'
     && mvwinch(stdscr, room->y + 1, room->x + room->width - 1) != '+')){
        mvaddch(room->y + 1, room->x + room->width - 1, '+');
        room->doors[count].type = 'n';
        room->doors[count].cord.x = room->x  + room->width - 1;
        room->doors[count].cord.y = room->y + 1;
        count++;
    }
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            if(mvwinch(stdscr, y, x) == 'O' && (mvwinch(stdscr, y - 1, x) == '+' || mvwinch(stdscr, y + 1, x) == '+'
             || mvwinch(stdscr, y, x - 1) == '+' || mvwinch(stdscr, y, x + 1) == '+')){
                mvaddch(y, x, '.');
             }
        }
    }
    room->doorCount = count;
}
void draw_window(Room *room) {
    int num_windows = rand() % 2;
    if(game->currentLevel == 6){
        num_windows = 1;
    }
    int side = 4, last_side = 4;
    room->window.x = 0;
    room->window.y = 0;
    for (int i = 0; i < num_windows; i++) {
        while(last_side == side){
            side = rand() % 4;
        }
        last_side = side;
        int position;
        switch (side) {
            case 0:
                position = room->x + 1 + rand() % (room->width - 2);
                if(mvwinch(stdscr, room->y, position) != '|' && 
                mvwinch(stdscr, room->y, position) != '-' && 
                mvwinch(stdscr, room->y, position) != '_'){
                    draw_window(room);
                }
                mvaddch(room->y, position, '=');
                room->window.x = position;
                room->window.y = room->y;
                break;
            case 1:
                position = room->x + 1 + rand() % (room->width - 2);
                if(mvwinch(stdscr, room->y + room->height - 1, position) != '|' && 
                mvwinch(stdscr, room->y + room->height - 1, position) != '-' && 
                mvwinch(stdscr, room->y + room->height - 1, position) != '_'){
                    draw_window(room);
                }
                mvaddch(room->y + room->height - 1, position, '=');
                room->window.x = position;
                room->window.y = room->y + room->height - 1;
                break;
            case 2:
                position = room->y + 1 + rand() % (room->height - 2);
                if(mvwinch(stdscr, position, room->x) != '|' && 
                mvwinch(stdscr, position, room->x) != '-' && 
                mvwinch(stdscr, position, room->x) != '_'){
                    draw_window(room);
                }
                mvaddch(position, room->x, '=');
                room->window.x = room->x;
                room->window.y = position;
                break;
            case 3:
                position = room->y + 1 + rand() % (room->height - 2);
                if(mvwinch(stdscr, position, room->x + room->width - 1) != '|' && 
                mvwinch(stdscr, position, room->x + room->width - 1) != '-' && 
                mvwinch(stdscr, position, room->x + room->width - 1) != '_'){
                    draw_window(room);
                }
                mvaddch(position, room->x + room->width - 1, '=');
                room->window.x = room->x + room->width - 1;
                room->window.y = position;
                break;
        }
    }
}
void create_pillars(Room *room) {
    int num_objects = 1 + rand() % (room->width * room->height / 50);
    if(room->type == 'b' || is_battle1){
        num_objects = 7 + rand() % 6;
    }
    room->pillars = malloc(num_objects * sizeof(Point));
    room->pillarCount = num_objects;
    for (int i = 0; i < num_objects; i++) {
        int x = room->x + 2 + rand() % (room->width - 4);
        int y = room->y + 2 + rand() % (room->height - 4);
        if(mvwinch(stdscr, y, x) == '.'){
            mvaddch(y, x, 'O');
            room->pillars[i].x = x;
            room->pillars[i].y = y;
        }
        else{
            i--;
        }
    }
}
void create_food(Room *room) {
    int num_objects;
    if(is_healing){
        num_objects = 3 + rand() % 3;
    }
    else if(is_treasure == 0){
        num_objects = 1 + rand() % (room->width * room->height / 50 + 1) + 1*is_secret;
    }
    else{
        num_objects = 20 + rand() % 5;
    }
    room->foods = malloc(num_objects * sizeof(Food));
    room->foodCount = num_objects;
    int numbers = 0;
    for (int i = 0; i < num_objects; i++) {
        int x = room->x + 1 + rand() % (room->width - 2);
        int y = room->y + 1 + rand() % (room->height - 2);
        if(mvwinch(stdscr, y, x) == '.'){
            mvaddch(y, x, '%');
            room->foods[i].cord.x = x;
            room->foods[i].cord.y = y;
            room->foods[i].health = (rand() % 4 + 1) * 5;
            room->foods[i].isUsed = false;
            room->foods[i].type = 'n';
            numbers++;
        }
        else{
            if(counter == 200){
                room->foodCount = numbers;
                counter = 0;
                break;
            }
            counter++;
            i--;
        }
    }
    counter = 0;
}
void create_shield(Room *room) {
    int num_objects;
    if(is_reinforce){
        num_objects = 3 + rand() % 3;
    }
    else if(is_treasure == 0){
        num_objects = rand() % 3 + 1*is_secret;
    }
    else{
        num_objects = 10 + rand() % 5;
    }
    room->shields = malloc(num_objects * sizeof(Shield));
    room->shieldCount = num_objects;
    int numbers = 0;
    for (int i = 0; i < num_objects; i++) {
        int x = room->x + 1 + rand() % (room->width - 2);
        int y = room->y + 1 + rand() % (room->height - 2);
        if(mvwinch(stdscr, y, x) == '.' && mvwinch(stdscr, y, x + 1) == '.'){
            mvaddch(y, x, '!');
            mvaddch(y, x + 1, '!');
            room->shields[i].cord.x = x;
            room->shields[i].cord.y = y;
            room->shields[i].amount = (rand() % 4 + 2) * 5;
            room->shields[i].isUsed = false;
            numbers++;
        }
        else{
            if(counter == 200){
                room->shieldCount = numbers;
                counter = 0;
                break;
            }
            counter++;
            i--;
        }
    }
    counter = 0;
}
void create_special_food(Room *room) {
    int num_objects;
    if(is_healing){
        num_objects = 3 + rand() % 3;
    }
    else if(is_treasure == 0){
        num_objects = 1 + rand() % (room->width * room->height / 50) + 2*is_secret + 2*is_spell;
    }
    else{
        num_objects = 15 + rand() % 6;
    }
    room->specialfoods = malloc(num_objects * sizeof(Food));
    room->specialfoodCount = num_objects;
    int numbers = 0;
    for (int i = 0; i < num_objects; i++) {
        int x = room->x + 1 + rand() % (room->width - 2);
        int y = room->y + 1 + rand() % (room->height - 2);
        if(mvwinch(stdscr, y, x) == '.' && mvwinch(stdscr, y, x + 1) == '.'){
            room->specialfoods[i].cord.x = x;
            room->specialfoods[i].cord.y = y;
            room->specialfoods[i].isUsed = false;
            int type = rand() % 4;
            if(type == 0){
                room->specialfoods[i].type = 'n';
                mvaddch(y, x, '6');
                mvaddch(y, x + 1, '6');
            }
            else if(type == 1){
                room->specialfoods[i].type = 'p';
                mvaddch(y, x, '7');
                mvaddch(y, x + 1, '7');
            }
            else if(type == 2){
                room->specialfoods[i].type = 's';
                mvaddch(y, x, '8');
                mvaddch(y, x + 1, '8');
            }
            else if(type == 3){
                room->specialfoods[i].type = 'r';
                mvaddch(y, x, '9');
                mvaddch(y, x + 1, '9');
            }
            numbers++;
        }
        else{
            if(counter == 200){
                room->specialfoodCount = numbers;
                counter = 0;
                break;
            }
            counter++;
            i--;
        }
    }
    counter = 0;
}
void create_gold(Room *room) {
    int num_objects;
    if(is_treasure == 0){
        num_objects = 1 + rand() % (room->width * room->height / 50) + 2*is_secret;
    }
    else{
        num_objects = 25 + rand() % 5;
    }
    room->golds = malloc((num_objects + 1) * sizeof(Gold));
    room->goldCount = num_objects;
    int numbers = 0;
    for (int i = 0; i < num_objects; i++) {
        int x = room->x + 2 + rand() % (room->width - 4);
        int y = room->y + 2 + rand() % (room->height - 4);
        if(mvwinch(stdscr, y, x) == '.'){
            mvaddch(y, x, '$');
            room->golds[i].cord.x = x;
            room->golds[i].cord.y = y;
            room->golds[i].type = 'n';
            room->golds[i].count = (rand() % 4 + 2);
            room->golds[i].isUsed = false;
            numbers++;
        }
        else{
            if(counter == 200){
                room->goldCount = numbers;
                counter = 0;
                break;
            }
            counter++;
            i--;
        }
    }
    if(rand() % 10 == 9){
        while(1){
            int x = room->x + 2 + rand() % (room->width - 4);
            int y = room->y + 2 + rand() % (room->height - 4);
            if(mvwinch(stdscr, y, x) == '.' && mvwinch(stdscr, y, x + 1) == '.'){
                room->golds[room->goldCount].cord.x = x;
                room->golds[room->goldCount].cord.y = y;
                room->golds[room->goldCount].type = 'b';
                room->golds[room->goldCount].count = (rand() % 10) * 5 + 10;
                room->golds[room->goldCount].isUsed = false;
                room->goldCount++;
                mvaddch(y, x, '*');
                mvaddch(y, x + 1, '*');
                break;
            }
            if(counter == 300){
                counter = 0;
                break;
            }
            counter++;
        }
    }
    counter = 0;
}
void create_spell(Room *room) {
    int num_objects;
    if(is_treasure == 0){
        num_objects = 1 + rand() % (room->width * room->height / 50) + 2*is_secret + 2*is_spell;
    }
    else{
        num_objects = 15 + rand() % 6;
    }
    room->spells = malloc(num_objects * sizeof(Spell));
    room->spellCount = num_objects;
    int numbers = 0;
    for (int i = 0; i < num_objects; i++) {
        int x = room->x + 2 + rand() % (room->width - 4);
        int y = room->y + 2 + rand() % (room->height - 4);
        if(mvwinch(stdscr, y, x) == '.' && mvwinch(stdscr, y, x + 1) == '.'){
            room->spells[i].cord.x = x;
            room->spells[i].cord.y = y;
            room->spells[i].isUsed = false;
            int type = rand() % 3;
            if(type == 0){
                room->spells[i].type = 's';
                strcpy(room->spells[i].name, "Speed");
                mvaddch(y, x, 'E');
                mvaddch(y, x + 1, 'E');
            }
            else if(type == 1){
                room->spells[i].type = 'p';
                strcpy(room->spells[i].name, "Power");
                mvaddch(y, x, 'P');
                mvaddch(y, x + 1, 'P');
            }
            else if(type == 2){
                room->spells[i].type = 'h';
                strcpy(room->spells[i].name, "Healing");
                mvaddch(y, x, 'H');
                mvaddch(y, x + 1, 'H');
            }
        }
        else{
            if(counter == 200){
                room->spellCount = numbers;
                counter = 0;
                break;
            }
            counter++;
            i--;
        }
    }
    counter = 0;
}
void create_weapon(Room *room) {
    int num_objects = 1 + rand() % (room->width * room->height / 50) + 2*is_secret + 2*is_nightmare;
    if(is_reinforce){
        num_objects = 3 + rand() % 3;
    }
    room->weapons = (Weapon *)malloc((room->width * room->height) * sizeof(Weapon));
    room->weaponCount = num_objects;
    int numbers = 0;
    for (int i = 0; i < num_objects; i++) {
        int x = room->x + 2 + rand() % (room->width - 4);
        int y = room->y + 2 + rand() % (room->height - 4);
        if(mvwinch(stdscr, y, x) == '.' && mvwinch(stdscr, y, x + 1) == '.'){
            room->weapons[i].cord.x = x;
            room->weapons[i].cord.y = y;
            room->weapons[i].isUsed = false;
            int type = rand() % 6;
            numbers++;
            // if(type == 0){
            //     room->weapons[i].type = 'm';
            //     room->weapons[i].damage = 5;
                // room->weapons[i].range = 1`;
            //     strcpy(room->weapons[i].name, "Mace");
            //     mvaddch(y, x, '1');
            //     mvaddch(y, x + 1, '1');
            // }
            if(type == 1 || type == 0){
                room->weapons[i].type = 'd';
                room->weapons[i].damage = 12;
                room->weapons[i].range = 5;
                room->weapons[i].count = 10;
                strcpy(room->weapons[i].name, "Dagger");
                mvaddch(y, x, '2');
                mvaddch(y, x + 1, '2');
            }
            else if(type == 2){
                room->weapons[i].type = 'w';
                room->weapons[i].damage = 15;
                room->weapons[i].range = 10;
                room->weapons[i].count = 8;
                strcpy(room->weapons[i].name, "MagicWand");
                mvaddch(y, x, '3');
                // mvaddch(y, x + 1, '3');
            }
            else if(type == 3 || type == 4){
                room->weapons[i].type = 'a';
                room->weapons[i].damage = 5;
                room->weapons[i].range = 5;
                room->weapons[i].count = 20;
                strcpy(room->weapons[i].name, "NormalArrow");
                mvaddch(y, x, '4');
                mvaddch(y, x + 1, '4');
            }
            else if(type == 5){
                room->weapons[i].type = 's';
                room->weapons[i].damage = 10;
                room->weapons[i].range = 1;
                room->weapons[i].count = 1;
                strcpy(room->weapons[i].name, "Sword");
                mvaddch(y, x, '5');
                mvaddch(y, x + 1, '5');
            }
        }
        else{
            i--;
            if(counter == 200){
                room->weaponCount = numbers;
                counter = 0;
                break;
            }
            counter++;
        }
    }
    counter = 0;
    for(int i = 0; i < numbers; i++){
        if(room->weapons[i].type != 'w'){
            mvaddch(room->weapons[i].cord.y, room->weapons[i].cord.x + 1, '.');
        }
    }
}
void draw_traps(Room *room) {
    int num_objects;
    if(is_treasure == 0){
        num_objects = 1 + rand() % (room->width * room->height / 81);
    }
    else{
        num_objects = 25 + rand() % 10;
    }
    room->traps = malloc(num_objects * sizeof(Trap));
    room->trapCount = num_objects;
    int numbers;
    for (int i = 0; i < num_objects; i++) {
        int x = room->x + 1 + rand() % (room->width - 2);
        int y = room->y + 1 + rand() % (room->height - 2);
        if(mvwinch(stdscr, y, x) == '.'){
            mvaddch(y, x, '^');
            room->traps[i].cord.x = x;
            room->traps[i].cord.y = y;
            room->traps[i].damage = 5 + 5 * (rand() % 3);
            room->traps[i].isVisible = false;
            numbers++;
        }
        else{
            if(counter == 200){
                room->trapCount = numbers;
                counter = 0;
                break;
            }
            counter++;
            i--;
        }
    }
    counter = 0;
}
void draw_enemies(Room *room) {
    int num_objects;
    if(room->type == 'b'){
        num_objects = 1 + rand() % 3;
    }
    else if(room->type == 'm'){
        num_objects = rand() % 2 + difficulty_efficiency / 3;
    }
    else if(is_treasure == 0){
        num_objects = 1 + rand() % (room->width * room->height / 81 + difficulty_efficiency / 2);
    }
    else{
        // num_objects = 1;
        num_objects = 20 + rand() % 5;
    }
    if(difficulty_efficiency >= 3){
        num_objects += rand() % (room->width * room->height / 81);
    }
    room->enemies = malloc((num_objects + 1) * sizeof(Enemy));
    room->enemyCount = num_objects;
    room->enemyLeft = num_objects;
    int numbers;
    for (int i = 0; i < num_objects; i++) {
        int x = room->x + 2 + rand() % (room->width - 4);
        int y = room->y + 2 + rand() % (room->height - 4);
        if(mvwinch(stdscr, y, x) == '.'){
            room->enemies[i].cord.x = x;
            room->enemies[i].cord.y = y;
            room->enemies[i].isAlive = true;
            room->enemies[i].isVisible = false;
            if(room->type == 't' || room->type == 'b'){
                room->enemies[i].isVisible = true;
            }
            room->enemies[i].canMove = true;
            room->enemies[i].moves = 8;
            int type = rand() % 5;
            numbers++;
            if(type == 0){
                room->enemies[i].type = 'd';
                strcpy(room->enemies[i].name, "Deamon");
                room->enemies[i].health = 10;
                room->enemies[i].damage = 5;
                mvaddch(y, x, 'D');
            }
            else if(type == 1){
                room->enemies[i].type = 'f';
                strcpy(room->enemies[i].name, "FireBreathingMonster");
                room->enemies[i].health = 10;
                room->enemies[i].damage = 10;
                mvaddch(y, x, 'F');
            }
            else if(type == 2){
                room->enemies[i].type = 'g';
                strcpy(room->enemies[i].name, "Giant");
                room->enemies[i].health = 20;
                room->enemies[i].damage = 10;
                mvaddch(y, x, 'G');
            }
            else if(type == 3){
                room->enemies[i].type = 's';
                strcpy(room->enemies[i].name, "Snake");
                room->enemies[i].health = 20;
                room->enemies[i].damage = 15;
                room->enemies[i].isVisible = true;
                if(room->type != 't'){
                    room->enemies[i].moves = 1;
                }
                mvaddch(y, x, 'S');
            }
            else if(type == 4){
                room->enemies[i].type = 'u';
                strcpy(room->enemies[i].name, "Undead");
                room->enemies[i].health = 30;
                room->enemies[i].damage = 20;
                mvaddch(y, x, 'U');
            }
        }
        else{
            i--;
            if(counter == 200){
                room->enemyLeft = numbers;
                room->enemyCount = numbers;
                counter = 0;
                break;
            }
            counter++;
        }
    }
    if(room->type == 't'){
        for (int i = 0; i < 1; i++) {
            int x = room->x + 2 + rand() % (room->width - 4);
            int y = room->y + 2 + rand() % (room->height - 4);
            if(mvwinch(stdscr, y, x) == '.'){
                room->enemies[room->enemyCount].type = 'i';
                strcpy(room->enemies[room->enemyCount].name, "Immortal");
                room->enemies[room->enemyCount].health = 200;
                room->enemies[room->enemyCount].damage = 25;
                room->enemies[room->enemyCount].cord.x = x;
                room->enemies[room->enemyCount].cord.y = y;
                room->enemies[room->enemyCount].canMove = true;
                room->enemies[room->enemyCount].isVisible = true;
                room->enemies[room->enemyCount].isAlive = false;
                room->enemies[room->enemyCount].moves = 8;
                mvaddch(y, x, 'I');
                break;
            }
            else{
                i--;
            }
        }
    }
    counter = 0;
}
void draw_stair_1(Room *room) {
    int x = room->x + 2 + rand() % (room->width - 3);
    int y = room->y + 2 + rand() % (room->height - 3);
    if(mvwinch(stdscr, y, x) == '.' && mvwinch(stdscr, y, x + 1) == '.'){
        mvaddch(y, x, '>');
        mvaddch(y, x + 1, '>');
        room->stair.from = game->currentLevel;
        room->stair.to = game->currentLevel + 1;
        room->stairCount = 1;
        room->stair.cord.x = x;
        room->stair.cord.y = y;
    }
    else{
        draw_stair_1(room);
    }
}
void draw_stair_2(Room *room) {
    int x = room->x + 2 + rand() % (room->width - 3);
    int y = room->y + 2 + rand() % (room->height - 3);
    if(mvwinch(stdscr, y, x) == '.' && mvwinch(stdscr, y, x + 1) == '.'){
        mvaddch(y, x, '<');
        mvaddch(y, x + 1, '<');
        room->stair.from = game->currentLevel;
        room->stair.to = game->currentLevel - 1;
        room->stair.cord.x = x;
        room->stair.cord.y = y;
        room->stairCount = 1;
        game->levels[game->currentLevel]->key.x = x;
        game->levels[game->currentLevel]->key.y = y;
    }
    else{
        draw_stair_2(room);
    }
}
void draw_ancient_key(Room *room) {
    int x = room->x + 2 + rand() % (room->width - 3);
    int y = room->y + 2 + rand() % (room->height - 3);
    if(mvwinch(stdscr, y, x) == '.'){
        // attron(COLOR_PAIR(12));
        mvaddch(y, x, 'A');
        // attroff(COLOR_PAIR(12));
        room->keyCount = 1;
        room->ancientkey.x = x;
        room->ancientkey.y = y;
    }
    else{
        draw_ancient_key(room);
    }
}
void draw_player(Room *room) {
    int x = room->x + 2 + rand() % (room->width - 3);
    int y = room->y + 2 + rand() % (room->height - 3);
    if(mvwinch(stdscr, y, x) == '.'){
        if(!strcmp(which_color, " Yellow ")){
            attron(COLOR_PAIR(1));
        }
        else if(!strcmp(which_color, " Red ")){
            attron(COLOR_PAIR(6));
        }
        else if(!strcmp(which_color, " Blue ")){
            attron(COLOR_PAIR(9));
        }
        else if(!strcmp(which_color, " Green ")){
            attron(COLOR_PAIR(7));
        }
        mvaddch(y, x, '@');
        game->player->cord.x = x;
        game->player->cord.y = y;
        if(!strcmp(which_color, " Yellow ")){
            attroff(COLOR_PAIR(1));
        }
        else if(!strcmp(which_color, " Red ")){
            attroff(COLOR_PAIR(6));
        }
        else if(!strcmp(which_color, " Blue ")){
            attroff(COLOR_PAIR(9));
        }
        else if(!strcmp(which_color, " Green ")){
            attroff(COLOR_PAIR(7));
        }
    }
    else{
        draw_player(room);
    }
}
void create_lock_key(Room *room){
    int exist = 0;
    for(int i = 0; i < room->doorCount; i++){
        int invalid_check = 0;
        int x2 = room->doors[i].cord.x;
        int y2 = room->doors[i].cord.y;
        for(int j = 0; j < room->doorCount; j++){
            int x1 = room->doors[j].cord.x;
            int y1 = room->doors[j].cord.y;
            if((x1 == x2 && (y1 == y2 + 1 || y1 == y2 - 1)) || (y1 == y2 && (x1 == x2 + 1 || x1 == x2 - 1))){
                invalid_check = 1;
            }
        }
        if(invalid_check == 0){
            if(game->currentLevel < 1){
                room->doors[i].type = 'l';
            }
            else{
                room->doors[i].type = 'h';
                room->doors[i].password2 = rand() % 9999;
            }
            mvaddch(y2, x2, 'L');
            room->lock_door = i;
            room->doors[i].password = rand() % 9999;
            room->doors[i].is_open = false;
            exist = 1;
            break;
        }
    }
    if(exist && game->currentLevel != 6){
        for(int i = 0; i < 1; i++){
            int a = rand() % 4;
            int x2, y2;
            switch (a){
                case 0:
                    x2 = room->x + 1 + rand() % (room->width - 2);
                    y2 = room->y + room->height - 2;
                    if(mvwinch(stdscr, y2 + 1, x2) != '+' && mvwinch(stdscr, y2 + 1, x2) != 'L' && mvwinch(stdscr, y2 + 1, x2) != '=' && mvwinch(stdscr, y2, x2) == '.'){
                        mvaddch(y2, x2, '&');
                        room->password_generator.x = x2;
                        room->password_generator.y = y2;
                        i++;
                    }
                    break;
                case 1:
                    x2 = room->x + 1 + rand() % (room->width - 2);
                    y2 = room->y + 1;
                    if(mvwinch(stdscr, y2 - 1, x2) != '+' && mvwinch(stdscr, y2 - 1, x2) != 'L' && mvwinch(stdscr, y2 - 1, x2) != '=' && mvwinch(stdscr, y2, x2) == '.'){
                        mvaddch(y2, x2, '&');
                        room->password_generator.x = x2;
                        room->password_generator.y = y2;
                        i++;
                    }
                    break;
                case 2:
                    y2 = room->y + 1 + rand() % (room->height - 2);
                    x2 = room->x + 1;
                    if(mvwinch(stdscr, y2, x2 - 1) != '+' && mvwinch(stdscr, y2, x2 - 1) != 'L' && mvwinch(stdscr, y2, x2 - 1) != '=' && mvwinch(stdscr, y2, x2) == '.'){
                        mvaddch(y2, x2, '&');
                        room->password_generator.x = x2;
                        room->password_generator.y = y2;
                        i++;
                    }
                    break;
                case 3:
                    y2 = room->y + 1 + rand() % (room->height - 2);
                    x2 = room->x + room->width - 2;
                    if(mvwinch(stdscr, y2, x2 + 1) != '+' && mvwinch(stdscr, y2, x2 + 1) != 'L' && mvwinch(stdscr, y2, x2 + 1) != '=' && mvwinch(stdscr, y2, x2) == '.'){
                        mvaddch(y2, x2, '&');
                        room->password_generator.x = x2;
                        room->password_generator.y = y2;
                        i++;
                    }
                    break;
            }
            i--;
        }
        if(room->doors[room->lock_door].type == 'h'){
            for(int i = 0; i < 1; i++){
                int a = rand() % 4;
                int x2, y2;
                switch (a){
                    case 0:
                        x2 = room->x + 1 + rand() % (room->width - 2);
                        y2 = room->y + room->height - 2;
                        if(mvwinch(stdscr, y2 + 1, x2) != '+' && mvwinch(stdscr, y2 + 1, x2) != 'L' && mvwinch(stdscr, y2 + 1, x2) != '=' && mvwinch(stdscr, y2, x2) == '.'){
                            mvaddch(y2, x2, '&');
                            room->password_generator2.x = x2;
                            room->password_generator2.y = y2;
                            i++;
                        }
                        break;
                    case 1:
                        x2 = room->x + 1 + rand() % (room->width - 2);
                        y2 = room->y + 1;
                        if(mvwinch(stdscr, y2 - 1, x2) != '+' && mvwinch(stdscr, y2 - 1, x2) != 'L' && mvwinch(stdscr, y2 - 1, x2) != '=' && mvwinch(stdscr, y2, x2) == '.'){
                            mvaddch(y2, x2, '&');
                            room->password_generator2.x = x2;
                            room->password_generator2.y = y2;
                            i++;
                        }
                        break;
                    case 2:
                        y2 = room->y + 1 + rand() % (room->height - 2);
                        x2 = room->x + 1;
                        if(mvwinch(stdscr, y2, x2 - 1) != '+' && mvwinch(stdscr, y2, x2 - 1) != 'L' && mvwinch(stdscr, y2, x2 - 1) != '=' && mvwinch(stdscr, y2, x2) == '.'){
                            mvaddch(y2, x2, '&');
                            room->password_generator2.x = x2;
                            room->password_generator2.y = y2;
                            i++;
                        }
                        break;
                    case 3:
                        y2 = room->y + 1 + rand() % (room->height - 2);
                        x2 = room->x + room->width - 2;
                        if(mvwinch(stdscr, y2, x2 + 1) != '+' && mvwinch(stdscr, y2, x2 + 1) != 'L' && mvwinch(stdscr, y2, x2 + 1) != '=' && mvwinch(stdscr, y2, x2) == '.'){
                            mvaddch(y2, x2, '&');
                            room->password_generator2.x = x2;
                            room->password_generator2.y = y2;
                            i++;
                        }
                        break;
                }
                i--;
            }
        }
    }
}
int intersects(Room *rooms, int num_rooms, Room room, int total_rooms) {
    for (int i = 0; i < num_rooms; i++) {
        if (room.x < rooms[i].x + rooms[i].width + ROOM_PADDING + (INTERSECT_SPACE - inter) &&
            room.x + room.width + ROOM_PADDING > rooms[i].x - (INTERSECT_SPACE - inter) &&
            room.y < rooms[i].y + rooms[i].height + ROOM_PADDING + (INTERSECT_SPACE - inter) &&
            room.y + room.height + ROOM_PADDING > rooms[i].y - (INTERSECT_SPACE - inter)) {
            return 1;
        }
    }
    if (room.x < rooms[total_rooms - 1].x + rooms[total_rooms - 1].width + ROOM_PADDING + (INTERSECT_SPACE - inter) &&
        room.x + room.width + ROOM_PADDING > rooms[total_rooms - 1].x - (INTERSECT_SPACE - inter) &&
        room.y < rooms[total_rooms - 1].y + rooms[total_rooms - 1].height + ROOM_PADDING + (INTERSECT_SPACE - inter) &&
        room.y + room.height + ROOM_PADDING > rooms[total_rooms - 1].y - (INTERSECT_SPACE - inter)) {
        return 1;
    }
    return 0;
}
int is_valid_move(int x, int y, Room *rooms, int num_rooms) {
    for (int i = 0; i < num_rooms; i++) {
        if (x >= rooms[i].x && x < rooms[i].x + rooms[i].width &&
            y >= rooms[i].y && y < rooms[i].y + rooms[i].height) {
            return 0;
        }
    }
    return 1;
}
int check_rooms_validate(int room_checker[1000][2], int t, int num_rooms){
    int a = 0, b = 0, c = 0;
    for(int i = 0; i < t; i++){
        if(room_checker[i][0] == 0 && b == 0){
            a++;
            b = 1;
        }
        if(room_checker[i][0] == num_rooms - 1 && c == 0){
            a++;
            c = 1;
        }
    }
    if(a == 2){
        return 0;
    }
    return 1;
}
void connect_random_rooms(Room *rooms, int num_rooms, int room_checker[1000][2], int *t) {
    if (num_rooms < 2) return;

    int room1_index = rand() % num_rooms;
    int room2_index;
    do {
        room2_index = rand() % num_rooms;
    } while (room2_index == room1_index);

    for (int i = 0; i < *t; i++){
        if((room_checker[i][0] == room1_index && room_checker[i][1] == room2_index)){
            return ;
        }
    }

    room_checker[*t][0] = room1_index;
    room_checker[*t][1] = room2_index;
    (*t)++;
    Room room1 = rooms[room1_index];
    Room room2 = rooms[room2_index];

    int side1 = rand() % 4;
    int side2 = rand() % 4;

    int x1, y1, x2, y2;

    switch (side1) {
        case 0:
            x1 = room1.x + 1 + rand() % (room1.width - 3);
            y1 = room1.y;
            break;
        case 1:
            x1 = room1.x + 1 + rand() % (room1.width - 3);
            y1 = room1.y + room1.height - 1;
            break;
        case 2:
            x1 = room1.x;
            y1 = room1.y + 1 + rand() % (room1.height - 3);
            break;
        case 3:
            x1 = room1.x + room1.width - 1;
            y1 = room1.y + 1 + rand() % (room1.height - 3);
            break;
    }

    switch (side2) {
        case 0:
            x2 = room2.x + 1 + rand() % (room2.width - 3);
            y2 = room2.y;
            break;
        case 1:
            x2 = room2.x + 1 + rand() % (room2.width - 3);
            y2 = room2.y + room2.height - 1;
            break;
        case 2:
            x2 = room2.x;
            y2 = room2.y + 1 + rand() % (room2.height - 3);
            break;
        case 3:
            x2 = room2.x + room2.width - 1;
            y2 = room2.y + 1 + rand() % (room2.height - 3);
            break;
    }

    int dx = (x2 > x1) ? 1 : (x2 < x1) ? -1 : 0;
    int dy = (y2 > y1) ? 1 : (y2 < y1) ? -1 : 0;

    int x = x1 + dx, y = y1 + dy;
    k1 = 0;
    while (x != x2 || y != y2) {
        for(int i = 0; i < rand () % 15 && x != x2; i++){
            if(!is_valid_move(x, y, rooms, num_rooms)){
                if(x != x1 + dx){
                    //corridors[k]->points[k1].x = x;
                    //corridors[k]->points[k1].y = y;
                    mvaddch(y, x, '#');
                    k1++;
                    //corridors[k]->points[k1].x = x + dx;
                    //corridors[k]->points[k1].y = y;
                    k1++;
                    mvaddch(y, x + dx, '#');
                    // mvaddch(x1, y1, '#');
                    //corridors[k]->index = k;
                    //corridors[k]->count = k1;
                }
                else{
                    k--;
                    if(room_checker[*t - 1][0] == 0){
                        room_checker[*t - 1][0] = 100;
                        room_checker[*t - 1][1] = 100;
                    }
                }
                return ;
            }
            else{
                //corridors[k]->points[k1].x = x;
                //corridors[k]->points[k1].y = y;
                k1++;
                mvaddch(y, x, '#');
                x += dx;
            }
        }
        for(int i = 0; i < rand () % 15 && y != y2; i++){
            if(!is_valid_move(x, y, rooms, num_rooms)){
                if(y != y1 + dx){
                    //corridors[k]->points[k1].x = x;
                    //corridors[k]->points[k1].y = y;
                    k1++;
                    mvaddch(y, x, '#');
                    //corridors[k]->points[k1].x = x;
                    //corridors[k]->points[k1].y = y + dy;
                    k1++;
                    mvaddch(y + dy, x, '#');
                    //corridors[k]->index = k;
                    //corridors[k]->count = k1;
                    k++;
                }
                else{
                    k--;
                    if(room_checker[*t - 1][0] == num_rooms - 1){
                        room_checker[*t - 1][0] = 100;
                        room_checker[*t - 1][1] = 100;
                    }
                }
                return ;
            }
            else{
                //corridors[k]->points[k1].x = x;
                //corridors[k]->points[k1].y = y;
                k1++;
                mvaddch(y, x, '#');
                y += dy;
            }
        }
    }
    //corridors[k]->index = k;
    //corridors[k]->count = k1;
    mvaddch(x1, y1, '#');
    // mvaddch(x, y, '#');
    return;
}
void sleeping(){
    sleep(1);
}
void generate_map() {
    is_battle1 = 0;
    is_treasure = 0;
    is_healing = 0;
    is_spell = 0;
    is_healing = 0;
    clear();
    // Room rooms[10];
    Room rooms1[12];
    Level level1;
    int num_rooms = 0;
    int room_checker[1000][2], t = 0;
    int map_width, map_height;
    int is_secret_room = 0;
    getmaxyx(stdscr, map_height, map_width);
    int start_x = BORDER_PADDING + 2;
    int start_y = BORDER_PADDING + 1;
    int end_x = map_width * 3 / 4 - BORDER_PADDING - 5;
    int end_y = map_height * 3 / 4 - BORDER_PADDING - 2;
    srand(time(NULL));
    start_color();
    // init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    // corridors = malloc(50 * sizeof(Corridor *));
    // for(int i = 0; i < 50; i++){
    //     corridors[i] = malloc(1 * sizeof(Corridor));
    // }
    if(game->currentLevel < game->levelCount - 1){
        total_rooms = 6 + rand() % 3;
        int total_rooms_1 = total_rooms;
        game->levels[game->currentLevel]->is_secret_room = false;
        if(rand() % 2){
            is_secret_room = 1;
            game->levels[game->currentLevel]->is_secret_room = true;
            total_rooms_1++;
        }
        // game->levels[game->currentLevel] = &level1;
        // game->levels[game->currentLevel] = malloc(sizeof(Level));
        // game->levels[game->currentLevel]->rooms = malloc(total_rooms_1 * sizeof(Room*));
        game->levels[game->currentLevel]->roomsCount = total_rooms_1;
        // for(int i = 0; i < total_rooms_1; i++){
        //     game->levels[game->currentLevel]->rooms[i] = malloc(sizeof(Room));
        // }
        // for(int i = 0; i < total_rooms; i++){
        //     game->levels[game->currentLevel]->rooms[i] = &rooms1[i];
        // }
        Room room;
        room.width = ROOM_MIN_SIZE + 3 + rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE + 1);
        room.height = ROOM_MIN_SIZE + 2 + rand() % (5);
        room.x = start_x + end_x - 30 - rand() % (25 - room.width - start_x);
        room.y = start_y + end_y - 15 - rand() % (25 - room.height - start_y);
        room.isVisible = false;
        is_stair = 1;
        create_room(&room);
        *game->levels[game->currentLevel]->rooms[total_rooms - 1] = room;
        game->levels[game->currentLevel]->rooms[total_rooms - 1]->index = total_rooms - 1;
        
        while (num_rooms < total_rooms - 1) {
            Room room;
            room.width = ROOM_MIN_SIZE + 3 + rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE + 1);
            room.height = ROOM_MIN_SIZE + 2 + rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE + 1);

            room.x = start_x + rand() % (end_x - room.width - start_x - 10);
            room.y = start_y + rand() % (end_y - room.height - start_y);

            if(num_rooms == 0){
                room.x = start_x + rand() % (25 - room.width - start_x);
                room.y = start_y + rand() % (25 - room.height - start_y);
                room.isVisible = false;
                if(current_level == 0){
                    is_stair = 0;
                }
                create_room(&room);
                is_stair = 0;
                room.index = 0;
                *game->levels[game->currentLevel]->rooms[num_rooms++] = room;
            }
            counter++;
            if(counter >= 10000){
                Room room;
                room.width = ROOM_MIN_SIZE + 2 + rand() % (3);
                room.height = ROOM_MIN_SIZE + 2 + rand() % (3);

                room.x = start_x + 5 + rand() % (end_x - room.width - start_x);
                room.y = start_y + 5 + rand() % (end_y - room.height - start_y - 5);
                inter = 4;
                if (!intersects(*game->levels[game->currentLevel]->rooms, num_rooms, room, total_rooms)) {
                    counter = 0;
                    inter = 0;
                    create_room(&room);
                    room.index = num_rooms;
                    room.isVisible = false;
                    *game->levels[game->currentLevel]->rooms[num_rooms++] = room;
                }
            }
            else if (!intersects(*game->levels[game->currentLevel]->rooms, num_rooms, room, total_rooms)) {
                counter = 0;
                inter = 0;
                create_room(&room);
                room.index = num_rooms;
                room.isVisible = false;
                *game->levels[game->currentLevel]->rooms[num_rooms++] = room;
            }
        }
        num_rooms++;

        int k = 0;
        while(k < total_rooms * total_rooms * 2 || check_rooms_validate(room_checker, t, num_rooms)){
            connect_random_rooms(*game->levels[game->currentLevel]->rooms, num_rooms, room_checker, &t);
            k++;
        }
        draw_stair_1(game->levels[game->currentLevel]->rooms[num_rooms - 1]);
        if(game->currentLevel > 0){
            draw_stair_2(game->levels[game->currentLevel]->rooms[0]);
        }
        draw_ancient_key(game->levels[game->currentLevel]->rooms[rand() % total_rooms]);
        for (int i = 0; i < num_rooms; i++) {
            is_nightmare = 0;
            is_reinforce = 0;
            is_healing = 0;
            is_spell = 0;
            if(game->levels[game->currentLevel]->rooms[i]->type == 'p'){
                is_spell = 1;
            }
            else if(game->levels[game->currentLevel]->rooms[i]->type == 'r'){
                is_reinforce = 1;
            }
            else if(game->levels[game->currentLevel]->rooms[i]->type == 's'){
                is_healing = 1;
            }
            else if(game->levels[game->currentLevel]->rooms[i]->type == 'm'){
                is_nightmare = 1;
            }
            create_pillars(game->levels[game->currentLevel]->rooms[i]);
            if(is_reinforce == 0){
                create_food(game->levels[game->currentLevel]->rooms[i]);
                create_special_food(game->levels[game->currentLevel]->rooms[i]);
            }
            if(is_healing == 0){
                create_shield(game->levels[game->currentLevel]->rooms[i]);
            }
            create_gold(game->levels[game->currentLevel]->rooms[i]);
            if(is_reinforce == 0){
                create_spell(game->levels[game->currentLevel]->rooms[i]);
                if(is_healing == 0){
                    draw_traps(game->levels[game->currentLevel]->rooms[i]);
                }
            }
            if(game->levels[game->currentLevel]->rooms[i]->type != 'p' && is_healing == 0){
                create_weapon(game->levels[game->currentLevel]->rooms[i]);
            }
            if(is_spell == 0 && is_reinforce == 0 && is_healing == 0 && i != 0 && i != total_rooms - 1){
                draw_enemies(game->levels[game->currentLevel]->rooms[i]);
            }
            else{
                game->levels[game->currentLevel]->rooms[i]->enemyCount = 0;
            }
            draw_window(game->levels[game->currentLevel]->rooms[i]);
        }
        draw_player(game->levels[game->currentLevel]->rooms[0]);
        for (int i = 0; i < num_rooms; i++) {
            correct_room(game->levels[game->currentLevel]->rooms[i]);
        }
        for (int i = 0; i < num_rooms; i++) {
            if(rand() % 3 == 0){
                create_lock_key(game->levels[game->currentLevel]->rooms[i]);
            }
        }
        if(is_secret_room){
            total_rooms++;
            Room room;
            room.index = total_rooms - 1;
            room.width = 8 + rand() % 6;
            room.height = 8 + rand() % 6;
            room.x = game->levels[game->currentLevel]->rooms[total_rooms - 2]->x + game->levels[game->currentLevel]->rooms[total_rooms - 2]->width + 24;
            room.y = game->levels[game->currentLevel]->rooms[total_rooms - 2]->y + 5 - rand() % 6;
            is_secret = 1;
            create_room(&room);
            create_secret_door(&room, *game->levels[game->currentLevel]->rooms);
            create_food(&room);;
            create_shield(&room);;
            create_gold(&room);
            create_spell(&room);
            create_weapon(&room);
            create_special_food(&room);
            room.type = 's';
            game->levels[game->currentLevel]->rooms[total_rooms - 1] = &room;
            game->levels[game->currentLevel]->roomsCount = total_rooms;
        }
        // for(int i = 0; i < total_rooms; i++){
        //     game->levels[game->currentLevel]->rooms[i] = &rooms[i];
        // }
    }
    else if(game->currentLevel == 6){
        is_battle1 = 1;
        game->levels[game->currentLevel]->rooms[0] = &rooms1[0];
        game->levels[game->currentLevel]->roomsCount = 1;
        game->levels[game->currentLevel]->rooms[0]->width = 12 + rand() % 10;
        game->levels[game->currentLevel]->rooms[0]->height = 10 + rand() % 5;
        game->levels[game->currentLevel]->rooms[0]->x = start_x + 10 + rand() % 40;
        game->levels[game->currentLevel]->rooms[0]->y = start_y + 2 + rand() % 4;
        game->levels[game->currentLevel]->rooms[0]->isVisible = true;
        create_room(game->levels[game->currentLevel]->rooms[0]);
        game->levels[game->currentLevel]->rooms[0]->type = 'b';
        create_pillars(game->levels[game->currentLevel]->rooms[0]);
        draw_enemies(game->levels[game->currentLevel]->rooms[0]);
        draw_window(game->levels[game->currentLevel]->rooms[0]);
        mvwaddch(stdscr, game->levels[game->currentLevel]->rooms[0]->window.y, game->levels[game->currentLevel]->rooms[0]->window.x, 'L');
        game->levels[game->currentLevel]->rooms[0]->doors = malloc(sizeof(Door));
        game->levels[game->currentLevel]->rooms[0]->doorCount = 1;
        game->levels[game->currentLevel]->rooms[0]->doors[0].cord.x = game->levels[game->currentLevel]->rooms[0]->window.x;
        game->levels[game->currentLevel]->rooms[0]->doors[0].cord.y = game->levels[game->currentLevel]->rooms[0]->window.y;
        game->levels[game->currentLevel]->rooms[0]->window.x = -1;
        game->levels[game->currentLevel]->rooms[0]->window.y = -1;
        game->levels[game->currentLevel]->rooms[0]->stair.cord.x = -1;
        game->levels[game->currentLevel]->rooms[0]->stair.cord.y = -1;
        game->levels[game->currentLevel]->rooms[0]->password_generator.x = -1;
        game->levels[game->currentLevel]->rooms[0]->password_generator.y = -1;
        game->levels[game->currentLevel]->rooms[0]->doors[0].is_open = false;
        game->levels[game->currentLevel]->rooms[0]->doors[0].type = 'l';
        game->levels[game->currentLevel]->rooms[0]->doors[0].password = 1029;
        draw_player(game->levels[game->currentLevel]->rooms[0]);
    }
    else{
        is_treasure = 1;
        // game->levels[game->currentLevel] = malloc(sizeof(Level));
        // game->levels[game->currentLevel]->rooms = malloc(1 * sizeof(Room*));
        game->levels[game->currentLevel]->rooms[0] = &rooms1[0];
        game->levels[game->currentLevel]->roomsCount = 1;
        // game->levels[game->currentLevel]->rooms[0] = malloc(sizeof(Room));
        game->levels[game->currentLevel]->rooms[0]->width = 80 + rand() % (11);
        game->levels[game->currentLevel]->rooms[0]->height = 24;
        game->levels[game->currentLevel]->rooms[0]->x = start_x + 3;
        game->levels[game->currentLevel]->rooms[0]->y = start_y + 1;
        game->levels[game->currentLevel]->rooms[0]->stair.cord.x = -1;
        game->levels[game->currentLevel]->rooms[0]->stair.cord.y = -1;
        game->levels[game->currentLevel]->rooms[0]->isVisible = true;
        create_room(game->levels[game->currentLevel]->rooms[0]);
        game->levels[game->currentLevel]->rooms[0]->type = 't';
        draw_ancient_key(game->levels[game->currentLevel]->rooms[0]);
        create_pillars(game->levels[game->currentLevel]->rooms[0]);
        create_shield(game->levels[game->currentLevel]->rooms[0]);
        create_food(game->levels[game->currentLevel]->rooms[0]);
        create_gold(game->levels[game->currentLevel]->rooms[0]);
        create_spell(game->levels[game->currentLevel]->rooms[0]);
        draw_traps(game->levels[game->currentLevel]->rooms[0]);
        create_weapon(game->levels[game->currentLevel]->rooms[0]);
        create_special_food(game->levels[game->currentLevel]->rooms[0]);
        draw_enemies(game->levels[game->currentLevel]->rooms[0]);
        draw_player(game->levels[game->currentLevel]->rooms[0]);
    }
    k = 0;
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 300; j++){
            game->whole_map[game->currentLevel][i][j].item = ' ';
            game->whole_map[game->currentLevel][i][j].isVisible = false;
        }
    }
    for(int i = start_y; i < map_height; i++){
        for(int j = start_x; j < map_width; j++){
            if(i == game->player->cord.y && j == game->player->cord.x){
                game->whole_map[game->currentLevel][i][j].item = '.';
                game->whole_map[game->currentLevel][i][j].isVisible = false;
            }
            else{
                game->whole_map[game->currentLevel][i][j].item = mvwinch(stdscr, i, j);
                game->whole_map[game->currentLevel][i][j].isVisible = false;
            }
            k++;
        }
    }
    box(stdscr, 0, 0);
    // for(int i = start_y; i < end_y; i++){
    //     for(int j = start_x; j < end_x; j++){
    //         if(mvwinch(stdscr, i, j) == '#')
    //     }
    // }
    // mvprintw(0, 0, "Generated Map:");
    refresh();
    // getch();
    clear();
    erase();
    refresh();
}