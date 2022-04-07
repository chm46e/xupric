/*
 * frame/gtk_wrap.h
 * Intended for wrapping for gtk handlers
 */

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <frame/frame.h>
#include <frame/handle.h>
#include <uri/uri.h>
#include <extras/bookmark.h>
#include <unistd.h>

static inline void view_show_gwrap(GtkWidget *, int id)
{
    view_show(id);
}

static inline void uri_custom_load_gwrap(GtkWidget *, char *uri)
{
    uri_custom_load(current_frame_get(), uri, 0);
}

static void zoom_action_gwrap(GtkWidget *, int action)
{
    struct frame *f;
    char zoom[5];

    f = current_frame_get();
    zoom_action(f, action);

    sprintf(zoom, "%i%%", (int)(f->zoom*100));
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder_get(), "menu_zoom_reset_label")),
        zoom);
}

static void bookmark_remove_gwrap(GtkWidget *w, char *uri)
{
    GtkWidget *box, *mbox;

    bookmark_remove(uri);

    box = gtk_widget_get_parent(w);
    mbox = gtk_widget_get_parent(box);
    gtk_widget_destroy(box);
}

static void history_remove_gwrap(GtkWidget *w, char *uri)
{
    GtkWidget *box, *mbox;

    history_remove(uri);

    box = gtk_widget_get_parent(w);
    mbox = gtk_widget_get_parent(box);
    gtk_widget_destroy(box);
}

static inline void history_remove_all_gwrap(GtkWidget *)
{
    history_remove_all();
}

static void cookie_remove_gwrap(GtkWidget *w, char *text)
{
    GtkWidget *box, *mbox;
    char *delim = ": ";
    char *token;

    token = strtok(text, delim);
    token = strtok(NULL, delim);

    cookie_remove(token);

    box = gtk_widget_get_parent(w);
    mbox = gtk_widget_get_parent(box);
    gtk_widget_destroy(box);
}

static inline void cookie_remove_all_gwrap(GtkWidget *)
{
    cookie_remove_all();
}
