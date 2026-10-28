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
extern int is_last_room;

static int inventory_counts(char *line, char names[][MAX_NAME_LENGTH], int *counts, int expected) {
    int i=0; char *token=strtok(line, ", ");
    while(token && i<expected) {
        snprintf(names[i],MAX_NAME_LENGTH,"%s",token);
        size_t len=strlen(token), start=len;
        while(start && token[start-1]>='0' && token[start-1]<='9') start--;
        if(start==len || len>=MAX_NAME_LENGTH) return 0;
        long n=strtol(token+start,NULL,10);
        if(n<0 || n>1000000) return 0;
        counts[i++]=(int)n;token=strtok(NULL,", ");
    }
    return i==expected;
}
int load_user_info(const char *username, User *user) {
    char filepath[256];snprintf(filepath,sizeof(filepath),"users/%s.txt",username);
    FILE *file=fopen(filepath,"r");if(!file) return 0;
    User loaded={0};snprintf(loaded.username,sizeof(loaded.username),"%s",username);
    char weapons[500]={0}, spells[500]={0};
    int ok=fscanf(file,"health: %d\npoints: %lld\ngames count: %d\nplay time: %d\ngolds: %d\nfood: %d\n",
      &loaded.health,&loaded.points,&loaded.games_count,&loaded.play_time,&loaded.golds,&loaded.food)==6;
    ok=ok && fscanf(file,"weapons: %d %499[^\n]\n",&loaded.weaponsCount,weapons)==2;
    ok=ok && fscanf(file,"spells: %d %499[^\n]\n",&loaded.spellsCount,spells)==2;
    ok=ok && fscanf(file,"ancientkey: %d\nbroken ancientkey: %d\nlevel: %d\ngame: %d\n",
      &loaded.ancientkey,&loaded.brokenancientkey,&loaded.level,&loaded.have_game)==4;
    fclose(file);
    if(!ok || !inventory_counts(weapons,loaded.weapons,loaded.weaponsCount_each,5)
       || !inventory_counts(spells,loaded.spells,loaded.spellsCount_each,3)) return 0;
    *user=loaded;return 1;
}

int read_all_users(User *users, int max_users) {
    DIR *dir;
    struct dirent *ent;
    int count = 0;

    if ((dir = opendir("users")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            size_t len = strlen(ent->d_name);
            if (len > 4 && len-4 < 50 && !strcmp(ent->d_name+len-4, ".txt") && count < max_users) {
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

    char temporary[128];snprintf(temporary,sizeof(temporary),"%s.tmp",filepath);
    FILE *file = fopen(temporary, "w");
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

    int failed=ferror(file);
    if(fclose(file)!=0)failed=1;
    if(failed||rename(temporary,filepath)!=0){unlink(temporary);return 0;}
    return 1;
}

int save_user_game(Game *game) {
    char filepath[100] = "users/";
    strcat(filepath, game->player->name);
    strcat(filepath, ".txt");

    char temporary[128];snprintf(temporary,sizeof(temporary),"%s.tmp",filepath);
    FILE *file = fopen(temporary, "w");
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
    fprintf(file, "save-version: 2\n");
    fprintf(file,"final-battle: %d\n",is_last_room);
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
            fprintf(file, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %c %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
            game->levels[i]->rooms[j]->x, game->levels[i]->rooms[j]->y, game->levels[i]->rooms[j]->height, game->levels[i]->rooms[j]->width, 
            game->levels[i]->rooms[j]->doorCount, b, game->levels[i]->rooms[j]->index, game->levels[i]->rooms[j]->pillarCount, game->levels[i]->rooms[j]->keyCount, 
            game->levels[i]->rooms[j]->lock_door, game->levels[i]->rooms[j]->foodCount, game->levels[i]->rooms[j]->specialfoodCount, game->levels[i]->rooms[j]->goldCount, 
            game->levels[i]->rooms[j]->weaponCount, game->levels[i]->rooms[j]->spellCount, game->levels[i]->rooms[j]->trapCount, game->levels[i]->rooms[j]->enemyCount, 
            game->levels[i]->rooms[j]->enemyLeft, game->levels[i]->rooms[j]->type, game->levels[i]->rooms[j]->stairCount, game->levels[i]->rooms[j]->lock_attemps, 
            game->levels[i]->rooms[j]->window.x, game->levels[i]->rooms[j]->window.y, game->levels[i]->rooms[j]->stair.cord.x, game->levels[i]->rooms[j]->stair.cord.y, 
            game->levels[i]->rooms[j]->stair.from, game->levels[i]->rooms[j]->stair.to, 
            game->levels[i]->rooms[j]->ancientkey.x, game->levels[i]->rooms[j]->ancientkey.y, 
            game->levels[i]->rooms[j]->password_generator.x, game->levels[i]->rooms[j]->password_generator.y, 
            game->levels[i]->rooms[j]->password_generator2.x, game->levels[i]->rooms[j]->password_generator2.y, game->levels[i]->rooms[j]->shieldCount);
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
                if(game->levels[i]->rooms[j]->foods[k].isUsed == true){
                    c = 1;
                }
                fprintf(file, "%c %d %d %d %d\n", game->levels[i]->rooms[j]->foods[k].type, game->levels[i]->rooms[j]->foods[k].cord.x, game->levels[i]->rooms[j]->foods[k].cord.y, 
                game->levels[i]->rooms[j]->foods[k].health, c);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->shieldCount; k++){
                int c = 0;
                if(game->levels[i]->rooms[j]->shields[k].isUsed == true){
                    c = 1;
                }
                fprintf(file, "%d %d %d %d\n", game->levels[i]->rooms[j]->shields[k].cord.x, game->levels[i]->rooms[j]->shields[k].cord.y, game->levels[i]->rooms[j]->shields[k].amount, c);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->specialfoodCount; k++){
                int c = 0;
                if(game->levels[i]->rooms[j]->specialfoods[k].isUsed == true){
                    c = 1;
                }
                fprintf(file, "%c %d %d %d\n", game->levels[i]->rooms[j]->specialfoods[k].type, game->levels[i]->rooms[j]->specialfoods[k].cord.x, game->levels[i]->rooms[j]->specialfoods[k].cord.y, c);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->goldCount; k++){
                int c = 0;
                if(game->levels[i]->rooms[j]->golds[k].isUsed == true){
                    c = 1;
                }
                fprintf(file, "%c %d %d %d %d\n", game->levels[i]->rooms[j]->golds[k].type, game->levels[i]->rooms[j]->golds[k].cord.x, game->levels[i]->rooms[j]->golds[k].cord.y, game->levels[i]->rooms[j]->golds[k].count, c);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->weaponCount; k++){
                int c = 0;
                if(game->levels[i]->rooms[j]->weapons[k].isUsed == true){
                    c = 1;
                }
                fprintf(file, "%c %d %d %d %d %s %d %d\n", game->levels[i]->rooms[j]->weapons[k].type, game->levels[i]->rooms[j]->weapons[k].cord.x, game->levels[i]->rooms[j]->weapons[k].cord.y,
                game->levels[i]->rooms[j]->weapons[k].damage, game->levels[i]->rooms[j]->weapons[k].range, game->levels[i]->rooms[j]->weapons[k].name, c, game->levels[i]->rooms[j]->weapons[k].count);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->spellCount; k++){
                int c = 0;
                if(game->levels[i]->rooms[j]->spells[k].isUsed == true){
                    c = 1;
                }
                fprintf(file, "%c %d %d %s %d\n", game->levels[i]->rooms[j]->spells[k].type, game->levels[i]->rooms[j]->spells[k].cord.x, game->levels[i]->rooms[j]->spells[k].cord.y, 
                game->levels[i]->rooms[j]->spells[k].name, c);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->trapCount; k++){
                int c = 0;
                if(game->levels[i]->rooms[j]->traps[k].isVisible == true){
                    c = 1;
                }
                fprintf(file, "%d %d %d %d\n", game->levels[i]->rooms[j]->traps[k].cord.x, game->levels[i]->rooms[j]->traps[k].cord.y, game->levels[i]->rooms[j]->traps[k].damage, c);
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->enemyCount; k++){
                int c1 = 0, c2 = 0, c3 = 0;
                if(game->levels[i]->rooms[j]->enemies[k].isAlive == true){
                    c1 = 1;
                }
                if(game->levels[i]->rooms[j]->enemies[k].canMove == true){
                    c2 = 1;
                }
                if(game->levels[i]->rooms[j]->enemies[k].isVisible == true){
                    c3 = 1;
                }
                fprintf(file, "%c %d %d %d %d %d %s %d %d %d\n", game->levels[i]->rooms[j]->enemies[k].type, game->levels[i]->rooms[j]->enemies[k].cord.x, game->levels[i]->rooms[j]->enemies[k].cord.y, 
                game->levels[i]->rooms[j]->enemies[k].damage, game->levels[i]->rooms[j]->enemies[k].moves, game->levels[i]->rooms[j]->enemies[k].health, game->levels[i]->rooms[j]->enemies[k].name, c1, c2, c3);
            }
        }
    }
    int failed=ferror(file);if(fclose(file)!=0) failed=1;
    if(failed){remove(temporary);return 0;}
    char backup[128];snprintf(backup,sizeof(backup),"%s.bak",filepath);
    FILE *old=fopen(filepath,"rb");
    if(old){FILE *bak=fopen(backup,"wb");if(!bak){fclose(old);remove(temporary);return 0;}
        char buf[4096];size_t n;int bad=0;while((n=fread(buf,1,sizeof(buf),old))>0){if(fwrite(buf,1,n,bak)!=n){bad=1;break;}}
        bad|=ferror(old);fclose(old);if(fclose(bak)!=0)bad=1;
        if(bad){remove(temporary);return 0;}}
    if(rename(temporary,filepath)!=0){remove(temporary);return 0;}
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
    if (fscanf(file, "health: %d\n", &game->player->health) != 1) goto invalid_save;
    if (fscanf(file, "points: %lld\n", &game->player->points) != 1) goto invalid_save;
    if (fscanf(file, "games count: %d\n", &game->player->games_count) != 1) goto invalid_save;
    if (fscanf(file, "play time: %d\n", &game->player->play_time) != 1) goto invalid_save;
    if (fscanf(file, "golds: %d\n", &game->player->gold) != 1) goto invalid_save;
    if (fscanf(file, "food: %d\n", &game->player->foodCount) != 1) goto invalid_save;
    int spellscount, weaponscount;
    char weapons_line[500]={0};
    if (fscanf(file, "weapons: %d %499[^\n]\n", &weaponscount, weapons_line) != 2) goto invalid_save;
    char spells_line[500]={0};
    if (fscanf(file, "spells: %d %499[^\n]\n", &spellscount, spells_line) != 2) goto invalid_save;
    if (fscanf(file, "ancientkey: %d\n", &game->player->acientKey) != 1) goto invalid_save;
    if (fscanf(file, "broken ancientkey: %d\n", &game->player->brokenAcientKey) != 1) goto invalid_save;
    if (fscanf(file, "level: %d\n", &game->player->level) != 1) goto invalid_save;
    if (fscanf(file, "game: %d\n", &game->player->can_be_saved) != 1) goto invalid_save;

    int version=0;
    if(fscanf(file,"save-version: %d\n",&version)!=1 || version!=2) goto invalid_save;
    if(fscanf(file,"final-battle: %d\n",&is_last_room)!=1) goto invalid_save;
    if (fscanf(file, "%d %d %d %d %d %d %d %d %d %d %d %d\n", &game->player->cord.x, &game->player->cord.y, &game->player->room_num, &game->player->level,
    &game->player->Speed_effect, &game->player->Power_effect, &game->player->Health_effect, &game->player->shield, 
    &game->player->specialfoods[0].count, &game->player->specialfoods[1].count, &game->player->specialfoods[2].count, &game->player->specialfoods[3].count) != 12) goto invalid_save;
    if (fscanf(file, "%d %d %d %d %d %d\n", &game->currentLevel, &game->unlockedLevel, &game->newLevel, &game->levelCount, &game->map_height, &game->map_width) != 6) goto invalid_save;
    if(game->levelCount<1 || game->levelCount>6 || game->currentLevel<0 || game->currentLevel>=game->levelCount
       || game->unlockedLevel<game->currentLevel || game->unlockedLevel>=game->levelCount
       || game->map_height<0 || game->map_height>=MAP_ROWS || game->map_width<0 || game->map_width>=MAP_COLS
       || game->player->cord.x<0 || game->player->cord.x>=MAP_COLS || game->player->cord.y<0 || game->player->cord.y>=MAP_ROWS) goto invalid_save;
    for(int i = 0; i < game->unlockedLevel + 1; i++){
        for(int j = 0; j < game->map_height + 1; j++){
            for(int k = 0; k < game->map_width + 1; k++){
                int a;
                if (fscanf(file, "%d %d ", &game->whole_map[i][j][k].item, &a) != 2) goto invalid_save;
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
        if (fscanf(file, "%d %d %d %d %d %d\n", &game->levels[i]->roomsCount, &a, &game->levels[i]->level, &game->levels[i]->secret_room, &game->levels[i]->key.x, &game->levels[i]->key.y) != 6) goto invalid_save;
        if(game->levels[i]->roomsCount<1 || game->levels[i]->roomsCount>MAX_ROOMS) goto invalid_save;
        if(a == 1){
            game->levels[i]->is_secret_room = true;
        }
        else{
            game->levels[i]->is_secret_room = false;
        }
        for(int j = 0; j < game->levels[i]->roomsCount; j++){
            int b;
            if (fscanf(file, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %c %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
            &game->levels[i]->rooms[j]->x, &game->levels[i]->rooms[j]->y, &game->levels[i]->rooms[j]->height, &game->levels[i]->rooms[j]->width, 
            &game->levels[i]->rooms[j]->doorCount, &b, &game->levels[i]->rooms[j]->index, &game->levels[i]->rooms[j]->pillarCount, &game->levels[i]->rooms[j]->keyCount, 
            &game->levels[i]->rooms[j]->lock_door, &game->levels[i]->rooms[j]->foodCount, &game->levels[i]->rooms[j]->specialfoodCount, &game->levels[i]->rooms[j]->goldCount, 
            &game->levels[i]->rooms[j]->weaponCount, &game->levels[i]->rooms[j]->spellCount, &game->levels[i]->rooms[j]->trapCount, &game->levels[i]->rooms[j]->enemyCount, 
            &game->levels[i]->rooms[j]->enemyLeft, &game->levels[i]->rooms[j]->type, &game->levels[i]->rooms[j]->stairCount, &game->levels[i]->rooms[j]->lock_attemps, 
            &game->levels[i]->rooms[j]->window.x, &game->levels[i]->rooms[j]->window.y, &game->levels[i]->rooms[j]->stair.cord.x, &game->levels[i]->rooms[j]->stair.cord.y, 
            &game->levels[i]->rooms[j]->stair.from, &game->levels[i]->rooms[j]->stair.to, 
            &game->levels[i]->rooms[j]->ancientkey.x, &game->levels[i]->rooms[j]->ancientkey.y, 
            &game->levels[i]->rooms[j]->password_generator.x, &game->levels[i]->rooms[j]->password_generator.y, 
            &game->levels[i]->rooms[j]->password_generator2.x, &game->levels[i]->rooms[j]->password_generator2.y, &game->levels[i]->rooms[j]->shieldCount) != 34) goto invalid_save;
            Room *r=game->levels[i]->rooms[j];
            int counts[]={r->doorCount,r->pillarCount,r->foodCount,r->shieldCount,r->specialfoodCount,r->goldCount,r->weaponCount,r->spellCount,r->trapCount,r->enemyCount};
            if(r->width<5 || r->height<5 || r->x<0 || r->y<0 || r->x+r->width>MAP_COLS || r->y+r->height>MAP_ROWS) goto invalid_save;
            for(size_t n=0;n<sizeof(counts)/sizeof(counts[0]);n++) if(counts[n]<0 || counts[n]>MAP_ROWS*MAP_COLS) goto invalid_save;
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
            game->levels[i]->rooms[j]->enemies = calloc(game->levels[i]->rooms[j]->enemyCount + 1, sizeof(Enemy));
            for(int k = 0; k < game->levels[i]->rooms[j]->doorCount; k++){
                int c;
                if (fscanf(file, "%c %d %d %d %d %d\n", &game->levels[i]->rooms[j]->doors[k].type, &game->levels[i]->rooms[j]->doors[k].cord.x, &game->levels[i]->rooms[j]->doors[k].cord.y,
                &game->levels[i]->rooms[j]->doors[k].password, &game->levels[i]->rooms[j]->doors[k].password2, &c) != 6) goto invalid_save;
                if(c == 1){
                    game->levels[i]->rooms[j]->doors[k].is_open = true;
                }
                else{
                    game->levels[i]->rooms[j]->doors[k].is_open = false;
                }
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->pillarCount; k++){
                if (fscanf(file, "%d %d\n", &game->levels[i]->rooms[j]->pillars[k].x, &game->levels[i]->rooms[j]->pillars[k].y) != 2) goto invalid_save;
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->foodCount; k++){
                int c;
                if (fscanf(file, "%c %d %d %d %d\n", &game->levels[i]->rooms[j]->foods[k].type, &game->levels[i]->rooms[j]->foods[k].cord.x, &game->levels[i]->rooms[j]->foods[k].cord.y,
                &game->levels[i]->rooms[j]->foods[k].health, &c) != 5) goto invalid_save;
                if(c == 1){
                    game->levels[i]->rooms[j]->foods[k].isUsed = true;
                }
                else{
                    game->levels[i]->rooms[j]->foods[k].isUsed = false;
                }
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->shieldCount; k++){
                int c;
                if (fscanf(file, "%d %d %d %d\n", &game->levels[i]->rooms[j]->shields[k].cord.x, &game->levels[i]->rooms[j]->shields[k].cord.y, &game->levels[i]->rooms[j]->shields[k].amount, &c) != 4) goto invalid_save;
                if(c == 1){
                    game->levels[i]->rooms[j]->shields[k].isUsed = true;
                }
                else{
                    game->levels[i]->rooms[j]->shields[k].isUsed = false;
                }
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->specialfoodCount; k++){
                int c;
                if (fscanf(file, "%c %d %d %d\n", &game->levels[i]->rooms[j]->specialfoods[k].type, &game->levels[i]->rooms[j]->specialfoods[k].cord.x, &game->levels[i]->rooms[j]->specialfoods[k].cord.y, &c) != 4) goto invalid_save;
                if(c == 1){
                    game->levels[i]->rooms[j]->specialfoods[k].isUsed = true;
                }
                else{
                    game->levels[i]->rooms[j]->specialfoods[k].isUsed = false;
                }
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->goldCount; k++){
                int c;
                if (fscanf(file, "%c %d %d %d %d\n", &game->levels[i]->rooms[j]->golds[k].type, &game->levels[i]->rooms[j]->golds[k].cord.x, &game->levels[i]->rooms[j]->golds[k].cord.y, &game->levels[i]->rooms[j]->golds[k].count, &c) != 5) goto invalid_save;
                if(c == 1){
                    game->levels[i]->rooms[j]->golds[k].isUsed = true;
                }
                else{
                    game->levels[i]->rooms[j]->golds[k].isUsed = false;
                }
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->weaponCount; k++){
                int c;
                if (fscanf(file, "%c %d %d %d %d %14s %d %d\n", &game->levels[i]->rooms[j]->weapons[k].type, &game->levels[i]->rooms[j]->weapons[k].cord.x, &game->levels[i]->rooms[j]->weapons[k].cord.y,
                &game->levels[i]->rooms[j]->weapons[k].damage, &game->levels[i]->rooms[j]->weapons[k].range, game->levels[i]->rooms[j]->weapons[k].name, &c, &game->levels[i]->rooms[j]->weapons[k].count) != 8) goto invalid_save;
                if(c == 1){
                    game->levels[i]->rooms[j]->weapons[k].isUsed = true;
                }
                else{
                    game->levels[i]->rooms[j]->weapons[k].isUsed = false;
                }
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->spellCount; k++){
                int c;
                if (fscanf(file, "%c %d %d %9s %d\n", &game->levels[i]->rooms[j]->spells[k].type, &game->levels[i]->rooms[j]->spells[k].cord.x, &game->levels[i]->rooms[j]->spells[k].cord.y,
                game->levels[i]->rooms[j]->spells[k].name, &c) != 5) goto invalid_save;
                if(c == 1){
                    game->levels[i]->rooms[j]->spells[k].isUsed = true;
                }
                else{
                    game->levels[i]->rooms[j]->spells[k].isUsed = false;
                }
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->trapCount; k++){
                int c;
                if (fscanf(file, "%d %d %d %d\n", &game->levels[i]->rooms[j]->traps[k].cord.x, &game->levels[i]->rooms[j]->traps[k].cord.y, &game->levels[i]->rooms[j]->traps[k].damage, &c) != 4) goto invalid_save;
                if(c == 1){
                    game->levels[i]->rooms[j]->traps[k].isVisible = true;
                }
                else{
                    game->levels[i]->rooms[j]->traps[k].isVisible = false;
                }
            }
            for(int k = 0; k < game->levels[i]->rooms[j]->enemyCount; k++){
                int c1, c2, c3;
                if (fscanf(file, "%c %d %d %d %d %d %20s %d %d %d\n", &game->levels[i]->rooms[j]->enemies[k].type, &game->levels[i]->rooms[j]->enemies[k].cord.x, &game->levels[i]->rooms[j]->enemies[k].cord.y,
                &game->levels[i]->rooms[j]->enemies[k].damage, &game->levels[i]->rooms[j]->enemies[k].moves, &game->levels[i]->rooms[j]->enemies[k].health, game->levels[i]->rooms[j]->enemies[k].name, &c1, &c2, &c3) != 10) goto invalid_save;
                if(c1 == 1){
                    game->levels[i]->rooms[j]->enemies[k].isAlive = true;
                }
                else{
                    game->levels[i]->rooms[j]->enemies[k].isAlive = false;
                }
                if(c2 == 1){
                    game->levels[i]->rooms[j]->enemies[k].canMove = true;
                }
                else{
                    game->levels[i]->rooms[j]->enemies[k].canMove = false;
                }
                if(c3 == 1){
                    game->levels[i]->rooms[j]->enemies[k].isVisible = true;
                }
                else{
                    game->levels[i]->rooms[j]->enemies[k].isVisible = false;
                }
            }
            
        }
    }
    for(int i=0;i<=game->unlockedLevel;i++) for(int j=0;j<game->levels[i]->roomsCount;j++) {
        Room *r=game->levels[i]->rooms[j];
        if(r->doorCount && (r->lock_door<0 || r->lock_door>=r->doorCount)) goto invalid_save;
        if(game->levels[i]->is_secret_room && (game->levels[i]->secret_room<0 || game->levels[i]->secret_room>=game->levels[i]->roomsCount)) goto invalid_save;
#define CHECK_CELL(list,count,field) for(int k=0;k<r->count;k++) {Point c=r->list[k].field;if(c.x<0 || c.x>=MAP_COLS || c.y<0 || c.y>=MAP_ROWS)goto invalid_save;}
        CHECK_CELL(doors,doorCount,cord);CHECK_CELL(foods,foodCount,cord);CHECK_CELL(shields,shieldCount,cord);
        CHECK_CELL(specialfoods,specialfoodCount,cord);CHECK_CELL(golds,goldCount,cord);CHECK_CELL(weapons,weaponCount,cord);
        CHECK_CELL(spells,spellCount,cord);CHECK_CELL(traps,trapCount,cord);CHECK_CELL(enemies,enemyCount,cord);
#undef CHECK_CELL
    }
    if(game->player->room_num<0 || game->player->room_num>=game->levels[game->currentLevel]->roomsCount) goto invalid_save;
    for(int i=0;i<=game->unlockedLevel;i++) for(int j=0;j<game->levels[i]->roomsCount;j++) {
        Room *r=game->levels[i]->rooms[j];
        if(r->type=='t' && !is_last_room) r->enemies[r->enemyCount]=(Enemy){.type='i',.health=150,.damage=25,.moves=6,.isVisible=true,.canMove=true,.cord={r->x+r->width/2,r->y+r->height/2},.name="Immortal"};
    }
    fclose(file);
    return 1;
invalid_save:
    fclose(file);return 0;
}
