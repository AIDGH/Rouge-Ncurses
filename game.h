#ifndef GAME_H
#define GAME_H

typedef struct
{
    int x;
    int y;
} Point;

typedef struct
{
    bool isVisible;
    int item;
} Map;

typedef struct
{
    Point points[100]; // (x,y) of the way
    int count;
    int index;
    int visiblePoint;
} Corridor;

typedef struct
{
    Point cord;
    char type; // 'n' -> normal; 's' -> secret; 'l' -> locked;  'h' -> hard locked
    int password;
    int password2;
    bool is_open;
} Door;

typedef struct
{
    int health;
    char type; // 'n' -> normal; 'p' -> power; 's' -> speed; 'r' -> rotten
    bool isUsed;
    int count;
    Point cord;
} Food;

typedef struct
{
    int amount;
    bool isUsed;
    Point cord;
} Shield;

typedef struct
{
    char type; // m -> mace; d -> dagger; w -> magic wand; a -> normal arrow; s -> sword
    char name[15];
    bool isUsed;
    int count;
    int damage;
    int range;
    int move;
    Point cord;
} Weapon;

typedef struct
{
    char type; // s -> speed; p -> power; h -> health / healing
    char name[10];
    bool isUsed;
    int count;
    Point cord;
} Spell;

typedef struct
{
    int count;
    char type; // n -> normal; n -> black
    bool isUsed;
    Point cord;
} Gold;

typedef struct
{
    int damage;
    Point cord;
    bool isVisible;
} Trap;

typedef struct
{
    int from; // source level
    int to;   // dest level
    Point cord;
} Stair;

typedef struct
{
    char type; // d -> deamon; f -> fire breathing monster; g -> giant; s -> snake; u -> undead
    // bool isUsed;
    int health;
    int damage;
    int moves;
    bool isAlive;
    bool canMove;
    bool isVisible;
    char name[21];
    Point cord;
} Enemy;

typedef struct
{
    int x, y;
    int height;
    int width;
    Door *doors;
    int doorCount;
    Point window;
    Point *pillars;
    bool isVisible;
    int index;
    Shield *shields;
    Food *foods;
    Food *specialfoods;
    Gold *golds;
    Weapon *weapons;
    Weapon *newWeapons;
    Spell *spells;
    Trap *traps;
    Enemy *enemies;
    Stair stair;
    Point ancientkey;
    Point password_generator;
    Point password_generator2;
    int pillarCount;
    int keyCount;
    int lock_door;
    int foodCount;
    int shieldCount;
    int specialfoodCount;
    int goldCount;
    int weaponCount;
    int spellCount;
    int trapCount;
    int enemyCount;
    int enemyLeft;
    int newWeaponsCount;
    char type; // 'm' -> nightmare / madness; 'n' -> normal; 's' -> secret; 't' -> tresure; 'p' -> spell / posion; 'b' -> battle; 'r' -> reinforce
    int stairCount;
    int lock_attemps;
} Room;

typedef struct
{
    Point cord;
    int health;
    int state; // 0 -> in Corridor ; 1 -> in room
    // Room *room;
    // Corridor *Corridor;
    Weapon* weapons;
    Spell* spells;
    // Food* foods;
    int foodCount;
    int shield;
    int weaponCount;
    int spellCount;
    int room_num;
    int level;
    char *name;
    char color[10];
    int acientKey;
    int gold;
    long long int points;
    int games_count;
    int play_time;
    int brokenAcientKey;
    int Speed_effect;
    int Power_effect;
    int Health_effect;
    int can_be_saved;
    Food *specialfoods;
    Weapon currentWeapon;
} Player;

typedef struct
{
    Room **rooms;
    // Corridor **Corridors;
    Point key;
    int roomsCount;
    bool is_secret_room;
    int level;
    int secret_room;
} Level;

typedef struct
{
    Level **levels;
    Player *player;
    int currentLevel;
    int unlockedLevel;
    int newLevel;
    int levelCount;
    int map_height;
    int map_width;
    bool new_game;
    Map whole_map[6][100][300];
} Game;

void start_game();
void game_play(WINDOW *game_win);
void print_map(WINDOW *game_win);
void draw_room(WINDOW *game_win, Room *room, int room_number);

#endif