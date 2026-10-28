#include "music.h"
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "ui_theme.h"
extern char our_user[50],which_user[50];

char *display_menu(void) {
    static char result[50];
    const char *choices[]={" GAME MENU "," LOGIN "," SIGN UP "," SCOREBOARD "," SETTINGS "};
    const char *labels[]={"ENTER THE DUNGEON","CONTINUE AS A HERO","CREATE A HERO","HALL OF RECORDS","SETTINGS"};
    const int count=5,width=45,height=13;
    initscr();start_color();ui_theme();cbreak();noecho();curs_set(0);
    mousemask(ALL_MOUSE_EVENTS,NULL);mouseinterval(0);
    int selected=0,chosen=-1;
    while(chosen<0) {
        erase();ui_menu_backdrop();
        mvprintw(LINES-3,4,"%s: %s   |   V: Origin / New   |   L: Log out",!strcmp(which_user,"guest")?"Playing as":"Signed in",our_user);
        refresh();
        int y=(LINES-height)/2,x=(COLS-width)/2;
        WINDOW *win=newwin(height,width,y,x);
        if(!win){snprintf(result,sizeof(result),"exit");break;}
        keypad(win,TRUE);if(ui_old_visuals){wattron(win,COLOR_PAIR(1));wborder(win,'|','|','-','-','+','+','+','+');wattroff(win,COLOR_PAIR(1));}else ui_frame(win,"CAMPFIRE");
        for(int i=0;i<count;i++) {
            const char *label=ui_old_visuals?choices[i]:labels[i];
            int c=(width-(int)strlen(label))/2;
            wattron(win,COLOR_PAIR(i==selected?2:(ui_old_visuals?3:10))|(i==selected?A_BOLD:0));
            if(!ui_old_visuals)mvwprintw(win,2+2*i,3,"%s",i==selected?">":" ");
            mvwprintw(win,2+2*i,c,"%s",label);
            wattroff(win,COLOR_PAIR(i==selected?2:(ui_old_visuals?3:10))|(i==selected?A_BOLD:0));
        }
        wrefresh(win);int key=music_getch(win);delwin(win);
        if(key==KEY_UP)selected=(selected+count-1)%count;
        else if(key==KEY_DOWN)selected=(selected+1)%count;
        else if(key=='\n'||key==KEY_ENTER)chosen=selected;
        else if(key=='l'||key=='L'){snprintf(result,sizeof(result),"LOGOUT");break;}
        else if(key=='v'||key=='V'){ui_choose_visuals();}
        else if(key==27){snprintf(result,sizeof(result),"exit");break;}
        else if(key==KEY_MOUSE) {
            MEVENT m;
            if(getmouse(&m)==OK && m.x>x && m.x<x+width-1) {
                int row=m.y-y-2;
                if(row>=0 && row<2*count && row%2==0) {
                    selected=row/2;
                    if(m.bstate&(BUTTON1_CLICKED|BUTTON1_PRESSED))chosen=selected;
                }
            }
        }
    }
    if(chosen>=0)snprintf(result,sizeof(result),"%s",choices[chosen]);
    clear();refresh();return result;
}
