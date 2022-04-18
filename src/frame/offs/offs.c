#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <string.h>

#include "frame/frame.h"
#include "cfg/config.h"
#include "util/util.h"
#include "uri/uri.h"
#include "fun/sql/bookmark.h"
#include "fun/sql/history.h"
#include "fun/sql/cookie.h"
#include "frame/gwrap.h"

static void find_start(GtkWidget *e, GtkWidget *w);
static int find_win_key_press(GtkWidget *w, GdkEvent *ev);
static void win_bookmark_empty(GtkWidget *mbox);

void win_find_build(GtkWidget *)
{
	GtkWidget *win, *mbox, *label, *entry;

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(win), 300, 50);
	gtk_window_set_transient_for(GTK_WINDOW(win), GTK_WINDOW(current_frame_get()->win));
	gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_decorated(GTK_WINDOW(win), 0);

	mbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_start(mbox, 10);
	gtk_widget_set_margin_end(mbox, 10);
	gtk_widget_set_margin_top(mbox, 10);
	gtk_widget_set_margin_bottom(mbox, 10);

	label = gtk_label_new_with_mnemonic("Find:");
	gtk_widget_set_margin_start(label, 5);
	gtk_widget_set_margin_end(label, 10);
	gtk_box_pack_start(GTK_BOX(mbox), label, 0, 0, 0);

	entry = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(mbox), entry, 1, 1, 0);

	g_signal_connect(entry, "activate", G_CALLBACK(find_start), win);
	g_signal_connect(win, "key-press-event", G_CALLBACK(find_win_key_press), NULL);

	gtk_container_add(GTK_CONTAINER(win), mbox);
	gtk_widget_show_all(win);
}

static void find_start(GtkWidget *e, GtkWidget *w)
{
	webkit_find_controller_search(
		webkit_web_view_get_find_controller(current_frame_get()->view),
		gtk_entry_get_text(GTK_ENTRY(e)),
		WEBKIT_FIND_OPTIONS_CASE_INSENSITIVE|WEBKIT_FIND_OPTIONS_WRAP_AROUND,
		1000);

	gtk_window_close(GTK_WINDOW(w));
}

static int find_win_key_press(GtkWidget *w, GdkEvent *ev)
{
	if (ev->type == GDK_KEY_PRESS) {
		if (gdk_keyval_to_lower(ev->key.keyval) == GDK_KEY_Escape) {
			gtk_window_close(GTK_WINDOW(w));
			return 1;
		}
	}
	return 0;
}

void win_bookmark_build(GtkWidget *)
{
	GtkWidget *win, *mbox, *scroll, *viewport, *box, *button, *rmbutton, *label;
	GtkCssProvider *css;
	struct bookmark *books;
	int books_len;
	int i;

	books = bookmark_get();
	books_len = books_len_get();

	css = gtk_css_provider_new();
	gtk_css_provider_load_from_data(css, "box { border-top: 1px #35373d solid; }",
		 -1, NULL);

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(win), 800, 800);
	gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
	scroll = gtk_scrolled_window_new(NULL, NULL);
	viewport = gtk_viewport_new(NULL, NULL);
	mbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(mbox), 1);
	gtk_widget_set_valign(mbox, GTK_ALIGN_START);
	gtk_widget_set_margin_start(mbox, 5);
	gtk_widget_set_margin_end(mbox, 5);

	for (i = 0; i < books_len; i++) {
		box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_top(box, 5);
		button = gtk_button_new();
		gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
		label = gtk_label_new_with_mnemonic(books[i].uri);
		gtk_widget_set_halign(label, GTK_ALIGN_START);
		rmbutton = gtk_button_new();
		gtk_button_set_relief(GTK_BUTTON(rmbutton), GTK_RELIEF_NONE);
		gtk_widget_set_margin_end(rmbutton, 5);
		gtk_button_set_image(GTK_BUTTON(rmbutton), gtk_image_new_from_icon_name(
			"process-stop-symbolic", GTK_ICON_SIZE_MENU));

		gtk_container_add(GTK_CONTAINER(button), label);
		gtk_box_pack_start(GTK_BOX(box), rmbutton, 0, 0, 0);
		gtk_box_pack_start(GTK_BOX(box), button, 1, 1, 0);
		gtk_box_pack_start(GTK_BOX(mbox), box, 1, 1, 0);

		g_signal_connect(button, "clicked", G_CALLBACK(gwrap_uri_custom_load),
			books[i].uri);
		g_signal_connect(rmbutton, "clicked", G_CALLBACK(gwrap_bookmark_remove),
			&books[i]);

		if (i > 0)
			gtk_style_context_add_provider(gtk_widget_get_style_context(box),
				GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	}

	if (books_len == 0)
		win_bookmark_empty(mbox);

	gtk_container_add(GTK_CONTAINER(viewport), mbox);
	gtk_container_add(GTK_CONTAINER(scroll), viewport);
	gtk_container_add(GTK_CONTAINER(win), scroll);

	gtk_widget_show_all(win);
}

void win_history_build(GtkWidget *)
{
	GtkWidget *win, *mbox, *scroll, *viewport, *box, *button, *rmbutton, *label;
	GtkCssProvider *css;
	struct history *history;
	int history_len;
	int i;

	history = history_get();
	history_len = history_len_get();

	css = gtk_css_provider_new();
	gtk_css_provider_load_from_data(css, "box { border-top: 1px #35373d solid; }",
		-1, NULL);

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(win), 800, 800);
	gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
	scroll = gtk_scrolled_window_new(NULL, NULL);
	viewport = gtk_viewport_new(NULL, NULL);
	mbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(mbox), 1);
	gtk_widget_set_valign(mbox, GTK_ALIGN_START);
	gtk_widget_set_margin_start(mbox, 5);
	gtk_widget_set_margin_end(mbox, 5);

	for (i = 0; i < history_len; i++) {
		box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_top(box, 5);
		button = gtk_button_new();
		gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
		label = gtk_label_new_with_mnemonic(history[i].uri);
		gtk_widget_set_halign(label, GTK_ALIGN_START);
		rmbutton = gtk_button_new();
		gtk_button_set_relief(GTK_BUTTON(rmbutton), GTK_RELIEF_NONE);
		gtk_widget_set_margin_end(rmbutton, 5);
		gtk_button_set_image(GTK_BUTTON(rmbutton), gtk_image_new_from_icon_name(
			"user-trash-symbolic", GTK_ICON_SIZE_MENU));

		gtk_container_add(GTK_CONTAINER(button), label);
		gtk_box_pack_start(GTK_BOX(box), rmbutton, 0, 0, 0);
		gtk_box_pack_start(GTK_BOX(box), button, 1, 1, 0);
		gtk_box_pack_start(GTK_BOX(mbox), box, 1, 1, 0);

		g_signal_connect(button, "clicked", G_CALLBACK(gwrap_uri_custom_load),
			history[i].uri);
		g_signal_connect(rmbutton, "clicked", G_CALLBACK(gwrap_history_remove),
			&history[i]);

		if (i > 0)
			gtk_style_context_add_provider(gtk_widget_get_style_context(box),
				GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	}

	gtk_container_add(GTK_CONTAINER(viewport), mbox);
	gtk_container_add(GTK_CONTAINER(scroll), viewport);
	gtk_container_add(GTK_CONTAINER(win), scroll);

	gtk_widget_show_all(win);
}

void win_cookie_build(GtkWidget *)
{
	GtkWidget *win, *mbox, *scroll, *viewport, *box, *button, *rmbutton, *label;
	GtkCssProvider *css;
	struct cookie *cookies;
	char *info;
	int cookies_len;
	int i;

	cookies = cookie_get();
	cookies_len = cookie_len_get();

	css = gtk_css_provider_new();
	gtk_css_provider_load_from_data(css, "box { border-top: 1px #35373d solid; }",
		 -1, NULL);

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(win), 800, 800);
	gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
	scroll = gtk_scrolled_window_new(NULL, NULL);
	viewport = gtk_viewport_new(NULL, NULL);
	mbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(mbox), 1);
	gtk_widget_set_valign(mbox, GTK_ALIGN_START);
	gtk_widget_set_margin_start(mbox, 5);
	gtk_widget_set_margin_end(mbox, 5);

	for (i = 0; i < cookies_len; i++) {
		box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_top(box, 5);
		button = gtk_button_new();
		gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);

		info = emalloc(strlen(cookies[i].host) + strlen(cookies[i].name) + 3);
		strcpy(info, cookies[i].host);
		strcat(info, ": ");
		strcat(info, cookies[i].name);

		label = gtk_label_new_with_mnemonic(info);
		gtk_widget_set_halign(label, GTK_ALIGN_START);
		rmbutton = gtk_button_new();
		gtk_button_set_relief(GTK_BUTTON(rmbutton), GTK_RELIEF_NONE);
		gtk_widget_set_margin_end(rmbutton, 5);
		gtk_button_set_image(GTK_BUTTON(rmbutton), gtk_image_new_from_icon_name(
			"user-trash-symbolic", GTK_ICON_SIZE_MENU));

		gtk_container_add(GTK_CONTAINER(button), label);
		gtk_box_pack_start(GTK_BOX(box), rmbutton, 0, 0, 0);
		gtk_box_pack_start(GTK_BOX(box), button, 1, 1, 0);
		gtk_box_pack_start(GTK_BOX(mbox), box, 1, 1, 0);

		g_signal_connect(rmbutton, "clicked", G_CALLBACK(gwrap_cookie_remove),
			&cookies[i]);

		if (i > 0)
			gtk_style_context_add_provider(gtk_widget_get_style_context(box),
				GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	}

	gtk_container_add(GTK_CONTAINER(viewport), mbox);
	gtk_container_add(GTK_CONTAINER(scroll), viewport);
	gtk_container_add(GTK_CONTAINER(win), scroll);

	gtk_widget_show_all(win);
}

static void win_bookmark_empty(GtkWidget *mbox)
{
	GtkWidget *label;

	label = gtk_label_new_with_mnemonic("Hmm. Pretty quiet here.");
	gtk_box_pack_start(GTK_BOX(mbox), label, 1, 1, 0);
	gtk_widget_set_valign(mbox, GTK_ALIGN_FILL);
}
