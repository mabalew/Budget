#ifndef SHOP_UTILS_H
#define SHOP_UTILS_H
void free_shop_list(Shop *list[], int size);
void __show_help_for_shop_module();
int _update_shop(char *old_name, char *new_name);
int _add_shop(char *name);
int _del_shop(char *name);
int __list_shops();
int manage_shop(int argc, char *argv[]);
#endif
