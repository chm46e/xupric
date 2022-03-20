#ifndef _FRAME_FRAME_H
#define _FRAME_FRAME_H

#include "X11/Xlib.h"
#include "gtk/gtk.h"
#include "webkit2/webkit2.h"

struct frame {
	GtkWidget *win;
	WebKitWebView *view;
	WebKitFindController *finder;
	Window winid;
	char *uri;
	int empty, fullscreen;
	float zoom;
};

extern void frame_list_create(void);
extern void view_order_show(int increment);
extern void view_show(int id);
extern struct frame **frames_get(void);
extern struct frame *current_frame_get(void);
extern GtkBuilder *builder_get(void);
extern void frame_cleanup(void);

#endif /* _FRAME_FRAME_H */
