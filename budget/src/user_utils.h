#ifndef USER_UTILS_H
#define USER_UTILS_H
void __add_user(char *argv[]);
void __update_user(char *argv[]);
void __del_user(char *argv[]);
void __login(char *argv[]);
void __list_users();
void __get_user_by_id(char *argv[]);
void __get_user_by_login(char *argv[]);
void manage_user(int argc, char *argv[]);
#endif
