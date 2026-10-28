#ifndef ACCOUNTS_H
#define ACCOUNTS_H
#include <stddef.h>
int accounts_init(void);
void accounts_close(void);
int account_exists(const char *name);
int account_register(const char *name,const char *password,const char *email);
int account_verify(const char *name,const char *password);
int account_remember(const char *name);
int account_restore(char *name,size_t size);
void account_logout(void);
int account_email(const char *name,char *email,size_t size);
int account_valid_name(const char *name);
void account_login_form(void);
void account_register_form(void);
void account_restore_player(void);
void account_logout_player(void);
#endif
