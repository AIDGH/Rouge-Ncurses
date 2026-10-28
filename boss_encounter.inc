/* Turn-based additions to Immortal. Existing pursuit, contact damage and weapons remain. */
typedef struct {
    int active, max_health, phase, cooldown, windup, recovery, attack, sequence;
    int lane, vertical, radius, flash, dodges, hits;
    Point centre;
    char notice[80];
} BossEncounter;
static BossEncounter boss_encounter;

void boss_encounter_reset(void) { memset(&boss_encounter,0,sizeof(boss_encounter)); }
void boss_encounter_stats(int *dodges,int *hits) {
    *dodges=boss_encounter.dodges;*hits=boss_encounter.hits;
}

void boss_encounter_begin(Enemy *enemy) {
    boss_encounter_reset();
    boss_encounter.active=1;
    boss_encounter.max_health=enemy->health>200?enemy->health:200;
    boss_encounter.phase=enemy->health<=boss_encounter.max_health/2?2:1;
    boss_encounter.cooldown=4;
    snprintf(boss_encounter.notice,sizeof(boss_encounter.notice),"Watch the marked tiles. Move to dodge.");
}

static int boss_in_danger(Point p) {
    if(boss_encounter.attack==1)
        return boss_encounter.vertical?p.x==boss_encounter.lane:p.y==boss_encounter.lane;
    return abs(p.x-boss_encounter.centre.x)<=boss_encounter.radius
        &&abs(p.y-boss_encounter.centre.y)<=boss_encounter.radius;
}

void boss_encounter_tick(Enemy *enemy,WINDOW *win) {
    BossEncounter *b=&boss_encounter;
    if(!is_last_room||run_finished||escape_check||!enemy->isAlive||enemy->health<=0)return;
    if(!b->active)boss_encounter_begin(enemy);
    b->flash=0;
    if(b->phase==1&&enemy->health<=b->max_health/2){
        b->phase=2;b->windup=b->recovery=b->attack=0;b->cooldown=1;
        snprintf(b->notice,sizeof(b->notice),"ENRAGED! Faster attacks. Watch the floor.");
        return; /* The transition itself never deals damage. */
    }
    if(b->windup){
        if(--b->windup)return;
        int damage=b->attack==1?(b->phase==1?14:18):(b->phase==1?18:22);
        if(boss_in_danger(game->player->cord)){
            b->hits++;
            int absorbed=game->player->shield<damage?game->player->shield:damage;
            game->player->shield-=absorbed;game->player->health-=damage-absorbed;
            first_damage=first_damage==0?1:first_damage;
            snprintf(b->notice,sizeof(b->notice),"%s hit: %d damage (%d absorbed).",b->attack==1?"Dragonfire":"Ground slam",damage,absorbed);
        }else {
            b->dodges++;
            snprintf(b->notice,sizeof(b->notice),"DODGED! Immortal is recovering - strike!");
        }
        b->flash=1;b->recovery=2;b->cooldown=b->phase==1?4:3;
        return;
    }
    if(b->recovery){
        if(--b->recovery==0)
            snprintf(b->notice,sizeof(b->notice),"Immortal resumes pursuit. Prepare to dodge.");
        return;
    }
    if(--b->cooldown<=0){
        b->attack=++b->sequence%2?1:2;
        b->windup=b->phase==1?2:1;
        b->centre=enemy->cord;b->radius=b->phase==1?2:3;
        /* Target the current position, then lock it so the player can escape. */
        b->vertical=abs(enemy->cord.x-game->player->cord.x)<abs(enemy->cord.y-game->player->cord.y);
        b->lane=b->vertical?game->player->cord.x:game->player->cord.y;
        snprintf(b->notice,sizeof(b->notice),"%s - leave the marked tiles!",b->attack==1?"DRAGONFIRE":"GROUND SLAM");
        return;
    }
    enemy_move(enemy,win);
}

void boss_encounter_draw(WINDOW *win,Room *room) {
    if(!is_last_room||run_finished||!boss_encounter.active)return;
    Enemy *enemy=NULL;
    for(int i=0;i<room->enemyCount;i++)if(room->enemies[i].type=='i'&&room->enemies[i].isAlive){enemy=&room->enemies[i];break;}
    if(!enemy)return;
    BossEncounter *b=&boss_encounter;
    attr_t attrs;short pair;wattr_get(win,&attrs,&pair,NULL);
    if(b->windup||b->flash){
        wattr_set(win,A_BOLD,b->flash?6:12,NULL);
        for(int y=room->y+1;y<room->y+room->height-1;y++)
            for(int x=room->x+1;x<room->x+room->width-1;x++){
                Point p={x,y};
                if(!boss_in_danger(p)||(x==enemy->cord.x&&y==enemy->cord.y)
                    ||(x==game->player->cord.x&&y==game->player->cord.y))continue;
                /* A visual overlay only: never replace loot, traps or collision data. */
                int tile=game->whole_map[game->currentLevel][y][x].item;
                if(tile=='.')mvwaddch(win,y,x,b->flash?'*':b->attack==1?':':'+');
            }
        if(b->windup&&boss_in_danger(game->player->cord)){
            wattron(win,A_REVERSE);
            draw_player_avatar(win,game->player->cord.y,game->player->cord.x);
            wattroff(win,A_REVERSE);
        }
    }
    int height,width;getmaxyx(win,height,width);
    int x=width-62,y=5,w=56;
    if(x<0||height<14){wattr_set(win,attrs,pair,NULL);return;}
    wattr_set(win,A_NORMAL,b->phase==2?6:12,NULL);
    for(int i=0;i<8;i++)mvwhline(win,y+i,x,' ',w);
    mvwaddstr(win,y,x,"╭──────────────────────────────────────────────────────╮");
    mvwaddstr(win,y+7,x,"╰──────────────────────────────────────────────────────╯");
    for(int i=1;i<7;i++){mvwaddstr(win,y+i,x,"│");mvwaddstr(win,y+i,x+w-1,"│");}
    wattron(win,A_BOLD);
    mvwprintw(win,y+1,x+2,"IMMORTAL   /   PHASE %s",b->phase==1?"I : THE SENTINEL":"II : ENRAGED");
    int hp=enemy->health;if(hp<0)hp=0;if(hp>b->max_health)hp=b->max_health;
    mvwprintw(win,y+2,x+2,"%3d / %3d HP",hp,b->max_health);
    int filled=hp*34/b->max_health;
    for(int i=0;i<34;i++)mvwaddstr(win,y+2,x+17+i,i<filled?"━":"·");
    wattroff(win,A_BOLD);
    mvwaddnstr(win,y+4,x+2,b->notice,w-4);
    if(b->windup)mvwprintw(win,y+5,x+2,"DODGE : %d turn%s  |  %c marks danger",b->windup,b->windup==1?"":"s",b->attack==1?':':'+');
    else if(b->recovery)mvwaddstr(win,y+5,x+2,"RECOVERY : your opening to attack");
    else mvwaddstr(win,y+5,x+2,"Keep moving. Your next action advances the fight.");
    mvwaddstr(win,y+6,x+2,"Arrows/WASD dodge  |  SPACE / E strike  |  H help");
    wattr_set(win,attrs,pair,NULL);
}
