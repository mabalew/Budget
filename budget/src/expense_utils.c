#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "help.h"
#include "string_verifier.h"
#include "help.h"
#include "expense.h"
#include "expense_utils.h"
#include "utils.h"

void free_expense_list(Expense *list[], int size);

int __update_expense(int argc, char *argv[]) {
	verify_number_of_parameters(argc, 9);

	Expense *eo = malloc(sizeof (Expense));
	Expense *en = malloc(sizeof (Expense));
	//init_expense(eo);
	//init_expense(en);
	eo->id = atoi(argv[3]);
	en->exp_date = argv[4];
	en->product_id = atoi(argv[5]);
	en->shop_id = atoi(argv[6]);
	en->amount = atof(argv[7]);
	en->price = atof(argv[8]);
	int status = update_expense(eo, en);
	free(eo);
	free(en);
	return status;
}

int __add_expense(int argc, char *argv[]) {
	verify_number_of_parameters(argc, 8);
	Expense *e = malloc(sizeof (Expense));
	//init_expense(e);
	e->exp_date = argv[3];
	e->product_id = atoi(argv[4]);
	e->shop_id = atoi(argv[5]);
	e->amount = atof(argv[6]);
	e->price = atof(argv[7]);
	int status = add_expense(e, 0);
	free(e);
	return status;
}

int __del_expense(int argc, char *argv[]) {
	verify_number_of_parameters(argc, 4);
	Expense *e = malloc(sizeof (Expense));
	//init_expense(e);
	e->id = atoi(argv[3]);
	int status = del_expense(e);
	free(e);
	return status;
}

int __list_expenses(char *year, char *month) {
	int expenses_count = get_expenses_count(year, month);
	int counter = 0;
	Expense *list[expenses_count];
	puts("==================================");
	printf("Znaleziono %d\n", expenses_count);
	puts("==================================");
	int status = get_all_expenses(list);
	if (expenses_count > 0) {
		for (counter = 0; counter < expenses_count; counter++) {
			print_expense(list[counter]);
		}
	}
	puts("==================================");
	free_expense_list(list, expenses_count);
	return status;
}

void free_expense_list(Expense *list[], int size) {
	int counter = 0;
	for (counter = 0; counter < size; counter++) {
		free(list[counter]->exp_date);
		free(list[counter]->product);
		free(list[counter]->category);
		free(list[counter]->shop);
		free(list[counter]);
	}
}

void print_expense(Expense *e) {
	printf("\tdata: %s sklep: %s\n", e->exp_date, e->shop);
	printf("\tzakup nr. %.6d na kwotę %.2f / ilość  %.2f / cena jedn. %.2f\n", e->id, e->price, e->amount, e->unit_price);
	printf("\tprodukt: %s z kategorii '%s' [%d]\n", e->product, e->category, e->category_id);
	puts("----------------------------------");
}

int manage_expense(int argc, char *argv[]) {
	if (argc == 2) {
		__show_common_help();
	}
	if (is_update_string(argv[2])) {
		return __update_expense(argc, argv);
	} else if (is_add_string(argv[2])) {
		return __add_expense(argc, argv);
	} else if (is_delete_string(argv[2])) {
		return __del_expense(argc, argv);
	} else if (is_list_string(argv[2])) {
		return __list_expenses(argv[3], argv[4]);
	} else if (is_help_for_expense_module(argv[2])) {
		__show_help_for_expense_module();
	} else {
		__show_common_help();
	}
	return 0;
}
