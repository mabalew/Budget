#ifndef EXPENSE_UTILS_H
#define EXPENSE_UTILS_H
#include "expense.h"
int __update_expense(int argc, char *argv[]);
int __add_expense(int argc, char *argv[]);
int __del_expense(int argc, char *argv[]);
int __list_expenses();
void print_expense(Expense *e);
int manage_expense(int argc, char *argv[]);
void free_expense_list(Expense *list[], int size);
#endif
