#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <string.h>

#include "util/util.h"
#include "frame/frame.h"

#include "atom.h"

static Atom atoms[atom_len];
static Display *dpy;

void xdisplay_create(void)
{
	if (!(dpy = XOpenDisplay(NULL)))
		die(1, "[ERROR] Unable to open the default display\n");
}

void xdisplay_close(void)
{
	XCloseDisplay(dpy);
}

void atoms_init(void)
{
	atoms[_xupric_uri] = XInternAtom(dpy, "_XUPRIC_URI", False);
	atoms[_utf8str] = XInternAtom(dpy, "UTF8_STRING", False);
}

void atom_set(struct frame *f, int a, char *value)
{
	XChangeProperty(dpy, f->winid, atoms[a], atoms[_utf8str], 8, PropModeReplace,
		(unsigned char *)value, strlen(value)+1);
	XSync(dpy, False);
}
