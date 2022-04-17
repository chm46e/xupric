#ifndef _URI_URI_H
#define _URI_URI_H

#include "frame/frame.h"

void uri_init(void);
void uri_reload(struct frame *f, int bypass);
void uri_stop(struct frame *f);
char *uri_get(struct frame *f);
void uri_search_engine_load(struct frame *f);
void uri_custom_load(struct frame *f, char *uri, int secondary);
void uri_cleanup(void);

#endif /* _URI_URI_H */
