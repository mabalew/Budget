#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "category.h"
#include "product.h"
#include "product_utils.h"

void free_product_list(Product *list[], int size);

void __update_product(char *argv[]) {
	Product *po = (Product *)malloc(sizeof (Product));
	Product *pn = (Product *)malloc(sizeof (Product));
	po->name = argv[3];
	pn->name = argv[4];
	pn->category_id = atoi(argv[5]);
	update_product(po, pn);
	free(po);
	free(pn);
}

void __add_product(char *argv[]) {
	Product *p = (Product*)malloc(sizeof p);
	p->name = malloc(strlen(argv[3]));
	p->name = argv[3];
	p->category_id = atoi(argv[4]);
	add_product(p);
	free(p->name);
	free(p);
}

void __del_product(char *argv[]) {
	Product *p = malloc(sizeof (Product));
	p->name = argv[3];
	del_product(p);
	free(p);
}

void __add_to_category(char *argv[]) {
	Product *p = malloc(sizeof p);
	p->id = atoi(argv[3]);
	p->category_id = atoi(argv[4]);
	add_to_category(p);
	free(p);
}

void __list_products() {
	int products_count = get_products_count(0);
	int counter = 0;
	Product *list[products_count];
	puts("==================================");
	printf("Znaleziono %d\n", products_count);
	puts("==================================");
	get_all_products(list);
	printf("%-4s %-20s %-12s %-25s\n","id","nazwa","id kategorii","kategoria");
	if (products_count != 0) {
		for (counter = 0; counter < products_count; counter++) {
			Category *c = malloc(sizeof(Category));
			c->id = 0;
			c->id = list[counter]->category_id;
			get_category_by_id(c);
			printf("%-4d %-20s %-12d %-25s\n", list[counter]->id, list[counter]->name, list[counter]->category_id, c->name);
			free(c->name);
			free(c);
		}
	}
	puts("==================================");
	free_product_list(list, products_count);
}

void free_product_list(Product *list[], int size) {
	int counter = 0;
	for (counter = 0; counter < size; counter++) {
		free(list[counter]->name);
		free(list[counter]);
	}
}

void __list_products_in_category(int category_id) {
	int products_count = get_products_count(category_id);
	int counter = 0;
	Product *list[products_count];
	puts("==================================");
	printf("Znaleziono %d\n", products_count);
	puts("==================================");
	get_products_in_category(category_id, list);
	if (products_count != 0) {
		for (counter = 0; counter < products_count; counter++) {
			printf("\tid: %d\n", list[counter]->id);
			printf("\tnazwa: %s\n", list[counter]->name);
			printf("\tid kategorii: %d\n", list[counter]->category_id);
			Category *c = malloc(sizeof(Category));
			c->name = malloc(sizeof(char));
			c->id = list[counter]->category_id;
			get_category_by_id(c);
			printf("\tkategoria: %s\n", c->name);
			free(c->name);
			free(c);
			puts("----------------------------------");
		}
	}
	puts("==================================");

}

void manage_product(int argc, char *argv[]) {
	if (argc == 2) {
		__show_common_help();
	}
	if (is_update_string(argv[2])) {
		__update_product(argv);
	} else if (is_add_string(argv[2])) {
		__add_product(argv);
	} else if (is_delete_string(argv[2])) {
		__del_product(argv);
	} else if (is_list_string(argv[2])) {
		__list_products();
	} else if (is_help_string(argv[2])) {
		__show_help_for_product_module();
	} else if (is_add_to_category_string(argv[2])) {
		__add_to_category(argv);
	} else if (is_list_products_in_category_string(argv[2])) {
		__list_products_in_category(atoi(argv[3]));
	} else {
		__show_common_help();
	}
}
