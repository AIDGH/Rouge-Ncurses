static unsigned char reached[MAP_ROWS][MAP_COLS];
static Point queue_cells[MAP_ROWS*MAP_COLS];
static int test_walk(int x,int y,Point blocked){
    if(x<0||y<0||x>=game->map_width||y>=game->map_height||(x==blocked.x&&y==blocked.y))return 0;
    int c=game->whole_map[game->currentLevel][y][x].item&A_CHARTEXT;
    return c&&c!=' '&&c!='|'&&c!='-'&&c!='_'&&c!='O'&&c!='=';
}
static void test_flood(Point start,Point blocked){
    memset(reached,0,sizeof(reached));int h=0,t=0;assert(test_walk(start.x,start.y,blocked));
    reached[start.y][start.x]=1;queue_cells[t++]=start;
    int dx[]={1,-1,0,0},dy[]={0,0,1,-1};
    while(h<t){Point p=queue_cells[h++];for(int d=0;d<4;d++){
        int x=p.x+dx[d],y=p.y+dy[d];if(test_walk(x,y,blocked)&&!reached[y][x]){reached[y][x]=1;queue_cells[t++]=(Point){x,y};}
    }}
}
static int tested_locks,tested_swords,tested_weapons;
static void check_routes(void){
    Level *level=game->levels[game->currentLevel];
    if(game->currentLevel>=5)return;
    test_flood(game->player->cord,(Point){-1,-1});
    for(int r=0;r<level->roomsCount;r++){
        Room *room=level->rooms[r];
        if(room->doors&&room->doorCount&&room->doors[0].type=='s')continue; /* teleport annex */
        int accessible=0;
        for(int y=room->y+1;y<room->y+room->height-1;y++)for(int x=room->x+1;x<room->x+room->width-1;x++)accessible|=reached[y][x];
        if(!accessible){
            fprintf(stderr,"Disconnected room %d on floor %d player %d,%d\n",r,game->currentLevel,game->player->cord.x,game->player->cord.y);
            for(int z=0;z<level->roomsCount;z++){Room *v=level->rooms[z];fprintf(stderr,"room %d: %d,%d %dx%d doors%d\n",z,v->x,v->y,v->width,v->height,v->doorCount);}
            for(int y=0;y<40;y++){for(int x=0;x<130;x++){int c=game->whole_map[game->currentLevel][y][x].item&A_CHARTEXT;fputc(reached[y][x]?'@':c?c:' ',stderr);}fputc('\n',stderr);}
        }
        assert(accessible);
        for(int w=0;w<room->weaponCount;w++){tested_weapons++;tested_swords+=room->weapons[w].type=='s';}
    }
    for(int r=0;r<level->roomsCount;r++){
        Room *room=level->rooms[r];
        for(int d=0;d<room->doorCount;d++){
            Door *door=&room->doors[d];if(door->type!='l'&&door->type!='h')continue;
            int dx=door->cord.x==room->x?-1:door->cord.x==room->x+room->width-1?1:0;
            int dy=dx?0:door->cord.y==room->y?-1:1;
            Point in={door->cord.x-dx,door->cord.y-dy},out={door->cord.x+dx,door->cord.y+dy};
            test_flood(in,door->cord);
            assert(!reached[out.y][out.x]);assert(reached[room->password_generator.y][room->password_generator.x]);
            if(door->type=='h')assert(reached[room->password_generator2.y][room->password_generator2.x]);
            int saved_room=game->player->room_num;game->player->room_num=(r+1)%level->roomsCount;
            assert(!valid_move_check(door->cord.y,door->cord.x));door->is_open=true;assert(valid_move_check(door->cord.y,door->cord.x));door->is_open=false;game->player->room_num=saved_room;
            tested_locks++;
        }
    }
}
