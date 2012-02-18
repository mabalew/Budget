#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "help.h"
#include "expense.h"
#include "expense_utils.h"

void free_expense_list(Expense *list[], int size);

void __update_expense(char *argv[]) {
	Expense *eo = (Expense *)malloc(sizeof (Expense));
	Expense *en = (Expense *)malloc(sizeof (Expense));
	init_expense(eo);
	init_expense(en);
	eo->id = atoi(argv[3]);
	strcpy(en->exp_date, argv[4]);
	en->product_id = atoi(argv[5]);
	en->shop_id = atoi(argv[6]);
	en->amount = atof(argv[7]);
	en->price = atof(argv[8]);
	update_expense(eo, en);
	free(eo);
	free(en);
}

void __add_expense(char *argv[]) {
	Expense *e = (Expense*)malloc(sizeof (Expense));
	init_expense(e);
	e->exp_date = argv[3];
	e->product_id = atoi(argv[4]);
	e->shop_id = atoi(argv[5]);
	e->amount = atof(argv[6]);
	e->price = atof(argv[7]);
	add_expense(e);
	free(e);
}

void __del_expense(char *argv[]) {
	Expense *e = malloc(sizeof (Expense));
	init_expense(e);
	e->id = atoi(argv[3]);
	del_expense(e);
	free(e);
}

void __list_expenses() {
	int expenses_count = get_expenses_count();
	int counter = 0;
	Expense *list[expenses_count];
	puts("==================================");
	printf("Znaleziono %d\n", expenses_count);
	puts("==================================");
	get_all_expenses(list);
	if (expenses_count > 0) {
		for (counter = 0; counter < expenses_count; counter++) {
			print_expense(list[counter]);
		}
	}
	puts("==================================");
	free_expense_list(list, expenses_count);
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

void manage_expense(int argc, char *argv[]) {
	if (argc == 2) {
		__show_common_help();
	}
	if (is_update_string(argv[2])) {
		__update_expense(argv);
	} else if (is_add_string(argv[2])) {
		__add_expense(argv);
	} else if (is_delete_string(argv[2])) {
		__del_expense(argv);
	} else if (is_list_string(argv[2])) {
		__list_expenses();
	} else if (is_help_for_expense_module(argv[2])) {
		__show_help_for_expense_module();
	} else {
		__show_common_help();
	}
}
