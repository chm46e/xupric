#ifndef _FRAME_HANDLE_H
#define _FRAME_HANDLE_H

#include "frame/frame.h"

extern void zoom_action(struct frame *f, int action);
extern void fullscreen_toggle(struct frame *f);
extern void navigate(struct frame *f, int back);
extern void window_close();
extern int window_event_handle(GtkWidget *, GdkEvent *ev);
extern void bar_uri_enter_handle(GtkWidget *b);
extern void uri_changed(WebKitWebView *v);
extern void uri_load_progress(WebKitWebView *v);

#endif /* _FRAME_HANDLE_H */
