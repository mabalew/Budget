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
#include "utils.h"

void verify_number_of_parameters(int argc, int should_be) {
	if (argc < should_be) {
		printf("Niepoprawna liczba parametrow\n");
		exit(0);
	}
}

