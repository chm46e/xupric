#include <gtk/gtk.h>

#include <signal.h>
#include <wait.h>

#include "util/util.h"
#include "cfg/cfg.h"
#include "frame/frame.h"
#include "fun/fun.h"
#include "frame/view.h"
#include "frame/style/style.h"
#include "util/atom.h"
#include "uri/uri.h"
#include "cfg/config.h"
#include "fun/sql/bookmark.h"
#include "fun/sql/history.h"
#include "fun/sql/cookie.h"

static void sigchld();
static void sighup();
static void sigint();
static void setup(void);
static void parameter_handle(int argc, char *[]);
static void cleanup(void);

static void sigchld()
{
	/* SIGCHLD(17) -> child process terminates */
	while (waitpid(-1, NULL, WNOHANG) > 0);
}

static void sighup()
{
	/* SIGHUP(1) -> controlling terminal closed */
	window_close();
}

static void sigint()
{
	/* SIGINT(2) -> controlling terminal interrupt (usually ctrl+c) */
	window_close();
}

static void setup(void)
{
	if (signal(SIGCHLD, sigchld) == SIG_ERR)
		die(1, "[ERROR] Unable to install the SIGCHLD handler\n");
	if (signal(SIGHUP, sighup) == SIG_ERR)
		die(1, "[ERROR] Unable to install the SIGHUP handler\n");
	if (signal(SIGINT, sigint) == SIG_ERR)
		die(1, "[ERROR] Unable to install the SIGINT handler\n");
	sigchld();

	xdisplay_create();
	gtk_init(NULL, NULL);

	config_cache_create();
	atoms_init();
	bookmark_init(config_names[8]);
	history_init(cache_names[5]);
	cookie_init(cache_names[0]);
	style_init();
}

static void cleanup(void)
{
	bookmark_cleanup();
	history_cleanup();
	cookie_cleanup();
	view_cleanup();
	uri_cleanup();
	config_cache_cleanup();
	cfg_cleanup();

	efree(frames_get());
	xdisplay_close();
}

static void parameter_handle(int argc, char *[])
{
	if (argc >= 2) {

	} else {
		uri_init();
	}
}

int main(int argc, char *argv[])
{
	setup();
	parameter_handle(argc, argv);

	frame_list_create();
	view_show(0);

	gtk_main();

	cleanup();
	return 0;
}
