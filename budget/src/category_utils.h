#ifndef CATEGORY_UTILS_H
#define CATEGORY_UTILS_H
#include "category.h"
int _update_category(char *old_name, char *new_name);
int _add_category(char *name);
int _del_category(char *name);
int __list_categories();
int manage_category(int argc, char *argv[]);
void free_category_list(Category *list[], int size);
#endif
