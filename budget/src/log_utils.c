#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

void list_logs() {
	const int count = get_logs_count();
	int i = 0;
	Log list[count];
	get_logs(&list);
  for (i = 0; i < count; i++) {
		printf("%s :: %s\n", list[i]->date, list[i]->msg);
	}
}
