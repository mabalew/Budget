CC=gcc
OPTS=-lsqlite3 -g3 -O0 -Wall 
GTK_OPTS=`pkg-config --cflags --libs gtk+-2.0` -export-dynamic
TARGET_PATH=..

all: install

bin: budget budget-cli budget-gui report.o

copy: bin
	cp budget budget-cli budget.glade budget-gui $(TARGET_PATH)

install: bin copy
	mkdir -p $(TARGET_PATH)/db

uninstall:
	rm -f $(TARGET_PATH)/budget
	rm -f $(TARGET_PATH)/budget-cli
	rm -f $(TARGET_PATH)/budget-gui
#	rm -f $(TARGET_PATH)/report

uninstall-purge: uninstall
	rm -rf $(TARGET_PATH)/db

budget: budget-cli budget.c
	$(CC) budget.c $(OPTS) -lncurses -o budget
				
budget-cli: config.o db.o log.o utils.o user.o category.o user_utils.o category_utils.o \
				string_verifier.o help.o product.o product_utils.o report.o\
				shop.o shop_utils.o expense.o expense_utils.o \
				budget-cli.c 
	$(CC) config.o db.o log.o utils.o user.o category.o user_utils.o category_utils.o string_verifier.o help.o product.o product_utils.o shop.o expense.o expense_utils.o shop_utils.o report.o budget-cli.c -o budget-cli $(OPTS)

budget-gui: config.o log.o utils.o help.o product.o product_utils.o category.o category_utils.o report.o shop.o shop_utils.o budget main.c 
	$(CC) config.o log.o db.o utils.o help.o string_verifier.o product.o product_utils.o category.o category_utils.o report.o shop.o shop_utils.o expense.c main.c -o budget-gui $(GTK_OPTS) $(OPTS)

user.o: db.o user.c user.h
	$(CC) -c user.c -o user.o $(OPTS)

user_utils.o: user.o user_utils.h
	$(CC) -c user_utils.c -o user_utils.o $(OPTS)

category.o: db.o category.c category.h
	$(CC) -c category.c -o category.o $(OPTS)

category_utils.o: category.o category_utils.c category_utils.h
	$(CC) -c category_utils.c -o category_utils.o $(OPTS)

product.o: db.o product.c product.h
	$(CC) -c product.c -o product.o $(OPTS)

product_utils.o: product.o product_utils.c product_utils.h
	$(CC) -c product_utils.c -o product_utils.o $(OPTS)

shop.o: db.o shop.c shop.h
	$(CC) -c shop.c -o shop.o $(OPTS)

shop_utils.o: shop.o shop_utils.c shop_utils.h
	$(CC) -c shop_utils.c -o shop_utils.o $(OPTS)

db.o: utils.o db.c db.h
	$(CC) -c db.c -o db.o $(OPTS)

expense.o: db.o expense.c expense.h db.o utils.o
	$(CC) -c expense.c -o expense.o $(OPTS)

expense_utils.o: expense_utils.c expense_utils.h
	$(CC) -c expense_utils.c -o expense_utils.o $(OPTS)

report.o: db.o report.c config.o log.o db.o expense_utils.o expense.o utils.o
	$(CC) db.o config.o log.o string_verifier.o help.o utils.o expense_utils.o expense.o -c report.c -o report.o $(OPTS)

string_verifier.o: string_verifier.c string_verifier.h
	$(CC) -c string_verifier.c -o string_verifier.o $(OPTS)

utils.o: utils.c utils.h
	$(CC) -c utils.c -o utils.o $(OPTS)

config.o: config.c
	$(CC) -c config.c -o config.o $(OPTS)

log.o: log.c db.o config.o
	$(CC) config.o db.o -c log.c -o log.o $(OPTS)

help.o: help.c
	$(CC) -c help.c -o help.o $(OPTS)

create_db: db.sql
	sqlite3 budget.db < db.sql

clean:
	rm -rf *.o budget-cli budget-gui budget report budget.db
