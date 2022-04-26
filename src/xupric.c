#include <gtk/gtk.h>

#include <signal.h>
#include <wait.h>
#include <stdio.h>

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
static void show_help(void);
static void show_version_info(void);
static int parameter_handle(int argc, char *argv[]);
static void cleanup(void);

static char *uri = "";

static void sigchld()
{
	/* SIGCHLD(17) -> child process terminates */
	while (waitpid(-1, NULL, WNOHANG) > 0);
}

static void sighup()
{
	/* SIGHUP(1) -> controlling terminal closed */
	debug(D_DEBUG, "signal", "controlling terminal closed");
	window_close();
}

static void sigint()
{
	/* SIGINT(2) -> controlling terminal interrupt (usually ctrl+c) */
	debug(D_DEBUG, "signal", "controlling terminal interrupt");
	window_close();
}

static void setup(void)
{
	if (signal(SIGCHLD, sigchld) == SIG_ERR)
		debug(D_WARN, "signal", "failed to install the SIGCHLD handler");
	if (signal(SIGHUP, sighup) == SIG_ERR)
		debug(D_WARN, "signal", "failed to install the SIGHUP handler");
	if (signal(SIGINT, sigint) == SIG_ERR)
		debug(D_WARN, "signal", "failed to install the SIGINT handler");
	sigchld();

	xdisplay_create();
	gtk_init(NULL, NULL);

	config_cache_create();
	atoms_init();
	bookmark_init(config_names[7]);
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
	frame_cleanup();

	xdisplay_close();
}

static void show_help(void)
{
	printf("xupric: A Modern Web Browser based on webkit2gtk\n" \
		   "Usage: xupric [options] [uri]\n\n" \
		   "Options: \n"
		   "  -p (--private) open in private mode\n" \
		   "  -d (--debug)   enable debugging\n" \
		   "  -v (--version) show version info\n" \
		   "  -h (--help)    show this message\n\n" \
		   "About: \n" \
		   "https://github.com/chm46e/xupric");
}

static void show_version_info(void)
{
	printf("Xupric, version 1.0");
}

static int parameter_handle(int argc, char *argv[])
{
	int i;
	int opt = 0;

	if (argc >= 2) {
		for (i = 1; i < argc; i++) {
			if (argv[i][0] == '-') {
				if (!(strcmp(argv[i], "-v")) ||
					!(strcmp(argv[i], "--version"))) {
					show_version_info();
					return 0;
				} else if (!(strcmp(argv[i], "-d")) ||
					!(strcmp(argv[i], "--debug"))) {
					opt |= 4;
				} else if (!(strcmp(argv[i], "-p")) ||
					!(strcmp(argv[i], "--private"))) {
					opt |= 2;
				} else {
					show_help();
					return 0;
				}
			} else {
				uri = argv[i];
				opt |= 1;
			}
		}
	}
	if (opt == 0)
		return 8;
	return opt;
}

int main(int argc, char *argv[])
{
	int opt;

	opt  = parameter_handle(argc, argv);
	if (opt == 0)
		return 0;

	setup();
	if (opt & 2)
		cfg_get()[conf_ephemeral].i = 1;
	if (opt & 4) {
		cfg_get()[conf_debug].i = 1;
	}

	uri_init();
	frame_list_create();
	debug(D_DEBUG, "all done", "ready for deployment:D");
	view_show(0);

	if (strcmp(uri, "") && opt & 1)
		uri_custom_load(current_frame_get(), uri, 0);

	gtk_main();

	cleanup();
	return 0;
}
