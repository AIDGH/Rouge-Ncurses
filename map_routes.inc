/* Generation-time topology checks use the authoritative draft map, not door proximity. */
static unsigned char route_seen[MAP_ROWS][MAP_COLS];
static Point route_queue[MAP_ROWS*MAP_COLS];
static int route_walk(int x,int y,Point blocked) {
    if(x<BORDER_PADDING+2||y<BORDER_PADDING+1||x>=COLS||y>=LINES||x>=MAP_COLS||y>=MAP_ROWS||(x==blocked.x&&y==blocked.y))return 0;
    int c=mvwinch(stdscr,y,x)&A_CHARTEXT;
    return c && c!=' ' && c!='|' && c!='-' && c!='_' && c!='O' && c!='=' && c!='L';
}
static void route_flood(Point start,Point blocked) {
    memset(route_seen,0,sizeof(route_seen));int head=0,tail=0;
    if(!route_walk(start.x,start.y,blocked))return;
    route_queue[tail++]=start;route_seen[start.y][start.x]=1;
    const int dx[]={1,-1,0,0},dy[]={0,0,1,-1};
    while(head<tail){Point p=route_queue[head++];for(int i=0;i<4;i++){
        int x=p.x+dx[i],y=p.y+dy[i];
        if(route_walk(x,y,blocked)&&!route_seen[y][x]){route_seen[y][x]=1;route_queue[tail++]=(Point){x,y};}
    }}
}
static int door_sides(Room *r,Door *d,Point *inside,Point *outside) {
    int x=d->cord.x,y=d->cord.y,dx=0,dy=0;
    if(x==r->x&&y>r->y&&y<r->y+r->height-1)dx=-1;
    else if(x==r->x+r->width-1&&y>r->y&&y<r->y+r->height-1)dx=1;
    else if(y==r->y&&x>r->x&&x<r->x+r->width-1)dy=-1;
    else if(y==r->y+r->height-1&&x>r->x&&x<r->x+r->width-1)dy=1;
    else return 0;
    *inside=(Point){x-dx,y-dy};*outside=(Point){x+dx,y+dy};
    return route_walk(inside->x,inside->y,(Point){-1,-1})&&route_walk(outside->x,outside->y,(Point){-1,-1});
}
static void prune_bypass_doors(int rooms) {
    for(int r=0;r<rooms;r++){
        Room *room=game->levels[game->currentLevel]->rooms[r];
        for(int i=0;i<room->doorCount;){
            Door *d=&room->doors[i];Point in,out;
            if(!door_sides(room,d,&in,&out)){i++;continue;}
            route_flood(in,d->cord);
            int redundant=1;
            const int dx[]={1,-1,0,0},dy[]={0,0,1,-1};
            for(int n=0;n<4;n++){
                int x=d->cord.x+dx[n],y=d->cord.y+dy[n];
                if(route_walk(x,y,d->cord)&&!route_seen[y][x])redundant=0;
            }
            if(!redundant){i++;continue;}
            /* An alternate route exists: closing this entrance cannot disconnect the map. */
            mvaddch(d->cord.y,d->cord.x,(d->cord.y==room->y||d->cord.y==room->y+room->height-1)?'-':'|');
            memmove(d,d+1,(room->doorCount-i-1)*sizeof(*d));room->doorCount--;
        }
    }
}
/* Repair rare disconnected drafts after pillars/windows have been placed. */
static void ensure_connected_rooms(int count) {
    static Point parent[MAP_ROWS][MAP_COLS];
    static unsigned char queued[MAP_ROWS][MAP_COLS];
    int maxx=game->map_width*3/4-BORDER_PADDING-5,maxy=game->map_height*3/4-BORDER_PADDING-2;
    for(int r=0;r<count;r++){
        route_flood(game->player->cord,(Point){-1,-1});
        Room *room=game->levels[game->currentLevel]->rooms[r];Point start={-1,-1};int connected=0;
        for(int y=room->y+1;y<room->y+room->height-1;y++)for(int x=room->x+1;x<room->x+room->width-1;x++){
            connected|=route_seen[y][x];if(route_walk(x,y,(Point){-1,-1}))start=(Point){x,y};
        }
        if(connected||start.x<0)continue;
        memset(queued,0,sizeof(queued));int h=0,t=0;route_queue[t++]=start;queued[start.y][start.x]=1;Point end={-1,-1};
        int dx[]={1,-1,0,0},dy[]={0,0,1,-1};
        while(h<t&&end.x<0){Point p=route_queue[h++];for(int i=0;i<4;i++){
            int x=p.x+dx[i],y=p.y+dy[i];
            if(x<=BORDER_PADDING+1||y<=BORDER_PADDING||x>=maxx||y>=maxy||queued[y][x])continue;
            int c=mvwinch(stdscr,y,x)&A_CHARTEXT;if(c=='O'||c=='=')continue;
            int corner=0;for(int a=0;a<count;a++){Room *q=game->levels[game->currentLevel]->rooms[a];if((x==q->x||x==q->x+q->width-1)&&(y==q->y||y==q->y+q->height-1))corner=1;}
            if(corner)continue;
            parent[y][x]=p;queued[y][x]=1;route_queue[t++]=(Point){x,y};
            if(route_seen[y][x]){end=(Point){x,y};break;}
        }}
        if(end.x<0)continue;
        for(Point p=end;p.x!=start.x||p.y!=start.y;p=parent[p.y][p.x]){
            int c=mvwinch(stdscr,p.y,p.x)&A_CHARTEXT;
            if(c==' '||c==0)mvaddch(p.y,p.x,'#');
            else if(c=='|'||c=='-'||c=='_'){
                for(int a=0;a<count;a++){
                    Room *q=game->levels[game->currentLevel]->rooms[a];
                    if(p.x<q->x||p.x>=q->x+q->width||p.y<q->y||p.y>=q->y+q->height)continue;
                    Door *grown=realloc(q->doors,(q->doorCount+2)*sizeof(Door));if(!grown)break;
                    q->doors=grown;q->doors[q->doorCount++]=(Door){.cord=p,.type='n'};mvaddch(p.y,p.x,'+');break;
                }
            }
        }
    }
}
