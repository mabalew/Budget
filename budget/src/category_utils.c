#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "category.h"
#include "category_utils.h"

void free_category_list(Category *list[], int size);
void verify_number_of_parameters(int argc, int should_be);

int __update_category(int argc, char *argv[]) {
	verify_number_of_parameters(argc, 5);
	Category *co = (Category *)malloc(sizeof co);
	Category *cn = (Category *)malloc(sizeof cn);
	co->name = (char *)malloc(sizeof (char));
	cn->name = (char *)malloc(sizeof (char));
	co->name = argv[3];
	cn->name = argv[4];
	int status = update_category(co, cn);
	free(co);
	free(cn);
	return status;
}

int __add_category(int argc, char *argv[]) {
	verify_number_of_parameters(argc, 4);
	Category *c = (Category*)malloc(sizeof c);
	c->name = argv[3];
	int status = add_category(c);
	free(c);
	return status;
}

int __del_category(int argc, char *argv[]) {
	verify_number_of_parameters(argc, 4);
	Category *c = (Category *) malloc(sizeof c);
	c->name = (char *)malloc(sizeof(char));
	c->name = argv[3];
	int status = del_category(c);
	free(c);
	return status;
}

int __list_categories() {
	int categories_count = get_categories_count();
	int counter = 0;
	Category *list[categories_count];
	puts("==================================");
	printf("Znaleziono %d\n", categories_count);
	puts("==================================");
	int status = get_all_categories(list);
	if (categories_count != 0 && NULL != list[0]) {
		printf("id  | nazwa\n");
		printf("----+-----------------------------\n");
		for (counter = 0; counter < categories_count; counter++) {
			printf("%.3d | %s\n", list[counter]->id, list[counter]->name);
		}
	}
	puts("==================================");
	free_category_list(list, categories_count);
	return status;
}

void free_category_list(Category *list[], int size) {
	int counter = 0;
	for (counter = 0; counter < size; counter++) {
		free(list[counter]->name);
		free(list[counter]);
	}
}

int manage_category(int argc, char *argv[]) {
	if (argc == 2) {
		__show_common_help();
	}
	if (is_update_string(argv[2])) {
		return __update_category(argc, argv);
	} else if (is_add_string(argv[2])) {
		return __add_category(argc, argv);
	} else if (is_delete_string(argv[2])) {
		return __del_category(argc, argv);
	} else if (is_list_string(argv[2])) {
		return __list_categories();
	} else if (is_help_for_category_module(argv[2])) {
		__show_help_for_category_module();
	} else {
		__show_common_help();
	}
}
