#ifndef PRODUCT_UTILS_H
#define PRODUCT_UTILS_H
#include "product.h"
int __update_product(int argc, char *argv[]);
int __add_product(int argc, char *argv[]);
int __del_product(int argc, char *argv[]);
int __list_products_in_category(int argc, char *argv[]);
int __list_products();
void free_product_list(Product *list[], int size);
int get_products_count(int category_id);
int manage_product(int argc, char *argv[]);
#endif
