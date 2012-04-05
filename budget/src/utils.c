/*
 * =====================================================================================
 *
 *       Filename:  utils.c
 *
 *    Description:  Utilities
 *
 *        Version:  1.0
 *        Created:  20.02.2012 15:33:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mariusz Balewski (MB), mariusz@balewski.eu
 *   Organization:  balewski.eu
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

void verify_number_of_parameters(int argc, int should_be) {
	if (argc < should_be) {
		printf("Niepoprawna liczba parametrow\n");
		exit(0);
	}
}

char *replace_str(char *str, char *orig, char *rep) {
	static char buffer[4096];
	char *p;

	if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
		return str;
	strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
	buffer[p-str] = '\0';
	sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));
	return buffer;
}
