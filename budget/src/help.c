#include <stdio.h>
#include <stdlib.h>
#include "help.h"

void __show_common_help() {
	puts("\nUżycie:");
	puts("\tbudget-cli [moduł] [operacja]");
	puts("\n\tModuły:");
	puts("\t\tcategory / cat / c");
	puts("\t\texpense / exp / e");
	puts("\t\tproduct / prod / p");
	puts("\t\tuser / u");
	puts("\n\tŻeby uzyskać szczegółowe komendy dla modułu, uruchom:\n\tbudget modul help\n");
	exit(0);
}

void __show_help_for_product_module() {
	puts("\ndostępne komendy:");
	puts("\tadd lub a [nazwa]");
	puts("\tupdate lub upd lub u [istniejąca nazwa] [nowa nazwa]");
	puts("\tdelete lub del lub d [nazwa]");
	puts("\tatc [identyfikator kategorii]");
	puts("\tlc [identyfikator kategorii]");
	puts("\tlist lub ls\n");
}

void __show_help_for_expense_module() {
	puts("\ndostępne komendy:");
	puts("\tadd lub a [data] [id produktu] [id sklepu] [ilość] [wartość]");
	puts("\tupdate lub upd lub u [id] [data] [id produktu] [id sklepu] [ilość] [wartość]");
	puts("\tdelete lub del lub d [id]");
	puts("\tlist lub ls\n");
}

void __show_help_for_category_module() {
	puts("\ndostępne komendy:");
	puts("\tadd lub a [nazwa]");
	puts("\tupdate lub upd lub u [istniejąca nazwa] [nowa nazwa]");
	puts("\tdelete lub del lub d [nazwa]");
	puts("\tlist lub ls\n");
}

void __show_help_for_user_module() {
	puts("\ndostępne komendy:");
	puts("\tadd lub a [login] [hasło]");
	puts("\tupdate lub upd lub u [istniejący login] [nowy login] [nowe hasło]");
	puts("\tdelete lub del lub d [login]");
	puts("\tlogin lub l [login] [hasło]");
	puts("\tlist lub ls");
	puts("\tgetbyid lub gid [identyfikator użytkownika]");
	puts("\tgetbylogin lub gl [identyfikator użytkownika]\n");
}

void help(char *h) {
	if (strcmp(h, "main") == 0) {
		__show_common_help();
	}
}
