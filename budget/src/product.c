#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "product.h"
#include "db.h"
#include "log.h"

int add_product(Product *p) {
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);
	char *msg = malloc(255);


	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; INSERT INTO products(product_name, category_id) VALUES('%q', %d)", p->name, p->category_id);
	error = sqlite3_exec(conn, sql, NULL, NULL, NULL);
	if (error != SQLITE_OK) {
		sprintf(msg, "add_product: SQL Error(%d): %s",error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		return error;
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	sprintf(msg, "add_product: added [%s] with category id [%d]", p->name, p->category_id);
	_log(INFO, msg);
	free(msg);
	return error;
}

int del_product(Product *p) {
	sqlite3 *conn;
	char *msg = malloc(255);
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);


	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; DELETE FROM products WHERE product_name='%s'", p->name);
	error = sqlite3_exec(conn, sql, NULL, NULL, NULL);
	if (error != SQLITE_OK) {
		sprintf(msg, "del_product: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		return error;
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	sprintf(msg, "del_product: deleted [%s]", p->name);
	_log(INFO, msg);
	free(msg);
	return error;
}

int update_product(Product *old_product, Product *new_product) {
	char *msg = malloc(255);
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; UPDATE products SET product_name='%s', category_id='%d' WHERE product_name='%s'", new_product->name, new_product->category_id, old_product->name);
	error = sqlite3_exec(conn, sql, NULL, NULL, NULL);
	if (error != SQLITE_OK) {
		sprintf(msg, "update_product: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		return error;
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	sprintf(msg, "update_product: updated [%s] to [%s] with category id [%d]", old_product->name, new_product->name, new_product->category_id);
	_log(INFO, msg);
	free(msg);
	return error;
}

int add_to_category(Product *p) {
	char *msg = malloc(255);
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; UPDATE products SET category_id=%d WHERE id=%d", p->category_id, p->id);
	error = sqlite3_exec(conn, sql, NULL, NULL, NULL);
	if (error != SQLITE_OK) {
		sprintf(msg, "add_to_category: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		return error;
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	sprintf(msg, "add_to_category: adding [%d] to category [%d]", p->id, p->category_id);
	_log(INFO, msg);
	free(msg);
	return error;
}

int get_products_count(int category_id) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int row_count = 0;
	char *msg = malloc(255);

	error = sqlite3_open(DB_FILE, &conn);

	if (error) {
		sprintf(msg, "get_products_count: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		return error;
	}

	char *sql = "SELECT COUNT(*) FROM products";
	int is_malloced = 0;
	if (category_id != 0) {
		is_malloced = 1;
		sql = sqlite3_mprintf("SELECT COUNT(*) FROM products WHERE category_id=%d", category_id);
	}

	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);
	
	if (error != SQLITE_OK) {
		sprintf(msg, "get_products_count: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		return error;
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		row_count = sqlite3_column_int(res, 0);	
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	if (is_malloced == 1) {
		sqlite3_free(sql);
	}
	sprintf(msg, "get_products_count: counting in category [%d] (result: [%d])", category_id, row_count);
	_log(DEBUG, msg);
	free(msg);
	return row_count;
}

int get_all_products(Product *list[]) {
	sqlite3 *conn;
	char *msg = malloc(255);
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int counter = 0;

	error = sqlite3_open(DB_FILE, &conn);

	if (error) {
		sprintf(msg, "get_all_products: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		return error;
	}

	char *sql = "SELECT * FROM products ORDER BY category_id";
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);
	if (error != SQLITE_OK) {
		sprintf(msg, "get_all_products: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		return error;
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		list[counter] = malloc(sizeof(Product));
		list[counter]->name = malloc((strlen((char*)sqlite3_column_text(res, 1)) * sizeof(char)) + 1);
		list[counter]->id = sqlite3_column_int(res, 0);
		strcpy(list[counter]->name, (char*)sqlite3_column_text(res, 1));
		list[counter]->category_id = sqlite3_column_int(res, 2);
		counter++;
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sprintf(msg, "get_all_products: got [%d] products", counter);
	_log(DEBUG, msg);
	free(msg);
	return error;
}

int get_products_in_category(int category_id, Product *list[]) {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int counter = 0;
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		sprintf(msg, "get_products_in_category: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		return error;
	}

	char *sql = sqlite3_mprintf("SELECT * FROM products WHERE category_id=%d", category_id);
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);
	if (error != SQLITE_OK) {
		sprintf(msg, "get_products_in_category: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		return error;
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		list[counter] = malloc(sizeof (Product));
		list[counter]->name = malloc((strlen((char*)sqlite3_column_text(res, 1)) * sizeof(char)) + 1);
		list[counter]->id = sqlite3_column_int(res, 0);
		strcpy(list[counter]->name, (char*)sqlite3_column_text(res, 1));
		list[counter]->category_id = sqlite3_column_int(res, 2);
		counter++;
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sqlite3_free(sql);
	sprintf(msg, "get_products_in_category: got [%d] products from category [%d]", counter, category_id);
	_log(DEBUG, msg);
	free(msg);
	return error;
}

int get_product_by_id(Product *p) {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	sprintf(msg, "get_product_by_id: getting [%d]", p->id);
	_log(DEBUG, msg);
	char *sql = sqlite3_mprintf("SELECT * FROM products WHERE id=%u", p->id);
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);

	while (sqlite3_step(res) == SQLITE_ROW) {
		p->name = (char*)sqlite3_column_text(res, 1);
		p->category_id = sqlite3_column_int(res, 2);
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);

	if (error != 0) {
		sprintf(msg, "get_product_by_id: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		return error;
	}
	sprintf(msg, "get_product_by_id: got [%s] for id [%d]", p->name, p->id);
	_log(DEBUG, msg);
	free(msg);
	return error;
}

int get_product_by_name(Product *p) {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	sprintf(msg, "get_product_by_name: getting [%s]", p->name);
	_log(DEBUG, msg);

	char *sql = sqlite3_mprintf("SELECT * FROM products WHERE product_name='%s'", p->name);
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);

	while (sqlite3_step(res) == SQLITE_ROW) {
		p->id = sqlite3_column_int(res, 0);
		p->category_id = sqlite3_column_int(res, 0);
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);

	if (error != 0) {
		sprintf(msg, "get_product_by_name: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		return error;
	}
	sprintf(msg, "get_product_by_name: got [%s] with id [%d]", p->name, p->id);
	_log(DEBUG, msg);
	free(msg);
	return error;
}
