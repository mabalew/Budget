#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "shop.h"
#include "db.h"

int add_shop(Shop *s) {
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("INSERT INTO shops(shop_name) VALUES('%q')", s->name);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n",error);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	return 0;
}

int del_shop(Shop *s) {
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("DELETE FROM shops WHERE shop_name='%s'", s->name);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	return 0;
}

int update_shop(Shop *old_shop, Shop *new_shop) {
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("UPDATE shops SET shop_name='%s' WHERE shop_name='%s'", new_shop->name, old_shop->name);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	return 0;
}

int get_shops_count() {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int row_count = 0;
	int counter = 0;
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		puts("Can't open database");
		exit(0);
	}
	char *sql = "SELECT COUNT(*) FROM shops";
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);

	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
		exit(error);
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		row_count = sqlite3_column_int(res, 0);	
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	return row_count;
}

int get_all_shops(Shop *list[]) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int row_count = 0;
	int counter = 0;
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		puts("Can't open database");
		exit(0);
	}

	char *sql = "SELECT * FROM shops";
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
		exit(error);
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		list[counter] = malloc(sizeof (Shop));
		list[counter]->name = malloc((strlen(sqlite3_column_text(res, 1)) * sizeof (char)) + 1);
		list[counter]->id = sqlite3_column_int(res, 0);
		strcpy(list[counter]->name, sqlite3_column_text(res, 1));
		counter++;
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	return 0;
}

int get_shop_by_id(Shop *s) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("SELECT * FROM shops WHERE id=%u", s->id);
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);

	while (sqlite3_step(res) == SQLITE_ROW) {
		strcpy(s->name, sqlite3_column_text(res, 1));
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sqlite3_free(sql);

	if (error != 0) {
		printf("ERROR: %d\n", error);
		exit(error);
	}
	return error;
}

int get_shop_by_name(Shop *s) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("SELECT * FROM shops WHERE shop_name='%s'", s->name);
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);

	while (sqlite3_step(res) == SQLITE_ROW) {
		s->id = sqlite3_column_int(res, 0);
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sqlite3_free(sql);

	if (error != 0) {
		printf("ERROR: %d\n", error);
		exit(error);
	}
	return error;
}
