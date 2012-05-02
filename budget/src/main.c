#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <time.h>
#include "product_utils.h"
#include "product.h"
#include "category_utils.h"
#include "category.h"
#include "shop.h"
#include "shop_utils.h"
#include "log.h"
#include "report.h"
#include "expense_utils.h"
#include "expense.h"
#include "config.h"

GtkBuilder *builder;
GtkMenu *exp_tmp_context_menu;
GtkMenuItem *exp_tmp_menuitem_delete;
GtkLabel  *status_label, *err_label, *avg_value_label, *shops_max_price_label, *shops_min_price_label, *sum_month_label, *sum_year_label;
GtkButton *min_button, *max_button, *delete_new_expense_button;
GtkWidget  *product_new_name_entry, *category_new_name_entry, *shop_new_name_entry, *count_entry, *amount_entry, *price_entry;
GtkComboBox  *product_new_categories_cb, *exp_categories_combo, *exp_shops_combo, *exp_products_combo;
GtkListStore *products_store, *categories_store, *shops_store, *monthly_expenses_store, *yearly_expenses_store, *shopping_list_store;
GtkTreeView *treeview_products, *treeview_categories, *treeview_shops, *treeview_monthly_exp, *treeview_yearly_exp, *treeview_shopping_list;
GtkTreeSelection *treeview_products_selection, *treeview_categories_selection, *treeview_shops_selection, *treeview_monthly_exp_selection, *treeview_yearly_exp_selection, *treeview_shopping_list_selection;
GtkTreeIter iter;
Expense **max_expenses, **min_expenses;
int max_expenses_count, min_expenses_count, selected_category_id=0;
double max_price, min_price, avg_price;
char *old_product_name, *old_category_name, *new_category_name, *old_shop_name, *new_shop_name;

void view_exp_tmp_context_menu();
void on_exp_tmp_menuitem_delete_activate(GtkWidget *widget, GdkEvent *event);
void add_tmp_expense_to_table(Expense *e);
void create_expense_from_form(Expense *e);
int get_id_from_combo(GtkComboBox *combo, int *id);
void on_category_add_button_clicked(GtkWidget *widget, gpointer data);
void on_category_update_button_clicked(GtkWidget *widget, gpointer data);
void on_category_delete_button_clicked(GtkWidget *widget, gpointer data);
void on_treeview_categories_changed(GtkWidget *widget, gpointer window);
void on_treeview_shops_changed(GtkWidget *widget, gpointer window);
void print_status(int msg_no, int data);
void fetch_product_list();
void fetch_shopping_list();
void fetch_category_list();
void fetch_shop_list();
void select_combo(GtkComboBox *combo, char *item_text);
void select_list(GtkTreeView *tree_view, char *item_text);
void process_max_price();
void process_min_price();
void process_avg_price();

void fetch_product_list() {
	int products_count = get_products_count(selected_category_id);
	print_status(2, -1);
	int counter = 0;
	int status = 0;
	Product *list[products_count];
	if (selected_category_id == 0) {
		status = get_all_products(list);
	} else {
		status = get_products_in_category(selected_category_id, list);
	}
		for (counter = 0; counter < products_count; counter++) {
			Category *c = malloc(sizeof(Category));
			c->id = 0;
			c->id = list[counter]->category_id;
			get_category_by_id(c);
		}
	gtk_list_store_clear(products_store);
	if (products_count != 0) {
		for (counter = 0; counter < products_count; counter++) {
			Category *c = malloc(sizeof(Category));
			c->id = 0;
			c->id = list[counter]->category_id;
			get_category_by_id(c);
			gtk_list_store_append(products_store, &iter);
			gtk_list_store_set(products_store, &iter, 0, list[counter]->id, 1, list[counter]->name, 2, c->name, 3, 0.0, -1);
		}
	}

	if (status == 0) {
		print_status(3, products_count);
	} else {
		print_status(3, -1);
	}
	if (products_count > 0 && list != NULL) {
		free_product_list(list, products_count);
	}
	products_count = 0;
}

void fetch_category_list() {
	int categories_count = get_categories_count();
	char *msg = malloc(125);
	print_status(6, categories_count);
	int counter = 0;
	Category *list[categories_count];
	int status = get_all_categories(list);
	gtk_list_store_clear(categories_store);
	if (categories_count != 0 && NULL != list[0]) {
		for (counter = 0; counter < categories_count; counter++) {
			gtk_list_store_append(categories_store, &iter);
			gtk_list_store_set(categories_store, &iter, 0, list[counter]->id, 1, list[counter]->name, -1);
		}
	}
	if (status == 0) {
		print_status(7, categories_count);
	} else {
		print_status(7, -1);
	}
	free_category_list(list, categories_count);
	free(msg);
}

void fetch_shop_list() {
	int shops_count = get_shops_count();
	char *msg = malloc(125);
	print_status(6, shops_count);
	int counter = 0;
	Shop *list[shops_count];
	int status = get_all_shops(list);
	gtk_list_store_clear(shops_store);
	if (shops_count != 0 && NULL != list[0]) {
		for (counter = 0; counter < shops_count; counter++) {
			gtk_list_store_append(shops_store, &iter);
			gtk_list_store_set(shops_store, &iter, 0, list[counter]->id, 1, list[counter]->name, -1);
		}
	}
	if (status == 0) {
		print_status(11, shops_count);
	} else {
		print_status(11, -1);
	}
	free_shop_list(list, shops_count);
	free(msg);
}

void fetch_monthly_exp_list() {
	struct tm *tm;
	time_t t;
	char year[10];
	char month[5];
	t = time(NULL);
	tm = localtime(&t);
	strftime(year, sizeof(year), "%Y", tm);
	strftime(month, sizeof(month), "%m", tm);
	int rows_count = count_monthly_categories(year, month);
	char *msg = malloc(125);
	print_status(6, rows_count);
	int counter = 0;
	float sum = 0.0;
	Row *list[rows_count];
	fetch_monthly_report("2012", "02", list);
	gtk_list_store_clear(monthly_expenses_store);
	if (rows_count != 0 && NULL != list[0]) {
		for (counter = 0; counter < rows_count; counter++) {
			gtk_list_store_append(monthly_expenses_store, &iter);
			gtk_list_store_set(monthly_expenses_store, &iter, 0, list[counter]->category, 1, list[counter]->value, -1);
			sum += list[counter]->value;
		}
	}
	char *c_sum = malloc(30);
	sprintf(c_sum, "<b>Razem: %.2f</b>", sum);
	gtk_label_set_markup(sum_month_label, c_sum);
	free(c_sum);
	free_rows_list(list, rows_count);
	free(msg);
}

void fetch_yearly_exp_list() {
	int rows_count = count_yearly_categories("2012");
	char *msg = malloc(125);
	print_status(6, rows_count);
	int counter = 0;
	float sum = 0.0;
	Row *list[rows_count];
	fetch_yearly_report("2012", list);
	gtk_list_store_clear(yearly_expenses_store);
	if (rows_count != 0 && NULL != list[0]) {
		for (counter = 0; counter < rows_count; counter++) {
			gtk_list_store_append(yearly_expenses_store, &iter);
			gtk_list_store_set(yearly_expenses_store, &iter, 0, list[counter]->category, 1, list[counter]->value, -1);
			sum += list[counter]->value;
		}
	}
	char *c_sum = malloc(30);
	sprintf(c_sum, "<b>Razem: %.2f</b>", sum);
	gtk_label_set_markup(sum_year_label, c_sum);
	free(c_sum);
	free_rows_list(list, rows_count);
	free(msg);
}


//
// Events
//
void on_window_destroy(GtkObject *object, gpointer data) {
	gtk_main_quit();
}

void on_product_new_add_button_clicked(GtkObject *object, gpointer data) {
	char *nazwa;
	nazwa = g_strdup(gtk_entry_get_text(GTK_ENTRY(product_new_name_entry)));
	if (strlen((char*)nazwa) == 0) {
		print_status(1, -1);
	} else {
		int category_id;
		if (get_id_from_combo(product_new_categories_cb, &category_id) == 0) {
			_add_product(nazwa, category_id);
			fetch_product_list();
			select_combo(product_new_categories_cb, "-");
		}
	}
}

// Gets id (int value) from the combo given as parameter.
// Returns 0 in case of success, 1 otherwise.
int get_id_from_combo(GtkComboBox *combo, int *id) {
		if (gtk_combo_box_get_active_iter(combo, &iter)) {
			GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo));
			gtk_tree_model_get(model, &iter, 0, id, -1);
		} else {
			return 1;
		}
		return 0;
}

void on_treeview_products_changed(GtkWidget *widget, gpointer window) {
	GtkTreeModel *model;
	int product_id;
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(treeview_products_selection), &model, &iter)) {
		gtk_tree_model_get(model, &iter, 0, &product_id, -1);
		gtk_tree_model_get(model, &iter, 1, &old_product_name, -1);
		gtk_tree_model_get(model, &iter, 2, &old_category_name, -1);
		gtk_entry_set_text(GTK_ENTRY(product_new_name_entry), old_product_name);
		select_combo(product_new_categories_cb, old_category_name);
		max_expenses = find_max_price(product_id, &max_price, &max_expenses_count);
		min_expenses = find_min_price(product_id, &min_price, &min_expenses_count);
		avg_price = find_avg_price(product_id);

		process_max_price();
		process_min_price();
		process_avg_price();

		product_id = 0;

	}
}

void process_min_price() {
		int i=0, size=0;
		char *min_char = malloc(2048);
		char *null = "\0";
		char *_min_price = malloc(10);
		sprintf(_min_price, "%.2f", min_price);
		strcpy(min_char, null);
		if (min_expenses_count > 0) { // do we have any expenses with the product?
			for (i=0; i<min_expenses_count; i++) {
				// getting length of names of all shop with min price to allocate memory
				size += strlen(min_expenses[i]->shop);
			}
			strcat(min_char, "Najtaniej w: ");
			for (i=0; i<min_expenses_count; ++i) {
				min_char = strcat(min_char, min_expenses[i]->shop);
				min_char = strcat(min_char, ", ");
			}
		}
		if (min_expenses_count > 0 && strcmp(min_char, "") != 0) {
			gtk_label_set_markup(GTK_LABEL(shops_min_price_label), min_char);
			gtk_button_set_label(GTK_BUTTON(min_button), _min_price);
			gtk_widget_set_sensitive(GTK_WIDGET(min_button), TRUE);
		} else {
			gtk_label_set_markup(GTK_LABEL(shops_min_price_label), "-");
			gtk_button_set_label(GTK_BUTTON(min_button), "-");
			gtk_widget_set_sensitive(GTK_WIDGET(min_button), FALSE);
		}
		free(min_char);
		min_expenses = NULL;
		min_expenses_count = 0;
}

void process_max_price() {
		int i=0, size=0;
		char *max_char = malloc(2048);
		char *null = "\0";
		char *_max_price = malloc(10);
		sprintf(_max_price, "%.2f", max_price);
		strcpy(max_char, null);
		if (max_expenses_count > 0) { // do we have any expenses with the product?
			for (i=0; i<max_expenses_count; i++) {
				// getting length of names of all shop with max price to allocate memory
				size += strlen(max_expenses[i]->shop);
			}
			strcat(max_char, "Najdrożej w: ");
			for (i=0; i<max_expenses_count; ++i) {
				max_char = strcat(max_char, max_expenses[i]->shop);
				max_char = strcat(max_char, ", ");
			}
		}
		if (max_expenses_count > 0 && strcmp(max_char, "") != 0) {
			gtk_label_set_markup(GTK_LABEL(shops_max_price_label), max_char);
			gtk_button_set_label(GTK_BUTTON(max_button), _max_price);
			gtk_widget_set_sensitive(GTK_WIDGET(max_button), TRUE);
		} else {
			gtk_label_set_markup(GTK_LABEL(shops_max_price_label), "-");
			gtk_button_set_label(GTK_BUTTON(max_button), "-");
			gtk_widget_set_sensitive(GTK_WIDGET(max_button), FALSE);
		}
		free(max_char);
		max_expenses = NULL;
		max_expenses_count = 0;
}

void process_avg_price() {
	char *_avg_price = malloc(100);
	sprintf(_avg_price, "%.2f", avg_price);
	gtk_label_set_markup(GTK_LABEL(avg_value_label), _avg_price);
	free(_avg_price);
}

void on_product_new_update_button_clicked(GtkWidget *widget, gpointer data) {
	int category_id;
	if (get_id_from_combo(GTK_COMBO_BOX(product_new_categories_cb), &category_id) == 0) {
		char *new_product_name = (char*)gtk_entry_get_text(GTK_ENTRY(product_new_name_entry));
		int status = _update_product(old_product_name, new_product_name, category_id);
		if (status == 0) {
			print_status(4, status);
		} else {
			print_status(4, status);
		}
	} else {
		print_status(5, 0);
	}
	fetch_product_list();
	select_list(treeview_products, (char*)gtk_entry_get_text(GTK_ENTRY(product_new_name_entry)));
	gtk_entry_set_text(GTK_ENTRY(product_new_name_entry), "");
	select_combo(product_new_categories_cb, "-");
	gtk_label_set_markup(GTK_LABEL(shops_max_price_label), "");
	gtk_label_set_markup(GTK_LABEL(shops_min_price_label), "");
	gtk_label_set_markup(GTK_LABEL(avg_value_label), "");
	gtk_button_set_label(GTK_BUTTON(max_button), "");
	gtk_button_set_label(GTK_BUTTON(min_button), "");
}

//
// End Events
//

void select_combo(GtkComboBox *combo, char *item_text) {
	gboolean end_search = FALSE;
	gchar *current_text = NULL;
	GtkTreeModel *model = gtk_combo_box_get_model(combo);
	if (gtk_tree_model_get_iter_first(model, &iter)) {
		do {
			gtk_tree_model_get(model, &iter, 1, &current_text, -1);
			end_search = g_strcmp0(current_text, item_text) == 0;
		} while (!end_search && gtk_tree_model_iter_next(model, &iter));
	}
	if (end_search == TRUE) {
		gtk_combo_box_set_active_iter(combo, &iter);
	}
	g_free(current_text);
}

void select_list(GtkTreeView *tree_view, char *item_text) {
	gboolean end_search = FALSE;
	gchar *current_text = NULL;
	GtkTreeModel *model = gtk_tree_view_get_model(tree_view);
	if (gtk_tree_model_get_iter_first(model, &iter)) {
		do {
			gtk_tree_model_get(model, &iter, 1, &current_text, -1);
			end_search = g_strcmp0(current_text, item_text) == 0;
		} while (!end_search && gtk_tree_model_iter_next(model, &iter));
	}
	if (end_search == TRUE) {
		gtk_tree_selection_select_iter(treeview_products_selection, &iter);
	}
	g_free(current_text);
}

void init_components() {
	fetch_product_list();
	fetch_category_list();
	fetch_shop_list();
	fetch_monthly_exp_list();
	fetch_yearly_exp_list();

	int last_selected_shop_is_set = 1;
	char *last_selected_shop = malloc(255);
	config_exists("LAST_SELECTED_SHOP", &last_selected_shop_is_set);
	if (last_selected_shop_is_set > 0) {
		get_config("LAST_SELECTED_SHOP", last_selected_shop);
		select_combo(exp_shops_combo, last_selected_shop);
	}
	fetch_shopping_list();
	free(last_selected_shop);
}

int main(int argc, char *argv[]) {
	GtkWidget  *window;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "budget.glade", NULL);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	gtk_builder_connect_signals(builder, NULL);
	product_new_name_entry = GTK_WIDGET(gtk_builder_get_object(builder, "product_new_name_entry"));
	category_new_name_entry = GTK_WIDGET(gtk_builder_get_object(builder, "category_new_name_entry"));
	shop_new_name_entry = GTK_WIDGET(gtk_builder_get_object(builder, "shop_new_name_entry"));
	amount_entry = GTK_WIDGET(gtk_builder_get_object(builder, "amount_entry"));
	count_entry = GTK_WIDGET(gtk_builder_get_object(builder, "count_entry"));
	price_entry = GTK_WIDGET(gtk_builder_get_object(builder, "price_entry"));
	status_label = GTK_LABEL(gtk_builder_get_object(builder, "status_label"));
	err_label = GTK_LABEL(gtk_builder_get_object(builder, "err_label"));
	shops_max_price_label = GTK_LABEL(gtk_builder_get_object (builder, "shops_max_price_label"));
	status_label = GTK_LABEL(gtk_builder_get_object(builder, "status_label"));
	err_label = GTK_LABEL(gtk_builder_get_object(builder, "err_label"));
	shops_max_price_label = GTK_LABEL(gtk_builder_get_object (builder, "shops_max_price_label"));
	shops_min_price_label = GTK_LABEL(gtk_builder_get_object (builder, "shops_min_price_label"));
	avg_value_label = GTK_LABEL(gtk_builder_get_object(builder, "avg_value_label"));
	min_button = GTK_BUTTON(gtk_builder_get_object(builder, "min_button"));
	max_button = GTK_BUTTON(gtk_builder_get_object (builder, "max_button"));
	product_new_categories_cb = GTK_COMBO_BOX(gtk_builder_get_object(builder, "product_new_category_combobox"));

	products_store = GTK_LIST_STORE(gtk_builder_get_object(builder, "products_store"));
	treeview_products = GTK_TREE_VIEW(gtk_builder_get_object(builder, "treeview_products"));
	treeview_products_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_products));
	g_signal_connect(treeview_products_selection, "changed", G_CALLBACK(on_treeview_products_changed), NULL);

	categories_store = GTK_LIST_STORE(gtk_builder_get_object(builder, "categories_store"));
	treeview_categories = GTK_TREE_VIEW(gtk_builder_get_object(builder, "treeview_categories"));
	treeview_categories_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_categories));
	g_signal_connect(treeview_categories_selection, "changed", G_CALLBACK(on_treeview_categories_changed), NULL);

	shops_store = GTK_LIST_STORE(gtk_builder_get_object(builder, "shops_store"));
	treeview_shops = GTK_TREE_VIEW(gtk_builder_get_object(builder, "treeview_shops"));
	treeview_shops_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_shops));
	g_signal_connect(treeview_shops_selection, "changed", G_CALLBACK(on_treeview_shops_changed), NULL);
	exp_shops_combo = GTK_COMBO_BOX(gtk_builder_get_object(builder, "exp_shops_combo"));

	monthly_expenses_store = GTK_LIST_STORE(gtk_builder_get_object(builder, "monthly_expenses_store"));
	treeview_monthly_exp = GTK_TREE_VIEW(gtk_builder_get_object(builder, "treeview_monthly_exp"));
	treeview_monthly_exp_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_monthly_exp));


	shopping_list_store = GTK_LIST_STORE(gtk_builder_get_object(builder, "shopping_list_store"));
	treeview_shopping_list = GTK_TREE_VIEW(gtk_builder_get_object(builder, "treeview_shopping_list"));
	treeview_shopping_list_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_shopping_list));

	yearly_expenses_store = GTK_LIST_STORE(gtk_builder_get_object(builder, "yearly_expenses_store"));
	treeview_yearly_exp = GTK_TREE_VIEW(gtk_builder_get_object(builder, "treeview_yearly_exp"));
	treeview_yearly_exp_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_yearly_exp));

	exp_categories_combo = GTK_COMBO_BOX(gtk_builder_get_object(builder, "exp_categories_combo"));
	exp_products_combo = GTK_COMBO_BOX(gtk_builder_get_object(builder, "exp_products_combo"));

	sum_month_label = GTK_LABEL(gtk_builder_get_object(builder, "sum_month_label"));
	sum_year_label = GTK_LABEL(gtk_builder_get_object(builder, "sum_year_label"));

	delete_new_expense_button = GTK_BUTTON(gtk_builder_get_object(builder, "delete_new_expense_button"));

	exp_tmp_context_menu = GTK_MENU(gtk_builder_get_object(builder, "exp_tmp_context_menu"));
	exp_tmp_menuitem_delete = GTK_MENU_ITEM(gtk_builder_get_object(builder, "exp_tmp_menuitem_delete"));

	init_components();
	g_object_unref(G_OBJECT(GTK_BUILDER(builder)));

	gtk_widget_show(window);
	gtk_main();

	g_free(old_product_name);

	return 0;
}

void print_status(int msg_no, int data) {
	char *msg = malloc(125);

	switch (msg_no) {
		case 1: msg = "<span foreground='red'><b>Wpisz nazwę</b></span>";
						break;
		case 2:	msg = "<span foreground='blue'><b>Pobieranie produktów</b></span>";
						break;
		case 3:	if (data > -1) {
							sprintf(msg, "<span foreground='blue'><b>Pobrano %d produktów</b></span>", data);
						} else {
							sprintf(msg, "<span foreground='red'><b>NIE pobrano listy produktów</b></span>");
						}
						break;
		case 4:	if (data > 0) {
							sprintf(msg, "<span foreground='blue'><b>Zaktualizowano produkt (status %d)</b></span>", data);
						} else {
							sprintf(msg, "<span foreground='red'><b>NIE zaktualizowano produktu (status %d)</b></span>", data);
						}
						break;
		case 5:	sprintf(msg, "<span foreground='red'><b>Wybierz kategorię</b></span>");
						break;
		case 6:	sprintf(msg, "<span foreground='blue'><b>Pobieranie %d kategorii</b></span>", data);
						break;
		case 7: if (data > -1) {
							sprintf(msg, "<span foreground='blue'><b>Pobrano %d kategorii</b></span>", data);
						} else {
							sprintf(msg, "<span foreground='red'><b>NIE pobrano listy kategorii</b></span>");
						}
						break;
		case 8: if (data == 0) {
							sprintf(msg, "<span foreground='blue'><b>Usunięto kategorię %s</b></span>", old_category_name);
						} else {
							sprintf(msg, "<span foreground='red'><b>Kategoria jest w użyciu</b></span>");
						}
						break;
		case 9: if (data == 0) {
							sprintf(msg, "<span foreground='blue'><b>Dodano kategorię %s</b></span>", new_category_name);
						} else if (data == 1) {
							sprintf(msg, "<span foreground='red'><b>Nie dodano %s</b></span>", new_category_name);
						}
						break;
		case 10: sprintf(msg, "<span foreground='red'><b>Podaj nazwę kategorii</b></span>");
						 break;
		case 11: if (data > -1) {
							sprintf(msg, "<span foreground='blue'><b>Pobrano %d sklepów</b></span>", data);
						} else {
							sprintf(msg, "<span foreground='red'><b>NIE pobrano listy sklepów</b></span>");
							gtk_label_set_markup(GTK_LABEL(err_label), msg);
						}
						 break;
		case 12: if (data == 0) {
							sprintf(msg, "<span foreground='blue'><b>Usunięto sklep %s</b></span>", old_shop_name);
						} else {
							sprintf(msg, "<span foreground='red'><b>Sklep jest w użyciu</b></span>");
						}
						break;
		case 13: if (data == 0) {
							sprintf(msg, "<span foreground='blue'><b>Dodano sklep %s</b></span>", new_shop_name);
						} else if (data == 1) {
							sprintf(msg, "<span foreground='red'><b>Nie dodano sklepu %s</b></span>", new_shop_name);
						}
						break;
		case 14: sprintf(msg, "<span foreground='red'><b>Podaj nazwę sklepu</b></span>");
						break;
		case 15: if (data > -1) {
							sprintf(msg, "<span foreground='blue'><b>Pobrano %d wydatków miesięcznych</b></span>", data);
						} else {
							sprintf(msg, "<span foreground='red'><b>NIE pobrano listy wydatków miesięcznych</b></span>");
							gtk_label_set_markup(GTK_LABEL(err_label), msg);
						}
						 break;
		case 16:	msg = "<span foreground='blue'><b>Pobieranie %d zakupów</b></span>";
						break;
		case 17:	if (data > -1) {
							sprintf(msg, "<span foreground='blue'><b>Pobrano %d zakupów</b></span>", data);
						} else {
							sprintf(msg, "<span foreground='red'><b>NIE pobrano listy zakupów</b></span>");
						}
						break;

		default:
						msg = "";
	}
	gtk_label_set_markup(GTK_LABEL(status_label), msg);
}

// CATEGORIES
void on_category_add_button_clicked(GtkWidget *widget, gpointer data) {
	new_category_name = (char*)gtk_entry_get_text(GTK_ENTRY(category_new_name_entry));
	if (strlen(gtk_entry_get_text(GTK_ENTRY(category_new_name_entry))) > 0) {
		int status = _add_category((char*)gtk_entry_get_text(GTK_ENTRY(category_new_name_entry)));
		if (status == 0) {
			fetch_category_list();
			gtk_entry_set_text(GTK_ENTRY(category_new_name_entry), "");
			print_status(9, 0);
		} else {
			print_status(9, 1);
		}
	} else {
		print_status(10, 0);
	}
}

void on_category_update_button_clicked(GtkWidget *widget, gpointer data) {
	new_category_name = (char*)gtk_entry_get_text(GTK_ENTRY(category_new_name_entry));
	if (strlen(new_category_name) > 0) {
		_update_category(old_category_name, new_category_name);
		fetch_category_list();
		gtk_entry_set_text(GTK_ENTRY(category_new_name_entry), "");
	}
}

void on_category_delete_button_clicked(GtkWidget *widget, gpointer data) {
	if (strlen(gtk_entry_get_text(GTK_ENTRY(category_new_name_entry))) > 0) {
		int status = _del_category((char*)gtk_entry_get_text(GTK_ENTRY(category_new_name_entry)));
		if (status == 0) {
			fetch_category_list();
			gtk_entry_set_text(GTK_ENTRY(category_new_name_entry), "");
			print_status(8, 0);
		} else {
			print_status(8, 1);
		}
	}
}

void on_treeview_categories_changed(GtkWidget *widget, gpointer window) {
	GtkTreeModel *model;
	int category_id;
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(treeview_categories_selection), &model, &iter)) {
		gtk_tree_model_get(model, &iter, 0, &category_id, -1);
		gtk_tree_model_get(model, &iter, 1, &old_category_name, -1);
		gtk_entry_set_text(GTK_ENTRY(category_new_name_entry), old_category_name);
		category_id = 0;
	}
}

// SHOPS
void on_shop_add_button_clicked(GtkWidget *widget, gpointer data) {
	new_shop_name = (char*)gtk_entry_get_text(GTK_ENTRY(shop_new_name_entry));
	if (strlen(gtk_entry_get_text(GTK_ENTRY(shop_new_name_entry))) > 0) {
		int status = _add_shop((char*)gtk_entry_get_text(GTK_ENTRY(shop_new_name_entry)));
		if (status == 0) {
			fetch_shop_list();
			gtk_entry_set_text(GTK_ENTRY(shop_new_name_entry), "");
			print_status(13, 0);
		} else {
			print_status(13, 1);
		}
	} else {
		print_status(14, 0);
	}
}

void on_shop_update_button_clicked(GtkWidget *widget, gpointer data) {
	new_shop_name = (char*)gtk_entry_get_text(GTK_ENTRY(shop_new_name_entry));
	if (strlen(new_shop_name) > 0) {
		_update_shop(old_shop_name, new_shop_name);
		fetch_shop_list();
		gtk_entry_set_text(GTK_ENTRY(shop_new_name_entry), "");
	}
}

void on_shop_delete_button_clicked(GtkWidget *widget, gpointer data) {
	if (strlen(gtk_entry_get_text(GTK_ENTRY(shop_new_name_entry))) > 0) {
		int status = _del_shop((char*)gtk_entry_get_text(GTK_ENTRY(shop_new_name_entry)));
		if (status == 0) {
			fetch_shop_list();
			gtk_entry_set_text(GTK_ENTRY(shop_new_name_entry), "");
			print_status(12, 0);
		} else {
			print_status(12, 1);
		}
	}
}

void on_treeview_shops_changed(GtkWidget *widget, gpointer data) {
	GtkTreeModel *model;
	int shop_id;
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(treeview_shops_selection), &model, &iter)) {
		gtk_tree_model_get(model, &iter, 0, &shop_id, -1);
		gtk_tree_model_get(model, &iter, 1, &old_shop_name, -1);
		gtk_entry_set_text(GTK_ENTRY(shop_new_name_entry), old_shop_name);
		shop_id = 0;
	}
}

// Expenses
void fetch_shopping_list() {
	int expenses_count = get_tmp_expenses_count(selected_category_id);
	print_status(16, -1);
	int counter = 0;
	Expense *list[expenses_count];
	gtk_list_store_clear(shopping_list_store);
	if (expenses_count != 0) {
		get_all_tmp_expenses(list);
		for (counter = 0; counter < expenses_count; counter++) {
			add_tmp_expense_to_table(list[counter]);
		}
	}
	//free_expense_list(list, expenses_count);
}

void on_exp_categories_combo_changed(GtkWidget *widget, gpointer data) {
	if (get_id_from_combo(exp_categories_combo, &selected_category_id) == 0) {
			fetch_product_list();
 	}
	fetch_product_list();
}

void on_exp_shops_combo_changed(GtkWidget *widget, gpointer data) {
	int count;
	char *last_selected_shop = malloc(255);
	config_exists("LAST_SELECTED_SHOP", &count);
	if (gtk_combo_box_get_active_iter(exp_shops_combo, &iter)) {
		GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(exp_shops_combo));
		gtk_tree_model_get(model, &iter, 1, &last_selected_shop, -1);
	}	
	if (count > 0) {
		update_config("LAST_SELECTED_SHOP", last_selected_shop);
	} else {
		add_config("LAST_SELECTED_SHOP", last_selected_shop);
	}
	free(last_selected_shop);
}

void on_add_new_expense_button_clicked(GtkWidget *widget, gpointer data) {
	Expense *e = malloc(sizeof (Expense));
	e->category = malloc(255);
	create_expense_from_form(e);
	int id;
	add_tmp_expense(e, &id);
	e->id = id;
	add_tmp_expense_to_table(e);
}

void add_tmp_expense_to_table(Expense *e) {
	gtk_list_store_append(shopping_list_store, &iter);
	gtk_list_store_set(shopping_list_store, &iter, 0, e->id, 1, e->product_id, 2, e->product, 3, e->count, 4, e->amount, 5, e->price, 6, e->category_id, 7, e->category, 8, e->shop_id, 9, e->shop, -1);
}

void create_expense_from_form(Expense *e) {
	get_id_from_combo(exp_categories_combo, &(e->category_id));
	GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(exp_categories_combo));
	gtk_tree_model_get(model, &iter, 1, &(e->category), -1);
	get_id_from_combo(exp_products_combo, &(e->product_id));
	model = gtk_combo_box_get_model(GTK_COMBO_BOX(exp_products_combo));
	gtk_tree_model_get(model, &iter, 1, &(e->product), -1);
	get_id_from_combo(exp_shops_combo, &(e->shop_id));
	model = gtk_combo_box_get_model(GTK_COMBO_BOX(exp_shops_combo));
	gtk_tree_model_get(model, &iter, 1, &(e->shop), -1);
	char *tmp;
	tmp = g_strdup(gtk_entry_get_text(GTK_ENTRY(count_entry)));
	e->count = atoi(tmp);
	tmp = g_strdup(gtk_entry_get_text(GTK_ENTRY(amount_entry)));
	e->amount = atof(tmp);
	tmp = g_strdup(gtk_entry_get_text(GTK_ENTRY(price_entry)));
	e->price = atof(tmp);
}

void create_expense_from_list(Expense *e) {
	GtkTreeModel *model;
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(treeview_shopping_list_selection), &model, &iter)) {
		e->product = malloc(255);
		e->category = malloc(255);
		e->shop = malloc(255);
		gtk_tree_model_get(model, &iter, 0, &(e->id), -1);
		gtk_tree_model_get(model, &iter, 1, &(e->product_id), -1);
		gtk_tree_model_get(model, &iter, 2, &(e->product), -1);
		gtk_tree_model_get(model, &iter, 3, &(e->count), -1);
		gtk_tree_model_get(model, &iter, 4, &(e->amount), -1);
		gtk_tree_model_get(model, &iter, 5, &(e->price), -1);
		gtk_tree_model_get(model, &iter, 6, &(e->category_id), -1);
		gtk_tree_model_get(model, &iter, 7, &(e->category), -1);
		gtk_tree_model_get(model, &iter, 8, &(e->shop_id), -1);
		gtk_tree_model_get(model, &iter, 9, &(e->shop), -1);
	}
}

void on_count_cell_edited() {
	GtkTreeModel *model;
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(treeview_shopping_list_selection), &model, &iter)) {
		int count = 0;
		gtk_tree_model_get(model, &iter, 3, &count, -1);
		Expense *e = malloc(sizeof(Expense));
		create_expense_from_list(e);
		gtk_list_store_set(GTK_LIST_STORE(shopping_list_store), &iter, 3, e->count, NULL);
		update_tmp_count(e);
		free(e);
	}
}

void on_clear_tmp_expenses_button_clicked() {
	del_all_tmp_expenses();
	gtk_list_store_clear(shopping_list_store);
}

void on_save_expense_button_clicked() {
	copy_tmp_expenses_to_expenses();
	del_all_tmp_expenses();
	gtk_list_store_clear(shopping_list_store);
}

void on_exp_tmp_menuitem_delete_activate(GtkWidget *widget, GdkEvent *event) {}
	/*
	if (event->type == GDK_BUTTON_PRESS) {
		GdkEventButton *event_button = (GdkEventButton*) event;
		if (event_button->button == 3) {
			gtk_menu_popup(GTK_MENU(widget), NULL, NULL, NULL, NULL, event_button->button, event_button->time);
		}
	}
}*/
