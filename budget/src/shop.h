#ifndef SHOP_H
#define SHOP_H

typedef struct shop {
	int id;
	char *name;
} Shop;

int add_shop(Shop *s);
int del_shop(Shop *s);
int update_shop(Shop *old_category, Shop *new_category);
int get_shops_count();
int get_all_shops(Shop *list[]);
int get_shop_by_id(Shop *s);
int get_shop_by_name(Shop *s);

#endif
