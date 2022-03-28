/*
 * frame/handle.c
 * Intended for handling signals from frame.c
 * and wrap.h
 */

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "frame.h"
#include "config.h"
#include "util/macros.h"
#include "extras/bookmark.h"

#define CLEANMASK(mask) (mask & (GDK_CONTROL_MASK|GDK_SHIFT_MASK|GDK_SUPER_MASK|GDK_MOD1_MASK))

extern void zoom_action(struct frame *f, int action)
{
    if (action == 1)
        webkit_web_view_set_zoom_level(f->view, f->zoom + 0.1);
    else if (action == 0)
        webkit_web_view_set_zoom_level(f->view, f->zoom - 0.1);
    else
        webkit_web_view_set_zoom_level(f->view, 1.0);

    f->zoom = webkit_web_view_get_zoom_level(f->view);
}

extern void fullscreen_toggle(struct frame *f)
{
    if (f->fullscreen)
        gtk_window_unfullscreen(GTK_WINDOW(f->win));
    else
        gtk_window_fullscreen(GTK_WINDOW(f->win));
}

extern void navigate(struct frame *f, int back)
{
    if (back)
        webkit_web_view_go_back(f->view);
    else
        webkit_web_view_go_forward(f->view);
}

extern void window_close()
{
    gtk_main_quit();
}

extern int window_event_handle(GtkWidget *, GdkEvent *ev)
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

extern void bar_uri_enter_handle(GtkWidget *b)
{
    struct frame *f;
    char *text;

    f = current_frame_get();
    text = gtk_entry_get_text(GTK_ENTRY(b));

    if (!(strcmp(gtk_widget_get_name(b), "bar_uri_entry"))) {
        uri_custom_load(f, text, 0);
        gtk_entry_set_text(GTK_ENTRY(b), uri_get(f));
    } else if (!(strcmp(gtk_widget_get_name(b), "bar_uri_entry_secondary"))){
        uri_custom_load(f, text, 1);
    }
}

extern void uri_changed(WebKitWebView *v)
{
    GtkBuilder *builder;
    GtkImage *bookmark_image;
    GtkEntry *e;
    char *uri;

    builder = builder_get();
    e = GTK_ENTRY(gtk_builder_get_object(builder, "bar_uri_entry"));
    uri = uri_get(current_frame_get());
    if (strcmp(gtk_entry_get_text(e), uri)) {
        gtk_entry_set_text(e, uri);
        current_frame_get()->uri = uri;
    }

    bookmark_image = GTK_IMAGE(gtk_builder_get_object(builder, "bookmark_image"));
    if (bookmark_exists(uri))
        gtk_image_set_from_icon_name(bookmark_image, "sulfer_star_yes", 18);
    else
        gtk_image_set_from_icon_name(bookmark_image, "sulfer_star_no", 18);

    gtk_widget_grab_focus(v);
}

extern void bookmark_button_toggle_handle(GtkWidget *b)
{
    GtkImage *bookmark_image;
    char *uri;

    uri = uri_get(current_frame_get());
    bookmark_image = GTK_IMAGE(gtk_builder_get_object(builder_get(), "bookmark_image"));

    if (bookmark_exists(uri)) {
        bookmark_remove(uri);
        gtk_image_set_from_icon_name(bookmark_image, "sulfer_star_no", 18);
    } else {
        bookmark_add(uri);
        gtk_image_set_from_icon_name(bookmark_image, "sulfer_star_yes", 18);
    }
}

extern void uri_load_progress(WebKitWebView *v)
{
    GdkCursor *c;

    if (webkit_web_view_get_estimated_load_progress(v) != 1.0)
        c = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_WATCH);
    else
        c = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_ARROW);
    gdk_window_set_cursor(gtk_widget_get_window(current_frame_get()->win), c);
}
