#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include "product_utils.h"
#include "product.h"
#include "category_utils.h"
#include "category.h"
#include "shop.h"
#include "shop_utils.h"
#include "log.h"
#include "expense.h"

GtkBuilder *builder;
GtkLabel  *status_label, *err_label, *avg_value_label, *shops_max_price_label, *shops_min_price_label;
GtkButton *min_button, *max_button;
GtkWidget  *product_new_name_entry, *category_new_name_entry, *shop_new_name_entry;
GtkComboBox  *product_new_categories_cb;
GtkListStore *products_store, *categories_store, *shops_store;
GtkTreeView *treeview_products, *treeview_categories, *treeview_shops;
GtkTreeSelection *treeview_products_selection, *treeview_categories_selection, *treeview_shops_selection;
GtkTreeIter iter;
Expense **max_expenses, **min_expenses;
int max_expenses_count, min_expenses_count;
double max_price, min_price, avg_price;
char *old_product_name, *old_category_name, *new_category_name, *old_shop_name, *new_shop_name;

void on_category_add_button_clicked(GtkWidget *widget, gpointer data);
void on_category_update_button_clicked(GtkWidget *widget, gpointer data);
void on_category_delete_button_clicked(GtkWidget *widget, gpointer data);
void on_treeview_categories_changed(GtkWidget *widget, gpointer window);
void on_treeview_shops_changed(GtkWidget *widget, gpointer window);
void print_status(int msg_no, int data);
void fetch_product_list();
void fetch_category_list();
void fetch_shop_list();
void select_combo(GtkComboBox *combo, char *item_text);
void select_list(GtkTreeView *tree_view, char *item_text);
void process_max_price();
void process_min_price();
void process_avg_price();

void fetch_product_list() {
	int products_count = get_products_count(0);
	print_status(2, -1);
	int counter = 0;
	Product *list[products_count];
	int status = get_all_products(list);
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
	free_product_list(list, products_count);
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
		if (gtk_combo_box_get_active_iter(product_new_categories_cb, &iter)) {
			GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(product_new_categories_cb));
			int category_id;
			gtk_tree_model_get(model, &iter, 0, &category_id, -1);
			_add_product(nazwa, category_id);
			fetch_product_list();
			select_combo(product_new_categories_cb, "-");
		}
	}
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
		g_print("max: %.2f (count: %d), min: %.2f (count: %d), avg: %.2f\n", max_price, max_expenses_count, min_price, min_expenses_count, avg_price);

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
				g_print("%s\n", min_expenses[i]->shop);
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
				g_print("%s\n", max_expenses[i]->shop);
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
	char *msg = malloc(125);
	int id = 0;
	int category_id;
	if ((id = gtk_combo_box_get_active(GTK_COMBO_BOX(product_new_categories_cb))) != -1) {
		gtk_combo_box_get_active_iter(product_new_categories_cb, &iter);
		GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(product_new_categories_cb));
		gtk_tree_model_get(model, &iter, 0, &category_id, -1);
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
	free(msg);
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
		case 2:	msg = "<span foreground='blue'><b>Pobieranie %d produktów</b></span>";
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
