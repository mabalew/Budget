#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "string_verifier.h"

bool is_add_string(char *s) {
	if (strcmp(s, "a") == 0 || strcmp(s, "add") == 0) {
		return true;
	}
	return false;
}

bool is_update_string(char *s) {
	if (strcmp(s, "u") == 0 || strcmp(s, "upd") == 0 || strcmp(s, "update") == 0) {
		return true;
	}
	return false;
}

bool is_delete_string(char *s) {
	if (strcmp(s, "d") == 0 || strcmp(s, "del") == 0 || strcmp(s, "delete") == 0) {
		return true;
	}
	return false;
}

bool is_login_string(char *s) {
	if (strcmp(s, "lo") == 0 || strcmp(s, "login") == 0) {
		return true;
	}
	return false;
}

bool is_list_string(char *s) {
	if (strcmp(s, "l") == 0 || strcmp(s, "ls") == 0 || strcmp(s, "list") == 0) {
		return true;
	}
	return false;
}

bool is_list_products_in_category_string(char *s) {
	if (strcmp(s, "lc") == 0) {
		return true;
	}
	return false;
}

bool is_get_by_id_string(char *s) {
	if (strcmp(s, "gid") == 0 || strcmp(s, "getbyid") == 0) {
		return true;
	}
	return false;
}

bool is_get_by_login_string(char *s) {
	if (strcmp(s, "gl") == 0 || strcmp(s, "getbylogin") == 0) {
		return true;
	}
	return false;
}

bool is_help_for_expense_module(char *s) {
	return is_help_string(s);
}

bool is_help_for_user_module(char *s) {
	return is_help_string(s);
}

bool is_help_for_category_module(char *s) {
	return is_help_string(s);
}

bool is_help_for_shop_module(char *s) {
	return is_help_string(s);
}

bool is_add_to_category_string(char *s) {
	if (strcmp(s, "atc") == 0) {
		return true;
	}
	return false;
}

bool is_help_string(char *s) {
	if (strcmp(s, "help") == 0) {
		return true;
	}
	return false;
}
