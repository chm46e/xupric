#ifndef _FRAME_HANDLE_H
#define _FRAME_HANDLE_H

#include "frame/frame.h"

extern void zoom_action(struct frame *f, int action);
extern void fullscreen_toggle(struct frame *f);
extern void navigate(struct frame *f, int back);
extern void window_close();
extern int window_event_handle(GtkWidget *, GdkEvent *ev);
extern void bar_uri_enter_handle(GtkWidget *b);
extern void uri_changed(WebKitWebView *);
extern void bookmark_button_toggle_handle(GtkWidget *);
extern void uri_load_progress(WebKitWebView *v);
extern void debug_toggle(GtkWidget *);
extern void win_bookmark_build(GtkWidget *);
extern void win_history_build(GtkWidget *);
extern void win_bookmark_empty(GtkWidget *mbox);
extern void win_cookie_build(GtkWidget *);

#endif /* _FRAME_HANDLE_H */
