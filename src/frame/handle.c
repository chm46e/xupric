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

    f = current_frame_get();
    uri_custom_load(f, (char *)gtk_entry_get_text(GTK_ENTRY(b)));
    gtk_entry_set_text(GTK_ENTRY(b), uri_get(f));
}

extern void uri_changed(WebKitWebView *)
{
    GtkEntry *e;
    char *uri;

    e = GTK_ENTRY(gtk_builder_get_object(builder_get(), "bar_uri_entry"));
    uri = uri_get(current_frame_get());
    if (strcmp(gtk_entry_get_text(e), uri)) {
        gtk_entry_set_text(e, uri);
        current_frame_get()->uri = uri;
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
