#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <unistd.h>
#include <stdio.h>

#include "util/util.h"
#include "cfg/cfg.h"
#include "frame/frame.h"
#include "uri/uri.h"
#include "frame/view.h"
#include "fun/fun.h"

void zoom_action(struct frame *f, int action)
{
	if (action == 1)
		webkit_web_view_set_zoom_level(f->view, f->zoom + 0.1);
	else if (action == 0)
		webkit_web_view_set_zoom_level(f->view, f->zoom - 0.1);
	else
		webkit_web_view_set_zoom_level(f->view, 1.0);

	f->zoom = webkit_web_view_get_zoom_level(f->view);
	zoom_label_update(f);
}

void zoom_label_update(struct frame *f)
{
	char zoom[5];

	sprintf(zoom, "%i%%", (int)(f->zoom*100));
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(
		builder_get(), "menu_zoom_reset_label")), zoom);
}

void fullscreen_toggle(struct frame *f)
{
	if (f->fullscreen) {
		gtk_window_unfullscreen(GTK_WINDOW(f->win));
		f->fullscreen = 0;
	} else {
		gtk_window_fullscreen(GTK_WINDOW(f->win));
		f->fullscreen = 1;
	}
}

void fullscreen_action(struct frame *f, int action)
{
	if (action == 0) {
		gtk_window_unfullscreen(GTK_WINDOW(f->win));
		f->fullscreen = 0;
	} else {
		gtk_window_fullscreen(GTK_WINDOW(f->win));
		f->fullscreen = 1;
	}
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

void clear_focus_entry(void)
{
	GtkWidget *entry;

	entry = GTK_WIDGET(gtk_builder_get_object(builder_get(), "bar_uri_entry"));
	gtk_entry_set_text(GTK_ENTRY(entry), "");
	gtk_widget_grab_focus(entry);
}

void clear_focus_secondary_entry(void)
{
	GtkWidget *entry;

	entry = GTK_WIDGET(gtk_builder_get_object(builder_get(), "bar_uri_entry_secondary"));
	gtk_entry_set_text(GTK_ENTRY(entry), "");
	gtk_widget_grab_focus(entry);
}

void workspace_load(int c)
{
	conf_opt *config;
	struct frame *fs;
	int len, i, ws, startf;

	config = cfg_get();
	len = cfg_workspace_len_get()[c];
	if (len == 0)
		return;
	if (len > 10)
		len = 10;

	if (c > 4)
		return;

	switch (c) {
	case 0:
		ws = conf_workspace_1;
		break;
	default:
		ws = conf_workspace_1+c;
		break;
	}

	startf = view_last_get();
	fs = frames_get();
	for (i = 0; i < len; i++) {
		if (fs[i].empty) {
			fs[i].empty = 0;
			view_show(i);
			view_show(startf);
		}
		uri_custom_load(&fs[i], config[ws].p[i], 0);
	}
}

void new_window_spawn(char *uri)
{
	char *bin;

	bin = g_strdup_printf("xupric %s", uri);
	char *const cmd[] = {"/bin/sh", "-c", bin, NULL};

	if (fork() == 0) {
		setsid();
		execvp(cmd[0], cmd);
		exit(0);
	}

	efree(bin);
}

void toggle_webinspector(void)
{
	WebKitWebInspector *in;
	struct frame *f;

	f = current_frame_get();
	in = webkit_web_view_get_inspector(f->view);

	if (f->inspector) {
		webkit_web_inspector_close(in);
		f->inspector = 0;
	} else if (!(f->inspector)) {
		webkit_web_inspector_show(in);
		f->inspector = 1;
	}
}
