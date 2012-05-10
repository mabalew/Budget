################################################################################
#  file: Makefile                                                              #
#  app: Budget                                                                 #
#  author: Mariusz Balewski <mariusz@balewski.eu>                              #
################################################################################
################################################################################
#                            DISCLAIMER                                        # 
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES,   #
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND #
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE       #
# REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,      #
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, #
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;  #
# OR BUSINESS INTERRUPTION)                                                    #
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT   #
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    #
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH  #
# DAMAGE.                                                                      #
################################################################################
################################################################################
#                            CONFIGURATION                                     # 
###                                                                            # 
# Configure installation parameters                                            #
#                                                                              #
# Set target path for installation (ie. /usr/local or C:\Progra~1\Budget       #
TARGET_PATH=/home/mabalew/Programy/C/budget/dist
###                                                                            #
# Configure language                                                           #
#                                                                              #
# Select language for initial data inserted into database.                     #
# Available options are:                                                       #
# pl - Polish                                                                  #
# en - English                                                                 #
LANGUAGE=pl
#                                                                              #
################################################################################

###############################
# Do not edit below this line #
###############################
CC=gcc
OPTS=-lsqlite3 -g3 -O0 -Wall 
GTK_OPTS=`pkg-config --cflags --libs gtk+-2.0` -export-dynamic
LIBBUDGET_NAME=libbudget

all: install-dev

#bin: budget budget-cli budget-gui report.o
bin: set-db-path budget-gui

copy-bin: bin create-dirs
	cp budget.glade budget-gui $(TARGET_PATH)/bin

#cp budget budget-cli budget.glade budget-gui $(TARGET_PATH)

copy-db: create-db
	mv budget.db $(HOME)/.budget/
	mv logs.db $(HOME)/.budget/

create-dirs:
	@if [ ! -d $(TARGET_PATH) ]; then \
		mkdir $(TARGET_PATH); \
	fi
	@if [ ! -d $(TARGET_PATH)/bin ]; then \
		mkdir $(TARGET_PATH)/bin; \
	fi
	@if [ ! -d $(HOME)/.budget ]; then \
		mkdir $(HOME)/.budget; \
	fi

install: bin copy-bin create-db copy-db insert-example-data

install-dev: bin copy-bin

uninstall:
	rm -f $(TARGET_PATH)/bin/budget-gui
	rm -f $(TARGET_PATH)/bin/budget.glade
#	rm -f $(TARGET_PATH)/bin/budget
#	rm -f $(TARGET_PATH)/bin/budget-cli
#	rm -f $(TARGET_PATH)/report

uninstall-purge: uninstall
	rm -r $(HOME)/.budget

budget-cli: lib
	$(CC) budget-cli.c -o budget-cli $(LIBBUDGET_NAME).a $(OPTS)

budget-gui: lib main.c 
	$(CC) main.c -o budget-gui $(LIBBUDGET_NAME).a $(GTK_OPTS) $(OPTS)

lib: config.o db.o log.o utils.o help.o product.o string_verifier.o product_utils.o category.o category_utils.o shop.o shop_utils.o expense.o expense_utils.o user.o user_utils.o category.o category_utils.o report.o
	ar -cvq $(LIBBUDGET_NAME).a *.o
	rm -f *.o

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

report.o: report.c
	$(CC) -c report.c -o report.o $(OPTS)

create-db: db.sql
	sqlite3 budget.db < db.sql
	sqlite3 logs.db < logs.sql

insert-example-data: create-db
	@if [ -e example/categories_$(LANGUAGE).sql ]; then \
		sqlite3 $(HOME)/.budget/budget.db < example/categories_$(LANGUAGE).sql; \
	fi

clean:
	rm -rf *.o budget-cli budget-gui budget budget.db $(LIBBUDGET_NAME).a

set-db-path:
	cat db.h | sed 's!DB_FILE "db!DB_FILE "$(HOME)/.budget!' > db2.h
	cat db2.h | sed 's!LOG_DB_FILE "db!LOG_DB_FILE "$(HOME)/.budget!' > db3.h
	cp db3.h db.h
	rm db2.h db3.h

#define LOG_DB_FILE "db/logs.db"
