#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "cfg/config.h"
#include "util/util.h"

static WebKitUserStyleSheet *dark_style = NULL;

void style_init(void)
{
	gchar *style;

	if (!g_file_get_contents(config_names[4], &style, NULL, NULL)) {
		debug(D_WARN, "style", "failed to read the dark style file: %s", config_names[4]);
		debug(D_FOLD, "", "Dark Mode - disabled");
		return;
	}

	dark_style = webkit_user_style_sheet_new(style,
		WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES,
		WEBKIT_USER_STYLE_LEVEL_USER,
		NULL, NULL);

	g_free(style);
}

void style_file_set(WebKitWebView *view, char *file)
{
	WebKitUserStyleSheet *sh;
	gchar *style;

	if (!g_file_get_contents(file, &style, NULL, NULL)) {
		debug(D_WARN, "style", "failed to read style file: %s", file);
		return;
	}

	sh = webkit_user_style_sheet_new(style,
		WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES,
		WEBKIT_USER_STYLE_LEVEL_USER,
		NULL, NULL);
	webkit_user_content_manager_add_style_sheet(
		webkit_web_view_get_user_content_manager(view),
		sh);
	g_free(style);
}

void dark_mode_set(WebKitWebView *view)
{
	if (dark_style == NULL)
		return;

	webkit_user_content_manager_add_style_sheet(
		webkit_web_view_get_user_content_manager(view),
		dark_style);
}

void dark_mode_remove(WebKitWebView *view)
{
	if (dark_style == NULL)
		return;

	webkit_user_content_manager_remove_style_sheet(
		webkit_web_view_get_user_content_manager(view),
		dark_style);
}
