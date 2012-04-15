#ifndef PRODUCT_UTILS_H
#define PRODUCT_UTILS_H
#include "product.h"
int _add_product(char *name, int category_id);
int _update_product(char *old_name, char *new_name, int category_id);
int _del_product(char *name);
int __list_products();
void free_product_list(Product *list[], int size);
int get_products_count(int category_id);
int manage_product(int argc, char *argv[]);
#endif
