#ifndef REPORT_H
#define REPORT_H
typedef struct report_row {
	char *category;
	double value;
} Row;
void free_rows_list(Row **rows, int count);
void fetch_monthly_report(char *month, char *year, Row **rows);
void fetch_yearly_report(char *year, Row **rows);
int count_monthly_categories(char *year, char *month);
int count_yearly_categories(char *year);
#endif
