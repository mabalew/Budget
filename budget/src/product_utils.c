#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "category.h"
#include "product.h"
#include "product_utils.h"

void free_product_list(Product *list[], int size);
void verify_number_of_parameters(int argc, int should_be);

void verify_number_of_parameters(int argc, int should_be) {
	if (argc < should_be) {
		printf("Niepoprawna liczba parametrow\n");
		exit(0);
	}
}

int __update_product(int argc, char *argv[]) {
	verify_number_of_parameters(argc, 6);

	Product *po = (Product *)malloc(sizeof (Product));
	Product *pn = (Product *)malloc(sizeof (Product));
	po->name = argv[3];
	pn->name = argv[4];
	pn->category_id = atoi(argv[5]);
	int status = update_product(po, pn);
	free(po);
	free(pn);
	return status;
}

int __add_product(int argc, char *argv[]) {
	verify_number_of_parameters(argc, 5);

	Product *p = malloc(sizeof (Product));
	p->name = argv[3];
	p->category_id = atoi(argv[4]);
	int status = add_product(p);
	free(p);
	return status;
}

int __del_product(int argc, char *argv[]) {
	verify_number_of_parameters(argc, 4);

	Product *p = malloc(sizeof (Product));
	p->name = argv[3];
	int status = del_product(p);
	free(p);
	return status;
}

int __add_to_category(int argc, char *argv[]) {
	verify_number_of_parameters(argc, 4);

	Product *p = malloc(sizeof (Product));
	p->id = atoi(argv[3]);
	p->category_id = atoi(argv[4]);
	int status = add_to_category(p);
	free(p);
	return status;
}

int __list_products() {
	int products_count = get_products_count(0);
	int counter = 0;
	Product *list[products_count];
	puts("==================================");
	printf("Znaleziono %d\n", products_count);
	puts("==================================");
	int status = get_all_products(list);
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
	return status;
}

void free_product_list(Product *list[], int size) {
	int counter = 0;
	for (counter = 0; counter < size; counter++) {
		free(list[counter]->name);
		free(list[counter]);
	}
}

int __list_products_in_category(int argc, char *argv[]) {
	verify_number_of_parameters(argc, 4);

	int category_id = atoi(argv[3]);
	int products_count = get_products_count(category_id);
	int counter = 0;
	Product *list[products_count];
	puts("==================================");
	printf("Znaleziono %d\n", products_count);
	puts("==================================");
	int status = get_products_in_category(category_id, list);
	if (products_count != 0) {
		for (counter = 0; counter < products_count; counter++) {
			printf("\tid: %d\n", list[counter]->id);
			printf("\tnazwa: %s\n", list[counter]->name);
			printf("\tid kategorii: %d\n", list[counter]->category_id);
			Category *c = malloc(sizeof(Category));
			c->id = list[counter]->category_id;
			get_category_by_id(c);
			printf("\tkategoria: %s\n", c->name);
			free(c->name);
			free(c);
			puts("----------------------------------");
		}
	}
	puts("==================================");
	free_product_list(list, products_count);
	return status;
}

int manage_product(int argc, char *argv[]) {
	if (argc == 2) {
		__show_common_help();
	}
	if (is_update_string(argv[2])) {
		return __update_product(argc, argv);
	} else if (is_add_string(argv[2])) {
		return __add_product(argc, argv);
	} else if (is_delete_string(argv[2])) {
		return __del_product(argc, argv);
	} else if (is_list_string(argv[2])) {
		return __list_products();
	} else if (is_help_string(argv[2])) {
		return __show_help_for_product_module();
	} else if (is_add_to_category_string(argv[2])) {
		return __add_to_category(argc, argv);
	} else if (is_list_products_in_category_string(argv[2])) {
		return __list_products_in_category(argc, argv);
	} else {
		__show_common_help();
	}
	return 0;
}
