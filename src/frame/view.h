#ifndef _FRAME_VIEW_H
#define _FRAME_VIEW_H

#include <webkit2/webkit2.h>

void view_order_show(int increment);
void view_show(int id);
void view_list_create(void);
WebKitWebView **views_get(void);
int view_last_get(void);
void view_cleanup(void);

#endif /* _FRAME_VIEW_H  */
