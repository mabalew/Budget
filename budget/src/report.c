#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <stdbool.h>
#include "expense.h"
#include "expense_utils.h"
#include "db.h"

typedef struct report_row {
	char *category;
	double *value;
} Row;

void get_exp_from(char *argv[], Expense *list[]);
void get_monthly_expenses(char *argv[], Row **rows);
void fetch_monthly_report(char *month, char *year, Row **rows);
void print_monthly_report(Row **rows, int row_count);
bool is_two_digit_number(int number);

int main(int argc, char *argv[]) {
	char *year = malloc(sizeof(char));
	strcpy(year, argv[1]);
	char *month = malloc(sizeof(char));
	strcpy(month, argv[2]);
	char *c[] = {argv[1],argv[2], argv[3]};
	int exp_count = get_expenses_count();

	puts("======== get_exp_from ==========");
	printf("zainicjowane z %d elementami\n", exp_count);
	Expense **e = calloc(exp_count, sizeof(Expense));
	get_exp_from(c, e);
	int counter = 0;
	while (NULL != e[counter]) {
		print_expense(e[counter]);
		counter++;
	}

	puts("======== print_monthly_report =======");
	int row_count = count_monthly_categories(month, year);
	printf("zainicjowane z %d elementami\n", row_count);
	Row **rows;
	rows = calloc(row_count, sizeof(Row));
	fetch_monthly_report(month, year, rows);
	print_monthly_report(rows, row_count);

	return 0;
}

void print_monthly_report(Row **rows, int row_count) {
	int counter = 0;
	double total = 0;
	printf("\n");
	printf("\n");
	puts("------------------+------------------------");
	for (counter = 0; counter < row_count; counter++) {
		printf("%17s | %.2f\n", rows[counter]->category, *(rows[counter]->value));
		total += *(rows[counter]->value);
	}
	puts("------------------+------------------------");
	printf(" Całkowity koszt: %.2f\n", total);
	puts("------------------+------------------------");
	printf("\n");
}

bool is_two_digit_number(int number) {
	return ((number >= 10 && number < 100) || (number <= -10 && number > -100));
}

void fetch_monthly_report(char *month, char *year, Row **rows) {
	if (atoi(month) < 1 || atoi(month) > 12) {
		printf("ERROR: niepoprawny miesiąc. Miesiąc musi zawierać się w przedziale od 1 do 12.");
		exit(0);
	}
	char *date[] = {year, month, "01"};
	get_monthly_expenses(date, rows);
}

void get_monthly_expenses(char *argv[], Row **rows) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0, counter = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("select category_name, sum(price) from v_expenses where exp_date >= '%s-%s-01' and exp_date <= '%s-%s-31' group by category_name", argv[0], argv[1], argv[0], argv[1]);
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);

	while (sqlite3_step(res) == SQLITE_ROW) {
		Row *r = malloc(sizeof(Row));
		r->category = malloc(sizeof(char));
		r->value = malloc(sizeof(double));
		strcpy(r->category, sqlite3_column_text(res, 0));
		*(r->value) = sqlite3_column_double(res, 1);
		rows[counter] = r;
		counter++;
	}

	if (error != 0) {
		printf("ERROR: %d\n", error);
		exit(error);
	}

}

int count_monthly_categories(char *month, char *year) {
	sqlite3 *conn;
	sqlite3_stmt *res;
	const char *tail;
	int error = 0, counter = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);

	char *sql = sqlite3_mprintf("select count(*) from (select category_name from v_expenses where exp_date >= '%s-%s-01' and exp_date <= '%s-%s-31' group by category_name)", year, month, year, month);
	error = sqlite3_prepare_v2(conn, sql, 1000, &res, &tail);

	while (sqlite3_step(res) == SQLITE_ROW) {
		counter = sqlite3_column_int(res, 0);
	}

	if (error != 0) {
		printf("ERROR: %d\n", error);
		exit(error);
	}

	return counter;
}

void get_exp_from(char *argv[], Expense **list) {
	char *date = malloc(sizeof(char));

	char *year = argv[0];
	char *month= argv[1];
	char *day  = argv[2];

	if (strlen(month) == 1 && !is_two_digit_number(atoi(month))) {
		char *tmp = malloc(sizeof(char));
		sprintf(tmp, "%s%s", "0", month);
		month = tmp;
	}
	if (strlen(day) == 1 && !is_two_digit_number(atoi(day))) {
		char *tmp = malloc(sizeof(char));
		sprintf(tmp, "%s%s", "0", day);
		day = tmp;
	}

	sprintf(date, "%s-%s-%s", year, month, day);

	Expense *e = malloc(sizeof(Expense));
	e->exp_date = malloc(sizeof(char));
	e->exp_date = date;

	get_expenses_from(e, list);
}
