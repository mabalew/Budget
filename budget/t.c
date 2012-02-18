#include <time.h>
#include <stdio.h>


int main(void) {
	struct tm *_t;
	time_t akt;
	time(&akt);

	_t = localtime(&akt);

	printf("\
		    \r- Dzień: %d\n\
				\r- Miesiąc: %d\n\
				\r- Rok: %d\n\
				\r- Dzień tygodnia: %d\n",
				_t->tm_mday, _t->tm_mon + 1, 1900 + _t->tm_year, _t->tm_wday);
	return 0;
}
