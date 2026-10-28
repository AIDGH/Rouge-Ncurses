#include "../accounts.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sqlite3.h>
int main(void){
    FILE *f=fopen("users.txt","w");assert(f);
    fputs("Username: legacy_hero\nEmail: hero@example.test\nPassword: Example-secret\n\n",f);fclose(f);
    assert(accounts_init());assert(account_exists("legacy_hero"));assert(!account_exists("legacy"));
    assert(account_verify("legacy_hero","Example-secret"));assert(!account_verify("legacy_hero","wrong"));
    assert(account_register("new_hero","Another-secret",""));assert(!account_register("new_hero","Another-secret",""));
    assert(!account_register("../bad","Example-secret",""));assert(!account_register("guest","Example-secret",""));
    assert(account_remember("new_hero"));accounts_close();
    char name[50],email[50];assert(accounts_init());assert(account_restore(name,sizeof(name)));assert(!strcmp(name,"new_hero"));
    assert(account_email("legacy_hero",email,sizeof(email)));assert(!strcmp(email,"hero@example.test"));
    struct stat st;assert(stat(".local-data/session",&st)==0 && (st.st_mode&0777)==0600);
    f=fopen(".local-data/session","r+");assert(f);fputc(0,f);fclose(f);
    /* A forged token is rejected (flip rather than assume a nonzero first byte). */
    assert(account_remember("new_hero"));f=fopen(".local-data/session","r+");int c=fgetc(f);rewind(f);fputc(c^0xff,f);fclose(f);assert(!account_restore(name,sizeof(name)));
    assert(account_remember("legacy_hero"));account_logout();accounts_close();assert(accounts_init());assert(!account_restore(name,sizeof(name)));
    sqlite3 *db;sqlite3_stmt *q;assert(sqlite3_open(".local-data/accounts.sqlite3",&db)==SQLITE_OK);
    assert(sqlite3_prepare_v2(db,"SELECT length(salt),length(password_hash) FROM accounts",-1,&q,0)==SQLITE_OK);
    int n=0;while(sqlite3_step(q)==SQLITE_ROW){assert(sqlite3_column_int(q,0)==16);assert(sqlite3_column_int(q,1)==32);n++;}assert(n==2);
    sqlite3_finalize(q);sqlite3_close(db);accounts_close();
    puts("PASS legacy migration, exact usernames, salted password verification, duplicate/path rejection, persistent login, tamper rejection and logout");
}
