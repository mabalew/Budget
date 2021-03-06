#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>
#include "db.h"

enum backup_operation {
	RESTORE= 0,
	BACKUP = 1
}; 

void check_db_open(int error) {
	if (error) {
		puts("Can't open database");
		exit(0);
	}
}

int backup_db(char *file) {
	sqlite3 *conn;
	int error = 0;
	error = sqlite3_open(DB_FILE, &conn);
	check_db_open(error);
	if (error == SQLITE_OK) {
		loadOrSaveDb(conn, file, BACKUP);
	} else {
		printf("backup_db: SQL Error(%d): %s\n", error, sqlite3_errmsg(conn));
	}
	sqlite3_close(conn);
	return error;
}

// Copied from SQLite page (http://www.sqlite.org/backup.html)
/*
 ** This function is used to load the contents of a database file on disk 
 ** into the "main" database of open database connection pInMemory, or
 ** to save the current contents of the database opened by pInMemory into
 ** a database file on disk. pInMemory is probably an in-memory database, 
 ** but this function will also work fine if it is not.
 **
 ** Parameter zFilename points to a nul-terminated string containing the
 ** name of the database file on disk to load from or save to. If parameter
 ** isSave is non-zero, then the contents of the file zFilename are 
 ** overwritten with the contents of the database opened by pInMemory. If
 ** parameter isSave is zero, then the contents of the database opened by
 ** pInMemory are replaced by data loaded from the file zFilename.
 **
 ** If the operation is successful, SQLITE_OK is returned. Otherwise, if
 ** an error occurs, an SQLite error code is returned.
 */
int loadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave){
	int rc;                   /*  Function return code */
	sqlite3 *pFile;           /*  Database connection opened on zFilename */
	sqlite3_backup *pBackup;  /*  Backup object used to copy data */
	sqlite3 *pTo;             /*  Database to copy to (pFile or pInMemory) */
	sqlite3 *pFrom;           /*  Database to copy from (pFile or pInMemory) */
						
	/*  Open the database file identified by zFilename. Exit early if this fails
	** for any reason. */
	rc = sqlite3_open(zFilename, &pFile);
	if( rc==SQLITE_OK ){

		/*  If this is a 'load' operation (isSave==0), then data is copied
		** from the database file just opened to database pInMemory. 
		** Otherwise, if this is a 'save' operation (isSave==1), then data
		** is copied from pInMemory to pFile.  Set the variables pFrom and
		** pTo accordingly. */
		pFrom = (isSave ? pInMemory : pFile);
		pTo   = (isSave ? pFile     : pInMemory);
													
		/*  Set up the backup procedure to copy from the "main" database of 
		** connection pFile to the main database of connection pInMemory.
		** If something goes wrong, pBackup will be set to NULL and an error
		** code and  message left in connection pTo.
		**
		** If the backup object is successfully created, call backup_step()
		** to copy data from pFile to pInMemory. Then call backup_finish()
		** to release resources associated with the pBackup object.  If an
		** error occurred, then  an error code and message will be left in
		** connection pTo. If no error occurred, then the error code belonging
		** to pTo is set to SQLITE_OK.
		*/
		pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");
		if( pBackup ){
			(void)sqlite3_backup_step(pBackup, -1);
			(void)sqlite3_backup_finish(pBackup);
		}
		rc = sqlite3_errcode(pTo);
	}

	/*  Close the database connection opened on database file zFilename
	** and return the result of this function. */
	(void)sqlite3_close(pFile);
	return rc;
}
