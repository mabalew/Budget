#ifndef USER_H
#define USER_H

#include <stdbool.h>

typedef struct user {
	int  id;
	char *nick;
	char *password;
} User;
bool login(struct user *p);
int get_all_users(User *list[]);
int get_user_by_id(User *p);
int get_user_by_login(User *p);
int add_user(User *p);
int del_user(User *p);
int update_user(User *old_user, User *new_user);
int get_user_count();

#endif
