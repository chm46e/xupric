#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <stdio.h>

#include "cfg/cfg.h"
#include "frame/frame.h"
#include "uri/uri.h"

void zoom_action(struct frame *f, int action)
{
	char zoom[5];

	if (action == 1)
		webkit_web_view_set_zoom_level(f->view, f->zoom + 0.1);
	else if (action == 0)
		webkit_web_view_set_zoom_level(f->view, f->zoom - 0.1);
	else
		webkit_web_view_set_zoom_level(f->view, 1.0);

	f->zoom = webkit_web_view_get_zoom_level(f->view);

	sprintf(zoom, "%i%%", (int)(f->zoom*100));
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(
		builder_get(), "menu_zoom_reset_label")), zoom);
}

void fullscreen_toggle(struct frame *f)
{
	if (f->fullscreen)
		gtk_window_unfullscreen(GTK_WINDOW(f->win));
	else
		gtk_window_fullscreen(GTK_WINDOW(f->win));
}

void view_navigate(struct frame *f, int back)
{
	if (back)
		webkit_web_view_go_back(f->view);
	else
		webkit_web_view_go_forward(f->view);
}

void window_close(void)
{
	gtk_main_quit();
}

void debug_toggle(void)
{
	conf_opt *config = cfg_get();
	config[conf_debug].i = !(config[conf_debug].i);
}

void download_current_page(void)
{
	struct frame *f;

	f = current_frame_get();
	webkit_web_view_download_uri(f->view, uri_get(f));
}
