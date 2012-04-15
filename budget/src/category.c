#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "category.h"
#include "log.h"
#include "db.h"

int add_category(Category *c) {
	char *msg = malloc(255);
	sqlite3 *conn;
	int error = 0;
	sprintf(msg, "add_category: adding [%s]", c->name);
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; INSERT INTO categories(category_name) VALUES('%q')", c->name);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		sprintf(msg, "add_category: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	sprintf(msg, "add_category: added [%s]", c->name);
	_log(INFO, c->name);
	free(msg);
	return error;
}

int del_category(Category *c) {
	char *msg = malloc(255);
	sqlite3 *conn;
	int error = 0;
	sprintf(msg, "del_category: deleting [%s]", c->name);
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; DELETE FROM categories WHERE category_name='%s'", c->name);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		sprintf(msg, "del_category: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	sprintf(msg, "del_category: deleted [%s]", c->name);
	_log(INFO, msg);
	free(msg);
	return error;
}

int update_category(Category *old_category, Category *new_category) {
	char *msg = malloc(255);
	sqlite3 *conn;
	int error = 0;
	sprintf(msg, "update_category: updating [%s] to [%s]", old_category->name, new_category->name);
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; UPDATE categories SET category_name='%s' WHERE category_name='%s'", new_category->name, old_category->name);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		sprintf(msg, "update_category: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	sprintf(msg, "update_category: updated [%s] to [%s]", old_category->name, new_category->name);
	_log(INFO, msg);
	free(msg);
	return error;
}

int get_categories_count() {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int row_count = 0;
	sprintf(msg, "get_categories_count: counting");
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		sprintf(msg, "get_categories_count: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	char *sql = "SELECT COUNT(*) FROM categories";
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);

	if (error != SQLITE_OK) {
		sprintf(msg, "get_categories_count: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		return error;
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		row_count = sqlite3_column_int(res, 0);	
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sprintf(msg, "get_categories_count: result [%d]", row_count);
	_log(INFO, msg);
	free(msg);
	return row_count;
}

int get_all_categories(Category *list[]) {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int counter = 0;
	sprintf(msg, "get_all_categories: getting all");
	_log(DEBUG, msg);
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		sprintf(msg, "get_all_categories: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}

	char *sql = "SELECT * FROM categories ORDER BY category_name";
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);
	if (error != SQLITE_OK) {
		sprintf(msg, "get_all_categories: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		sqlite3_close(conn);
		free(msg);
		return error;
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		list[counter] = malloc(sizeof (Category));
		list[counter]->name = malloc((strlen((char*)sqlite3_column_text(res, 1)) * sizeof (char)) + 1);
		list[counter]->id = sqlite3_column_int(res, 0);
		strcpy(list[counter]->name, (char*)sqlite3_column_text(res, 1));
		counter++;
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sprintf(msg, "get_all_categories: got %d categories", counter);
	_log(DEBUG, msg);
	free(msg);
	return error;
}

int get_category_by_id(Category *c) {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	sprintf(msg, "get_category_by_id: getting");
	_log(DEBUG, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("SELECT * FROM categories WHERE id=%d", c->id);
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);
	
	while (sqlite3_step(res) == SQLITE_ROW) {
		c->name = malloc((strlen((char*)sqlite3_column_text(res, 1)) * sizeof(char)) + 1);
		strcpy(c->name, (char*)sqlite3_column_text(res, 1));
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sqlite3_free(sql);

	if (error != 0) {
		sprintf(msg, "get_category_by_id: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		sqlite3_close(conn);
		free(msg);
		return error;
	}
	sprintf(msg, "get_category_by_id: got category [%s] for id [%d]", c->name, c->id);
	_log(DEBUG, msg);
	free(msg);
	return error;
}

int get_category_by_name(Category *c) {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	sprintf(msg, "get_category_by_name: getting by name [%s]", c->name);
	_log(DEBUG, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("SELECT * FROM categories WHERE category_name='%s'", c->name);
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);

	while (sqlite3_step(res) == SQLITE_ROW) {
		c->id = sqlite3_column_int(res, 0);
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sqlite3_free(sql);
	sprintf(msg, "get_category_by_id: got [%s] with id [%d]", c->name, c->id);
	_log(DEBUG, msg);

	if (error != 0) {
		sprintf(msg, "get_category_by_id: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	sprintf(msg, "get_category_by_name: got id [%d]", c->id);
	_log(DEBUG, msg);
	free(msg);
	return error;
}
