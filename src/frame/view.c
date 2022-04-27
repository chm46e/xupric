#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <string.h>

#include "fun/fun.h"
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

#define CLEANMASK(mask) (mask & (GDK_CONTROL_MASK|GDK_SHIFT_MASK|GDK_SUPER_MASK|GDK_MOD1_MASK))

static void resource_load_started(WebKitWebView *, WebKitWebResource *res,
	WebKitURIRequest *);
static void resource_load_failed(WebKitWebResource *, GError *err);
static void resource_load_tls_failed(WebKitWebResource *, GTlsCertificate *,
	GTlsCertificateFlags flags);
static void resource_finished(WebKitWebResource *);
static void resource_received_data(WebKitWebResource *res, guint64);
static int context_menu(WebKitWebView *, WebKitContextMenu *,
	GdkEvent *, WebKitHitTestResult *);
static int inspector_event(WebKitWebInspector *, int type);
static int scroll_event(GtkWidget *, GdkEvent *ev);
static int button_release_event(GtkWidget *, GdkEvent *ev);
static void mouse_target_changed(WebKitWebView *, WebKitHitTestResult *ht, guint);
static void view_crashed(WebKitWebView *, WebKitWebProcessTerminationReason r);
static int webkit_fullscreen(WebKitWebView *, int action);
static void *view_navigation(WebKitWebView *, WebKitNavigationAction *na);
static void uri_changed(WebKitWebView *);
static void uri_load_progress(WebKitWebView *v);
static int permission_request(WebKitWebView *, WebKitPermissionRequest *r, GtkWindow *p);

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
	WebKitWebInspector *inspector;
    WebKitCookieManager *cookiemanager;
    WebKitWebsiteDataManager *datamanager;
	WebKitUserStyleSheet **css, *blast;
	WebKitUserScript **script;
	GTlsCertificate *cert;
	conf_opt *config;
	char *content, *file;
	int i, j;

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
	css = ecalloc(style_names_len, sizeof(WebKitUserStyleSheet *));
	script = ecalloc(script_names_len, sizeof(WebKitUserScript *));

	for (i = 0; i < style_names_len; i++) {
		file = g_strdup_printf("%s%s", config_names[2], style_names[i]);
		if (!g_file_get_contents(file, &content, NULL, NULL)) {
			debug(D_WARN, "view", "failed to read custom style file: %s", file);
			continue;
		}
		const gchar *allow[] = { g_strdup_printf("https://%s/*", style_names[i]), NULL };
		css[i] = webkit_user_style_sheet_new(content,
			WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES,
			WEBKIT_USER_STYLE_LEVEL_USER,
			allow,
			NULL);

		g_free(content);
		g_free(file);
	}

	for (i = 0; i < script_names_len; i++) {
		file = g_strdup_printf("%s%s", config_names[1], script_names[i]);
		if (!g_file_get_contents(file, &content, NULL, NULL)) {
			debug(D_WARN, "view", "failed to read custom script file: %s", file);
			continue;
		}
		const gchar *allow[] = { g_strdup_printf("https://%s/*", script_names[i]), NULL };
		script[i] = webkit_user_script_new(content,
			WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES,
			WEBKIT_USER_SCRIPT_INJECT_AT_DOCUMENT_START,
			allow,
			NULL);

		g_free(content);
		g_free(file);
	}

	for (i = 0; i < cert_names_len; i++) {
		file = g_strdup_printf("%s%s", config_names[3], cert_names[i]);
		cert = g_tls_certificate_new_from_file(file, NULL);

		if (g_tls_certificate_verify(cert, NULL, NULL) & G_TLS_CERTIFICATE_VALIDATE_ALL) {
			debug(D_WARN, "view", "certificate validation failed: %s", file);
			continue;
		}

		webkit_web_context_allow_tls_certificate_for_host(context, cert, config_names[3]);

		g_free(file);
	}

	/* loading screen color, so it's not gonna blast your eyes out with the white.
	 * Also tweaks text color, so file viewer will be readable.
	 */
	blast = webkit_user_style_sheet_new(
		"body {background-color: #1d1f21; color: #c5c8c6;}",
		WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES,
		WEBKIT_USER_STYLE_LEVEL_USER, NULL, NULL);

	for (i = 0; i < 10; i++) {
		views[i] = g_object_new(WEBKIT_TYPE_WEB_VIEW,
			"settings", settings,
			"user-content-manager", webkit_user_content_manager_new(),
			"web-context", context,
			NULL);

		for (j = 0; j < style_names_len; j++) {
			webkit_user_content_manager_add_style_sheet(
				webkit_web_view_get_user_content_manager(views[i]),
				css[j]);
		}

		for (j = 0; j < script_names_len; j++) {
			webkit_user_content_manager_add_script(
				webkit_web_view_get_user_content_manager(views[i]),
				script[j]);
		}

		webkit_user_content_manager_add_style_sheet(
			webkit_web_view_get_user_content_manager(views[i]), blast);

		if (config[conf_dark_mode].i)
			dark_mode_set(views[i]);
		if (config[conf_scrollbar].i)
			style_file_set(views[i], config_names[5]);

		g_signal_connect(G_OBJECT(views[i]), "permission-request",
			G_CALLBACK(permission_request), current_frame_get()->win);
		g_signal_connect(G_OBJECT(views[i]), "load-changed",
			G_CALLBACK(uri_changed), NULL);
		g_signal_connect(G_OBJECT(views[i]), "resource-load-started",
			G_CALLBACK(resource_load_started), NULL);
		g_signal_connect(G_OBJECT(views[i]), "notify::estimated-load-progress",
			G_CALLBACK(uri_load_progress), NULL);
		g_signal_connect(G_OBJECT(views[i]), "create",
			G_CALLBACK(view_navigation), NULL);
		g_signal_connect(G_OBJECT(views[i]), "close",
			G_CALLBACK(window_close), NULL);
		g_signal_connect(G_OBJECT(views[i]), "enter-fullscreen",
			G_CALLBACK(webkit_fullscreen), (int *)1);
		g_signal_connect(G_OBJECT(views[i]), "leave-fullscreen",
			G_CALLBACK(webkit_fullscreen), (int *)0);
		g_signal_connect(G_OBJECT(views[i]), "web-process-terminated",
			G_CALLBACK(view_crashed), NULL);
		g_signal_connect(G_OBJECT(views[i]), "mouse-target-changed",
			G_CALLBACK(mouse_target_changed), NULL);
		g_signal_connect(G_OBJECT(views[i]), "button-release-event",
			G_CALLBACK(button_release_event), NULL);
		g_signal_connect(G_OBJECT(views[i]), "scroll-event",
			G_CALLBACK(scroll_event), NULL);
		g_signal_connect(G_OBJECT(views[i]), "context-menu",
			G_CALLBACK(context_menu), NULL);

		inspector = webkit_web_view_get_inspector(views[i]);
		g_signal_connect(G_OBJECT(inspector), "bring-to-front",
			G_CALLBACK(inspector_event), (int *)1);
		g_signal_connect(G_OBJECT(inspector), "closed",
			G_CALLBACK(inspector_event), (int *)0);
		g_signal_connect(G_OBJECT(inspector), "open-window",
			G_CALLBACK(inspector_event), (int *)2);
	}
	g_signal_connect(G_OBJECT(context), "download-started",
		G_CALLBACK(download_started), NULL);

	efree(css);
	efree(script);
}

static void resource_load_started(WebKitWebView *, WebKitWebResource *res,
	WebKitURIRequest *)
{
	debug(D_DEBUG, "WebKitWebView", "signal :resource-load-started:");

	g_signal_connect(G_OBJECT(res), "failed", G_CALLBACK(resource_load_failed), NULL);
	g_signal_connect(G_OBJECT(res), "failed-with-tls-errors",
		G_CALLBACK(resource_load_tls_failed), NULL);
	g_signal_connect(G_OBJECT(res), "finished", G_CALLBACK(resource_finished), NULL);
	g_signal_connect(G_OBJECT(res), "received-data",
		G_CALLBACK(resource_received_data), NULL);
}

static void resource_load_failed(WebKitWebResource *, GError *err)
{
	debug(D_DEBUG, "WebKitWebResource", "signal :failed:");
	debug(D_FOLD, "", "%i: %s", err->code, err->message);
}

static void resource_load_tls_failed(WebKitWebResource *, GTlsCertificate *,
	GTlsCertificateFlags flags)
{
	debug(D_DEBUG, "WebKitWebResource", "signal :failed-with-tls-errors:");
	debug(D_FOLD, "", "flags: %i", flags);
}

static void resource_finished(WebKitWebResource *)
{
	debug(D_DEBUG, "WebKitWebResource", "signal :finished:");
}

static void resource_received_data(WebKitWebResource *res, guint64)
{
	debug(D_DEBUG, "WebKitWebResource", "signal :received-data:");
	debug(D_FOLD, "", "%s", webkit_web_resource_get_uri(res));
}

static int context_menu(WebKitWebView *, WebKitContextMenu *,
	GdkEvent *, WebKitHitTestResult *)
{
	debug(D_DEBUG, "WebKitWebView", "signal :context-menu:");

	return 0;
}

static int inspector_event(WebKitWebInspector *, int type)
{
	struct frame *f;

	debug(D_DEBUG, "WebKitWebView", "inspector %i", type);

	f = current_frame_get();
	switch (type) {
	case 1:
		f->inspector = 1;
		break;
	case 2:
	case 0:
		f->inspector = 0;
		break;
	default:
		break;
	}
	return 0;
}

static int scroll_event(GtkWidget *, GdkEvent *ev)
{
	GdkScrollDirection d;
	struct frame *f;
	float x, y;

	debug(D_DEBUG, "WebKitWebView", "signal :scroll-event:");

	if (!gdk_event_get_scroll_deltas(ev, (double *)&x, (double *)&y)) {
		return 0;
	} else if (gdk_event_get_scroll_direction(ev, &d)) {
		switch (d) {
		case GDK_SCROLL_UP:
			x = -2;
			break;
		case GDK_SCROLL_DOWN:
			x = 2;
			break;
		case GDK_SCROLL_LEFT:
		case GDK_SCROLL_RIGHT:
		default:
			break;
		}
	}

	f = current_frame_get();
	if (CLEANMASK(ev->scroll.state) == GDK_CONTROL_MASK) {
		f->zoom -= x/70;
		webkit_web_view_set_zoom_level(f->view, f->zoom);
		zoom_label_update(f);
		return 1;
	}
	return 0;
}

static int button_release_event(GtkWidget *, GdkEvent *ev)
{
	WebKitHitTestResultContext htc;
	struct frame *f;

	debug(D_DEBUG, "WebKitWebView", "signal :button-release-event:");

	f = current_frame_get();
	if (f->ht == NULL)
		return 0;
	htc = webkit_hit_test_result_get_context(f->ht);


	if ((htc & WEBKIT_HIT_TEST_RESULT_CONTEXT_LINK ||
		htc & WEBKIT_HIT_TEST_RESULT_CONTEXT_IMAGE ||
		htc & WEBKIT_HIT_TEST_RESULT_CONTEXT_MEDIA) && ev->button.button == 2 &&
		CLEANMASK(ev->button.state) == 0) {
		new_window_spawn(f->onuri);
		return 1;
	}
	return 0;
}

static void mouse_target_changed(WebKitWebView *, WebKitHitTestResult *ht, guint)
{
	WebKitHitTestResultContext htc;
	struct frame *f;


	f = current_frame_get();
	htc = webkit_hit_test_result_get_context(ht);
	f->ht = ht;

	if (htc & WEBKIT_HIT_TEST_RESULT_CONTEXT_LINK)
		f->onuri = (char *)webkit_hit_test_result_get_link_uri(ht);
	else if (htc & WEBKIT_HIT_TEST_RESULT_CONTEXT_IMAGE)
		f->onuri = (char *)webkit_hit_test_result_get_image_uri(ht);
	else if (htc & WEBKIT_HIT_TEST_RESULT_CONTEXT_MEDIA)
		f->onuri = (char *)webkit_hit_test_result_get_media_uri(ht);
	else
		f->onuri = NULL;

	debug(D_DEBUG, "WebKitWebView", "signal :mouse-target-changed:");
	debug(D_FOLD, "", "%s", f->onuri);
}

static void view_crashed(WebKitWebView *, WebKitWebProcessTerminationReason r)
{
	switch (r) {
	case WEBKIT_WEB_PROCESS_CRASHED:
		debug(D_ERR, "WebProcess", "crashed");
		break;
	case WEBKIT_WEB_PROCESS_EXCEEDED_MEMORY_LIMIT:
		debug(D_ERR, "WebProcess", "exceeded memory limit");
		break;
	default:
		debug(D_ERR, "WebProcess", "terminated");
		break;
	}
}

static int webkit_fullscreen(WebKitWebView *, int action)
{
	debug(D_DEBUG, "WebKitWebView", "fullscreen %i", action);

	fullscreen_action(current_frame_get(), action);
	return 1;
}

static void *view_navigation(WebKitWebView *, WebKitNavigationAction *na)
{
	debug(D_DEBUG, "WebKitWebView", "signal :create:");

	switch (webkit_navigation_action_get_navigation_type(na)) {
	case WEBKIT_NAVIGATION_TYPE_OTHER:
		if (webkit_navigation_action_is_user_gesture(na))
			goto same_frame;
		goto new_window;
	case WEBKIT_NAVIGATION_TYPE_LINK_CLICKED:
	case WEBKIT_NAVIGATION_TYPE_FORM_SUBMITTED:
	case WEBKIT_NAVIGATION_TYPE_BACK_FORWARD:
	case WEBKIT_NAVIGATION_TYPE_RELOAD:
	case WEBKIT_NAVIGATION_TYPE_FORM_RESUBMITTED:
	default:
		goto same_frame;
	}

new_window:
	new_window_spawn((char *)webkit_uri_request_get_uri(
		webkit_navigation_action_get_request(na)));
	return NULL;

same_frame:
	uri_custom_load(current_frame_get(), (char *)webkit_uri_request_get_uri(
		webkit_navigation_action_get_request(na)), 0);
	return NULL;
}

static void uri_changed(WebKitWebView *)
{
	GtkBuilder *builder;
	GtkCssProvider *css;
	GtkImage *bookmark_image;
	GtkEntry *e;
	char *uri, *title;


	builder = builder_get();
	e = GTK_ENTRY(gtk_builder_get_object(builder, "bar_uri_entry"));
	uri = uri_get(current_frame_get());
	css = gtk_css_provider_new();
	if (strcmp(gtk_entry_get_text(e), uri)) {
		gtk_entry_set_text(e, uri);
	}

	debug(D_DEBUG, "WebKitWebView", "signal :load-changed:");
	debug(D_FOLD, "", "%s", uri);

	bookmark_image = GTK_IMAGE(gtk_builder_get_object(builder, "bookmark_image"));
	if (bookmark_exists(uri)) {
		gtk_css_provider_load_from_data(css, "#bookmark_image { color: #f0c674; }",
			-1, NULL);
		gtk_image_set_from_icon_name(bookmark_image, "starred-symbolic", 18);
	} else {
		gtk_css_provider_load_from_data(css, "#bookmark_image { color: #abadac; }",
			-1, NULL);
		gtk_image_set_from_icon_name(bookmark_image, "non-starred-symbolic", 18);
	}
	gtk_style_context_add_provider(gtk_widget_get_style_context(
		GTK_WIDGET(bookmark_image)), GTK_STYLE_PROVIDER(css),
		GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	if (strcmp(uri, uri_last))
		history_add(uri);
	uri_last = uri;

	title = ecalloc(60, sizeof(char));
	strcpy(title, "Xupric <");
	strncat(title, uri, 50);
	strcat(title, ">");
	gtk_window_set_title(GTK_WINDOW(current_frame_get()->win), title);
	efree(title);
}

static void uri_load_progress(WebKitWebView *v)
{
	GdkCursor *c;

	debug(D_DEBUG, "WebKitWebView", "signal :notify::estimated-load-progress: %f",
		webkit_web_view_get_estimated_load_progress(v));

	if (webkit_web_view_get_estimated_load_progress(v) != 1.0)
		c = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_WATCH);
	else
		c = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_ARROW);
	gdk_window_set_cursor(gtk_widget_get_window(current_frame_get()->win), c);
}

static int permission_request(WebKitWebView *, WebKitPermissionRequest *r, GtkWindow *p)
{
	GtkWidget *dialog;
	conf_opt *config;
	char *type, *question;
	int ret;

	debug(D_DEBUG, "WebKitWebView", "signal :permission-request:");

	config = cfg_get();
	if (WEBKIT_IS_DEVICE_INFO_PERMISSION_REQUEST(r)) {
		webkit_permission_request_allow(r);
		return 1;
	} else if (WEBKIT_IS_GEOLOCATION_PERMISSION_REQUEST(r)) {
		if (config[conf_permission_geolocation].i == 1) {
			webkit_permission_request_allow(r);
			return 1;
		} else if (config[conf_permission_geolocation].i == 0) {
			webkit_permission_request_deny(r);
			return 1;
		}
		type = "geolocation";
	} else if (WEBKIT_IS_INSTALL_MISSING_MEDIA_PLUGINS_PERMISSION_REQUEST(r)) {
		webkit_permission_request_allow(r);
		return 1;
	} else if (WEBKIT_IS_MEDIA_KEY_SYSTEM_PERMISSION_REQUEST(r)) {
		webkit_permission_request_deny(r);
		return 1;
	} else if (WEBKIT_IS_NOTIFICATION_PERMISSION_REQUEST(r)) {
		if (config[conf_permission_notification].i == 1) {
			webkit_permission_request_allow(r);
			return 1;
		} else if (config[conf_permission_notification].i == 0) {
			webkit_permission_request_deny(r);
			return 1;
		}
		type = "notification";
	} else if (WEBKIT_IS_POINTER_LOCK_PERMISSION_REQUEST(r)) {
		webkit_permission_request_deny(r);
		return 1;
	} else if (WEBKIT_IS_USER_MEDIA_PERMISSION_REQUEST(r)) {
		if (webkit_user_media_permission_is_for_audio_device(
			WEBKIT_USER_MEDIA_PERMISSION_REQUEST(r))) {
			if (config[conf_permission_microphone].i == 1) {
				webkit_permission_request_allow(r);
				return 1;
			} else if (config[conf_permission_microphone].i == 0) {
				webkit_permission_request_deny(r);
				return 1;
			}
			type = "microphone";
		} else if (webkit_user_media_permission_is_for_video_device(
			WEBKIT_USER_MEDIA_PERMISSION_REQUEST(r))) {
			if (config[conf_permission_camera].i == 1) {
				webkit_permission_request_allow(r);
				return 1;
			} else if (config[conf_permission_camera].i == 0) {
				webkit_permission_request_deny(r);
				return 1;
			}
			type = "camera";
		} else {
			type = "media";
		}
	} else if (WEBKIT_IS_WEBSITE_DATA_ACCESS_PERMISSION_REQUEST(r)) {
		webkit_permission_request_deny(r);
		return 1;
	} else {
		return 0;
	}

	question = ecalloc(30+strlen(uri_get(current_frame_get())), sizeof(char));
	strcpy(question, "Allow ");
	strcat(question, type);
	strcat(question, " access?\n");
	strcat(question, uri_get(current_frame_get()));

	dialog = gtk_message_dialog_new(p, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION,
		GTK_BUTTONS_YES_NO, question);

	gtk_widget_show(dialog);
	ret = gtk_dialog_run(GTK_DIALOG(dialog));

	switch (ret) {
	case GTK_RESPONSE_YES:
		webkit_permission_request_allow(r);
		break;
	default:
		webkit_permission_request_deny(r);
		break;
	}

	gtk_widget_destroy(dialog);
	efree(question);
	return 1;
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
	int i;

	for (i = 0; i < 10; i++)
		g_object_ref_sink(views[i]);
	efree(views);
}
