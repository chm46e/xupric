#ifndef _UTIL_ATOM_H
#define _UTIL_ATOM_H

#include "frame/frame.h"

enum {
	_xupric_uri,
	_utf8str,
	atom_len
};

void xdisplay_create(void);
void xdisplay_close(void);
void atoms_init(void);
void atom_set(struct frame *f, int a, char *value);

#endif /* _UTIL_ATOM_H */
