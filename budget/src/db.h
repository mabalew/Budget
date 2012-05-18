#ifndef DB_H
#define DB_H

#define DB_FILE "/home/mabalew/.budget/budget.db"
#define LOG_DB_FILE "/home/mabalew/.budget/logs.db"
void check_db_open(int error);
int backup_db(char *file);
int loadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave);

#endif
