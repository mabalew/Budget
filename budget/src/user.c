#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sqlite3.h>
#include <string.h>
#include "db.h"
#include "user.h"

extern void check_db_open(int error);

bool login(User *p) {
	//printf("%s :: %s\n", p->nick, p->password);
	User *user = (User *)malloc(sizeof user);
	user->nick = p->nick;
	get_user_by_login(user);
	if (strcmp(p->password, user->password) == 0) {
		//printf("OK\n");
		return true;
	} else {
		//printf("ERROR\n");
		return false;
	}
}

int add_user(User *p) {
	//printf("adding: %s, %s\n", p->nick, p->password);
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("INSERT INTO users(login, password) VALUES('%q','%q')", p->nick, p->password);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n",error);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	return 0;
}

int del_user(User *p) {
	//printf("removing: %s\n", p->nick);
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("DELETE FROM users WHERE login='%s'", p->nick);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	return 0;
}

int update_user(User *old_user, User *new_user) {
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("UPDATE users SET login='%s', password='%s' WHERE login='%s'", new_user->nick, new_user->password, old_user->nick);
	error = sqlite3_exec(conn, sql, 0, 0, 0);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
		sqlite3_close(conn);
		return error;
	}
	sqlite3_close(conn);
	return 0;
}

int get_users_count() {
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

	char *sql = "SELECT COUNT(*) FROM users";
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

int get_all_users(User *list[]) {
	//printf("listing of all the users\n");
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

	char *sql = "SELECT * FROM users";
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);
	if (error != SQLITE_OK) {
		printf("ERROR: %d\n", error);
		exit(error);
	}

	//puts("======================================");
	while (sqlite3_step(res) == SQLITE_ROW) {
		User *u = malloc(sizeof (User));
		u->nick = malloc(sizeof (char));
		u->password = malloc(sizeof (char));
		u->id = sqlite3_column_int(res, 0);
		strcpy(u->nick, sqlite3_column_text(res, 1));
		strcpy(u->password, sqlite3_column_text(res, 2));
		list[counter] = u;
		counter++;
	}
	sqlite3_finalize(res);
	sqlite3_close(conn);
	return 0;
}

int get_user_by_id(User *p) {
	//printf("getting user with id %d\n", p->id);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("SELECT * FROM  users WHERE id=%u", p->id);
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);

	while (sqlite3_step(res) == SQLITE_ROW) {
		strcpy(p->nick, sqlite3_column_text(res, 1));
		strcpy(p->password, sqlite3_column_text(res, 2));
	}
	//printf("%s %s\n", p->nick, p->password);
	sqlite3_finalize(res);
	sqlite3_close(conn);
	sqlite3_free(sql);

	if (error != 0) {
		printf("ERROR: %d\n", error);
		exit(error);
	}

	return error;
}

int get_user_by_login(User *p) {
	//printf("getting user with login %s\n", p->nick);
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("SELECT * FROM  users WHERE login='%s'", p->nick);
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);

	while (sqlite3_step(res) == SQLITE_ROW) {
		p->id = sqlite3_column_int(res, 0);
		strcpy(p->password, sqlite3_column_text(res, 2));
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
