#ifndef LOG_H
#define LOG_H
typedef enum log_level {
	TRACE = 0,
	DEBUG,
	INFO,
	WARN,
	ERROR
} Level;

typedef struct log {
	int id;
	char *date;
  Level level;
	char *msg;
} Log;

void _log(Level level, char *msg);
void get_logs(Level level, Log *list[]);
int get_logs_count();
#endif
