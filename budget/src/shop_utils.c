#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shop.h"
#include "shop_utils.h"

void free_shop_list(Shop *list[], int size);

void __show_help_for_shop_module() {
	puts("\ndostępne komendy:");
	puts("\tadd lub a [nazwa]");
	puts("\tupdate lub upd lub u [istniejąca nazwa] [nowa nazwa]");
	puts("\tdelete lub del lub d [nazwa]");
	puts("\tlist lub ls\n");
}

void __update_shop(char *argv[]) {
	Shop *so = (Shop *)malloc(sizeof so);
	Shop *sn = (Shop *)malloc(sizeof sn);
	so->name = (char *)malloc(sizeof (char));
	sn->name = (char *)malloc(sizeof (char));
	so->name = (char *)argv[3];
	sn->name = (char *)argv[4];
	update_shop(so, sn);
	free(so);
	free(sn);
}

void __add_shop(char *argv[]) {
	Shop *s = (Shop*)malloc(sizeof s);
	s->name = (char *)argv[3];
	add_shop(s);
	free(s);
}

void __del_shop(char *argv[]) {
	Shop *s = (Shop *) malloc(sizeof s);
	s->name = (char *)malloc(sizeof(char));
	s->name = (char *)argv[3];
	del_shop(s);
	free(s);
}

void __list_shops() {
	int shops_count = get_shops_count();
	int counter = 0;
	puts("==================================");
	printf("Znaleziono %d\n", shops_count);
	puts("==================================");
	Shop *list[shops_count];
	get_all_shops(list);
	if (shops_count != 0) {
		for (counter = 0; counter < shops_count; counter++) {
			printf("\tid: %d\n", list[counter]->id);
			printf("\tnazwa: %s\n", list[counter]->name);
			puts("----------------------------------");
		}
	}
	puts("==================================");
	free_shop_list(list, shops_count);
}

void free_shop_list(Shop *list[], int size) {
	int counter = 0;
	for (counter = 0; counter < size; counter++) {
		free(list[counter]->name);
		free(list[counter]);
	}
}

void manage_shop(int argc, char *argv[]) {
	if (argc == 2) {
		__show_common_help();
	}
	if (is_update_string(argv[2])) {
		__update_shop(argv);
	} else if (is_add_string(argv[2])) {
		__add_shop(argv);
	} else if (is_delete_string(argv[2])) {
		__del_shop(argv);
	} else if (is_list_string(argv[2])) {
		__list_shops();
	} else if (is_help_for_shop_module(argv[2])) {
		__show_help_for_shop_module();
	} else {
		__show_common_help();
	}
}
