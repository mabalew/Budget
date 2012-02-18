#include <stdlib.h>
#include <stdio.h>
#include "db.h"

void check_db_open(int error) {
	if (error) {
		puts("Can't open database");
		exit(0);
	}
}
