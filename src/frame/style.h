#ifndef _FRAME_STYLE_H
#define _FRAME_STYLE_H

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include <util/util.h>

static void style_file_set(WebKitWebView *view, char *file)
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

#endif /* _FRAME_STYLE_H */
