#ifndef _FRAME_GWRAP_H
#define _FRAME_GWRAP_H

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <unistd.h>

#include "frame/frame.h"
#include "frame/view.h"
#include "uri/uri.h"
#include "fun/fun.h"
#include "fun/sql/bookmark.h"
#include "fun/sql/history.h"
#include "fun/sql/cookie.h"

static inline void gwrap_view_show(GtkWidget *, int id)
{
	view_show(id);
}

static inline void gwrap_uri_custom_load(GtkWidget *, char *uri)
{
	uri_custom_load(current_frame_get(), uri, 0);
}

#ifdef _FRAME_FRAME_H
static inline void gwrap_zoom_action(GtkWidget *, int action)
{
	struct frame *f;

	f = current_frame_get();
	zoom_action(f, action);
}
#endif /* _FRAME_FRAME_H */

static inline void gwrap_bookmark_remove(GtkWidget *w, struct bookmark *b)
{
	GtkWidget *box;

	bookmark_remove(b);

	box = gtk_widget_get_parent(w);
	gtk_widget_destroy(box);
}

static inline void gwrap_history_remove(GtkWidget *w, struct history *h)
{
	GtkWidget *box;

	history_remove(h);

	box = gtk_widget_get_parent(w);
	gtk_widget_destroy(box);
}

static inline void gwrap_history_remove_all(GtkWidget *)
{
	history_remove_all();
}

static inline void gwrap_cookie_remove(GtkWidget *w, struct cookie *c)
{
	GtkWidget *box;

	cookie_remove(c);

	box = gtk_widget_get_parent(w);
	gtk_widget_destroy(box);
}

static inline void gwrap_cookie_remove_all(GtkWidget *)
{
	cookie_remove_all();
}

#endif /* _FRAME_GWRAP_H */
