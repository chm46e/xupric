#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "frame.h"
#include "util/util.h"

static GtkBuilder *builder;
static WebKitDownload *download_last = 0;

static void download_destination_created(WebKitDownload *, char *dest);
static void download_progress_changed(WebKitDownload *d);
static void download_finished(WebKitDownload *);

void download_started(WebKitWebContext *, WebKitDownload *d)
{
	builder = builder_get();

	g_signal_connect(G_OBJECT(d), "created-destination",
		G_CALLBACK(download_destination_created), NULL);
	g_signal_connect(G_OBJECT(d), "notify::estimated-progress",
		G_CALLBACK(download_progress_changed), NULL);
	g_signal_connect(G_OBJECT(d), "finished",
		G_CALLBACK(download_finished), NULL);

	download_last = d;
}

static void download_destination_created(WebKitDownload *, char *dest)
{
	char *name;

	name = ecalloc(21, sizeof(char));
	strncpy(name, g_filename_display_basename(dest), 20);

	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "menu_download_label")),
		name);

	efree(name);
}

static void download_progress_changed(WebKitDownload *d)
{
	char progress[6];

	sprintf(progress, "%i%%:", (int)(webkit_download_get_estimated_progress(d)*100));

	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
		"menu_download_progress_label")), progress);
}

static void download_finished(WebKitDownload *)
{
	download_last = 0;
}

void download_quit(GtkWidget *)
{
	if (download_last != 0) {
		webkit_download_cancel(download_last);
		gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
			"menu_download_progress_label")), "quit:");
	}
}
