#include "music.h"
#include "ui_theme.h"
#include <string.h>
#define COLOR_KAKA 5
#define COLOR_GREEN1 30
#define COLOR_YELLOW1 31
#define COLOR_SILVER1 32
#define COLOR_BRONZE1 33
#define COLOR_GOLD1 34
#define COLOR_ORANGE1 35
#define COLOR_SECRET 36
#define COLOR_Bisque 37
#define COLOR_PURPLE1 38
#define COLOR_ZERESHKI 39
#define COLOR_BLUE1 40
#define COLOR_SILVER2 41
#define COLOR_TREASURE 42
int ui_old_visuals=0;
static void old_palette(void) {
    init_color(COLOR_CYAN, 700, 1000, 1000);
    init_color(COLOR_KAKA, 1000, 600, 600);
    init_color(COLOR_RED, 1000, 0, 0);
    init_color(COLOR_GREEN1, 0, 1000, 0);
    init_color(COLOR_YELLOW1, 1000, 1000, 200);
    init_color(COLOR_SILVER1, 753, 753, 753);
    init_color(COLOR_BRONZE1, 801, 496, 195);
    init_color(COLOR_GOLD1, 1000, 840, 0);
    init_color(COLOR_ORANGE1, 1000, 647, 0);
    init_color(COLOR_SECRET, 118, 1000, 784);
    init_color(COLOR_Bisque, 1000, 894, 769);
    init_color(COLOR_PURPLE1, 655, 0, 843);
    init_color(COLOR_ZERESHKI, 530, 0, 78);
    init_color(COLOR_BLUE1, 0, 0, 1000);
    init_color(COLOR_SILVER2, 550, 550, 550);
    init_color(COLOR_TREASURE, 224 * 4, 217 * 4, 128 * 4);
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_KAKA, COLOR_BLACK);
    init_pair(6, COLOR_RED, COLOR_BLACK);
    init_pair(7, COLOR_GREEN1, COLOR_BLACK);
    init_pair(8, COLOR_YELLOW1, COLOR_BLACK);
    init_pair(9, COLOR_BLUE, COLOR_BLACK);
    init_pair(10, COLOR_SILVER1, COLOR_BLACK);
    init_pair(11, COLOR_BRONZE1, COLOR_BLACK);
    init_pair(12, COLOR_GOLD1, COLOR_BLACK);
    init_pair(13, COLOR_WHITE, COLOR_BLACK);
    init_pair(14, COLOR_ORANGE1, COLOR_BLACK);
    init_pair(15, COLOR_SECRET, COLOR_BLACK);
    init_pair(16, COLOR_Bisque, COLOR_BLACK);
    init_pair(17, COLOR_PURPLE1, COLOR_BLACK);
    init_pair(18, COLOR_ZERESHKI, COLOR_BLACK);
    init_pair(19, COLOR_BLUE1, COLOR_BLACK);
    init_pair(20, COLOR_SILVER2, COLOR_BLACK);
    init_pair(21, 247, COLOR_BLACK);
    init_pair(22, COLOR_TREASURE, COLOR_BLACK);
    init_pair(23, 43, COLOR_BLACK);
    init_pair(24, 44, COLOR_BLACK);
    init_pair(25, 45, COLOR_BLACK);
    init_pair(26, 46, COLOR_BLACK);
    init_pair(27, 47, COLOR_BLACK);
    init_pair(28, 48, COLOR_BLACK);
    init_pair(29, 49, COLOR_BLACK);
    init_pair(30, 50, COLOR_BLACK);
    init_pair(31, 51, COLOR_BLACK);
    init_pair(32, 52, COLOR_BLACK);
    init_pair(33, 53, COLOR_BLACK);
    init_pair(34, 54, COLOR_BLACK);
    init_pair(35, 55, COLOR_BLACK);
    init_pair(36, 56, COLOR_BLACK);
    init_pair(37, 57, COLOR_BLACK);
    init_pair(38, 58, COLOR_BLACK);
    init_pair(39, 59, COLOR_BLACK);
    init_pair(40, 60, COLOR_BLACK);
    init_pair(41, 61, COLOR_BLACK);
    init_pair(42, 62, COLOR_BLACK);
    init_pair(43, 63, COLOR_BLACK);
    init_pair(44, 64, COLOR_BLACK);
    init_pair(45, 65, COLOR_BLACK);
    init_pair(46, 66, COLOR_BLACK);
    init_pair(47, 67, COLOR_BLACK);
    init_pair(48, 68, COLOR_BLACK);
    init_pair(49, 69, COLOR_BLACK);
    init_pair(50, 70, COLOR_BLACK);
    init_pair(124, 124, COLOR_BLACK);
    init_pair(117, 117, COLOR_BLACK);
    init_pair(172, 172, COLOR_BLACK);
    init_pair(208, 208, COLOR_BLACK);
    init_pair(222, 222, COLOR_BLACK);
    init_pair(223, 223, COLOR_BLACK);
}

void ui_old_backdrop(void) {
 int width=45;
    int y_kaka = 15;
    attron(COLOR_PAIR(4));
    box(stdscr, '|', '-');
    attroff(COLOR_PAIR(4));
    attron(COLOR_PAIR(6));
    mvprintw(LINES / 2 - 24 + y_kaka, (COLS - width) / 2 - 20, "          __     ");
    mvprintw(LINES / 2 - 23 + y_kaka, (COLS - width) / 2 - 20, "      |__|__     ");
    mvprintw(LINES / 2 - 22 + y_kaka, (COLS - width) / 2 - 20, "       __|  |     ");
    attroff(COLOR_PAIR(6));
    attron(COLOR_PAIR(5));
    mvprintw(LINES / 2 - 21 + y_kaka, (COLS - width) / 2 - 20, "      ___|__     ");
    mvprintw(LINES / 2 - 20 + y_kaka, (COLS - width) / 2 - 20, "     /      \\   ");
    mvprintw(LINES / 2 - 19 + y_kaka, (COLS - width) / 2 - 20, "    /  _  _  \\ ");
    mvprintw(LINES / 2 - 18 + y_kaka, (COLS - width) / 2 - 20, "   |          |   ");
    mvprintw(LINES / 2 - 17 + y_kaka, (COLS - width) / 2 - 20, "   |     |    |   ");
    mvprintw(LINES / 2 - 16 + y_kaka, (COLS - width) / 2 - 20, "   |  ______  |   ");
    mvprintw(LINES / 2 - 15 + y_kaka, (COLS - width) / 2 - 20, "  |   |____|  |  ");
    mvprintw(LINES / 2 - 14 + y_kaka, (COLS - width) / 2 - 20, " |            | ");
    mvprintw(LINES / 2 - 13 + y_kaka, (COLS - width) / 2 - 20, " \\           /  ");
    mvprintw(LINES / 2 - 12 + y_kaka, (COLS - width) / 2 - 20, "  \\ ______ /  ");
    attroff(COLOR_PAIR(5));
    attron(COLOR_PAIR(6));
    mvprintw(LINES / 2 - 22 + y_kaka, (COLS - width) / 2 + 45, "       ___      ");
    mvprintw(LINES / 2 - 21 + y_kaka, (COLS - width) / 2 + 45, "      _|*|__     ");
    attroff(COLOR_PAIR(6));
    attron(COLOR_PAIR(5));
    mvprintw(LINES / 2 - 20 + y_kaka, (COLS - width) / 2 + 45, "     /      \\   ");
    mvprintw(LINES / 2 - 19 + y_kaka, (COLS - width) / 2 + 45, "    /  _  _  \\ ");
    mvprintw(LINES / 2 - 18 + y_kaka, (COLS - width) / 2 + 45, "   |  |_||_|  |   ");
    mvprintw(LINES / 2 - 17 + y_kaka, (COLS - width) / 2 + 45, "   |    _|    |   ");
    mvprintw(LINES / 2 - 16 + y_kaka, (COLS - width) / 2 + 45, "   |  ______  |   ");
    mvprintw(LINES / 2 - 15 + y_kaka, (COLS - width) / 2 + 45, "   |    |+|    |  ");
    mvprintw(LINES / 2 - 14 + y_kaka, (COLS - width) / 2 + 45, "   |            | ");
    mvprintw(LINES / 2 - 13 + y_kaka, (COLS - width) / 2 + 45, "    \\           /  ");
    mvprintw(LINES / 2 - 12 + y_kaka, (COLS - width) / 2 + 45, "      \\ ______ /  ");
    attroff(COLOR_PAIR(5));
    mvprintw(LINES / 2 - 21, (COLS - width) / 2 + 6, "_____________________________");
    mvprintw(LINES / 2 - 20, (COLS - width) / 2 + 6, "|                           |");
    mvprintw(LINES / 2 - 19, (COLS - width) / 2 + 6, "|  Welcome to AID Rouge =)  |");
    mvprintw(LINES / 2 - 18, (COLS - width) / 2 + 6, "|🗿  🗿  🗿   🗿  🗿  🗿  🗿|");
    mvprintw(LINES / 2 - 17, (COLS - width) / 2 - 14, "____________________|  🗿  🗿  🗿   🗿  🗿  🗿  |______________________");
    mvprintw(LINES / 2 - 16, (COLS - width) / 2 - 14, "|_____________________________________________________________________|");
    mvprintw(LINES / 2 - 15, (COLS - width) / 2 - 14, "|                                                                     |");
    mvprintw(LINES / 2 - 14, (COLS - width) / 2 - 14, "|  Use arrow keys to go up and down, press enter to select a choice!  |");
    mvprintw(LINES / 2 - 13, (COLS - width) / 2 - 14, "|_____________________________________________________________________|");
}


/* Shared semantic palette; do not redefine the terminal's system colours. */
void ui_theme(void) {
    if(ui_old_visuals){old_palette();return;}
    static const short colors[] = {0, 179, 231, 116, 116, 174, 203, 114,
        186, 75, 250, 137, 222, 252, 209, 115, 180, 141, 168, 81, 243, 246, 222};
    for (short i=1;i<=22;i++)
        init_pair(i, COLORS>=256 ? colors[i] : (short)(i==6?COLOR_RED:i==7?COLOR_GREEN:i==3?COLOR_CYAN:COLOR_WHITE),
                  i==2 ? (COLORS>=256?24:COLOR_BLUE) : COLOR_BLACK);
    for(short i=23;i<=50;i++) init_pair(i,COLORS>=256?(i==25?117:109):COLOR_CYAN,COLOR_BLACK);
    const short extras[]={117,124,172,208,222,223};
    for(unsigned i=0;i<sizeof(extras)/sizeof(extras[0]);i++)
        if(extras[i]<COLOR_PAIRS) init_pair(extras[i],COLORS>=256?extras[i]:COLOR_YELLOW,COLOR_BLACK);
}

void ui_frame(WINDOW *win, const char *title) {
    wattron(win,COLOR_PAIR(3));box(win,0,0);
    if(title && *title) {
        int w=getmaxx(win), n=(int)strlen(title);
        if(n+4<w) {wattron(win,A_BOLD);mvwprintw(win,0,(w-n-4)/2,"[ %s ]",title);wattroff(win,A_BOLD);}
    }
    wattroff(win,COLOR_PAIR(3));
}

void ui_menu_backdrop(void) {
    if(ui_old_visuals){ui_old_backdrop();return;}
    static const char *art[]={
        "                  .                  ",
        "                 /\\                 ",
        "      /\\        /  \\        /\\      ",
        "     /__\\    __/____\\__    /__\\     ",
        "     |[]|====|  .--.  |====|[]|     ",
        "     |  |    | /    \\ |    |  |     ",
        "  ___|__|____|_|____|_|____|__|___  "
    };
    int top=LINES/2-18;
    wattron(stdscr,COLOR_PAIR(3));
    for(int i=0;i<7;i++) mvaddstr(top+i,(COLS-(int)strlen(art[i]))/2,art[i]);
    wattroff(stdscr,COLOR_PAIR(3));
    wattron(stdscr,COLOR_PAIR(12)|A_BOLD);
    const char *title="R O U G E   D U N G E O N";
    mvaddstr(top+9,(COLS-(int)strlen(title))/2,title);
    wattroff(stdscr,COLOR_PAIR(12)|A_BOLD);
    const char *tag="A lantern. A labyrinth. One more floor.";
    wattron(stdscr,COLOR_PAIR(10));mvaddstr(top+11,(COLS-(int)strlen(tag))/2,tag);
    const char *hint="Arrows: choose    Enter: open    Mouse: click    Esc: leave";
    mvaddstr(LINES/2+10,(COLS-(int)strlen(hint))/2,hint);wattroff(stdscr,COLOR_PAIR(10));
}

int ui_choose_visuals(void) {
    int previous=ui_old_visuals,selected=ui_old_visuals?0:1;
    keypad(stdscr,TRUE);mousemask(ALL_MOUSE_EVENTS|REPORT_MOUSE_POSITION,NULL);
    for(;;){
        ui_old_visuals=selected==0;ui_theme();erase();ui_menu_backdrop();
        int y=LINES/2+7,x=COLS/2-28;
        wattron(stdscr,COLOR_PAIR(12)|A_BOLD);mvaddstr(y-2,x,"CHOOSE YOUR VISUALS  /  live preview");wattroff(stdscr,COLOR_PAIR(12)|A_BOLD);
        for(int i=0;i<2;i++){
            wattron(stdscr,COLOR_PAIR(selected==i?2:10)|A_BOLD);
            mvprintw(y,x+i*30,"  %s  ",i==0?"ORIGIN":"NEW / LANTERN");
            wattroff(stdscr,COLOR_PAIR(selected==i?2:10)|A_BOLD);
        }
        mvaddstr(y+2,x,"Arrows or hover: preview   Enter / click: choose");
        mvaddstr(y+3,x,"Same gameplay and fixes. V in the menu: switch again.");
        refresh();int ch=music_getch(stdscr);
        if(ch==KEY_LEFT||ch==KEY_RIGHT||ch==KEY_UP||ch==KEY_DOWN||ch=='\t')selected=1-selected;
        else if(ch=='\n'||ch==KEY_ENTER)break;
        else if(ch==27){ui_old_visuals=previous;ui_theme();return 0;}
        else if(ch==KEY_MOUSE){MEVENT m;if(getmouse(&m)==OK && m.y==y){
            for(int i=0;i<2;i++)if(m.x>=x+i*30 && m.x<x+i*30+20){selected=i;
                if(m.bstate&(BUTTON1_CLICKED|BUTTON1_PRESSED)){ui_old_visuals=selected==0;ui_theme();return 1;}
            }
        }}
    }
    ui_old_visuals=selected==0;ui_theme();return 1;
}

/* Use terminal line-drawing glyphs, which reach the edge of each cell. */
void ui_connected_art(WINDOW *win) {
    if(ui_old_visuals)return;
    int h=getmaxy(win),w=getmaxx(win);
    for(int y=0;y<h;y++)for(int x=0;x<w;x++){
        chtype c=mvwinch(win,y,x),a=c&A_ATTRIBUTES,ch=c&A_CHARTEXT;
        if(c&A_ALTCHARSET)continue;
        if(ch=='|')mvwaddch(win,y,x,ACS_VLINE|a);
        else if(ch=='_')mvwaddch(win,y,x,ACS_HLINE|a);
    }
}

void ui_settings_emblem(WINDOW *win) {
    const char *lines[]={
        "    ╭──────┬──────╮    ",
        "╭───┤      │      ├───╮",
        "│   │  ╭───┴───╮  │   │",
        "│   ├──┤   ◉   ├──┤   │",
        "│   │  ╰───┬───╯  │   │",
        "╰───┤      │      ├───╯",
        "    ╰──────┴──────╯    "};
    wattron(win,COLOR_PAIR(3));
    for(int i=0;i<7;i++)mvwaddstr(win,i+1,(getmaxx(win)-23)/2,lines[i]);
    wattroff(win,COLOR_PAIR(3));
}

/* One connected stonework border, replacing the disconnected comb ornaments. */
void ui_menu_ornament(WINDOW *win, int top, int bottom) {
    if (ui_old_visuals) return;
    int right=getmaxx(win)-4;
    attr_t attrs; short pair; wattr_get(win,&attrs,&pair,NULL);
    wattr_set(win,A_NORMAL,3,NULL);
    for(int y=top;y<top+3;y++)mvwhline(win,y,2,' ',right-1);
    for(int y=bottom;y<bottom+4;y++)mvwhline(win,y,2,' ',right-1);
    for(int y=top+2;y<=bottom+1;y++)mvwaddch(win,y,right,ACS_VLINE);
    for(int y=top+3;y<bottom;y++)
        if((mvwinch(win,y,3)&A_CHARTEXT)!='*')mvwaddch(win,y,3,ACS_VLINE);
    mvwhline(win,top+1,3,ACS_HLINE,right-3);
    mvwaddch(win,top+1,3,ACS_ULCORNER);mvwaddch(win,top+1,right,ACS_URCORNER);
    mvwhline(win,top+2,3,ACS_HLINE,right-3);
    mvwaddch(win,top+2,3,ACS_LTEE);mvwaddch(win,top+2,right,ACS_RTEE);
    for(int x=7;x<right-2;x+=8){
        mvwaddch(win,top,x,ACS_ULCORNER);mvwhline(win,top,x+1,ACS_HLINE,3);mvwaddch(win,top,x+4,ACS_URCORNER);
        mvwaddch(win,top+1,x,ACS_BTEE);mvwaddch(win,top+1,x+4,ACS_BTEE);
    }
    for(int y=bottom;y<=bottom+1;y++)mvwaddch(win,y,3,ACS_VLINE);
    mvwhline(win,bottom+1,4,ACS_HLINE,right-4);
    mvwaddch(win,bottom+1,3,ACS_LLCORNER);mvwaddch(win,bottom+1,right,ACS_LRCORNER);
    int mid=(right+3)/2;
    mvwaddch(win,bottom+1,mid,ACS_TTEE);
    mvwaddch(win,bottom+2,mid,ACS_VLINE);
    wattron(win,A_BOLD|COLOR_PAIR(12));mvwaddstr(win,bottom+3,mid-1,"◆◆◆");
    wattr_set(win,attrs,pair,NULL);
}
