#ifndef _UTIL_ATOM_H
#define _UTIL_ATOM_H

#include "frame/frame.h"

enum {
	_sulfer_uri,
	_utf8str,
	atom_len
};

extern void xdisplay_create(void);
extern void xdisplay_close(void);
extern void atoms_init(void);
extern void atom_set(struct frame *f, int a, char *value);

#endif /* _UTIL_ATOM_H */
