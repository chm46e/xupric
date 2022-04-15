/*
 * frame/handle.c
 * Intended for handling signals from frame.c
 * and wrap.h
 */

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <stdio.h>

#include "cfg/cfg.h"
#include "frame.h"
#include "config.h"
#include "util/macros.h"
#include "extras/bookmark.h"
#include "extras/history.h"
#include "extras/cookie.h"
#include "gtk_wrap.h"
#include "handle.h"

#define CLEANMASK(mask) (mask & (GDK_CONTROL_MASK|GDK_SHIFT_MASK|GDK_SUPER_MASK|GDK_MOD1_MASK))

static char *uri_last = "";

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
    text = (char *)gtk_entry_get_text(GTK_ENTRY(b));

    if (!(strcmp(gtk_widget_get_name(b), "bar_uri_entry"))) {
        uri_custom_load(f, text, 0);
        gtk_entry_set_text(GTK_ENTRY(b), uri_get(f));
    } else if (!(strcmp(gtk_widget_get_name(b), "bar_uri_entry_secondary"))){
        uri_custom_load(f, text, 1);
    }
}

extern void uri_changed(WebKitWebView *)
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
        gtk_image_set_from_icon_name(bookmark_image, "xupric_star_yes", 18);
    else
        gtk_image_set_from_icon_name(bookmark_image, "xupric_star_no", 18);

    if (strcmp(uri, uri_last))
        history_add(uri);
    uri_last = uri;
}

extern void bookmark_button_toggle_handle(GtkWidget *)
{
    GtkImage *bookmark_image;
    char *uri;

    uri = uri_get(current_frame_get());
    bookmark_image = GTK_IMAGE(gtk_builder_get_object(builder_get(), "bookmark_image"));

    if (bookmark_exists(uri)) {
        bookmark_remove(uri);
        gtk_image_set_from_icon_name(bookmark_image, "xupric_star_no", 18);
    } else {
        bookmark_add(uri);
        gtk_image_set_from_icon_name(bookmark_image, "xupric_star_yes", 18);
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

extern void debug_toggle(GtkWidget *)
{
    conf_opt *config = cfg_get();
    config[conf_debug].i = !(config[conf_debug].i);

	printf("This button is very useful:D\n");
}

extern void win_bookmark_build(GtkWidget *)
{
    GtkWidget *win, *mbox, *scroll, *viewport, *box, *button, *rmbutton, *label;
    GtkCssProvider *css;
    char **books;
    int books_len;
    int i;

    books = bookmark_get();
    books_len = books_len_get();

    css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css, "box { border-top: 1px #35373d solid; }", -1, NULL);

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
        label = gtk_label_new_with_mnemonic(books[i]);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        rmbutton = gtk_button_new();
        gtk_button_set_relief(GTK_BUTTON(rmbutton), GTK_RELIEF_NONE);
        gtk_widget_set_margin_end(rmbutton, 5);
        gtk_button_set_image(GTK_BUTTON(rmbutton), gtk_image_new_from_icon_name("process-stop-symbolic",
            GTK_ICON_SIZE_MENU));

        gtk_container_add(GTK_CONTAINER(button), label);
        gtk_box_pack_start(GTK_BOX(box), rmbutton, 0, 0, 0);
        gtk_box_pack_start(GTK_BOX(box), button, 1, 1, 0);
        gtk_box_pack_start(GTK_BOX(mbox), box, 1, 1, 0);

        g_signal_connect(button, "clicked", G_CALLBACK(uri_custom_load_gwrap), books[i]);
        g_signal_connect(rmbutton, "clicked", G_CALLBACK(bookmark_remove_gwrap), books[i]);

        if (i > 0)
            gtk_style_context_add_provider(gtk_widget_get_style_context(box), GTK_STYLE_PROVIDER(css),
                GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }

    if (books_len == 0)
        win_bookmark_empty(mbox);

    gtk_container_add(GTK_CONTAINER(viewport), mbox);
    gtk_container_add(GTK_CONTAINER(scroll), viewport);
    gtk_container_add(GTK_CONTAINER(win), scroll);

    gtk_widget_show_all(win);
}

extern void win_history_build(GtkWidget *)
{
    GtkWidget *win, *mbox, *scroll, *viewport, *box, *button, *rmbutton, *label;
    GtkCssProvider *css;
    char **history;
    int history_len;
    int i;

    history = history_get();
    history_len = history_len_get();

    css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css, "box { border-top: 1px #35373d solid; }", -1, NULL);

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
        label = gtk_label_new_with_mnemonic(history[i]);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        rmbutton = gtk_button_new();
        gtk_button_set_relief(GTK_BUTTON(rmbutton), GTK_RELIEF_NONE);
        gtk_widget_set_margin_end(rmbutton, 5);
        gtk_button_set_image(GTK_BUTTON(rmbutton), gtk_image_new_from_icon_name("user-trash-symbolic",
            GTK_ICON_SIZE_MENU));

        gtk_container_add(GTK_CONTAINER(button), label);
        gtk_box_pack_start(GTK_BOX(box), rmbutton, 0, 0, 0);
        gtk_box_pack_start(GTK_BOX(box), button, 1, 1, 0);
        gtk_box_pack_start(GTK_BOX(mbox), box, 1, 1, 0);

        g_signal_connect(button, "clicked", G_CALLBACK(uri_custom_load_gwrap), history[i]);
        g_signal_connect(rmbutton, "clicked", G_CALLBACK(history_remove_gwrap), history[i]);

        if (i > 0)
            gtk_style_context_add_provider(gtk_widget_get_style_context(box), GTK_STYLE_PROVIDER(css),
                GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }

    gtk_container_add(GTK_CONTAINER(viewport), mbox);
    gtk_container_add(GTK_CONTAINER(scroll), viewport);
    gtk_container_add(GTK_CONTAINER(win), scroll);

    gtk_widget_show_all(win);
}

extern void win_cookie_build(GtkWidget *)
{
    GtkWidget *win, *mbox, *scroll, *viewport, *box, *button, *rmbutton, *label;
    GtkCssProvider *css;
    char **cookies;
    int cookies_len;
    int i;

    cookies = cookie_get();
    cookies_len = cookie_len_get();

    css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css, "box { border-top: 1px #35373d solid; }", -1, NULL);

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
        label = gtk_label_new_with_mnemonic(cookies[i]);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        rmbutton = gtk_button_new();
        gtk_button_set_relief(GTK_BUTTON(rmbutton), GTK_RELIEF_NONE);
        gtk_widget_set_margin_end(rmbutton, 5);
        gtk_button_set_image(GTK_BUTTON(rmbutton), gtk_image_new_from_icon_name("user-trash-symbolic",
            GTK_ICON_SIZE_MENU));

        gtk_container_add(GTK_CONTAINER(button), label);
        gtk_box_pack_start(GTK_BOX(box), rmbutton, 0, 0, 0);
        gtk_box_pack_start(GTK_BOX(box), button, 1, 1, 0);
        gtk_box_pack_start(GTK_BOX(mbox), box, 1, 1, 0);

        g_signal_connect(rmbutton, "clicked", G_CALLBACK(cookie_remove_gwrap), cookies[i]);

        if (i > 0)
            gtk_style_context_add_provider(gtk_widget_get_style_context(box), GTK_STYLE_PROVIDER(css),
                GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }

    gtk_container_add(GTK_CONTAINER(viewport), mbox);
    gtk_container_add(GTK_CONTAINER(scroll), viewport);
    gtk_container_add(GTK_CONTAINER(win), scroll);

    gtk_widget_show_all(win);
}

extern void win_bookmark_empty(GtkWidget *mbox)
{
    GtkWidget *label;

    label = gtk_label_new_with_mnemonic("Hmm. Pretty quiet here.");
    gtk_box_pack_start(GTK_BOX(mbox), label, 1, 1, 0);
    gtk_widget_set_valign(mbox, GTK_ALIGN_FILL);
}
