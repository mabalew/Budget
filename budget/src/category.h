#ifndef CATEGORY_H
#define CATEGORY_H

typedef struct category {
	int id;
	char *name;
} Category;

int add_category(Category *c);
int del_category(Category *c);
int update_category(Category *old_category, Category *new_category);
int get_categories_count();
int get_all_categories(Category *list[]);
int get_category_by_id(Category *c);
int get_category_by_name(Category *c);

#endif
