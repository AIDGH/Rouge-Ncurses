#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"

#define ROOM_MIN_SIZE 7
#define ROOM_MAX_SIZE 13
#define ROOM_PADDING 2
#define BORDER_PADDING 6
#define INTERSECT_SPACE 4

// typedef struct
// {
//     int x;
//     int y;
// } Point;

// typedef struct
// {
//     int x, y;
//     int height;
//     int width;
//     // Door *doors;
//     int doorCount;
//     Point window;
//     Point *pillar;
//     bool isVisible;
//     int index;
//     // Food *foods;
//     // Gold *golds;
//     // Gun *guns;
//     // Enchant *enchants;
//     // Trap *traps;
//     // Stair stair;
//     int keyCount;
//     int foodCount;
//     int goldCount;
//     int gunCount;
//     int enchantCount;
//     int trapCount;
//     char type;
//     int stairCount;
// } Room;

void create_room(Room *room);
void draw_window(Room *room);
void create_pillars(Room *room);
void draw_traps(Room *room);
void draw_stair(Room *room);
void draw_player(Room *room);
void connect_random_rooms(Room *rooms, int num_rooms, int room_checker[1000][2], int *t);
int intersects(Room *rooms, int num_rooms, Room room, int total_rooms);
int is_valid_move(int x, int y, Room *rooms, int num_rooms);
void connect_rooms(Room room1, Room room2, Room *rooms, int num_rooms);
void generate_map();

#endif // MAP_GENERATOR_H
