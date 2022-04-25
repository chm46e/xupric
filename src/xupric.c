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

	if (argc >= 2) {
		for (i = 1; i < argc; i++) {
			if (argv[i][0] == '-') {
				if (!(strcmp(argv[i], "-v")) || !(strcmp(argv[i], "--version"))) {
					show_version_info();
					return 0;
				} else if (!(strcmp(argv[i], "-d")) || !(strcmp(argv[i], "--debug"))) {
					cfg_get()[conf_debug].i = 1;
				} else if (!(strcmp(argv[i], "-p")) || !(strcmp(argv[i], "--private"))) {
					cfg_get()[conf_ephemeral].i = 1;
				} else {
					show_help();
					return 0;
				}
			} else {
				uri = argv[i];
				return 1;
			}
		}
	}
	return 1;
}

int main(int argc, char *argv[])
{
	setup();

	if (parameter_handle(argc, argv)) {
		uri_init();
		frame_list_create();
		view_show(0);

		if (strcmp(uri, ""))
			uri_custom_load(current_frame_get(), uri, 0);

		gtk_main();
	}

	cleanup();
	return 0;
}
