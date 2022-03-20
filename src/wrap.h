/*
 * wrap.h
 * Intended for wrapping the handling functions
 */

#include "frame/frame.h"
#include "frame/handle.h"
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
    navigate(current_frame_get(), arg->i);
}
