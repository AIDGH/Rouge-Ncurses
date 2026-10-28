/* One reward per completed expedition. Profile totals remain compatible with old saves. */
typedef struct {
    int ready, closed, choice, seconds, kills, rooms, dodges, hits, saved;
    int gold_before, starting_gold, base_play_time;
    long long points_before, starting_points;
} VictoryResults;
static VictoryResults victory_results={.choice=-1};
static const char *victory_names[]={"DRAGON'S HOARD","IMMORTAL'S SIGIL","TRAVELLER'S CACHE"};

void victory_results_reset(void) { memset(&victory_results,0,sizeof(victory_results));victory_results.choice=-1; }
static void victory_results_start(int gold,int play_time) {
    victory_results_reset();victory_results.starting_gold=gold;victory_results.base_play_time=play_time;
    victory_results.starting_points=pointss;
}
int victory_results_ready(void) { return victory_results.ready; }
static int victory_results_closed(void) { return victory_results.closed; }

static int victory_gold(int choice) { return choice==0?750:choice==2?350:0; }
static long long victory_points(int choice) {
    return (choice==1?12000LL:choice==2?6000LL:0)*(difficulty_efficiency>0?difficulty_efficiency:1);
}
static void victory_store(void) {
    Player *p=game->player;
    if(!strcmp(p->name,"guest")||!strcmp(p->name,"You")){victory_results.saved=2;return;}
    victory_results.saved=save_just_user_info(p)?1:0;
}
static void victory_claim(int choice) {
    if(victory_results.choice>=0||choice<0||choice>2)return;
    victory_results.choice=choice;
    game->player->gold+=victory_gold(choice);
    game->player->points+=victory_points(choice);
    victory_store();
}
static void victory_prepare(time_t began) {
    VictoryResults *v=&victory_results;
    if(v->ready)return;
    v->ready=1;
    double elapsed=difftime(time(NULL),began);
    v->seconds=elapsed>0?(int)elapsed:0;
    game->player->play_time=v->base_play_time+v->seconds;
    v->gold_before=game->player->gold;v->points_before=game->player->points;
    for(int l=0;l<game->levelCount;l++)for(int r=0;r<game->levels[l]->roomsCount;r++){
        Room *room=game->levels[l]->rooms[r];
        if(room->isVisible)v->rooms++;
        for(int e=0;e<room->enemyCount;e++)
            if(!room->enemies[e].isAlive&&room->enemies[e].health<=0)v->kills++;
    }
    boss_encounter_stats(&v->dodges,&v->hits);
}
static void victory_draw(WINDOW *win,int selected) {
    VictoryResults *v=&victory_results;Player *p=game->player;
    werase(win);ui_frame(win," EXPEDITION COMPLETE ");
    wattron(win,A_BOLD|COLOR_PAIR(12));
    mvwaddstr(win,2,48,"   .------------.   ");
    mvwaddstr(win,3,48,"  (|  IMMORTAL  |)  ");
    mvwaddstr(win,4,48,"   |   FALLEN   |   ");
    mvwaddstr(win,5,48,"    '----..----'    ");
    mvwaddstr(win,6,48,"       __||__       ");
    mvwaddstr(win,7,48,"      |______|      ");
    mvwprintw(win,9,5,"HERO  %-38.38s",p->name);
    wattroff(win,A_BOLD|COLOR_PAIR(12));
    mvwprintw(win,11,5,"Session time      %02d:%02d:%02d",v->seconds/3600,(v->seconds/60)%60,v->seconds%60);
    mvwprintw(win,12,5,"Enemies defeated  %d",v->kills);
    mvwprintw(win,13,5,"Rooms discovered  %d",v->rooms);
    mvwprintw(win,14,5,"Health remaining  %d    Shield %d",p->health,p->shield);
    mvwprintw(win,11,64,"Boss specials dodged   %d",v->dodges);
    mvwprintw(win,12,64,"Boss specials taken    %d",v->hits);
    mvwprintw(win,13,64,"Gold gained            %d",p->gold-v->starting_gold);
    mvwprintw(win,14,64,"Profile score          %lld",p->points);
    long long gained=p->points-v->starting_points;
    mvwprintw(win,15,64,"Score gained           %lld",gained>0?gained:0);
    wattron(win,A_BOLD|COLOR_PAIR(12));
    mvwaddstr(win,17,5,v->choice<0?"CLAIM ONE TROPHY  -  bonus on top of the boss's normal reward":"TROPHY CLAIMED  -  your expedition is complete");
    wattroff(win,A_BOLD|COLOR_PAIR(12));
    for(int i=0;i<3;i++){
        int x=4+i*37,y=19,w=35;
        int pair=v->choice>=0?(v->choice==i?12:10):(selected==i?12:10);
        wattron(win,COLOR_PAIR(pair));if((v->choice<0&&selected==i)||v->choice==i)wattron(win,A_BOLD);
        mvwaddstr(win,y,x,"╭─────────────────────────────────╮");
        for(int r=1;r<7;r++){mvwaddstr(win,y+r,x,"│");mvwaddstr(win,y+r,x+w-1,"│");}
        mvwaddstr(win,y+7,x,"╰─────────────────────────────────╯");
        mvwprintw(win,y+1,x+3,"%d  %s",i+1,victory_names[i]);
        mvwaddstr(win,y+2,x+3,i==0?"    .--------.    ":i==1?"        /\\       ":"     _______      ");
        mvwaddstr(win,y+3,x+3,i==0?"    |  GOLD  |    ":i==1?"       <  >      ":"    /__|_|__\\    ");
        mvwprintw(win,y+5,x+3,"+%d gold",victory_gold(i));
        mvwprintw(win,y+6,x+3,"+%lld score",victory_points(i));
        wattroff(win,A_BOLD|COLOR_PAIR(pair));
    }
    if(v->choice<0){
        mvwprintw(win,28,5,"PREVIEW  Gold: %d -> %d   |   Score: %lld -> %lld",v->gold_before,v->gold_before+victory_gold(selected),v->points_before,v->points_before+victory_points(selected));
        mvwaddstr(win,30,5,"Left / Right or 1 / 2 / 3 : select    ENTER : claim    ESC : leave without bonus");
    }else{
        wattron(win,A_BOLD|COLOR_PAIR(12));mvwprintw(win,28,5,"CLAIMED: %s   |   Gold %d   Score %lld",victory_names[v->choice],p->gold,p->points);wattroff(win,A_BOLD|COLOR_PAIR(12));
        mvwaddstr(win,30,5,"ENTER / ESC : return to the main menu");
        mvwaddstr(win,32,5,v->saved==1?"Result saved to your profile and local scoreboard.":v->saved==2?"Guest expedition. Local result; no player profile.":"Could not save the result yet. R : retry saving.");
    }
    mvwaddstr(win,35,5,"Timmy: I knew you would make it. Until our next adventure!");
    wrefresh(win);
}
static void victory_results_screen(WINDOW *canvas,time_t began) {
    victory_prepare(began);
    WINDOW *win=derwin(canvas,38,120,(getmaxy(canvas)-38)/2,(getmaxx(canvas)-120)/2);
    if(!win){victory_store();victory_results.closed=1;return;}
    keypad(win,TRUE);int selected=0;
    for(;;){
        victory_draw(win,selected);
        int key=music_getch(win);
        if(key==27){if(victory_results.choice<0)victory_store();break;}
        if(victory_results.choice>=0){
            if(key==10||key==KEY_ENTER)break;
            if(key=='r'||key=='R')victory_store();
        }else{
            if(key==KEY_LEFT||key==KEY_UP)selected=(selected+2)%3;
            if(key==KEY_RIGHT||key==KEY_DOWN)selected=(selected+1)%3;
            if(key>='1'&&key<='3')selected=key-'1';
            if(key==10||key==KEY_ENTER)victory_claim(selected);
        }
    }
    victory_results.closed=1;delwin(win);
}
