#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include "product_utils.h"
#include "product.h"
#include "category_utils.h"
#include "category.h"

GtkBuilder *builder;
GtkWidget  *status_label;
GtkWidget  *entry;
GtkComboBox  *product_new_categories_cb;
GtkListStore *categories_store;
GtkListStore *products_store;
GtkTreeView *treeview_products;
GtkTreeSelection *treeview_products_selection;
GtkTreeIter iter;
char *old_product_name;
char *old_category_name, *new_category_name;

void print_status(int err_no, int data);
void fetch_category_list();
void fetch_product_list();
void select_combo(GtkComboBox *combo, char *item_text);
void select_list(GtkTreeView *tree_view, char *item_text);

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
			free(c->name);
			free(c);
		}
	}

	if (status == 0) {
		print_status(3, products_count);
	} else {
		print_status(3, -1);
	}
	free_product_list(list, products_count);
}

//
// Events
//
void on_window_destroy(GtkObject *object, gpointer data) {
	gtk_main_quit();
}

void on_product_new_add_button_clicked(GtkObject *object, gpointer data) {
	char *nazwa;
	nazwa = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
	if (strlen((char*)nazwa) == 0) {
		print_status(1, -1);
	} else {
		if (gtk_combo_box_get_active_iter(product_new_categories_cb, &iter)) {
			GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(product_new_categories_cb));
			int category_id;
			char *category = malloc(10);
			gtk_tree_model_get(model, &iter, 0, &category_id, -1);
			sprintf(category, "%d", category_id);
			char *args[] = {NULL, NULL, NULL, nazwa, category};
			__add_product(5, args);
			fetch_product_list();
			select_combo(product_new_categories_cb, "-");
			free(category);
		}
	}
}

void on_treeview_products_changed(GtkWidget *widget, gpointer window) {
	GtkTreeModel *model;
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(treeview_products_selection), &model, &iter)) {
		gtk_tree_model_get(model, &iter, 1, &old_product_name, -1);
		gtk_tree_model_get(model, &iter, 2, &old_category_name, -1);
		gtk_entry_set_text(GTK_ENTRY(entry), old_product_name);
		select_combo(product_new_categories_cb, old_category_name);
	}
}

void on_product_new_update_button_clicked(GtkWidget *widget, gpointer data) {
	char *msg = malloc(125);
	int id = 0;
	if ((id = gtk_combo_box_get_active(GTK_COMBO_BOX(product_new_categories_cb))) != -1) {
		char *cat_id = malloc(5);
		sprintf(cat_id, "%d", id);
		// creating args for products.__update_product function
		char *args[] = {NULL, NULL, NULL, old_product_name, (char*)gtk_entry_get_text(GTK_ENTRY(entry)), cat_id };
		int status = __update_product(6, args);
		if (status == 0) {
			print_status(4, status);
		} else {
			print_status(4, status);
		}
		free(cat_id);
	} else {
		print_status(5, 0);
	}
	free(msg);
	fetch_product_list();
	select_list(treeview_products, (char*)gtk_entry_get_text(GTK_ENTRY(entry)));
	gtk_entry_set_text(GTK_ENTRY(entry), "");
	select_combo(product_new_categories_cb, "-");
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
			g_print("Porownuje: %s i %s: ", item_text, current_text);
			end_search = g_strcmp0(current_text, item_text) == 0;
			g_print(end_search ? "OK\n" : "NO\n");
		} while (!end_search && gtk_tree_model_iter_next(model, &iter));
	}
	if (end_search == TRUE) {
		g_print("Znalazlem %s\n", item_text);
		gtk_tree_selection_select_iter(treeview_products_selection, &iter);
	} else {
		g_print("Nie znalazlem %s\n", item_text);
	}
	g_free(current_text);
}

void fetch_category_list() {
	int categories_count = get_categories_count();
	char *msg = malloc(125);
	print_status(6, categories_count);
	int counter = 0;
	Category *list[categories_count];
	int status = get_all_categories(list);
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

void init_components() {
	fetch_category_list();
	fetch_product_list();
}

int main(int argc, char *argv[]) {
	GtkWidget  *window;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "budget.glade", NULL);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	gtk_builder_connect_signals(builder, NULL);
	entry = GTK_WIDGET(gtk_builder_get_object(builder, "product_new_name_entry"));
	status_label = GTK_WIDGET(gtk_builder_get_object(builder, "status_label"));
	product_new_categories_cb = GTK_COMBO_BOX(gtk_builder_get_object(builder, "product_new_category_combobox"));
	categories_store = GTK_LIST_STORE(gtk_builder_get_object(builder, "categories_store"));
	products_store = GTK_LIST_STORE(gtk_builder_get_object(builder, "products_store"));
	treeview_products = GTK_TREE_VIEW(gtk_builder_get_object(builder, "treeview_products"));
	treeview_products_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_products)); //GTK_TREE_SELECTION(gtk_builder_get_object(builder, "treeview_products_selection"));
	g_signal_connect(treeview_products_selection, "changed", G_CALLBACK(on_treeview_products_changed), NULL);

	init_components();
	g_object_unref(G_OBJECT(GTK_BUILDER(builder)));

	gtk_widget_show(window);
	gtk_main();

	g_free(old_product_name);

	return 0;
}

void print_status(int err_no, int data) {
	char *msg = malloc(125);

	switch (err_no) {
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
		default:
						msg = "";
	}
	gtk_label_set_markup(GTK_LABEL(status_label), msg);
}

