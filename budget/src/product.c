#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "product.h"
#include "db.h"

int add_product(Product *p) {
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; INSERT INTO products(product_name, category_id) VALUES('%q', %d)", p->name, p->category_id);
	error = sqlite3_exec(conn, sql, NULL, NULL, NULL);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n",error);
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	return error;
}

int del_product(Product *p) {
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; DELETE FROM products WHERE product_name='%s'", p->name);
	error = sqlite3_exec(conn, sql, NULL, NULL, NULL);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	return error;
}

int update_product(Product *old_product, Product *new_product) {
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; UPDATE products SET product_name='%s', category_id='%d' WHERE product_name='%s'", new_product->name, new_product->category_id, old_product->name);
	error = sqlite3_exec(conn, sql, NULL, NULL, NULL);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	return error;
}

int add_to_category(Product *p) {
	printf("adding %d to category %d\n", p->id, p->category_id);
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; UPDATE products SET category_id=%d WHERE id=%d", p->category_id, p->id);
	error = sqlite3_exec(conn, sql, NULL, NULL, NULL);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	return error;
}

int get_products_count(int category_id) {
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

	char *sql = "SELECT COUNT(*) FROM products";
	int is_malloced = 0;
	if (category_id != 0) {
		is_malloced = 1;
		sql = sqlite3_mprintf("SELECT COUNT(*) FROM products WHERE category_id=%d", category_id);
	}

	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);
	
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
		exit(error);
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		row_count = sqlite3_column_int(res, 0);	
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	if (is_malloced == 1) {
		sqlite3_free(sql);
	}
	return row_count;
}

int get_all_products(Product *list[]) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int counter = 0;

	error = sqlite3_open(DB_FILE, &conn);

	if (error) {
		puts("Can't open database");
		return error;
	}

	char *sql = "SELECT * FROM products ORDER BY category_id";
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
		return error;
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		list[counter] = malloc(sizeof(Product));
		list[counter]->name = malloc((strlen(sqlite3_column_text(res, 1)) * sizeof(char)) + 1);
		list[counter]->id = sqlite3_column_int(res, 0);
		strcpy(list[counter]->name, sqlite3_column_text(res, 1));
		list[counter]->category_id = sqlite3_column_int(res, 2);
		counter++;
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	return error;
}

int get_products_in_category(int category_id, Product *list[]) {
	printf("getting from category %d\n", category_id);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int row_count = 0;
	int counter = 0;
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		puts("Can't open database");
		return error;
	}

	char *sql = sqlite3_mprintf("SELECT * FROM products WHERE category_id=%d", category_id);
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
		return error;
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		list[counter] = malloc(sizeof (Product));
		list[counter]->name = malloc((strlen(sqlite3_column_text(res, 1)) * sizeof(char)) + 1);
		list[counter]->id = sqlite3_column_int(res, 0);
		strcpy(list[counter]->name, sqlite3_column_text(res, 1));
		list[counter]->category_id = sqlite3_column_int(res, 2);
		counter++;
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sqlite3_free(sql);
	return error;
}

int get_product_by_id(Product *p) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("SELECT * FROM products WHERE id=%u", p->id);
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);

	while (sqlite3_step(res) == SQLITE_ROW) {
		strcpy(p->name, sqlite3_column_text(res, 1));
		p->category_id = sqlite3_column_int(res, 2);
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);

	if (error != 0) {
		printf("ERROR: %d\n", error);
		return error;
	}
	return error;
}

int get_product_by_name(Product *p) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("SELECT * FROM products WHERE product_name='%s'", p->name);
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);

	while (sqlite3_step(res) == SQLITE_ROW) {
		p->id = sqlite3_column_int(res, 0);
		p->category_id = sqlite3_column_int(res, 0);
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);

	if (error != 0) {
		printf("ERROR: %d\n", error);
		return error;
	}
	return error;
}
