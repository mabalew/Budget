#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "product.h"
#include "product_utils.h"
#include "expense.h"
#include "expense_utils.h"
#include "user.h"
#include "user_utils.h"
#include "category.h"
#include "category_utils.h"
#include "shop.h"
#include "shop_utils.h"
#include "string_verifier.h"
#include "help.h"


int main(int argc, char *argv[]) {
	if (argc == 1) {
		help("main");
	}
	if (strcmp(argv[1], "u") == 0 || strcmp(argv[1], "user") == 0) {
		manage_user(argc, argv);
	} else if (strcmp(argv[1], "c") == 0 || strcmp(argv[1], "cat") == 0 ||
			strcmp(argv[1], "category") == 0) {
		manage_category(argc, argv);
	} else if (strcmp(argv[1], "p") == 0 || strcmp(argv[1], "prod") == 0 ||
			strcmp(argv[1], "product") == 0) {
		return manage_product(argc, argv);
	} else if (strcmp(argv[1], "s") == 0 || strcmp(argv[1], "shop") == 0) {
		manage_shop(argc, argv);
	} else if (strcmp(argv[1], "e") == 0 || strcmp(argv[1], "exp") == 0 || strcmp(argv[1], "expense") == 0) {
		manage_expense(argc, argv);
	} else if (strcmp(argv[1], "h")  == 0 || strcmp(argv[1], "help") == 0) {
		help("main");
	}
	return 0;
}
