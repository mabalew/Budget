#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "help.h"
#include "string_verifier.h"
#include "user.h"
#include "user_utils.h"

void __add_user(char *argv[]) {
	User *u = (User*)malloc(sizeof u);
	u->nick = argv[3];
	u->password = argv[4];
	add_user(u);
	free(u);
}

void __update_user(char *argv[]) {
	User *uo;
	User *un;
	uo = (User *)malloc(sizeof uo);
	un = (User *)malloc(sizeof un);
	uo->nick = (char *)malloc(sizeof (char));
	un->nick = (char *)malloc(sizeof (char));
	un->password = (char *)malloc(sizeof (char));
	uo->nick = argv[3];
	un->nick = argv[4];
	un->password = argv[5];
	update_user(uo, un);
	free(uo);
	free(un);
}

void __del_user(char *argv[]) {
	User *u = (User *) malloc(sizeof u);
	u->nick = (char *)malloc(sizeof(char));
	u->password = (char *)malloc(sizeof(char));
	u->nick = argv[3];
	del_user(u);
	free(u);
}

// Result is in argv[5]
void __login(char *argv[]) {
	User *u = (User *)malloc(sizeof u);
	u->nick = argv[3];
	u->password = argv[4];
	if (login(u)) {
		argv[5] = "OK";
	} else {
		argv[5] = "ERROR";
	}
}

void __list_users() {
	int users_count = get_users_count();
	int counter = 0;
	User *list[users_count];
	puts("==================================");
	printf("Znaleziono %d\n", users_count);
	puts("==================================");
	get_all_users(list);
	if (users_count != 0) {
		for (counter = 0; counter < users_count; counter++) {
			printf("\tid: %d\n", list[counter]->id);
			printf("\tlogin: %s\n", list[counter]->nick);
			printf("\tpassword: %s\n", list[counter]->password);
			puts("----------------------------------");
		}
	}
	puts("==================================");
}

void __get_user_by_id(char *argv[]) {
	User *p;
	p = (User*)malloc(sizeof p);
	p->nick = (char *)malloc(sizeof (char));
	p->password = (char *)malloc(sizeof (char));
	p->id = atoi(argv[3]);
	get_user_by_id(p);
	printf("login: %s | pass: %s\n", p->nick, p->password);
	free(p);
}

void __get_user_by_login(char *argv[]) {
	User *u;
	u = (User*)malloc(sizeof (User));
	u->nick = (char *)malloc(sizeof (char));
	u->password = (char *)malloc(sizeof(char));
	u->nick = argv[3];
	get_user_by_login(u);
	printf("login: %s | pass: %s\n", u->nick, u->password);
	free(u);
}

void manage_user(int argc, char *argv[]) {
	if (argc == 2) {
		__show_common_help();
	}
	if (is_update_string(argv[2])) {
		__update_user(argv);
	} else if (is_add_string(argv[2])) {
		__add_user(argv);
	} else if (is_delete_string(argv[2])) {
		__del_user(argv);
	} else if (is_login_string(argv[2])) {
		__login(argv);
	} else if (is_list_string(argv[2])) {
		__list_users();
	} else if (is_get_by_id_string(argv[2])) {
		__get_user_by_id(argv);
	} else if (is_get_by_login_string(argv[2])) {
		__get_user_by_login(argv);
	} else if (is_help_for_user_module(argv[2])) {
		__show_help_for_user_module();
	} else {
		__show_common_help();
	}
}
