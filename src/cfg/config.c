#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include "util/util.h"
#include "util/path.h"
#include "util/macros.h"
#include "cfg.h"

static conf_opt *config;
static char *config_path = "~/.config/xupric";
char *config_names[] = {
	"xupric.conf",
	"scripts/",
	"scripts/script.js",
	"styles/",
	"certs/",
	"styles/dark_mode.css",
	"styles/scrollbar.css",
	"styles/main.css",
	"bookmarks.db"
};

char *cache_names[] = {
	"cookies",
	"data/",
	"itp/",
	"offline/",
	"hsts/",
	"history.db"
};

void config_cache_create(void)
{
	config_path = home_path_expand(config_path);

	for (int i = 0; i < (int)LENGTH(config_names); i++) {
		config_names[i] = g_build_filename(config_path, config_names[i], NULL);
		if (config_names[i][strlen(config_names[i])-1] == '/') {
			path_create(config_names[i]);
		} else {
			file_create(config_names[i]);
		}
	}

	config = cfg_load(config_names[0]);

	config[conf_cache_prefix].s = home_path_expand(config[conf_cache_prefix].s);
	for (int i = 0; i < (int)LENGTH(cache_names); i++) {
		cache_names[i] = g_build_filename(config[conf_cache_prefix].s,
			 cache_names[i], NULL);
		if (cache_names[i][strlen(cache_names[i])-1] == '/')
			path_create(cache_names[i]);
		else
			file_create(cache_names[i]);
	}
}

void config_cache_cleanup(void)
{
	for (int i = 0; i < (int)LENGTH(cache_names); i++)
		g_free(cache_names[i]);
	for (int i = 0; i < (int)LENGTH(config_names); i++)
		g_free(config_names[i]);

	g_free(config_path);
}
