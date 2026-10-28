#include "music.h"
#include "accounts.h"
#include "user.h"
#include "ui_theme.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
extern char our_user[50],which_user[50],which_menu[50];
extern User current_user;

static void accept_player(const char *name){
    User u={0};if(!load_user_info(name,&u)){
        u.health=100;u.food=100;u.weaponsCount=1;u.weaponsCount_each[0]=1;
    }
    snprintf(u.username,sizeof(u.username),"%s",name);account_email(name,u.email,sizeof(u.email));
    current_user=u;snprintf(our_user,50,"%s",name);strcpy(which_user,"user");strcpy(which_menu,"Before game menu");
}
void account_restore_player(void){char name[50];if(account_restore(name,sizeof(name)))accept_player(name);}
void account_logout_player(void){account_logout();memset(&current_user,0,sizeof(current_user));strcpy(our_user,"guest");strcpy(which_user,"guest");strcpy(which_menu,"menu");}

static int input(WINDOW *win,int row,const char *label,char *out,size_t size,int secret){
    int n=0;out[0]=0;keypad(win,TRUE);curs_set(1);
    for(;;){
        mvwprintw(win,row,3,"%-12s",label);mvwhline(win,row,16,' ',36);
        int first=n>35?n-35:0;
        for(int i=first;i<n;i++)mvwaddch(win,row,16+i-first,secret?'*':out[i]);
        wmove(win,row,16+n-first);wrefresh(win);int ch=music_getch(win);
        if(ch==27){out[0]=0;curs_set(0);return 0;}
        if(ch=='\n'||ch==KEY_ENTER){out[n]=0;curs_set(0);return 1;}
        if(ch==KEY_BACKSPACE||ch==127||ch==8){if(n)n--;}
        else if(ch>=32&&ch<127&&(size_t)n<size-1)out[n++]=(char)ch;
        out[n]=0;
    }
}
static void form(int signup){
    strcpy(which_menu,"menu");
    WINDOW *win=newwin(16,62,(LINES-16)/2,(COLS-62)/2);if(!win)return;
    if (!accounts_init()) {
        ui_frame(win,"ACCOUNT STORAGE UNAVAILABLE");
        mvwaddstr(win,5,3,"Cannot open local account data. Guest play still works.");
        mvwaddstr(win,7,3,"Press any key to return.");
        wrefresh(win);music_getch(win);delwin(win);return;
    }
    char name[50]="",password[129]="";
    for(;;){
        werase(win);ui_frame(win,signup?"CREATE A HERO":"WELCOME BACK");
        mvwaddstr(win,2,3,signup?"Choose a name and password. No email is required.":"Sign in once; this computer will remember you.");
        mvwaddstr(win,12,3,"Enter: next / submit     Esc: cancel");
        mvwaddstr(win,13,3,"Log out later with L on the main menu.");
        if(!input(win,5,"Username",name,sizeof(name),0))break;
        if(!account_valid_name(name)){
            mvwaddstr(win,9,3,"Use letters, numbers, - or _. Avoid guest and You.");wrefresh(win);music_getch(win);continue;
        }
        if(!input(win,7,"Password",password,sizeof(password),1))break;
        int ok=signup?account_register(name,password,""):account_verify(name,password);
        memset(password,0,sizeof(password));
        if(ok){
            if (!account_remember(name)) {
                mvwaddstr(win,9,3,"Signed in, but could not remember this login.");
                mvwaddstr(win,10,3,"Press any key to continue for this session.");
                wrefresh(win);music_getch(win);
            }
            accept_player(name);break;
        }
        mvwaddstr(win,9,3,signup?"Name already exists, or password is under 6 characters.":"Username or password did not match. Please try again.");
        mvwaddstr(win,10,3,"Press any key to retry.");wrefresh(win);music_getch(win);
    }
    memset(password,0,sizeof(password));delwin(win);clear();refresh();
}
void account_login_form(void){form(0);}
void account_register_form(void){form(1);}
