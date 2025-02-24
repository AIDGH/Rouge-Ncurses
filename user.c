#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include "user.h"
#include "map_generator.h"
#include "music.h"
#include "game.h"
#include "before_game_menu.h"
#include "guest_before_game_menu.h"

extern char our_user[50];
extern Game *game;

int load_user_info(const char *username, User *user) {
    char filepath[100] = "users/";
    strcat(filepath, username);
    strcat(filepath, ".txt");

    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        return 0;
    }
    strcpy(user->username, our_user);
    fscanf(file, "health: %d\n", &user->health);
    fscanf(file, "points: %lld\n", &user->points);
    fscanf(file, "games count: %d\n", &user->games_count);
    fscanf(file, "play time: %d\n", &user->play_time);
    fscanf(file, "golds: %d\n", &user->golds);
    fscanf(file, "food: %d\n", &user->food);

    int spellscount, weaponscount;
    char weapons_line[500];
    fscanf(file, "weapons: %d %[^\n]\n", &weaponscount, weapons_line);
    char *weapon = strtok(weapons_line, ", ");
    int i = 0;
    while (weapon != NULL && i < 5) {
        strcpy(user->weapons[i], weapon);
        weapon = strtok(NULL, ", ");
        i++;
    }
    for(int i = 0; i < 5; i++){
        int k = strlen(user->weapons[i]) - 1;
        int num = 0;
        int a = 0;
        while(1){
            if(user->weapons[i][k] < '0' || user->weapons[i][k] > '9'){
                break;
            }
            k--;
            a++;
        }
        char num1[4];
        strcpy(num1, user->weapons[i] + k + 1);
        for(int j = 0; j < a; j++){
            num = num * 10 + num1[j] - '0';
        }
        user->weaponsCount_each[i] = num;
    }

    char spells_line[500];
    fscanf(file, "spells: %d %[^\n]\n", &spellscount, spells_line);
    char *spell = strtok(spells_line, ", ");
    int j = 0;
    while (spell != NULL && j < 3) {
        strcpy(user->spells[j], spell);
        spell = strtok(NULL, ", ");
        // sscanf(spell, "%s", user->spells[j]);
        j++;
    }
    fscanf(file, "ancientkey: %d\n", &user->ancientkey);
    fscanf(file, "broken ancientkey: %d\n", &user->brokenancientkey);
    for(int i = 0; i < 3; i++){
        int k = strlen(user->spells[i]) - 1;
        int num = 0;
        int a = 0;
        while(1){
            if(user->spells[i][k] < '0' || user->spells[i][k] > '9'){
                break;
            }
            k--;
            a++;
        }
        char num1[4];
        strcpy(num1, user->spells[i] + k + 1);
        for(int j = 0; j < a; j++){
            num = num * 10 + num1[j] - '0';
        }
        user->spellsCount_each[i] = num;
    }
    user->spellsCount = spellscount;
    user->weaponsCount = weaponscount;
    fscanf(file, "level: %d\n", &user->level);
    fscanf(file, "game: %d\n", &user->have_game);

    fclose(file);
    return 1;
}

int read_all_users(User *users, int max_users) {
    DIR *dir;
    struct dirent *ent;
    int count = 0;

    if ((dir = opendir("users")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strstr(ent->d_name, ".txt") != NULL && count < max_users) {
                char username[50];
                strncpy(username, ent->d_name, strlen(ent->d_name) - 4);
                username[strlen(ent->d_name) - 4] = '\0';
                if (load_user_info(username, &users[count])) {
                    strcpy(users[count].username, username);
                    count++;
                }
            }
        }
        closedir(dir);
    } else {
        perror("Could not open users directory.");
        return 0;
    }
    return count;
}

int save_just_user_info(Player *user) {
    char filepath[100] = "users/";
    strcat(filepath, user->name);
    strcat(filepath, ".txt");

    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        return 0;
    }
    fprintf(file, "health: %d\n", user->health);
    fprintf(file, "points: %lld\n", user->points);
    fprintf(file, "games count: %d\n", user->games_count);
    fprintf(file, "play time: %d\n", user->play_time);
    fprintf(file, "golds: %d\n", user->gold);
    fprintf(file, "food: %d\n", user->foodCount);

    fprintf(file, "weapons: %d %s%d, %s%d, %s%d, %s%d, %s%d\n", user->weaponCount,
    user->weapons[0].name, user->weapons[0].count, user->weapons[1].name, user->weapons[1].count, 
    user->weapons[2].name, user->weapons[2].count, user->weapons[3].name, user->weapons[3].count, 
    user->weapons[4].name, user->weapons[4].count);

    fprintf(file, "spells: %d %s%d, %s%d, %s%d\n", user->spellCount, user->spells[0].name, user->spells[0].count,
    user->spells[1].name, user->spells[1].count, user->spells[2].name, user->spells[2].count);
    fprintf(file, "ancientkey: %d\n", user->acientKey);
    fprintf(file, "broken ancientkey: %d\n", user->brokenAcientKey);
    fprintf(file, "level: %d\n", user->level);
    fprintf(file, "game: 0\n");

    fclose(file);
    return 1;
}

int save_user_game(Game *game) {
    char filepath[100] = "users/";
    strcat(filepath, game->player->name);
    strcat(filepath, ".txt");

    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        return 0;
    }
    fprintf(file, "health: %d\n", game->player->health);
    fprintf(file, "points: %lld\n", game->player->points);
    fprintf(file, "games count: %d\n", game->player->games_count);
    fprintf(file, "play time: %d\n", game->player->play_time);
    fprintf(file, "golds: %d\n", game->player->gold);
    fprintf(file, "food: %d\n", game->player->foodCount);

    fprintf(file, "weapons: %d %s%d, %s%d, %s%d, %s%d, %s%d\n", game->player->weaponCount,
    game->player->weapons[0].name, game->player->weapons[0].count, game->player->weapons[1].name, game->player->weapons[1].count, 
    game->player->weapons[2].name, game->player->weapons[2].count, game->player->weapons[3].name, game->player->weapons[3].count, 
    game->player->weapons[4].name, game->player->weapons[4].count);

    fprintf(file, "spells: %d %s%d, %s%d, %s%d\n", game->player->spellCount, game->player->spells[0].name, game->player->spells[0].count,
    game->player->spells[1].name, game->player->spells[1].count, game->player->spells[2].name, game->player->spells[2].count);
    fprintf(file, "ancientkey: %d\n", game->player->acientKey);
    fprintf(file, "broken ancientkey: %d\n", game->player->brokenAcientKey);
    fprintf(file, "level: %d\n", game->player->level);
    fprintf(file, "game: %d\n", game->player->can_be_saved);
    fprintf(file, "%d %d %d %d %d %d %d %d %d %d %d %d\n", game->player->cord.x, game->player->cord.y, game->player->room_num, game->player->level, 
    game->player->Speed_effect, game->player->Power_effect, game->player->Health_effect, game->player->shield, 
    game->player->specialfoods[0].count, game->player->specialfoods[1].count, game->player->specialfoods[2].count, game->player->specialfoods[3].count);
    fprintf(file, "%d %d %d %d %d %d\n", game->currentLevel, game->unlockedLevel, game->newLevel, game->levelCount, game->map_height, game->map_width);
    for(int i = 0; i < game->unlockedLevel + 1; i++){
        for(int j = 0; j < game->map_height + 1; j++){
            for(int k = 0; k < game->map_width + 1; k++){
                int a = 0;
                if(game->whole_map[i][j][k].isVisible == true){
                    a = 1;
                }
                fprintf(file, "%d %d ", game->whole_map[i][j][k].item, a);
            }
            fprintf(file, "\n");
        }
    }
    for(int i = 0; i < game->unlockedLevel + 1; i++){
        int a = 0;
        if(game->levels[i]->is_secret_room == true){
            a = 1;
        }
        fprintf(file, "%d %d %d %d %d %d\n", game->levels[i]->roomsCount, a, game->levels[i]->level, game->levels[i]->secret_room, game->levels[i]->key.x, game->levels[i]->key.y);
        for(int j = 0; j < game->levels[i]->roomsCount; j++){
            int b = 0;
            if(game->levels[i]->rooms[j]->isVisible == true){
                b = 1;
            }
            fprintf(file, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %c %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", 
            game->levels[i]->rooms[j]->x, game->levels[i]->rooms[j]->y, game->levels[i]->rooms[j]->height, game->levels[i]->rooms[j]->width, 
            game->levels[i]->rooms[j]->doorCount, b, game->levels[i]->rooms[j]->index, game->levels[i]->rooms[j]->pillarCount, game->levels[i]->rooms[j]->keyCount, 
            game->levels[i]->rooms[j]->lock_door, game->levels[i]->rooms[j]->foodCount, game->levels[i]->rooms[j]->specialfoodCount, game->levels[i]->rooms[j]->goldCount, 
            game->levels[i]->rooms[j]->weaponCount, game->levels[i]->rooms[j]->spellCount, game->levels[i]->rooms[j]->trapCount, game->levels[i]->rooms[j]->enemyCount, 
            game->levels[i]->rooms[j]->enemyLeft, game->levels[i]->rooms[j]->type, game->levels[i]->rooms[j]->stairCount, game->levels[i]->rooms[j]->lock_attemps, 
            game->levels[i]->rooms[j]->window.x, game->levels[i]->rooms[j]->window.y, game->levels[i]->rooms[j]->stair.cord.x, game->levels[i]->rooms[j]->stair.cord.y, 
            game->levels[i]->rooms[j]->stair.from, game->levels[i]->rooms[j]->stair.to, 
            game->levels[i]->rooms[j]->ancientkey.x, game->levels[i]->rooms[j]->ancientkey.y, 
            game->levels[i]->rooms[j]->password_generator.x, game->levels[i]->rooms[j]->password_generator.y, 
            game->levels[i]->rooms[j]->password_generator2.x, game->levels[i]->rooms[j]->password_generator2.y);
            for(int k = 0; k < game->levels[i]->rooms[j]->doorCount; k++){
                int c = 0;
                if(game->levels[i]->rooms[j]->doors[k].is_open == true){
                    c = 1;
                }
                fprintf(file, "%c %d %d %d %d %d\n", game->levels[i]->rooms[j]->doors[k].type, game->levels[i]->rooms[j]->doors[k].cord.x, game->levels[i]->rooms[j]->doors[k].cord.y, 
                game->levels[i]->rooms[j]->doors[k].password, game->levels[i]->rooms[j]->doors[k].password2, c);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->pillarCount; k++){
                fprintf(file, "%d %d\n", game->levels[i]->rooms[j]->pillars[k].x, game->levels[i]->rooms[j]->pillars[k].y);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->foodCount; k++){
                int c = 0;
                if(game->levels[i]->rooms[j]->foods[i].isUsed == true){
                    c = 1;
                }
                fprintf(file, "%c %d %d %d %d\n", game->levels[i]->rooms[j]->foods[k].type, game->levels[i]->rooms[j]->foods[k].cord.x, game->levels[i]->rooms[j]->foods[k].cord.y, 
                game->levels[i]->rooms[j]->foods[k].health, c);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->shieldCount; k++){
                int c = 0;
                if(game->levels[i]->rooms[j]->shields[i].isUsed == true){
                    c = 1;
                }
                fprintf(file, "%d %d %d %d\n", game->levels[i]->rooms[j]->shields[k].cord.x, game->levels[i]->rooms[j]->shields[k].cord.y, game->levels[i]->rooms[j]->shields[k].amount, c);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->specialfoodCount; k++){
                int c = 0;
                if(game->levels[i]->rooms[j]->specialfoods[i].isUsed == true){
                    c = 1;
                }
                fprintf(file, "%c %d %d %d\n", game->levels[i]->rooms[j]->specialfoods[k].type, game->levels[i]->rooms[j]->specialfoods[k].cord.x, game->levels[i]->rooms[j]->specialfoods[k].cord.y, c);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->goldCount; k++){
                int c = 0;
                if(game->levels[i]->rooms[j]->golds[i].isUsed == true){
                    c = 1;
                }
                fprintf(file, "%c %d %d %d %d\n", game->levels[i]->rooms[j]->golds[k].type, game->levels[i]->rooms[j]->golds[k].cord.x, game->levels[i]->rooms[j]->golds[k].cord.y, game->levels[i]->rooms[j]->golds[k].count, c);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->weaponCount; k++){
                int c = 0;
                if(game->levels[i]->rooms[j]->weapons[i].isUsed == true){
                    c = 1;
                }
                fprintf(file, "%c %d %d %d %d %s %d\n", game->levels[i]->rooms[j]->weapons[k].type, game->levels[i]->rooms[j]->weapons[k].cord.x, game->levels[i]->rooms[j]->weapons[k].cord.y, 
                game->levels[i]->rooms[j]->weapons[k].damage, game->levels[i]->rooms[j]->weapons[k].range, game->levels[i]->rooms[j]->weapons[k].name, c);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->spellCount; k++){
                int c = 0;
                if(game->levels[i]->rooms[j]->spells[i].isUsed == true){
                    c = 1;
                }
                fprintf(file, "%c %d %d %s %d\n", game->levels[i]->rooms[j]->spells[k].type, game->levels[i]->rooms[j]->spells[k].cord.x, game->levels[i]->rooms[j]->spells[k].cord.y, 
                game->levels[i]->rooms[j]->spells[k].name, c);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->trapCount; k++){
                int c = 0;
                if(game->levels[i]->rooms[j]->traps[i].isVisible == true){
                    c = 1;
                }
                fprintf(file, "%d %d %d %d\n", game->levels[i]->rooms[j]->traps[k].cord.x, game->levels[i]->rooms[j]->traps[k].cord.y, game->levels[i]->rooms[j]->traps[k].damage, c);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->enemyCount; k++){
                int c1 = 0, c2 = 0, c3 = 0;
                if(game->levels[i]->rooms[j]->enemies[i].isAlive == true){
                    c1 = 1;
                }
                if(game->levels[i]->rooms[j]->enemies[i].canMove == true){
                    c2 = 1;
                }
                if(game->levels[i]->rooms[j]->enemies[i].isVisible == true){
                    c3 = 1;
                }
                fprintf(file, "%c %d %d %d %d %d %s %d %d %d\n", game->levels[i]->rooms[j]->enemies[k].type, game->levels[i]->rooms[j]->enemies[k].cord.x, game->levels[i]->rooms[j]->enemies[k].cord.y, 
                game->levels[i]->rooms[j]->enemies[k].damage, game->levels[i]->rooms[j]->enemies[k].moves, game->levels[i]->rooms[j]->enemies[k].health, game->levels[i]->rooms[j]->enemies[k].name, c1, c2, c3);
            }
        }
    }
    fclose(file);
    return 1;
}

int load_user_game() {
    char filepath[100] = "users/";
    strcat(filepath, game->player->name);
    strcat(filepath, ".txt");

    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        return 0;
    }
    fscanf(file, "health: %d\n", &game->player->health);
    fscanf(file, "points: %lld\n", &game->player->points);
    fscanf(file, "games count: %d\n", &game->player->games_count);
    fscanf(file, "play time: %d\n", &game->player->play_time);
    fscanf(file, "golds: %d\n", &game->player->gold);
    fscanf(file, "food: %d\n", &game->player->foodCount);
    int spellscount, weaponscount;
    char weapons_line[500];
    fscanf(file, "weapons: %d %[^\n]\n", &weaponscount, weapons_line);
    char spells_line[500];
    fscanf(file, "spells: %d %[^\n]\n", &spellscount, spells_line);
    fscanf(file, "ancientkey: %d\n", &game->player->acientKey);
    fscanf(file, "broken ancientkey: %d\n", &game->player->brokenAcientKey);
    fscanf(file, "level: %d\n", &game->player->level);
    fscanf(file, "game: %d\n", &game->player->can_be_saved);

    fscanf(file, "%d %d %d %d %d %d %d %d %d %d %d %d\n", &game->player->cord.x, &game->player->cord.y, &game->player->room_num, &game->player->level, 
    &game->player->Speed_effect, &game->player->Power_effect, &game->player->Health_effect, &game->player->shield, 
    &game->player->specialfoods[0].count, &game->player->specialfoods[1].count, &game->player->specialfoods[2].count, &game->player->specialfoods[3].count);
    fscanf(file, "%d %d %d %d %d %d\n", &game->currentLevel, &game->unlockedLevel, &game->newLevel, &game->levelCount, &game->map_height, &game->map_width);
    for(int i = 0; i < game->unlockedLevel + 1; i++){
        for(int j = 0; j < game->map_height + 1; j++){
            for(int k = 0; k < game->map_width + 1; k++){
                int a;
                fscanf(file, "%d %d ", &game->whole_map[i][j][k].item, &a);
                if(a == 1){
                    game->whole_map[i][j][k].isVisible = true;
                }
                else{
                    game->whole_map[i][j][k].isVisible = false;
                }
            }
            fscanf(file, "\n");
        }
    }
    for(int i = 0; i < game->unlockedLevel + 1; i++){
        int a;
        fscanf(file, "%d %d %d %d %d %d\n", &game->levels[i]->roomsCount, &a, &game->levels[i]->level, &game->levels[i]->secret_room, &game->levels[i]->key.x, &game->levels[i]->key.y);
        if(a == 1){
            game->levels[i]->is_secret_room = true;
        }
        else{
            game->levels[i]->is_secret_room = false;
        }
        for(int j = 0; j < game->levels[i]->roomsCount; j++){
            int b;
            fscanf(file, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %c %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", 
            &game->levels[i]->rooms[j]->x, &game->levels[i]->rooms[j]->y, &game->levels[i]->rooms[j]->height, &game->levels[i]->rooms[j]->width, 
            &game->levels[i]->rooms[j]->doorCount, &b, &game->levels[i]->rooms[j]->index, &game->levels[i]->rooms[j]->pillarCount, &game->levels[i]->rooms[j]->keyCount, 
            &game->levels[i]->rooms[j]->lock_door, &game->levels[i]->rooms[j]->foodCount, &game->levels[i]->rooms[j]->specialfoodCount, &game->levels[i]->rooms[j]->goldCount, 
            &game->levels[i]->rooms[j]->weaponCount, &game->levels[i]->rooms[j]->spellCount, &game->levels[i]->rooms[j]->trapCount, &game->levels[i]->rooms[j]->enemyCount, 
            &game->levels[i]->rooms[j]->enemyLeft, &game->levels[i]->rooms[j]->type, &game->levels[i]->rooms[j]->stairCount, &game->levels[i]->rooms[j]->lock_attemps, 
            &game->levels[i]->rooms[j]->window.x, &game->levels[i]->rooms[j]->window.y, &game->levels[i]->rooms[j]->stair.cord.x, &game->levels[i]->rooms[j]->stair.cord.y, 
            &game->levels[i]->rooms[j]->stair.from, &game->levels[i]->rooms[j]->stair.to, 
            &game->levels[i]->rooms[j]->ancientkey.x, &game->levels[i]->rooms[j]->ancientkey.y, 
            &game->levels[i]->rooms[j]->password_generator.x, &game->levels[i]->rooms[j]->password_generator.y, 
            &game->levels[i]->rooms[j]->password_generator2.x, &game->levels[i]->rooms[j]->password_generator2.y);
            if(b == 1){
                game->levels[i]->rooms[j]->isVisible = true;
            }
            else{
                game->levels[i]->rooms[j]->isVisible = false;
            }
            game->levels[i]->rooms[j]->doors = malloc(game->levels[i]->rooms[j]->doorCount * sizeof(Door));
            game->levels[i]->rooms[j]->pillars = malloc(game->levels[i]->rooms[j]->pillarCount * sizeof(Point));
            game->levels[i]->rooms[j]->foods = malloc(game->levels[i]->rooms[j]->foodCount * sizeof(Food));
            game->levels[i]->rooms[j]->shields = malloc(game->levels[i]->rooms[j]->shieldCount * sizeof(Shield));
            game->levels[i]->rooms[j]->specialfoods = malloc(game->levels[i]->rooms[j]->specialfoodCount * sizeof(Food));
            game->levels[i]->rooms[j]->golds = malloc(game->levels[i]->rooms[j]->goldCount * sizeof(Gold));
            game->levels[i]->rooms[j]->weapons = malloc(game->levels[i]->rooms[j]->weaponCount * sizeof(Weapon));
            game->levels[i]->rooms[j]->spells = malloc(game->levels[i]->rooms[j]->spellCount * sizeof(Spell));
            game->levels[i]->rooms[j]->traps = malloc(game->levels[i]->rooms[j]->trapCount * sizeof(Trap));
            game->levels[i]->rooms[j]->enemies = malloc(game->levels[i]->rooms[j]->enemyCount * sizeof(Enemy));
            for(int k = 0; k < game->levels[i]->rooms[j]->doorCount; k++){
                int c;
                fscanf(file, "%c %d %d %d %d %d\n", &game->levels[i]->rooms[j]->doors[k].type, &game->levels[i]->rooms[j]->doors[k].cord.x, &game->levels[i]->rooms[j]->doors[k].cord.y, 
                &game->levels[i]->rooms[j]->doors[k].password, &game->levels[i]->rooms[j]->doors[k].password2, &c);
                if(c == 1){
                    game->levels[i]->rooms[j]->doors[k].is_open = true;
                }
                else{
                    game->levels[i]->rooms[j]->doors[k].is_open = false;
                }
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->pillarCount; k++){
                fscanf(file, "%d %d\n", &game->levels[i]->rooms[j]->pillars[k].x, &game->levels[i]->rooms[j]->pillars[k].y);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->foodCount; k++){
                int c;
                fscanf(file, "%c %d %d %d %d\n", &game->levels[i]->rooms[j]->foods[k].type, &game->levels[i]->rooms[j]->foods[k].cord.x, &game->levels[i]->rooms[j]->foods[k].cord.y, 
                &game->levels[i]->rooms[j]->foods[k].health, &c);
                if(c == 1){
                    game->levels[i]->rooms[j]->foods[k].isUsed = true;
                }
                else{
                    game->levels[i]->rooms[j]->foods[k].isUsed = false;
                }
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->shieldCount; k++){
                int c;
                fscanf(file, "%d %d %d %d\n", &game->levels[i]->rooms[j]->shields[k].cord.x, &game->levels[i]->rooms[j]->shields[k].cord.y, &game->levels[i]->rooms[j]->shields[k].amount, &c);
                if(c == 1){
                    game->levels[i]->rooms[j]->shields[i].isUsed = true;
                }
                else{
                    game->levels[i]->rooms[j]->shields[i].isUsed = false;
                }
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->specialfoodCount; k++){
                int c;
                fscanf(file, "%c %d %d %d\n", &game->levels[i]->rooms[j]->specialfoods[k].type, &game->levels[i]->rooms[j]->specialfoods[k].cord.x, &game->levels[i]->rooms[j]->specialfoods[k].cord.y, &c);
                if(c == 1){
                    game->levels[i]->rooms[j]->specialfoods[i].isUsed = true;
                }
                else{
                    game->levels[i]->rooms[j]->specialfoods[i].isUsed = false;
                }
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->goldCount; k++){
                int c;
                fscanf(file, "%c %d %d %d %d\n", &game->levels[i]->rooms[j]->golds[k].type, &game->levels[i]->rooms[j]->golds[k].cord.x, &game->levels[i]->rooms[j]->golds[k].cord.y, &game->levels[i]->rooms[j]->golds[k].count, &c);
                if(c == 1){
                    game->levels[i]->rooms[j]->golds[i].isUsed = true;
                }
                else{
                    game->levels[i]->rooms[j]->golds[i].isUsed = false;
                }
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->weaponCount; k++){
                int c;
                fscanf(file, "%c %d %d %d %d %s %d\n", &game->levels[i]->rooms[j]->weapons[k].type, &game->levels[i]->rooms[j]->weapons[k].cord.x, &game->levels[i]->rooms[j]->weapons[k].cord.y, 
                &game->levels[i]->rooms[j]->weapons[k].damage, &game->levels[i]->rooms[j]->weapons[k].range, game->levels[i]->rooms[j]->weapons[k].name, &c);
                if(c == 1){
                    game->levels[i]->rooms[j]->weapons[i].isUsed = true;
                }
                else{
                    game->levels[i]->rooms[j]->weapons[i].isUsed = false;
                }
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->spellCount; k++){
                int c;
                fscanf(file, "%c %d %d %s %d\n", &game->levels[i]->rooms[j]->spells[k].type, &game->levels[i]->rooms[j]->spells[k].cord.x, &game->levels[i]->rooms[j]->spells[k].cord.y, 
                game->levels[i]->rooms[j]->spells[k].name, &c);
                if(c == 1){
                    game->levels[i]->rooms[j]->spells[i].isUsed = true;
                }
                else{
                    game->levels[i]->rooms[j]->spells[i].isUsed = false;
                }
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->trapCount; k++){
                int c;
                fscanf(file, "%d %d %d %d\n", &game->levels[i]->rooms[j]->traps[k].cord.x, &game->levels[i]->rooms[j]->traps[k].cord.y, &game->levels[i]->rooms[j]->traps[k].damage, &c);
                if(c == 1){
                    game->levels[i]->rooms[j]->traps[i].isVisible = true;
                }
                else{
                    game->levels[i]->rooms[j]->traps[i].isVisible = false;
                }
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->enemyCount; k++){
                int c1, c2, c3;
                fscanf(file, "%c %d %d %d %d %d %s %d %d %d\n", &game->levels[i]->rooms[j]->enemies[k].type, &game->levels[i]->rooms[j]->enemies[k].cord.x, &game->levels[i]->rooms[j]->enemies[k].cord.y, 
                &game->levels[i]->rooms[j]->enemies[k].damage, &game->levels[i]->rooms[j]->enemies[k].moves, &game->levels[i]->rooms[j]->enemies[k].health, game->levels[i]->rooms[j]->enemies[k].name, &c1, &c2, &c3);
                if(c1 == 1){
                    game->levels[i]->rooms[j]->enemies[i].isAlive = true;
                }
                else{
                    game->levels[i]->rooms[j]->enemies[i].isAlive = false;
                }
                if(c2 == 1){
                    game->levels[i]->rooms[j]->enemies[i].canMove = true;
                }
                else{
                    game->levels[i]->rooms[j]->enemies[i].canMove = false;
                }
                if(c3 == 1){
                    game->levels[i]->rooms[j]->enemies[i].isVisible = true;
                }
                else{
                    game->levels[i]->rooms[j]->enemies[i].isVisible = false;
                }
            }
            
        }
    }
    fclose(file);
    return 1;
}