/*
 * frame/frame.c:
 * Intended for creating and showing the frame,
 * which contains gtkbuilder, view, styles, etc
 */

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkx.h>
#include <webkit2/webkit2.h>

#include "util/util.h"
#include "cfg/cfg.h"
#include "cfg/config.h"
#include "uri/uri.h"

#include "frame.h"
#include "handle.h"
#include "download.h"

#include "main_ui.h"
#include "gtk_wrap.h"

static conf_opt *config;
static GtkBuilder *builder;
static struct frame *frames;
static WebKitWebView **views;

static void view_list_create(void);
static void style_set(WebKitWebView *view, char *file);

static int last = 0;

extern void frame_list_create(void)
{
    GtkWidget *win;
    GtkCssProvider *css;

    frames = ecalloc(10, sizeof(struct frame));
    config = cfg_get();
    builder = gtk_builder_new_from_string((const gchar *)main_ui, main_ui_len);

    css = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css, config_names[7], NULL);

    g_signal_connect(gtk_builder_get_object(builder, "bar_uri_entry"), "activate",
        G_CALLBACK(bar_uri_enter_handle), gtk_builder_get_object(builder, "uri_buffer"));
    g_signal_connect(gtk_builder_get_object(builder, "bar_uri_entry_secondary"), "activate",
        G_CALLBACK(bar_uri_enter_handle), gtk_builder_get_object(builder, "uri_buffer_secondary"));
    g_signal_connect(gtk_builder_get_object(builder, "bar_bookmark_button"), "clicked",
        G_CALLBACK(bookmark_button_toggle_handle), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "menu_quit_button"), "clicked",
        G_CALLBACK(window_close), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "help_about_button"), "clicked",
        G_CALLBACK(uri_custom_load_gwrap), "https://github.com/chm46e/xupric");
    g_signal_connect(gtk_builder_get_object(builder, "help_debug_button"), "clicked",
        G_CALLBACK(debug_toggle), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "menu_zoom_dec_button"), "clicked",
        G_CALLBACK(zoom_action_gwrap), (int *)0);
    g_signal_connect(gtk_builder_get_object(builder, "menu_zoom_inc_button"), "clicked",
        G_CALLBACK(zoom_action_gwrap), (int *)1);
    g_signal_connect(gtk_builder_get_object(builder, "menu_zoom_reset_button"), "clicked",
        G_CALLBACK(zoom_action_gwrap), (int *)2);
    g_signal_connect(gtk_builder_get_object(builder, "menu_tab_1_button"), "clicked",
        G_CALLBACK(view_show_gwrap), (int *)0);
    g_signal_connect(gtk_builder_get_object(builder, "menu_tab_2_button"), "clicked",
        G_CALLBACK(view_show_gwrap), (int *)1);
    g_signal_connect(gtk_builder_get_object(builder, "menu_tab_3_button"), "clicked",
        G_CALLBACK(view_show_gwrap), (int *)2);
    g_signal_connect(gtk_builder_get_object(builder, "menu_tab_4_button"), "clicked",
        G_CALLBACK(view_show_gwrap), (int *)3);
    g_signal_connect(gtk_builder_get_object(builder, "menu_tab_5_button"), "clicked",
        G_CALLBACK(view_show_gwrap), (int *)4);
    g_signal_connect(gtk_builder_get_object(builder, "menu_download_quit_button"), "clicked",
        G_CALLBACK(download_quit), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "menu_bookmarks_button"), "clicked",
        G_CALLBACK(win_bookmark_build), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "menu_history_button"), "clicked",
        G_CALLBACK(win_history_build), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "history_all_button"), "clicked",
        G_CALLBACK(history_remove_all_gwrap), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "menu_cookies_button"), "clicked",
        G_CALLBACK(win_cookie_build), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "cookies_all_button"), "clicked",
        G_CALLBACK(cookie_remove_all_gwrap), NULL);

    win = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    g_signal_connect(win, "destroy", G_CALLBACK(window_close), NULL);
    g_signal_connect(win, "key-press-event", G_CALLBACK(window_event_handle), NULL);
    g_signal_connect(win, "window-state-event", G_CALLBACK(window_event_handle), NULL);

    gtk_window_maximize(GTK_WINDOW(win));

    view_list_create();

    for (int i = 0; i < 10; i++) {
        frames[i].view = views[i];
        frames[i].win = win;
        frames[i].empty = 1;
        frames[i].zoom = 1.0;
        frames[i].finder = webkit_web_view_get_find_controller(frames[i].view);
    }

    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

extern void view_order_show(int increment)
{
    if (increment) {
        if (last >= 9)
            view_show(0);
        else
            view_show(last+1);
    } else {
        if (last <= 0)
            view_show(9);
        else
            view_show(last-1);
    }
}

extern void view_show(int id)
{
    GtkBuilder *builder;
    WebKitWebView *dview;
    GtkWidget *box;
    char zoom[5];

    builder = builder_get();
    box = GTK_WIDGET(gtk_builder_get_object(builder, "main_box"));
    dview = g_object_ref(frames[last].view);

    gtk_container_remove(GTK_CONTAINER(box), GTK_WIDGET(frames[last].view));
    gtk_box_pack_end(GTK_BOX(box), GTK_WIDGET(frames[id].view), TRUE, TRUE, 0);

    gtk_widget_show_all(frames[id].win);

    gtk_widget_grab_focus(GTK_WIDGET(frames[id].view));
    frames[last].view = dview;
    last = id;

    if (frames[id].empty) {
        uri_search_engine_load(&(frames[id]));
        frames[id].uri = uri_get(&(frames[id]));
        frames[id].empty = 0;
    }

    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "bar_uri_entry")),
                       frames[id].uri);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "bar_uri_entry_secondary")),
                       "");

    sprintf(zoom, "%i%%", (int)(frames[id].zoom*100));
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"menu_zoom_reset_label")),
                       zoom);
}

extern struct frame **frames_get(void)
{
    return &frames;
}

extern struct frame *current_frame_get(void)
{
    return &frames[last];
}

extern GtkBuilder *builder_get(void)
{
    return builder;
}

static void style_set(WebKitWebView *view, char *file)
{
    gchar *style;

    if (!g_file_get_contents(file, &style, NULL, NULL)) {
        die(1, "[ERROR] Unable to read the style file: %s\n", file);
    }

    webkit_user_content_manager_add_style_sheet(
        webkit_web_view_get_user_content_manager(view),
        webkit_user_style_sheet_new(style,
            WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES,
            WEBKIT_USER_STYLE_LEVEL_USER,
            NULL, NULL
        )
    );
    g_free(style);
}

static void view_list_create(void)
{
    WebKitSettings *settings;
    WebKitWebContext *context;
    WebKitCookieManager *cookiemanager;
    WebKitWebsiteDataManager *datamanager;

    settings = webkit_settings_new_with_settings(
        "enable-java", config[conf_java].i,
        "enable-javascript", config[conf_javascript].i,
        "enable-xss-auditor", config[conf_xss_auditor].i,
        "javascript-can-open-windows-automatically", config[conf_js_auto_popups].i,
        "default-font-family", config[conf_font_family].s,
        "default-font-size", config[conf_font_size].i,
        "default-charset", config[conf_charset].s,
        "enable-developer-extras", config[conf_developer_extras].i,
        "enable-dns-prefetching", config[conf_dns_prefetching].i,
        "enable-caret-browsing", config[conf_caret_browsing].i,
        "enable-media", config[conf_media].i,
        "enable-webaudio", config[conf_webaudio].i,
        "enable-webgl", config[conf_webgl].i,
        "enable-site-specific-quirks", config[conf_site_quirks].i,
        "enable-smooth-scrolling", config[conf_smooth_scrolling].i,
        NULL
    );

    if (strcmp(config[conf_user_agent].s, ""))
        webkit_settings_set_user_agent(settings, config[conf_user_agent].s);

    if (config[conf_hardware_accel].i == 0)
        webkit_settings_set_hardware_acceleration_policy(settings, WEBKIT_HARDWARE_ACCELERATION_POLICY_NEVER);
    else if (config[conf_hardware_accel].i == 1)
        webkit_settings_set_hardware_acceleration_policy(settings, WEBKIT_HARDWARE_ACCELERATION_POLICY_ALWAYS);
    else
        webkit_settings_set_hardware_acceleration_policy(settings, WEBKIT_HARDWARE_ACCELERATION_POLICY_ON_DEMAND);

    datamanager = webkit_website_data_manager_new(
        "base-cache-directory", config[conf_cache_prefix],
        "base-data-directory", cache_names[1],
        "itp-directory", cache_names[2],
        "offline-application-cache-directory", cache_names[3],
        "hsts-cache-directory", cache_names[4],
        NULL
    );

    webkit_website_data_manager_set_itp_enabled(datamanager, config[conf_itp].i);
    webkit_website_data_manager_set_tls_errors_policy(datamanager, config[conf_tls_error_policy].i);

    if (config[conf_ephemeral].i)
        context = webkit_web_context_new_ephemeral();
    else
        context = webkit_web_context_new_with_website_data_manager(datamanager);

    cookiemanager = webkit_web_context_get_cookie_manager(context);

    webkit_web_context_set_process_model(context,
        WEBKIT_PROCESS_MODEL_MULTIPLE_SECONDARY_PROCESSES);

    webkit_web_context_set_cache_model(context,
        config[conf_ephemeral].i ? WEBKIT_CACHE_MODEL_WEB_BROWSER :
        WEBKIT_CACHE_MODEL_DOCUMENT_VIEWER);

    if (!config[conf_ephemeral].i) {
        if (config[conf_cookie_policy].i == 0)
            webkit_cookie_manager_set_accept_policy(cookiemanager, WEBKIT_COOKIE_POLICY_ACCEPT_NEVER);
        else if (config[conf_cookie_policy].i == 1)
            webkit_cookie_manager_set_accept_policy(cookiemanager, WEBKIT_COOKIE_POLICY_ACCEPT_ALWAYS);
        else
            webkit_cookie_manager_set_accept_policy(cookiemanager, WEBKIT_COOKIE_POLICY_ACCEPT_NO_THIRD_PARTY);

        webkit_cookie_manager_set_persistent_storage(cookiemanager,
            cache_names[0], WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE);
    }

    views = ecalloc(10, sizeof(WebKitWebView));

    for (int i = 0; i < 10; i++) {
        views[i] = g_object_new(WEBKIT_TYPE_WEB_VIEW,
            "settings", settings,
            "user-content-manager", webkit_user_content_manager_new(),
            "web-context", context,
            NULL);

        if (config[conf_dark_mode].i)
            style_set(views[i], config_names[5]);
        if (config[conf_scrollbar].i)
            style_set(views[i], config_names[6]);

        g_signal_connect(G_OBJECT(views[i]), "load-changed",
            G_CALLBACK(uri_changed), NULL);
        g_signal_connect(G_OBJECT(views[i]), "notify::estimated-load-progress",
            G_CALLBACK(uri_load_progress), NULL);
    }
    g_signal_connect(G_OBJECT(context), "download-started",
        G_CALLBACK(download_started), NULL);
}

extern void frame_cleanup(void)
{
    free(views);
}
