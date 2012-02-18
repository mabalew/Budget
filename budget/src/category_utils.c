#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "category.h"
#include "category_utils.h"

void free_category_list(Category *list[], int size);

void __update_category(char *argv[]) {
	Category *co = (Category *)malloc(sizeof co);
	Category *cn = (Category *)malloc(sizeof cn);
	co->name = (char *)malloc(sizeof (char));
	cn->name = (char *)malloc(sizeof (char));
	co->name = argv[3];
	cn->name = argv[4];
	update_category(co, cn);
	free(co);
	free(cn);
}

void __add_category(char *argv[]) {
	Category *c = (Category*)malloc(sizeof c);
	c->name = argv[3];
	add_category(c);
	free(c);
}

void __del_category(char *argv[]) {
	Category *c = (Category *) malloc(sizeof c);
	c->name = (char *)malloc(sizeof(char));
	c->name = argv[3];
	del_category(c);
	free(c);
}

void __list_categories() {
	int categories_count = get_categories_count();
	int counter = 0;
	Category *list[categories_count];
	puts("==================================");
	printf("Znaleziono %d\n", categories_count);
	puts("==================================");
	get_all_categories(list);
	if (categories_count != 0) {
		for (counter = 0; counter < categories_count; counter++) {
			printf("\tid: %d\n", list[counter]->id);
			printf("\tnazwa: %s\n", list[counter]->name);
			puts("----------------------------------");
		}
	}
	puts("==================================");
	free_category_list(list, categories_count);
}

void free_category_list(Category *list[], int size) {
	int counter = 0;
	for (counter = 0; counter < size; counter++) {
		free(list[counter]->name);
		free(list[counter]);
	}
}

void manage_category(int argc, char *argv[]) {
	if (argc == 2) {
		__show_common_help();
	}
	if (is_update_string(argv[2])) {
		__update_category(argv);
	} else if (is_add_string(argv[2])) {
		__add_category(argv);
	} else if (is_delete_string(argv[2])) {
		__del_category(argv);
	} else if (is_list_string(argv[2])) {
		__list_categories();
	} else if (is_help_for_category_module(argv[2])) {
		__show_help_for_category_module();
	} else {
		__show_common_help();
	}
}
