#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "shop.h"
#include "log.h"
#include "db.h"

int add_shop(Shop *s) {
	char *msg = malloc(255);
	sqlite3 *conn;
	int error = 0;
	sprintf(msg, "add_shop: adding [%s]", s->name);
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; INSERT INTO shops(shop_name) VALUES('%q')", s->name);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		sprintf(msg, "add_shop: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	sprintf(msg, "add_shop: added [%s]", s->name);
	_log(INFO, msg);
	free(msg);
	return error;
}

int del_shop(Shop *s) {
	char *msg = malloc(255);
	sqlite3 *conn;
	int error = 0;
	sprintf(msg, "add_shop: adding [%s]", s->name);
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; DELETE FROM shops WHERE shop_name='%s'", s->name);
	error = sqlite3_exec(conn, sql, NULL, NULL, NULL);
	if (error != SQLITE_OK) {
		sprintf(msg, "del_shop: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	sprintf(msg, "del_shop: deleted [%s]", s->name);
	_log(INFO, msg);
	free(msg);
	return error;
}

int update_shop(Shop *old_shop, Shop *new_shop) {
	char *msg = malloc(255);
	sqlite3 *conn;
	int error = 0;
	sprintf(msg, "update_shop: updating [%s] to [%s]", old_shop->name, new_shop->name);
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("PRAGMA foreign_keys=ON; UPDATE shops SET shop_name='%s' WHERE shop_name='%s'", new_shop->name, old_shop->name);
	error = sqlite3_exec(conn, sql, NULL, NULL, NULL);
	if (error != SQLITE_OK) {
		sprintf(msg, "update_shop: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	sprintf(msg, "update_shop: updated [%s] to [%s]", old_shop->name, new_shop->name);
	_log(INFO, msg);
	free(msg);
	return error;
}

int get_shops_count() {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int row_count = 0;
	sprintf(msg, "get_shops_count: counting");
	_log(INFO, msg);
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		sprintf(msg, "get_shops_count: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	char *sql = "SELECT COUNT(*) FROM shops";
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);

	if (error != SQLITE_OK) {
		sprintf(msg, "get_shops_count: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
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
	sprintf(msg, "get_shops_count: result [%d]", row_count);
	_log(INFO, msg);
	free(msg);
	return row_count;
}

int get_all_shops(Shop *list[]) {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int counter = 0;
	sprintf(msg, "get_all_shops: getting all");
	_log(DEBUG, msg);
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		sprintf(msg, "get_all_shops: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}

	char *sql = "SELECT * FROM shops";
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);
	if (error != SQLITE_OK) {
		sprintf(msg, "get_all_shops: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		list[counter] = malloc(sizeof (Shop));
		list[counter]->name = malloc((strlen((char*)sqlite3_column_text(res, 1)) * sizeof (char)) + 1);
		list[counter]->id = sqlite3_column_int(res, 0);
		strcpy(list[counter]->name, (char*)sqlite3_column_text(res, 1));
		counter++;
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sprintf(msg, "get_all_shops: got %d shops", counter);
	_log(DEBUG, msg);
	free(msg);
	return error;
}

int get_shop_by_id(Shop *s) {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	sprintf(msg, "get_shop_by_id: getting");
	_log(DEBUG, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("SELECT * FROM shops WHERE id=%u", s->id);
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);

	while (sqlite3_step(res) == SQLITE_ROW) {
		s->name = malloc(strlen((char*)sqlite3_column_text(res, 1)) + 1);
		strcpy(s->name, (char*)sqlite3_column_text(res, 1));
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sqlite3_free(sql);

	if (error != 0) {
		sprintf(msg, "get_shop_by_id: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		sqlite3_close(conn);
		return error;
	}
	sprintf(msg, "get_shop_by_id: got shop [%s] for id [%d]", s->name, s->id);
	_log(DEBUG, msg);
	free(msg);
	return error;
}

int get_shop_by_name(Shop *s) {
	char *msg = malloc(255);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	sprintf(msg, "get_shop_by_name: getting by name [%s]", s->name);
	_log(DEBUG, msg);
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("SELECT * FROM shops WHERE shop_name='%s'", s->name);
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);

	while (sqlite3_step(res) == SQLITE_ROW) {
		s->id = sqlite3_column_int(res, 0);
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sqlite3_free(sql);

	if (error != 0) {
		sprintf(msg, "get_shop_by_id: SQL Error(%d): %s", error, sqlite3_errmsg(conn));
		_log(ERROR, msg);
		free(msg);
		sqlite3_close(conn);
		return error;
	}
	sprintf(msg, "get_shop_by_name: got id [%d]", s->id);
	free(msg);
	return error;
}
