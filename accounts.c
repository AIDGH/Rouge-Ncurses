#include "accounts.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#ifdef __APPLE__
#include <CommonCrypto/CommonKeyDerivation.h>
#include <CommonCrypto/CommonDigest.h>
#else
#include <openssl/evp.h>
#include <openssl/sha.h>
#endif

static sqlite3 *db;
static const char *session_file=".local-data/session";
static int exec_sql(const char *s){return sqlite3_exec(db,s,NULL,NULL,NULL)==SQLITE_OK;}
static int random_bytes(unsigned char *p,size_t n){
    int fd=open("/dev/urandom",O_RDONLY);if(fd<0)return 0;
    size_t done=0;while(done<n){ssize_t k=read(fd,p+done,n-done);if(k<=0){close(fd);return 0;}done+=k;}close(fd);return 1;
}
static int derive(const char *password,const unsigned char *salt,unsigned char *hash){
#ifdef __APPLE__
    return CCKeyDerivationPBKDF(kCCPBKDF2,password,strlen(password),salt,16,kCCPRFHmacAlgSHA256,210000,hash,32)==0;
#else
    return PKCS5_PBKDF2_HMAC(password,(int)strlen(password),salt,16,210000,EVP_sha256(),32,hash)==1;
#endif
}
static void token_hash(const unsigned char *token,unsigned char *hash){
#ifdef __APPLE__
    CC_SHA256(token,32,hash);
#else
    SHA256(token,32,hash);
#endif
}
int account_valid_name(const char *s){
    size_t n=strlen(s);if(n<1||n>40||!strcmp(s,"guest")||!strcmp(s,"You"))return 0;
    for(size_t i=0;i<n;i++)if(!isalnum((unsigned char)s[i])&&s[i]!='_'&&s[i]!='-')return 0;
    return 1;
}
static int insert_account(const char *name,const char *password,const char *email){
    unsigned char salt[16],hash[32];if(!random_bytes(salt,16)||!derive(password,salt,hash))return 0;
    sqlite3_stmt *st=NULL;
    if(sqlite3_prepare_v2(db,"INSERT INTO accounts(username,email,salt,password_hash) VALUES(?,?,?,?)",-1,&st,NULL)!=SQLITE_OK)return 0;
    sqlite3_bind_text(st,1,name,-1,SQLITE_TRANSIENT);sqlite3_bind_text(st,2,email,-1,SQLITE_TRANSIENT);
    sqlite3_bind_blob(st,3,salt,16,SQLITE_TRANSIENT);sqlite3_bind_blob(st,4,hash,32,SQLITE_TRANSIENT);
    int ok=sqlite3_step(st)==SQLITE_DONE;sqlite3_finalize(st);memset(hash,0,sizeof(hash));return ok;
}
int account_exists(const char *name){
    if(!db)return 0;sqlite3_stmt *st=NULL;
    if(sqlite3_prepare_v2(db,"SELECT 1 FROM accounts WHERE username=?",-1,&st,NULL)!=SQLITE_OK)return 0;
    sqlite3_bind_text(st,1,name,-1,SQLITE_TRANSIENT);int found=sqlite3_step(st)==SQLITE_ROW;sqlite3_finalize(st);return found;
}
static void migrate_legacy(void){
    FILE *f=fopen("users.txt","r");if(!f)return;
    char line[512],name[50]="",email[50]="",password[256]="";int ok=exec_sql("BEGIN IMMEDIATE"),count=0;
    while(ok&&fgets(line,sizeof(line),f)){
        line[strcspn(line,"\r\n")]=0;
        if(!strncmp(line,"Username: ",10)){if(*name){ok=0;break;}snprintf(name,sizeof(name),"%s",line+10);}
        else if(!strncmp(line,"Email: ",7))snprintf(email,sizeof(email),"%s",line+7);
        else if(!strncmp(line,"Password: ",10)){
            snprintf(password,sizeof(password),"%s",line+10);
            if(!account_valid_name(name)||!*password){ok=0;break;}
            if(!account_exists(name))ok=insert_account(name,password,email);
            memset(password,0,sizeof(password));name[0]=email[0]=0;count++;
        }
    }
    if(ferror(f)||*name)ok=0;fclose(f);
    if(!ok){exec_sql("ROLLBACK");return;}
    if(!exec_sql("COMMIT"))return;
    if(count){
        char archive[128];snprintf(archive,sizeof(archive),".local-data/legacy-users-%ld.txt",(long)time(NULL));
        if(access(archive,F_OK)!=0 && rename("users.txt",archive)==0){
            chmod(archive,0600);FILE *empty=fopen("users.txt","w");if(empty)fclose(empty);
        }
    }
}
int accounts_init(void){
    if(db)return 1;
    mkdir(".local-data",0700);chmod(".local-data",0700);
    if(sqlite3_open(".local-data/accounts.sqlite3",&db)!=SQLITE_OK){accounts_close();return 0;}
    chmod(".local-data/accounts.sqlite3",0600);sqlite3_busy_timeout(db,3000);
    if(!exec_sql("PRAGMA foreign_keys=ON; CREATE TABLE IF NOT EXISTS accounts(username TEXT PRIMARY KEY,email TEXT NOT NULL DEFAULT '',salt BLOB NOT NULL,password_hash BLOB NOT NULL); CREATE TABLE IF NOT EXISTS session(id INTEGER PRIMARY KEY CHECK(id=1),username TEXT NOT NULL REFERENCES accounts(username),token_hash BLOB NOT NULL); PRAGMA user_version=1;")){accounts_close();return 0;}
    migrate_legacy();return 1;
}
void accounts_close(void){if(db)sqlite3_close(db);db=NULL;}
int account_register(const char *name,const char *password,const char *email){
    if(!db||!account_valid_name(name)||strlen(password)<6||strlen(password)>128||strlen(email)>49)return 0;
    return insert_account(name,password,email);
}
int account_verify(const char *name,const char *password){
    if(!db)return 0;sqlite3_stmt *st=NULL;int ok=0;unsigned char got[32];
    if(sqlite3_prepare_v2(db,"SELECT salt,password_hash FROM accounts WHERE username=?",-1,&st,NULL)!=SQLITE_OK)return 0;
    sqlite3_bind_text(st,1,name,-1,SQLITE_TRANSIENT);
    if(sqlite3_step(st)==SQLITE_ROW && sqlite3_column_bytes(st,0)==16 && sqlite3_column_bytes(st,1)==32 && derive(password,sqlite3_column_blob(st,0),got)){
        const unsigned char *expected=sqlite3_column_blob(st,1);unsigned diff=0;for(int i=0;i<32;i++)diff|=got[i]^expected[i];ok=diff==0;
    }
    memset(got,0,sizeof(got));sqlite3_finalize(st);return ok;
}
int account_email(const char *name,char *email,size_t n){
    if(!db)return 0;sqlite3_stmt *st=NULL;int ok=0;email[0]=0;
    if(sqlite3_prepare_v2(db,"SELECT email FROM accounts WHERE username=?",-1,&st,NULL)!=SQLITE_OK)return 0;
    sqlite3_bind_text(st,1,name,-1,SQLITE_TRANSIENT);if(sqlite3_step(st)==SQLITE_ROW){snprintf(email,n,"%s",sqlite3_column_text(st,0));ok=1;}sqlite3_finalize(st);return ok;
}
int account_remember(const char *name){
    if(!db||!account_exists(name))return 0;
    unsigned char token[32],hash[32];if(!random_bytes(token,32))return 0;token_hash(token,hash);
    sqlite3_stmt *st=NULL;if(sqlite3_prepare_v2(db,"INSERT OR REPLACE INTO session VALUES(1,?,?)",-1,&st,NULL)!=SQLITE_OK)return 0;
    sqlite3_bind_text(st,1,name,-1,SQLITE_TRANSIENT);sqlite3_bind_blob(st,2,hash,32,SQLITE_TRANSIENT);
    int ok=sqlite3_step(st)==SQLITE_DONE;sqlite3_finalize(st);if(!ok)return 0;
    int fd=open(".local-data/session.tmp",O_WRONLY|O_CREAT|O_TRUNC,0600);if(fd<0)return 0;
    ok=write(fd,token,32)==32;close(fd);memset(token,0,sizeof(token));
    if(!ok){unlink(".local-data/session.tmp");return 0;}
    return rename(".local-data/session.tmp",session_file)==0;
}
int account_restore(char *name,size_t n){
    name[0]=0;if(!db)return 0;unsigned char token[33],hash[32];
    int fd=open(session_file,O_RDONLY);if(fd<0)return 0;ssize_t got=read(fd,token,sizeof(token));close(fd);if(got!=32)return 0;
    token_hash(token,hash);memset(token,0,sizeof(token));sqlite3_stmt *st=NULL;
    if(sqlite3_prepare_v2(db,"SELECT username FROM session WHERE id=1 AND token_hash=?",-1,&st,NULL)!=SQLITE_OK)return 0;
    sqlite3_bind_blob(st,1,hash,32,SQLITE_TRANSIENT);int ok=sqlite3_step(st)==SQLITE_ROW;
    if(ok)snprintf(name,n,"%s",sqlite3_column_text(st,0));sqlite3_finalize(st);return ok;
}
void account_logout(void){if(db)exec_sql("DELETE FROM session");unlink(session_file);}
