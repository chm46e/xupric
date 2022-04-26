#include "frame/frame.h"
#include "frame/offs/offs.h"
#include "fun/fun.h"
#include "uri/uri.h"

union arg {
	int i;
	float f;
	const void *p;
	char *s;
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
	fullscreen_action(current_frame_get(), 0);
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

static inline void wrap_win_find_build(union arg *)
{
	win_find_build(NULL);
}

static inline void wrap_bookmark_toggle(union arg *)
{
	bookmark_button_toggle_handle(NULL);
}

static inline void wrap_dark_mode_toggle(union arg *)
{
	dark_mode_button_toggle_handle(NULL);
}

static inline void wrap_debug_toggle(union arg *)
{
	debug_toggle();
}

static inline void wrap_download_page(union arg *)
{
	download_current_page();
}

static inline void wrap_focus_entry(union arg *)
{
	clear_focus_entry();
}

static inline void wrap_focus_secondary_entry(union arg *)
{
	clear_focus_secondary_entry();
}

static inline void wrap_uri_load(union arg *arg)
{
	uri_custom_load(current_frame_get(), arg->s, 0);
}

static inline void wrap_workspace_load(union arg *arg)
{
	workspace_load(arg->i);
}

static inline void wrap_new_window_spawn(union arg *)
{
	new_window_spawn(NULL);
}

static inline void wrap_toggle_webinspector(union arg *)
{
	toggle_webinspector();
}
