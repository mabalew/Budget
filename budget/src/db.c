#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>
#include "db.h"

void check_db_open(int error) {
	if (error) {
		puts("Can't open database");
		exit(0);
	}
}

void export_to_file(char *file) {
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf(".backup backup_budget_db");
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		printf("export_to_file: Error(%d): %s\n", error, sqlite3_errmsg(conn));
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	sqlite3_free(sql);
	return error;
}
