CC=gcc
OPTS=-lsqlite3 -g3 -O0

all: budget budget-cli report

budget: budget-cli budget.c
	$(CC) budget.c $(OPTS) -lncurses -o ../budget
				
budget-cli: db.o user.o category.o user_utils.o category_utils.o \
				string_verifier.o help.o product.o product_utils.o \
				shop.o shop_utils.o expense.o expense_utils.o \
				budget-cli.c 
	$(CC) db.o user.o category.o user_utils.o category_utils.o string_verifier.o help.o product.o product_utils.o shop.o expense.o expense_utils.o shop_utils.o budget-cli.c -o ../budget-cli $(OPTS)

user.o: db.o user.c user.h
	$(CC) -c user.c -o user.o $(OPTS)

user_utils.o: user.o user_utils.h
	$(CC) -c user_utils.c -o user_utils.o

category.o: db.o category.c category.h
	$(CC) -c category.c -o category.o $(OPTS)

category_utils.o: category.o category_utils.c category_utils.h
	$(CC) -c category_utils.c -o category_utils.o

product.o: db.o product.c product.h
	$(CC) -c product.c -o product.o $(OPTS)

product_utils.o: product.o product_utils.c product_utils.h
	$(CC) -c product_utils.c -o product_utils.o

shop.o: db.o shop.c shop.h
	$(CC) -c shop.c -o shop.o $(OPTS)

shop_utils.o: shop.o shop_utils.c shop_utils.h
	$(CC) -c shop_utils.c -o shop_utils.o

db.o: db.c db.h
	$(CC) -c db.c -o db.o $(OPTS)

expense.o: expense.c expense.h db.o
	$(CC) -c expense.c -o expense.o $(OPTS)

expense_utils.o: expense_utils.c expense_utils.h
	$(CC) -c expense_utils.c -o expense_utils.o $(OPTS)

report: report.c db.o expense_utils.o expense.o
	$(CC) string_verifier.o help.o expense_utils.o db.o expense.o report.c -o ../report $(OPTS)

string_verifier.o: string_verifier.c string_verifier.h
	$(CC) -c string_verifier.c -o string_verifier.o

help.o: help.c
	$(CC) -c help.c -o help.o $(OPTS)

clean:
	rm -f *.o ../budget-cli ../budget ../report
