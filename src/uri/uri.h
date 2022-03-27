#ifndef _URI_URI_H
#define _URI_URI_H

#include "frame/frame.h"

extern void uri_init(void);
extern void uri_reload(struct frame *f, int bypass);
extern void uri_stop(struct frame *f);
extern char *uri_get(struct frame *f);
extern void uri_search_engine_load(struct frame *f);
extern void uri_custom_load(struct frame *f, char *uri, int secondary);
extern void uri_cleanup(void);

#endif /* _URI_URI_H */
