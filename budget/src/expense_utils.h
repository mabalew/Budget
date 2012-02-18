#ifndef EXPENSE_UTILS_H
#define EXPENSE_UTILS_H
void __update_expense(char *argv[]);
void __add_expense(char *argv[]);
void __del_expense(char *argv[]);
void __list_expenses();
void print_expense(Expense *e);
void manage_expense(int argc, char *argv[]);
#endif
