#ifndef EXPENSE_H
#define EXPENSE_H

#include <sqlite3.h>

typedef struct expense {
	int id;
	char *exp_date;
	int category_id;
	char *category;
	int product_id;
	char *product;
	int shop_id;
	char *shop;
	float unit_price;
	float amount;
	float price;
} Expense;

int add_expense(Expense *c);
int del_expense(Expense *c);
int update_expense(Expense *old_expense, Expense *new_expense);
int get_expenses_count();
int get_all_expenses(Expense *list[]);
int get_expense_by_id(Expense *c);
int get_expense_by_name(Expense *c);
void init_expense(Expense *e, sqlite3_stmt *res);

#endif
