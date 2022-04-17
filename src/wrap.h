#include "frame/frame.h"
#include "fun/fun.h"
#include "uri/uri.h"

union arg {
	int i;
	float f;
	const void *p;
};

static inline void wrap_view_show(union arg *arg)
{
	view_show(arg->i);
}

static inline void wrap_window_close(union arg *)
{
	window_close();
}

static inline void wrap_uri_reload(union arg *arg)
{
	uri_reload(current_frame_get(), arg->i);
}

static inline void wrap_view_order_show(union arg *arg)
{
	view_order_show(arg->i);
}

static inline void wrap_uri_stop(union arg *)
{
	uri_stop(current_frame_get());
	webkit_find_controller_search_finish(
		webkit_web_view_get_find_controller(current_frame_get()->view));
}

static inline void wrap_fullscreen_toggle(union arg *)
{
	fullscreen_toggle(current_frame_get());
}

static inline void wrap_zoom_action(union arg *arg)
{
	zoom_action(current_frame_get(), arg->i);
}

static inline void wrap_uri_search_engine_load(union arg *)
{
	uri_search_engine_load(current_frame_get());
}

static inline void wrap_navigate(union arg *arg)
{
	view_navigate(current_frame_get(), arg->i);
}

static inline void wrap_find_start(union arg *)
{
	webkit_find_controller_search(
		webkit_web_view_get_find_controller(current_frame_get()->view),
		gtk_clipboard_wait_for_text(gtk_clipboard_get_default(
			GDK_DISPLAY(gtk_widget_get_display(current_frame_get()->win)))),
		WEBKIT_FIND_OPTIONS_CASE_INSENSITIVE|WEBKIT_FIND_OPTIONS_WRAP_AROUND,
		1000);
}

static inline void wrap_find_next(union arg *)
{
	webkit_find_controller_search_next(webkit_web_view_get_find_controller(
		current_frame_get()->view));
}

static inline void wrap_find_previous(union arg *)
{
	webkit_find_controller_search_previous(webkit_web_view_get_find_controller(
		current_frame_get()->view));
}
