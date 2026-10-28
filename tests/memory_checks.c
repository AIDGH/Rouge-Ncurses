#define main original_game_main
#include "../main.c"
#undef main
#include <assert.h>
#include <sys/stat.h>
extern int back_front, after_battle, is_battle, is_last_room;
extern int m_check, all_print, color_pair_num;
extern void enemy_move(Enemy *, WINDOW *);
extern void attacking(WINDOW *);
extern int direction,move_click;
extern void enemy_be_attaked(int,int,WINDOW *);
extern int valid_move_check(int,int);
extern int mouse_attack_event(const MEVENT *);
extern void draw_attacked_room(WINDOW *,Room *,int);

#include "routes_checks.inc"
#include "combat_checks.inc"
extern void draw_last_room(WINDOW *,Room *,int);
#include "boss_checks.inc"
#include "victory_checks.inc"

static void make_fixture(void) {
    character = L'@'; selected_music = "./Base_Sounds/silent.wav";
    game = calloc(1,sizeof(Game));
    game->player = calloc(1,sizeof(Player));
    game->player->name = "qa-review";
    strcpy(game->player->color, " White ");
    game->player->health=100; game->player->foodCount=100;
    game->player->weapons=calloc(5,sizeof(Weapon));
    game->player->spells=calloc(3,sizeof(Spell));
    game->player->specialfoods=calloc(4,sizeof(Food));
    game->levels=calloc(7,sizeof(Level *));
    for(int i=0;i<7;i++) {
        game->levels[i]=calloc(1,sizeof(Level));
        game->levels[i]->level=i;
        game->levels[i]->rooms=calloc(9,sizeof(Room *));
        Room *storage=calloc(9,sizeof(Room));
        for(int j=0;j<9;j++) game->levels[i]->rooms[j]=storage+j;
    }
    game->levelCount=6; difficulty_efficiency=1;
    color_pair_num=2; game->map_height=54;game->map_width=179;
}

int main(int argc,char **argv) {
    setlocale(LC_ALL,"en_US.UTF-8");
    FILE *sink=fopen("/dev/null","w"),*input=fopen("/dev/null","r");
    SCREEN *screen=newterm("xterm-256color",sink,input);assert(screen);
    resizeterm(55,180);start_color();
    for(int i=1;i<64;i++) init_pair(i,i%8,COLOR_BLACK);
    make_fixture();
    int rounds=argc>1?atoi(argv[1]):10;
    int rows=argc>2?atoi(argv[2]):55, cols=argc>3?atoi(argv[3]):180;resizeterm(rows,cols);
    game->map_height=(rows<MAP_ROWS?rows:MAP_ROWS)-1;game->map_width=(cols<MAP_COLS?cols:MAP_COLS)-1;
    for(int n=0;n<rounds;n++) {
        ui_old_visuals=n%2;
        srand(1000+n);difficulty_efficiency=1+n%4;
        for(int floor=0;floor<7;floor++) {
            game->currentLevel=floor;
            if(floor<6) game->unlockedLevel=floor;
            generate_map();
            check_routes();
            fprintf(stderr,"generated round %d floor %d rooms %d\n",n,floor,game->levels[floor]->roomsCount);
            for(int j=0;j<game->levels[floor]->roomsCount;j++) {
                Room *r=game->levels[floor]->rooms[j];
                game->player->room_num=j;
                r->isVisible=true;
                draw_room(stdscr,r,j);
                for(int k=0;k<r->enemyCount;k++) {
                    enemy_move(&r->enemies[k],stdscr);
                }
            }
            print_map(stdscr);
        }
    }
    fprintf(stderr,"PASS connected room routes, %d mandatory locks with accessible codes; swords %d / %d weapons\n",tested_locks,tested_swords,tested_weapons);
    game->currentLevel=0;game->player->room_num=0;
    Room *combat=game->levels[0]->rooms[0];
    game->player->cord=(Point){combat->x+2,combat->y+2};
    free(combat->enemies);combat->enemies=calloc(2,sizeof(Enemy));combat->enemyCount=combat->enemyLeft=1;
    combat->enemies[0]=(Enemy){.cord={combat->x+3,combat->y+2},.health=5,.isAlive=true,.canMove=true,.name="TestEnemy"};
    game->whole_map[0][combat->y+2][combat->x+3].item='D';
    game->player->currentWeapon=(Weapon){.type='m',.damage=5,.count=1};
    enemy_be_attaked(combat->y+2,combat->x+3,stdscr);
    assert(!combat->enemies[0].isAlive && combat->enemyLeft==0);
    combat->enemyCount=0;combat->type='n';
    free(combat->traps);combat->traps=calloc(1,sizeof(Trap));combat->trapCount=1;
    combat->traps[0].cord=(Point){combat->x+2,combat->y+3};
    MEVENT event={.x=combat->x+3,.y=combat->y+3,.bstate=REPORT_MOUSE_POSITION};
    assert(!mouse_attack_event(&event));event.bstate=BUTTON4_PRESSED;assert(!mouse_attack_event(&event));
    event.bstate=BUTTON1_PRESSED;assert(mouse_attack_event(&event));
    for(int visual=0;visual<2;visual++){
        ui_old_visuals=visual;game->player->currentWeapon=(Weapon){.type='m',.damage=5,.count=1};
        move_click='e';attacking(stdscr);draw_attacked_room(stdscr,combat,0);
        game->player->currentWeapon.type='s';game->player->weapons[4].count=2;
        draw_attacked_room(stdscr,combat,0);assert(combat->traps[0].isVisible);
    }
    fprintf(stderr,"PASS mouse hover/scroll filtering and melee trap rendering in both themes\n");

    int initial_weapons=combat->weaponCount;
    for(int x=combat->x+1;x<combat->x+combat->width-1;x++)game->whole_map[0][combat->y+2][x].item='.';
    game->whole_map[0][combat->y+2][combat->x+combat->width-1].item='|';
    direction='d';move_click='e';
    for(int i=0;i<24;i++) {
        game->player->currentWeapon=(Weapon){.type='a',.count=50,.damage=5,.range=5,.name="NormalArrow"};attacking(stdscr);
    }
    assert(combat->weaponCount==initial_weapons+24);
    fprintf(stderr,"PASS melee death and 24 recoverable ranged shots with dynamic room inventory\n");
    check_combat_controls(stdscr,combat);
    check_boss_encounter(stdscr);
    check_victory_rewards();
    /* Regression: room indices differ from item indices, including consumed items and shields. */
    game->currentLevel=0;game->unlockedLevel=0;game->player->room_num=0;
    Room *r=game->levels[0]->rooms[0];
    for(int i=0;i<5;i++){snprintf(game->player->weapons[i].name,15,"Weapon%c",'A'+i);game->player->weapons[i].count=i;}
    for(int i=0;i<3;i++){snprintf(game->player->spells[i].name,10,"Spell%c",'A'+i);game->player->spells[i].count=i;}
    if(!r->foodCount){free(r->foods);r->foods=calloc(1,sizeof(Food));r->foodCount=1;r->foods[0].cord=(Point){r->x+2,r->y+2};}
    r->foods[0].isUsed=true;
    if(r->shieldCount==0){free(r->shields);r->shields=calloc(1,sizeof(Shield));r->shieldCount=1;}
    r->shields[0]=(Shield){.amount=25,.isUsed=true,.cord={r->x+2,r->y+2}};
    int foods=r->foodCount, shields=r->shieldCount, savedx=r->x;
    mkdir("users",0700);
    assert(save_user_game(game));
    User info={0};assert(load_user_info("qa-review",&info));assert(info.weaponsCount_each[4]==4);
    for(int i=0;i<MAX_LEVELS;i++)for(int j=0;j<MAX_ROOMS;j++)release_room(game->levels[i]->rooms[j]);
    assert(load_user_game());r=game->levels[0]->rooms[0];
    assert(r->x==savedx && r->foodCount==foods && r->foods[0].isUsed && r->shieldCount==shields && r->shields[0].isUsed && r->shields[0].amount==25);
    for(int j=0;j<game->levels[0]->roomsCount;j++)draw_room(stdscr,game->levels[0]->rooms[j],j);
    assert(save_user_game(game));assert(access("users/qa-review.txt.bak",F_OK)==0);
    FILE *bad=fopen("users/qa-review.txt","w");fputs("health: 100\n",bad);fclose(bad);assert(!load_user_game());
    assert(!valid_move_check(-1,0));assert(!valid_move_check(0,-1));assert(!valid_move_check(MAP_ROWS,0));assert(!valid_move_check(0,MAP_COLS));
    fprintf(stderr,"PASS save/load roundtrip, consumed flags, shields, backup, truncated-save rejection and boundary movement\n");
    release_game();
    endwin();delscreen(screen);fclose(sink);fclose(input);
    fprintf(stderr,"PASS generation, all room renders and enemy updates: %d floors\n",rounds*7);
    return 0;
}
