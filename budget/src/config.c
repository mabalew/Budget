#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "config.h"
#include "db.h"

int add_config(char *param_name, char *value) {
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("INSERT INTO config(param, value) VALUES('%s', '%s')", param_name, value);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n",error);
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	return error;
}

int get_config(char *param_name, char *value) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	if (error) {
		puts("Can't open database");
		return error;
	}
	char *sql = sqlite3_mprintf("SELECT value FROM config WHERE param='%s'", param_name);
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);

	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
		return error;
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		strcpy(value, (char*)sqlite3_column_text(res, 0));
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sqlite3_free(sql);
	return error;
}
