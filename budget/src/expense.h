#ifndef EXPENSE_H
#define EXPENSE_H

#include <sqlite3.h>

enum price {
	MIN = 0,
	AVG,
	MAX
};

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
int get_expenses_count(char *year, char *month);
int get_expenses(char *year, char *month, Expense *list[]);
int get_expense(int expense_id, Expense *e);
int get_all_expenses(Expense *list[]);
int get_expense_by_id(Expense *c);
int get_expense_by_name(Expense *c);
void init_expense(Expense *e, sqlite3_stmt *res);
Expense **find_min_price(int product_id, double *min_price, int *count);
Expense **find_max_price(int product_id, double *max_price, int *count);
double find_avg_price(int product_id);
#endif
