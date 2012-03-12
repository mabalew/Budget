#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_verifier.h"
#include "help.h"
#include "shop.h"
#include "shop_utils.h"
#include "utils.h"

void free_shop_list(Shop *list[], int size);

void __show_help_for_shop_module() {
	puts("\ndostępne komendy:");
	puts("\tadd lub a [nazwa]");
	puts("\tupdate lub upd lub u [istniejąca nazwa] [nowa nazwa]");
	puts("\tdelete lub del lub d [nazwa]");
	puts("\tlist lub ls\n");
}

int __update_shop(int argc, char *argv[]) {
	verify_number_of_parameters(argc, 5);
	Shop *so = malloc(sizeof (Shop));
	Shop *sn = malloc(sizeof (Shop));
	so->name = (char *)argv[3];
	sn->name = (char *)argv[4];
	int status = update_shop(so, sn);
	free(so);
	free(sn);
	return status;
}

int __add_shop(int argc, char *argv[]) {
	verify_number_of_parameters(argc, 4);
	Shop *s = (Shop*)malloc(sizeof s);
	s->name = (char *)argv[3];
	int status = add_shop(s);
	free(s);
	return status;
}

int __del_shop(int argc, char *argv[]) {
	verify_number_of_parameters(argc, 4);
	Shop *s = malloc(sizeof (Shop));
	s->name = argv[3];
	int status = del_shop(s);
	free(s);
	return status;
}

int __list_shops() {
	int shops_count = get_shops_count();
	int counter = 0;
	puts("==================================");
	printf("Znaleziono %d\n", shops_count);
	puts("==================================");
	Shop *list[shops_count];
	int status = get_all_shops(list);
	if (shops_count != 0) {
		for (counter = 0; counter < shops_count; counter++) {
			printf("\tid: %d\n", list[counter]->id);
			printf("\tnazwa: %s\n", list[counter]->name);
			puts("----------------------------------");
		}
	}
	puts("==================================");
	free_shop_list(list, shops_count);
	return status;
}

void free_shop_list(Shop *list[], int size) {
	int counter = 0;
	for (counter = 0; counter < size; counter++) {
		free(list[counter]->name);
		free(list[counter]);
	}
}

int manage_shop(int argc, char *argv[]) {
	if (argc == 2) {
		__show_common_help();
	}
	if (is_update_string(argv[2])) {
		return __update_shop(argc, argv);
	} else if (is_add_string(argv[2])) {
		return __add_shop(argc, argv);
	} else if (is_delete_string(argv[2])) {
		return __del_shop(argc, argv);
	} else if (is_update_string(argv[2])) {
		return __update_shop(argc, argv);
	} else if (is_list_string(argv[2])) {
		return __list_shops();
	} else if (is_help_for_shop_module(argv[2])) {
		__show_help_for_shop_module();
	} else {
		__show_common_help();
	}
	return 0;
}
