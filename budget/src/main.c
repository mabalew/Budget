#include <gtk/gtk.h>
#include <string.h>

GtkBuilder *builder;

void print_error(int err_no);

void on_window_destroy(GtkObject *object, gpointer data) {
	g_object_unref(G_OBJECT(builder));
	gtk_main_quit();
}

void on_product_new_add_button_clicked(GtkObject *object, gpointer data) {
	const gchar *nazwa;
	GtkWidget *entry = GTK_WIDGET(gtk_builder_get_object(builder, "product_new_name_entry"));
	nazwa = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
	if (strlen((char*)nazwa) == 0) {
		print_error(1);
	} else {
		print_error(0);
		g_print("Nowy produkt: %s\n", nazwa);
	}
	//g_free(nazwa);
}

void print_error(int err_no) {
	char *msg;
	GtkWidget *err_label = GTK_WIDGET(gtk_builder_get_object(builder, "product_new_err_label"));

	switch (err_no) {
		case 1: msg = "<span foreground='red'><b>Wpisz nazwę</b></span>";
						break;
		default:
						msg = "";
	}
	gtk_label_set_markup(GTK_LABEL(err_label), msg);
}

void init_components() {
	GtkWidget *product_new_categories_cb = GTK_WIDGET(gtk_builder_get_object(builder, "product_new_category_combobox"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(product_new_categories_cb), "test");
}

int main(int argc, char *argv[]) {
	GtkWidget  *window;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "budget.glade", NULL);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	gtk_builder_connect_signals(builder, NULL);


	gtk_widget_show(window);
	gtk_main();

	init_components();

	return 0;
}
