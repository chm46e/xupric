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
	int empty, fullscreen, dark_mode;
	float zoom;
};

void frame_list_create(void);
void bookmark_button_toggle_handle(GtkWidget *);
void dark_mode_button_toggle_handle(GtkWidget *);
struct frame *frames_get(void);
struct frame *current_frame_get(void);
GtkBuilder *builder_get(void);

#endif /* _FRAME_FRAME_H */
