#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "expense.h"
#include "db.h"
#include "utils.h"

int add_expense(Expense *e) {
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; INSERT INTO expenses(exp_date, amount, product_id, shop_id, price) VALUES('%q', %.2f, %d, %d, %.2f)", e->exp_date, e->amount, e->product_id, e->shop_id, e->price);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n",error);
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	return error;
}

int del_expense(Expense *e) {
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; DELETE FROM expenses WHERE id=%d", e->id);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	return error;
}

int update_expense(Expense *old_expense, Expense *new_expense) {
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; UPDATE expenses SET exp_date='%s', product_id=%d, shop_id=%d, price=%.2f, amount=%.2f WHERE id=%d", new_expense->exp_date, new_expense->product_id, new_expense->shop_id, new_expense->price, new_expense->amount, old_expense->id);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	return error;
}

void fill_expense(sqlite3_stmt *res, Expense *e) {
	e->id = sqlite3_column_int(res, 0);
	e->exp_date = malloc((strlen((char*)sqlite3_column_text(res, 1)) * sizeof(char)) + 1);
	strcpy(e->exp_date, (char*)sqlite3_column_text(res, 1));
	e->category_id = sqlite3_column_int(res, 2);
	e->category = malloc((strlen((char*)sqlite3_column_text(res, 3)) * sizeof(char)) + 1);
	strcpy(e->category, (char*)sqlite3_column_text(res, 3));
	e->product_id = sqlite3_column_int(res, 4);
	e->product = malloc((strlen((char*)sqlite3_column_text(res, 5)) * sizeof(char)) + 1);
	strcpy(e->product, (char*)sqlite3_column_text(res, 5));
	e->amount = sqlite3_column_double(res, 6);
	e->price = sqlite3_column_double(res, 7);
	e->unit_price = sqlite3_column_double(res, 8);
	e->shop_id = sqlite3_column_int(res, 9);
	e->shop = malloc((strlen((char*)sqlite3_column_text(res, 10)) * sizeof(char)) + 1);
	strcpy(e->shop, (char*)sqlite3_column_text(res, 10));
}

// Funkcja pomocnicza dla funkcji max, min i avg.
Expense **_find_price(enum price which, int product_id, double *price, int *count) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	//double result = 0.0;
	error = sqlite3_open(DB_FILE, &conn);
	Expense **expenses;

	*count = 0;

	if (error) {
		puts("Can't open database");
		exit(0);
	}

	char *sql = malloc(200);
	char *sql1 = malloc(200);
	char *sql2 = malloc(200);
	
	switch (which) {
		case MIN:
			sprintf(sql, "SELECT min(unit_price) FROM v_expenses WHERE product_id=%d;", product_id);
			break;
		case AVG:
			sprintf(sql, "SELECT avg(unit_price) FROM v_expenses WHERE product_id=%d;", product_id);
			break;
		case MAX:
			sprintf(sql, "SELECT max(unit_price) FROM v_expenses WHERE product_id=%d;", product_id);
			break;
	}

	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);

	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		*price = sqlite3_column_double(res, 0);
	}

	sqlite3_finalize(res);
	sqlite3_close(conn);

	char *_price = malloc(10);
	sprintf(_price, "%.2f", *price);
	_price = replace_str(_price, ",", ".");
	printf("(expense.c) _price: %s\n", _price);
	printf("(expense.c) product_id: %d\n", product_id);

	error = sqlite3_open(DB_FILE, &conn);

	if (*price != 0.0 && which != AVG) {
		sprintf(sql1, "SELECT count(*) FROM (SELECT * FROM v_expenses WHERE product_id=%d AND unit_price=%s GROUP BY shop_name) sub;", product_id, _price);
		printf("(expense.c) sql1: %s\n",sql1);
		error = sqlite3_prepare_v2(conn, sql1, -1, &res, &tail);
		if (error == SQLITE_OK) {
			while (sqlite3_step(res) == SQLITE_ROW) {
				*count = sqlite3_column_int(res, 0);
			}
			sqlite3_finalize(res);
			sqlite3_close(conn);
			printf("(expense.c) count: %d\n", *count);
			error = sqlite3_open(DB_FILE, &conn);
			res = NULL;
			tail = NULL;
			if (*count > 0) {
				expenses = calloc(*count, sizeof(Expense));
				sprintf(sql2, "SELECT * FROM v_expenses WHERE product_id=%d AND unit_price=%s GROUP BY shop_name;", product_id, _price);
				printf("(expense.c) sql2: %s\n", sql2);
				error = sqlite3_prepare_v2(conn, sql2, -1, &res, &tail);
				if (error != SQLITE_OK) {
					printf("ERROR: %d\n", error);
					printf("ERROR: %s\n", sqlite3_errmsg(conn));
				}
				int i = 0;
				while(sqlite3_step(res) == SQLITE_ROW) {
					printf("(expense.c) i: %d\n", i);
					expenses[i] = malloc(sizeof(Expense));
					fill_expense(res, expenses[i]);
					printf(">>> shop: %s\n", expenses[i]->shop);
					i++;
				}
			}
			sqlite3_finalize(res);
			sqlite3_close(conn);
		}
	}
	free(sql);
	free(sql1);
	free(sql2);
	return expenses;
}

Expense **find_min_price(int product_id, double *min_price, int *count) {
	return _find_price(MIN, product_id, min_price, count);
}

Expense **find_max_price(int product_id, double *max_price, int *count) {
	return _find_price(MAX, product_id, max_price, count);
}

double find_avg_price(int product_id) {
	double avg_price = 0;
	int count = 0;
	_find_price(AVG, product_id, &avg_price, &count);
	return avg_price;
}

int get_expenses_count() {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int row_count = 0;
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		puts("Can't open database");
		exit(0);
	}
	char *sql = "SELECT COUNT(*) FROM v_expenses";
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);

	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
		printf("ERROR: %s\n", sqlite3_errmsg(conn));
		exit(error);
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		row_count = sqlite3_column_int(res, 0);	
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	return row_count;
}

/* /void init_expense(Expense *e, sqlite3_stmt *res) {
	e->exp_date = malloc((strlen((char*)sqlite3_column_text(res, 1)) * sizeof(char)) + 1);
	e->category = malloc((strlen((char*)sqlite3_column_text(res, 3)) * sizeof(char)) + 1);
	e->product = malloc((strlen((char*)sqlite3_column_text(res, 5)) * sizeof(char)) + 1);
	e->shop = malloc((strlen((char*)sqlite3_column_text(res, 10)) * sizeof(char)) + 1);
}*/

int get_expense(int expense_id, Expense *e) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		puts("Can't open database");
		exit(0);
	}

	char *sql = malloc(100);
	sprintf(sql, "SELECT * FROM v_expenses WHERE [exp id]=%d", expense_id);
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
		printf("ERROR: %s\n", sqlite3_errmsg(conn));
		return error;
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		fill_expense(res, e);
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sqlite3_free(sql);
	return error;
}

int get_all_expenses(Expense *list[]) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int counter = 0;
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		puts("Can't open database");
		exit(0);
	}

	char *sql = "SELECT * FROM v_expenses";
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
		printf("ERROR: %s\n", sqlite3_errmsg(conn));
		return error;
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		list[counter] = malloc(sizeof (Expense));
		fill_expense(res, list[counter]);
		counter++;
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	//sqlite3_free(sql);
	return error;
}

int get_expense_by_id(Expense *e) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("SELECT * FROM v_expenses WHERE id=%u", e->id);
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);

	while (sqlite3_step(res) == SQLITE_ROW) {
		Expense *e = malloc(sizeof(Expense));
		fill_expense(res, e);
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sqlite3_free(sql);

	if (error != 0) {
		printf("ERROR: %d\n", error);
		printf("ERROR: %s\n", sqlite3_errmsg(conn));
	}
	return error;
}

int get_expenses_from(Expense *e, Expense *list[]) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0, counter = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("SELECT * FROM v_expenses WHERE exp_date >= '%s'", e->exp_date);
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);

	while (sqlite3_step(res) == SQLITE_ROW) {
		list[counter] = malloc(sizeof(Expense));
		fill_expense(res, list[counter]);
		counter++;
	}

	sqlite3_finalize(res);
	sqlite3_close(conn);
	sqlite3_free(sql);

	if (error != 0) {
		printf("ERROR: %d\n", error);
		printf("ERROR: %s\n", sqlite3_errmsg(conn));
	}
	return error;
}
