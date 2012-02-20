#ifndef PRODUCT_H
#define PRODUCT_H

typedef struct product {
	int id;
	char *name;
	int category_id;
} Product;

int add_product(Product *p);
int add_to_category(Product *p);
int del_product(Product *p);
int update_product(Product *old_product, Product *new_product);
int get_products_count();
int get_all_products(Product *list[]);
int get_product_by_id(Product *p);
int get_product_by_name(Product *p);
int get_products_in_category(int category_id, Product *list[]);
#endif
