#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "log.h"
#include "db.h"
#include "config.h"

int configured_level = -1;

void _log(Level level, char *msg) {
	sqlite3 *conn;
	int error = 0;
	if (configured_level == -1) {
		char *LOGGING_LEVEL = malloc(2);
		get_config("LOGGING_LEVEL", LOGGING_LEVEL);
		configured_level = atoi(LOGGING_LEVEL);
		if (configured_level < 2)
			printf("LOGGING_LEVEL: %s\n", LOGGING_LEVEL);
		free(LOGGING_LEVEL);
	}
	if (level >= configured_level) {
		error = sqlite3_open(LOG_DB_FILE, &conn);
		check_db_open(error);

		char *sql = sqlite3_mprintf("INSERT INTO logs(message, level) VALUES('%s', %d)", msg, level);
		error = sqlite3_exec(conn, sql, 0, 0, 0);
		if (error != SQLITE_OK) {
			printf("ERROR: %d\n",error);
		}
		sqlite3_close(conn);
		sqlite3_free(sql);
	}
}

int get_logs_count(Level level) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int row_count = 0;
	error = sqlite3_open(LOG_DB_FILE, &conn);
	if (error) {
		puts("Can't open database");
		return error;
	}
	char *sql = malloc(255);
	sprintf(sql, "SELECT COUNT(*) FROM logs WHERE level=%d", level);
	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);

	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
		return error;
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		row_count = sqlite3_column_int(res, 0);	
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	return row_count;
}

void get_logs(Level level, Log *list[]) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	int counter = 0;
	error = sqlite3_open(LOG_DB_FILE, &conn);
	if (error) {
		puts("Can't open database");
		exit(error);
	}

	char *sql = malloc(100);
	char *c_level = malloc(10);
	switch (level) {
		case TRACE:
			c_level = "trace";
		case DEBUG:
			c_level = "debug";
			break;
		case INFO:
			c_level = "info";
			break;
		case WARN:
			c_level = "warn";
			break;
		case ERROR:
			c_level = "ERROR";
			break;
		default:
			c_level = "";
			break;
	}

	sprintf(sql, "SELECT * FROM v_%s_logs", c_level);

	error = sqlite3_prepare_v2(conn, sql, -1, &res, &tail);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
		exit(error);
	}

	while (sqlite3_step(res) == SQLITE_ROW) {
		list[counter] = malloc(sizeof (Log));
		list[counter]->id = sqlite3_column_int(res, 0);
		list[counter]->level = sqlite3_column_int(res, 2);
		list[counter]->date = malloc((strlen((char*)sqlite3_column_text(res, 1)) * sizeof (char)) + 1);
		list[counter]->msg = malloc((strlen((char*)sqlite3_column_text(res, 3)) * sizeof (char)) + 1);
		strcpy(list[counter]->date, (char*)sqlite3_column_text(res, 1));
		strcpy(list[counter]->msg, (char*)sqlite3_column_text(res, 3));
		counter++;
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	free(c_level);
	free(sql);
}
