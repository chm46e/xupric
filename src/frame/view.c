#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "fun/sql/bookmark.h"
#include "fun/sql/history.h"
#include "frame/frame.h"
#include "frame/style/style.h"
#include "util/util.h"
#include "cfg/cfg.h"
#include "cfg/config.h"
#include "uri/uri.h"

#include "download.h"
#include "view.h"

static void *uri_blank_handle(WebKitWebView *, WebKitNavigationAction *na);
static void uri_changed(WebKitWebView *);
static void uri_load_progress(WebKitWebView *v);

static WebKitWebView **views;
static int last = 0;
static char *uri_last = "";

void view_order_show(int increment)
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

void view_show(int id)
{
	GtkBuilder *builder;
	WebKitWebView *dview;
	GtkImage *dark_mode_image;
	GtkWidget *box;
	struct frame *frames;
	char zoom[5];

	frames = frames_get();
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
		uri_search_engine_load(&frames[id]);
		frames[id].empty = 0;
	}

	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "bar_uri_entry")),
		uri_get(&frames[id]));
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder,
		"bar_uri_entry_secondary")), "");

	sprintf(zoom, "%i%%", (int)(frames[id].zoom*100));
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
	"menu_zoom_reset_label")), zoom);

	dark_mode_image = GTK_IMAGE(gtk_builder_get_object(builder, "dark_mode_image"));
	if (frames[id].dark_mode)
		gtk_image_set_from_icon_name(dark_mode_image, "night-light-symbolic", 18);
	else
		gtk_image_set_from_icon_name(dark_mode_image, "night-light-disabled-symbolic",
			18);
}

void view_list_create(void)
{
    WebKitSettings *settings;
    WebKitWebContext *context;
    WebKitCookieManager *cookiemanager;
    WebKitWebsiteDataManager *datamanager;
	conf_opt *config;

	config = cfg_get();
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
		webkit_settings_set_hardware_acceleration_policy(settings,
			WEBKIT_HARDWARE_ACCELERATION_POLICY_NEVER);
	else if (config[conf_hardware_accel].i == 1)
		webkit_settings_set_hardware_acceleration_policy(settings,
			WEBKIT_HARDWARE_ACCELERATION_POLICY_ALWAYS);
	else
		webkit_settings_set_hardware_acceleration_policy(settings,
			WEBKIT_HARDWARE_ACCELERATION_POLICY_ON_DEMAND);

	datamanager = webkit_website_data_manager_new(
		"base-cache-directory", config[conf_cache_prefix],
		"base-data-directory", cache_names[1],
		"itp-directory", cache_names[2],
		"offline-application-cache-directory", cache_names[3],
		"hsts-cache-directory", cache_names[4],
		NULL
	);

	webkit_website_data_manager_set_itp_enabled(datamanager, config[conf_itp].i);
	webkit_website_data_manager_set_tls_errors_policy(datamanager,
		config[conf_tls_error_policy].i);

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
			webkit_cookie_manager_set_accept_policy(cookiemanager,
				WEBKIT_COOKIE_POLICY_ACCEPT_NEVER);
		else if (config[conf_cookie_policy].i == 1)
			webkit_cookie_manager_set_accept_policy(cookiemanager,
				WEBKIT_COOKIE_POLICY_ACCEPT_ALWAYS);
		else
			webkit_cookie_manager_set_accept_policy(cookiemanager,
				WEBKIT_COOKIE_POLICY_ACCEPT_NO_THIRD_PARTY);

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
			dark_mode_set(views[i]);
		if (config[conf_scrollbar].i)
			style_file_set(views[i], config_names[6]);

		g_signal_connect(G_OBJECT(views[i]), "load-changed",
			G_CALLBACK(uri_changed), NULL);
		g_signal_connect(G_OBJECT(views[i]), "notify::estimated-load-progress",
			G_CALLBACK(uri_load_progress), NULL);
		g_signal_connect(G_OBJECT(views[i]), "create",
			G_CALLBACK(uri_blank_handle), NULL);
	}
	g_signal_connect(G_OBJECT(context), "download-started",
		G_CALLBACK(download_started), NULL);
}

static void *uri_blank_handle(WebKitWebView *, WebKitNavigationAction *na)
{
	uri_custom_load(current_frame_get(), (char *)webkit_uri_request_get_uri(
		webkit_navigation_action_get_request(na)), 0);

	return NULL;
}

static void uri_changed(WebKitWebView *)
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

static void uri_load_progress(WebKitWebView *v)
{
	GdkCursor *c;

	if (webkit_web_view_get_estimated_load_progress(v) != 1.0)
		c = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_WATCH);
	else
		c = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_ARROW);
	gdk_window_set_cursor(gtk_widget_get_window(current_frame_get()->win), c);
}

WebKitWebView **views_get(void)
{
	return views;
}

int view_last_get(void)
{
	return last;
}

void view_cleanup(void)
{
	free(views);
}
