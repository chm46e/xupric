#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <string.h>

#include "util/util.h"
#include "frame/frame.h"

#include "atom.h"

static Atom atoms[atom_len];
static Display *dpy;
static int allow_atoms = 1;

void xdisplay_create(void)
{
	if (!(dpy = XOpenDisplay(NULL))) {
		debug(D_WARN, "XOpenDisplay", "failed to open the display");
		debug(D_FOLD, "", "Atoms - disabled");
		allow_atoms = 0;
	}
}

void xdisplay_close(void)
{
	XCloseDisplay(dpy);
}

void atoms_init(void)
{
	if (!(allow_atoms))
		return;

	atoms[_xupric_uri] = XInternAtom(dpy, "_XUPRIC_URI", False);
	atoms[_utf8str] = XInternAtom(dpy, "UTF8_STRING", False);
}

void atom_set(struct frame *f, int a, char *value)
{
	if (!(allow_atoms))
		return;

	XChangeProperty(dpy, f->winid, atoms[a], atoms[_utf8str], 8, PropModeReplace,
		(unsigned char *)value, strlen(value)+1);
	XSync(dpy, False);
}
