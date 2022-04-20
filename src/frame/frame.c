#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "frame/style/style.h"
#include "util/util.h"
#include "cfg/cfg.h"
#include "cfg/config.h"
#include "offs/offs.h"
#include "uri/uri.h"
#include "fun/fun.h"
#include "download.h"
#include "view.h"
#include "gwrap.h"
#include "frame.h"
#include "config.h"
#include "util/macros.h"

#include "main_ui.h"

#define CLEANMASK(mask) (mask & (GDK_CONTROL_MASK|GDK_SHIFT_MASK|GDK_SUPER_MASK|GDK_MOD1_MASK))

static void frame_signals_connect(void);
static void bar_uri_enter_handle(GtkWidget *b);
static int window_event_handle(GtkWidget *, GdkEvent *ev);

static GtkBuilder *builder;
static struct frame *frames;

void frame_list_create(void)
{
	GtkWidget *win;
	GtkCssProvider *css;

	frames = ecalloc(10, sizeof(struct frame));
	builder = gtk_builder_new_from_string((const gchar *)main_ui, main_ui_len);

	css = gtk_css_provider_new();
	gtk_css_provider_load_from_path(css, config_names[7], NULL);

	frame_signals_connect();

	win = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
	g_signal_connect(win, "destroy", G_CALLBACK(window_close), NULL);
	g_signal_connect(win, "key-press-event", G_CALLBACK(window_event_handle), NULL);
	g_signal_connect(win, "window-state-event", G_CALLBACK(window_event_handle), NULL);

	gtk_window_maximize(GTK_WINDOW(win));

	view_list_create();

	for (int i = 0; i < 10; i++) {
		frames[i].view = views_get()[i];
		frames[i].win = win;
		frames[i].empty = 1;
		frames[i].dark_mode = cfg_get()[conf_dark_mode].i;
		frames[i].zoom = 1.0;
		frames[i].finder = webkit_web_view_get_find_controller(frames[i].view);
	}

	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
		GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

static void frame_signals_connect(void)
{
	g_signal_connect(gtk_builder_get_object(builder, "bar_uri_entry"), "activate",
		G_CALLBACK(bar_uri_enter_handle), gtk_builder_get_object(builder, "uri_buffer"));
	g_signal_connect(gtk_builder_get_object(builder, "bar_uri_entry_secondary"),
		"activate", G_CALLBACK(bar_uri_enter_handle),
		gtk_builder_get_object(builder, "uri_buffer_secondary"));
	g_signal_connect(gtk_builder_get_object(builder, "bar_bookmark_button"), "clicked",
		G_CALLBACK(bookmark_button_toggle_handle), NULL);
	g_signal_connect(gtk_builder_get_object(builder, "bar_dark_mode_button"), "clicked",
		G_CALLBACK(dark_mode_button_toggle_handle), NULL);
	g_signal_connect(gtk_builder_get_object(builder, "menu_quit_button"), "clicked",
		G_CALLBACK(window_close), NULL);
	g_signal_connect(gtk_builder_get_object(builder, "help_about_button"), "clicked",
		G_CALLBACK(gwrap_uri_custom_load), "https://github.com/chm46e/xupric");
	g_signal_connect(gtk_builder_get_object(builder, "help_debug_button"), "clicked",
		G_CALLBACK(debug_toggle), NULL);
	g_signal_connect(gtk_builder_get_object(builder, "menu_zoom_dec_button"), "clicked",
		G_CALLBACK(gwrap_zoom_action), (int *)0);
	g_signal_connect(gtk_builder_get_object(builder, "menu_zoom_inc_button"), "clicked",
		G_CALLBACK(gwrap_zoom_action), (int *)1);
	g_signal_connect(gtk_builder_get_object(builder, "menu_zoom_reset_button"), "clicked",
		G_CALLBACK(gwrap_zoom_action), (int *)2);
	g_signal_connect(gtk_builder_get_object(builder, "menu_tab_1_button"), "clicked",
		G_CALLBACK(gwrap_view_show), (int *)0);
	g_signal_connect(gtk_builder_get_object(builder, "menu_tab_2_button"), "clicked",
		G_CALLBACK(gwrap_view_show), (int *)1);
	g_signal_connect(gtk_builder_get_object(builder, "menu_tab_3_button"), "clicked",
		G_CALLBACK(gwrap_view_show), (int *)2);
	g_signal_connect(gtk_builder_get_object(builder, "menu_tab_4_button"), "clicked",
		G_CALLBACK(gwrap_view_show), (int *)3);
	g_signal_connect(gtk_builder_get_object(builder, "menu_tab_5_button"), "clicked",
		G_CALLBACK(gwrap_view_show), (int *)4);
	g_signal_connect(gtk_builder_get_object(builder, "menu_download_quit_button"),
		"clicked", G_CALLBACK(download_quit), NULL);
	g_signal_connect(gtk_builder_get_object(builder, "menu_bookmarks_button"), "clicked",
		G_CALLBACK(win_bookmark_build), NULL);
	g_signal_connect(gtk_builder_get_object(builder, "menu_history_button"), "clicked",
		G_CALLBACK(win_history_build), NULL);
	g_signal_connect(gtk_builder_get_object(builder, "history_all_button"), "clicked",
		G_CALLBACK(gwrap_history_remove_all), NULL);
	g_signal_connect(gtk_builder_get_object(builder, "menu_cookies_button"), "clicked",
		G_CALLBACK(win_cookie_build), NULL);
	g_signal_connect(gtk_builder_get_object(builder, "cookies_all_button"), "clicked",
		G_CALLBACK(gwrap_cookie_remove_all), NULL);
}

static void bar_uri_enter_handle(GtkWidget *b)
{
	struct frame *f;
	char *text;

	f = current_frame_get();
	text = (char *)gtk_entry_get_text(GTK_ENTRY(b));

	if (!(strcmp(gtk_widget_get_name(b), "bar_uri_entry"))) {
		uri_custom_load(f, text, 0);
		gtk_entry_set_text(GTK_ENTRY(b), uri_get(f));
	} else if (!(strcmp(gtk_widget_get_name(b), "bar_uri_entry_secondary"))) {
		uri_custom_load(f, text, 1);
	}
}

static int window_event_handle(GtkWidget *, GdkEvent *ev)
{
	switch (ev->type) {
	case GDK_KEY_PRESS:
		for (int i = 0; i < (int)LENGTH(keys); i++) {
			if (gdk_keyval_to_lower(ev->key.keyval) == keys[i].keyval &&
				CLEANMASK(ev->key.state) == keys[i].mod &&
				keys[i].func) {
				keys[i].func(&(keys[i].arg));
				return 1;
			}
		}
		return 0;
	case GDK_WINDOW_STATE:
		if (ev->window_state.changed_mask == GDK_WINDOW_STATE_FULLSCREEN)
			current_frame_get()->fullscreen = ev->window_state.new_window_state &
			GDK_WINDOW_STATE_FULLSCREEN;
		return 1;
	default:
		return 0;
	}
	return 0;
}

void bookmark_button_toggle_handle(GtkWidget *)
{
	GtkImage *bookmark_image;
	char *uri;

	uri = uri_get(current_frame_get());
	bookmark_image = GTK_IMAGE(gtk_builder_get_object(builder, "bookmark_image"));

	if (bookmark_exists(uri)) {
		bookmark_remove_by_uri(uri);
		gtk_image_set_from_icon_name(bookmark_image, "xupric_star_no", 18);
	} else {
		bookmark_add(uri);
		gtk_image_set_from_icon_name(bookmark_image, "xupric_star_yes", 18);
	}
}

void dark_mode_button_toggle_handle(GtkWidget *)
{
	GtkImage *dark_mode_image;
	struct frame *f;

	dark_mode_image = GTK_IMAGE(gtk_builder_get_object(builder, "dark_mode_image"));
	f = current_frame_get();
	if (f->dark_mode) {
		dark_mode_remove(f->view);
		f->dark_mode = 0;
		gtk_image_set_from_icon_name(dark_mode_image, "night-light-disabled-symbolic", 18);
	} else {
		dark_mode_set(f->view);
		f->dark_mode = 1;
		gtk_image_set_from_icon_name(dark_mode_image, "night-light-symbolic", 18);
	}

}

struct frame *frames_get(void)
{
	return frames;
}

struct frame *current_frame_get(void)
{
	return &frames[view_last_get()];
}

GtkBuilder *builder_get(void)
{
	return builder;
}
