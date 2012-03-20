#ifndef CATEGORY_UTILS_H
#define CATEGORY_UTILS_H
#include "category.h"
int __update_category(int argc, char *argv[]);
int __add_category(int argc, char *argv[]);
int __del_category(int argc, char *argv[]);
int __list_categories();
int manage_category(int argc, char *argv[]);
void free_category_list(Category *list[], int size);
#endif
