static void check_boss_encounter(WINDOW *win) {
    int floor=game->currentLevel,last=is_last_room;
    Player saved=*game->player;
    Room *previous=game->levels[5]->rooms[0];
    Enemy enemy={.type='i',.health=200,.damage=25,.isAlive=true,.canMove=true,.cord={25,20},.name="Immortal"};
    Room room={.x=6,.y=6,.width=55,.height=30,.type='t',.isVisible=true,.enemies=&enemy,.enemyCount=1,.enemyLeft=1};
    game->levels[5]->rooms[0]=&room;game->currentLevel=5;is_last_room=1;
    game->player->room_num=0;game->player->cord=(Point){10,10};game->player->health=100;game->player->shield=0;
    for(int y=room.y;y<room.y+room.height;y++)for(int x=room.x;x<room.x+room.width;x++)
        game->whole_map[5][y][x].item=(y==room.y||y==room.y+room.height-1||x==room.x||x==room.x+room.width-1)?'|':'.';
    boss_encounter_begin(&enemy);
    for(int i=0;i<4;i++)boss_encounter_tick(&enemy,win);
    assert(game->player->health==100);
    char text[90]={0};
    for(int theme=0;theme<2;theme++){
        ui_old_visuals=theme;draw_last_room(win,&room,0);
        mvwinnstr(win,6,getmaxx(win)-60,text,50);assert(strstr(text,"PHASE I :"));
        mvwinnstr(win,10,getmaxx(win)-60,text,50);assert(strstr(text,"2 turns"));
        /* Warning overlays never change floor/collision data. */
        assert(game->whole_map[5][10][12].item=='.');
    }
    game->player->cord=(Point){11,11}; /* Leave both possible targeted lanes. */
    boss_encounter_tick(&enemy,win);assert(game->player->health==100);
    boss_encounter_tick(&enemy,win);assert(game->player->health==100);
    boss_encounter_draw(win,&room);mvwinnstr(win,9,getmaxx(win)-60,text,50);assert(strstr(text,"DODGED"));
    boss_encounter_begin(&enemy);
    for(int i=0;i<6;i++)boss_encounter_tick(&enemy,win);
    assert(game->player->health==86); /* Remaining in the warned lane takes fire damage. */
    game->player->health=100;
    enemy.health=100;boss_encounter_tick(&enemy,win);assert(game->player->health==100);
    boss_encounter_draw(win,&room);mvwinnstr(win,6,getmaxx(win)-60,text,50);assert(strstr(text,"PHASE II"));
    boss_encounter_tick(&enemy,win); /* Ground slam warning, held at its origin. */
    Point centre=enemy.cord;game->player->cord=centre;game->player->shield=5;
    boss_encounter_tick(&enemy,win);
    assert(game->player->shield==0&&game->player->health==83);
    boss_encounter_tick(&enemy,win);assert(game->player->health==83&&enemy.cord.x==centre.x&&enemy.cord.y==centre.y);
    run_finished=1;boss_encounter_tick(&enemy,win);assert(game->player->health==83);run_finished=0;
    enemy.isAlive=false;boss_encounter_tick(&enemy,win);assert(game->player->health==83);
    enemy.isAlive=true;enemy.health=200;boss_encounter_begin(&enemy);boss_encounter_draw(win,&room);
    mvwinnstr(win,6,getmaxx(win)-60,text,50);assert(strstr(text,"PHASE I :"));
    enemy.health=80;boss_encounter_begin(&enemy);boss_encounter_draw(win,&room);
    mvwinnstr(win,6,getmaxx(win)-60,text,50);assert(strstr(text,"PHASE II"));
    boss_encounter_reset();*game->player=saved;game->levels[5]->rooms[0]=previous;game->currentLevel=floor;is_last_room=last;
    fprintf(stderr,"PASS boss warning before damage, locked aim and dodge, two phases, shield overflow, recovery, reset/completion guard and panel in both themes\n");
}
