#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "expense.h"
#include "log.h"
#include "db.h"
#include "utils.h"

void fill_tmp_expense(sqlite3_stmt *res, Expense *e);

int add_expense(Expense *e, int with_exp_date) {
	char *msg = malloc(255);
	sqlite3 *conn;
	int error = 0;
	if (with_exp_date == 0) {
		sprintf(msg, "add_expense: adding [date: %s] [shop id: %d] [price: %.2f]", e->exp_date, e->shop_id, e->price);
	} else {
		sprintf(msg, "add_expense: adding [shop id: %d] [price: %.2f]", e->shop_id, e->price);
	}
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = malloc(500);
	if (with_exp_date == 0) {
		sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; INSERT INTO expenses(exp_date, amount, product_id, shop_id, price) VALUES('%q', %.2f, %d, %d, %.2f)", e->exp_date, e->amount, e->product_id, e->shop_id, e->price);
	} else {
		sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; INSERT INTO expenses(amount, product_id, shop_id, price) VALUES(%.2f, %d, %d, %.2f)", e->amount, e->product_id, e->shop_id, e->price);
	}
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		sprintf(msg, "add_expense: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	sqlite3_free(sql);

	if (with_exp_date == 0) {
		sprintf(msg, "add_expense: added [date: %s] [shop id: %d] [price: %.2f]", e->exp_date, e->shop_id, e->price);
	} else {
		sprintf(msg, "add_expense: added [shop id: %d] [price: %.2f]", e->shop_id, e->price);
	}
	_log(INFO, msg);
	free(msg);
	return error;
}

int add_tmp_expense(Expense *e, signed int *rowid) {
	char *msg = malloc(255);
	sqlite3 *conn;
	int error = 0;
	sprintf(msg, "add_tmp_expense: adding [shop id: %d] [price: %.2f]", e->shop_id, e->price);
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; INSERT INTO tmp_expenses(category_id, category, product_id, product, count, amount, price, shop_id, shop) VALUES(%d, '%s', %d, '%s', %d, %.3f, %.2f, %d, '%s')", e->category_id, e->category, e->product_id, e->product, e->count, e->amount, e->price, e->shop_id, e->shop);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		sprintf(msg, "add_tmp_expense: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	*rowid = sqlite3_last_insert_rowid(conn);
	sqlite3_close(conn);
	sqlite3_free(sql);
	sprintf(msg, "add_tmp_expense: added [shop id: %d] [price: %.2f]", e->shop_id, e->price);
	_log(INFO, msg);
	free(msg);
	return error;
}


int del_expense(Expense *e) {
	char *msg = malloc(255);
	sqlite3 *conn;
	int error = 0;
	sprintf(msg, "del_expense: deleting [id: %d]", e->id);
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; DELETE FROM expenses WHERE id=%d", e->id);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		sprintf(msg, "del_expense: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	sprintf(msg, "del_expense: deleted [%d]", e->id);
	_log(INFO, msg);
	free(msg);
	return error;
}

int update_expense(Expense *old_expense, Expense *new_expense) {
	char *msg = malloc(255);
	sqlite3 *conn;
	int error = 0;
	strcpy(msg, "update_expense: updating");
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; UPDATE expenses SET exp_date='%s', product_id=%d, shop_id=%d, price=%.2f, amount=%.2f WHERE id=%d", new_expense->exp_date, new_expense->product_id, new_expense->shop_id, new_expense->price, new_expense->amount, old_expense->id);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		sprintf(msg, "update_expense: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	strcpy(msg, "update_expense: updated");
	_log(INFO, msg);
	free(msg);
	return error;
}

int update_tmp_count(Expense *e) {
	char *msg = malloc(255);
	sqlite3 *conn;
	int error = 0;
	strcpy(msg, "update_count: updating");
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; UPDATE tmp_expenses SET count=%d WHERE id=%d", e->count, e->id);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		sprintf(msg, "update_tmp_count: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	strcpy(msg, "update_tmp_count: updated");
	_log(INFO, msg);
	free(msg);
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

void fill_tmp_expense(sqlite3_stmt *res, Expense *e) {
	e->id = sqlite3_column_int(res, 0);
	e->category_id = sqlite3_column_int(res, 1);
	e->category = malloc((strlen((char*)sqlite3_column_text(res, 2)) * sizeof(char)) + 1);
	strcpy(e->category, (char*)sqlite3_column_text(res, 2));
	e->product_id = sqlite3_column_int(res, 3);
	e->product = malloc((strlen((char*)sqlite3_column_text(res, 4)) * sizeof(char)) + 1);
	strcpy(e->product, (char*)sqlite3_column_text(res, 4));
	e->count = sqlite3_column_int(res, 5);
	e->amount = sqlite3_column_double(res, 6);
	e->price = sqlite3_column_double(res, 7);
	e->shop_id = sqlite3_column_int(res, 8);
	e->shop = malloc((strlen((char*)sqlite3_column_text(res, 9)) * sizeof(char)) + 1);
	strcpy(e->shop, (char*)sqlite3_column_text(res, 9));
}

// Funkcja pomocnicza dla funkcji max, min i avg.
Expense **_find_price(enum price which, int product_id, double *price, int *count) {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	//double result = 0.0;
	error = sqlite3_open(DB_FILE, &conn);
	Expense **expenses;

	*count = 0;

	if (error) {
		sprintf(msg, "_find_price: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		exit(error);
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
		sprintf(msg, "_find_price: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		exit(error);
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		*price = sqlite3_column_double(res, 0);
	}

	sqlite3_finalize(res);
	sqlite3_close(conn);

	char *_price = malloc(10);
	sprintf(_price, "%.2f", *price);
	_price = replace_str(_price, ",", ".");
	sprintf(msg, "_find_price: _price: [%s]", _price);
	_log(TRACE, msg);
	sprintf(msg, "_find_price: product_id [%d]", product_id);
	_log(TRACE, msg);

	error = sqlite3_open(DB_FILE, &conn);

	if (*price != 0.0 && which != AVG) {
		sprintf(sql1, "SELECT count(*) FROM (SELECT * FROM v_expenses WHERE product_id=%d AND unit_price=%s GROUP BY shop_name) sub;", product_id, _price);
		sprintf(msg, "_find_price: sql1 [%s]",sql1);
		_log(TRACE, msg);
		error = sqlite3_prepare_v2(conn, sql1, -1, &res, &tail);
		if (error == SQLITE_OK) {
			while (sqlite3_step(res) == SQLITE_ROW) {
				*count = sqlite3_column_int(res, 0);
			}
			sqlite3_finalize(res);
			sqlite3_close(conn);
			sprintf(msg, "_find_price: count [%d]", *count);
			_log(TRACE, msg);
			error = sqlite3_open(DB_FILE, &conn);
			res = NULL;
			tail = NULL;
			if (*count > 0) {
				expenses = calloc(*count, sizeof(Expense));
				sprintf(sql2, "SELECT * FROM v_expenses WHERE product_id=%d AND unit_price=%s GROUP BY shop_name;", product_id, _price);
				sprintf(msg, "_find_price: sql2 [%s]", sql2);
				_log(TRACE, msg);
				error = sqlite3_prepare_v2(conn, sql2, -1, &res, &tail);
				if (error != SQLITE_OK) {
					sprintf(msg, "_find_price: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
					_log(ERROR, msg);
					free(msg);
					sqlite3_close(conn);
					exit(error);
				}
				int i = 0;
				while(sqlite3_step(res) == SQLITE_ROW) {
					expenses[i] = malloc(sizeof(Expense));
					fill_expense(res, expenses[i]);
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

int get_expenses_count(char *year, char *month) {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int row_count = 0;
	sprintf(msg, "get_expenses_count: counting");
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		sprintf(msg, "get_expenses_count: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	char *sql = sqlite3_mprintf("SELECT count(*) FROM (SELECT * FROM v_expenses WHERE exp_date >= '%s-%s-01' AND exp_date <= '%s-%s-31') sub;", year, month, year, month);
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);

	if (error != SQLITE_OK) {
		sprintf(msg, "get_expenses_count: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		row_count = sqlite3_column_int(res, 0);	
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sprintf(msg, "get_expenses_count: result [%d]", row_count);
	_log(INFO, msg);
	free(msg);
	return row_count;
}

int get_tmp_expenses_count() {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int row_count = 0;
	sprintf(msg, "get_tmp_expenses_count: counting");
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		sprintf(msg, "get_tmp_expenses_count: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	char *sql = sqlite3_mprintf("SELECT count(*) FROM tmp_expenses");
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);

	if (error != SQLITE_OK) {
		sprintf(msg, "get_tmp_expenses_count: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		row_count = sqlite3_column_int(res, 0);	
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sprintf(msg, "get_tmp_expenses_count: result [%d]", row_count);
	_log(INFO, msg);
	free(msg);
	return row_count;
}

int get_expense(int expense_id, Expense *e) {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	sprintf(msg, "get_expense: getting [%d]", expense_id);
	_log(DEBUG, msg);
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		sprintf(msg, "get_expense: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}

	char *sql = malloc(100);
	sprintf(sql, "SELECT * FROM v_expenses WHERE [exp id]=%d", expense_id);
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);
	if (error != SQLITE_OK) {
		sprintf(msg, "get_expense: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
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
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int counter = 0;
	sprintf(msg, "get_all_expenses: getting all");
	_log(DEBUG, msg);
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		sprintf(msg, "get_all_expenses: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		sqlite3_close(conn);
		free(msg);
		return error;
	}

	char *sql = "SELECT * FROM v_expenses";
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);
	if (error != SQLITE_OK) {
		sprintf(msg, "get_all_expenses: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		sqlite3_close(conn);
		free(msg);
		return error;
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		list[counter] = malloc(sizeof (Expense));
		fill_expense(res, list[counter]);
		counter++;
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sprintf(msg, "get_all_expenses: got %d expenses", counter);
	_log(DEBUG, msg);
	free(msg);
	return error;
}

int get_all_tmp_expenses(Expense *list[]) {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int counter = 0;
	sprintf(msg, "get_all_tmp_expenses: getting all");
	_log(DEBUG, msg);
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		sprintf(msg, "get_all_tmp_expenses: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		sqlite3_close(conn);
		free(msg);
		return error;
	}

	char *sql = "SELECT * FROM tmp_expenses";
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);
	if (error != SQLITE_OK) {
		sprintf(msg, "get_all_tmp_expenses: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		sqlite3_close(conn);
		free(msg);
		return error;
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		list[counter] = malloc(sizeof (Expense));
		fill_tmp_expense(res, list[counter]);
		counter++;
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sprintf(msg, "get_all_tmp_expenses: got %d expenses", counter);
	_log(DEBUG, msg);
	free(msg);
	return error;
}


int get_expense_by_id(Expense *e) {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	sprintf(msg, "get_expense_by_id: getting");
	_log(DEBUG, msg);
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
		sprintf(msg, "get_expense_by_id: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		sqlite3_close(conn);
		free(msg);
		return error;
	}
	sprintf(msg, "get_expense_by_id: got expense [%d]", e->id);
	_log(DEBUG, msg);
	free(msg);
	return error;
}

int get_expenses(char *year, char *month, Expense *list[]) {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0, counter = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("SELECT * FROM v_expenses WHERE exp_date >= '%s-%s-01' AND exp_date <= '%s-%s-31'", year, month, year, month);
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
		sprintf(msg, "get_expenses_from: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	sprintf(msg, "get_expenses_from: got it");
	_log(DEBUG, msg);
	free(msg);
	return error;
}

int del_tmp_expense(int id) {
	char *msg = malloc(255);
	sqlite3 *conn;
	int error = 0;
	sprintf(msg, "del_tmp_expense: deleting");
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("DELETE FROM tmp_expenses WHERE id=%d", id);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		sprintf(msg, "del_tmp_expense: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	sprintf(msg, "del_tmp_expense: deleted");
	_log(INFO, msg);
	free(msg);
	return error;

}

int del_all_tmp_expenses() {
	char *msg = malloc(255);
	sqlite3 *conn;
	int error = 0;
	sprintf(msg, "del_all_tmp_expenses: deleting");
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("DELETE FROM tmp_expenses");
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		sprintf(msg, "del_all_tmp_expenses: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	sprintf(msg, "del_all_tmp_expenses: deleted");
	_log(INFO, msg);
	free(msg);
	return error;
}


void copy_tmp_expenses_to_expenses() {
	int tmp_expenses_count = get_tmp_expenses_count();
	Expense *list[tmp_expenses_count];
	int counter = 0;
	get_all_tmp_expenses(list);

	for (counter = 0; counter < tmp_expenses_count; counter++) {
		int subcounter = 0;
		double price = list[counter]->price / list[counter]->count;
		for (subcounter = 0; subcounter < list[counter]->count; subcounter++) {
			list[counter]->price = price;
			add_expense(list[counter], 1);
		}
	}
}
