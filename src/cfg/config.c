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
	"styles/",
	"certs/",
	"styles/app/dark_mode.css",
	"styles/app/scrollbar.css",
	"styles/app/main.css",
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

char **style_names;
int style_names_len;
char **script_names;
int script_names_len;
char **cert_names;
int cert_names_len;

void config_cache_create(void)
{
	GDir *dir;
	char *file;
	int i;

	config_path = home_path_expand(config_path);

	for (int i = 0; i < (int)LENGTH(config_names); i++) {
		config_names[i] = g_build_filename(config_path, config_names[i], NULL);
		if (config_names[i][strlen(config_names[i])-1] == '/') {
			efree(path_create(config_names[i]));
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
			efree(path_create(cache_names[i]));
		else
			file_create(cache_names[i]);
	}

	i = 0;
	if ((dir = g_dir_open(config_names[2], 0, NULL)) == NULL) {
		debug(D_WARN, "config", "failed to open custom style path: %s", config_names[2]);
		debug(D_FOLD, "", "Custom styles - disabled");
	}
	while ((file = (char *)g_dir_read_name(dir)) != NULL) {
		if (!(strcmp(file, "app")))
			continue;

		style_names = erealloc(style_names, (i + 1)*sizeof(style_names));
		style_names[i] = strdup(file);

		i++;
	}
	style_names_len = i;
	g_dir_close(dir);

	i = 0;
	if ((dir = g_dir_open(config_names[1], 0, NULL)) == NULL) {
		debug(D_WARN, "config", "failed to open custom script path: %s", script_names[1]);
		debug(D_FOLD, "", "Custom scripts - disabled");
	}
	while ((file = (char *)g_dir_read_name(dir)) != NULL) {
		script_names = erealloc(script_names, (i + 1)*sizeof(script_names));
		script_names[i] = strdup(file);

		i++;
	}
	script_names_len = i;
	g_dir_close(dir);

	i = 0;
	if ((dir = g_dir_open(config_names[3], 0, NULL)) == NULL) {
		debug(D_WARN, "config", "failed to open custom cert path: %s", script_names[3]);
		debug(D_FOLD, "", "Custom certs - disabled");
	}
	while ((file = (char *)g_dir_read_name(dir)) != NULL) {
		cert_names = erealloc(cert_names, (i + 1)*sizeof(cert_names));
		cert_names[i] = strdup(file);

		i++;
	}
	cert_names_len = i;
	g_dir_close(dir);
}

void config_cache_cleanup(void)
{
	int i;

	for (i = 0; i < (int)LENGTH(cache_names); i++)
		g_free(cache_names[i]);
	for (i = 0; i < (int)LENGTH(config_names); i++)
		g_free(config_names[i]);
	for (i = 0; i < style_names_len; i++)
		g_free(style_names[i]);
	for (i = 0; i < script_names_len; i++)
		g_free(script_names[i]);
	for (i = 0; i < cert_names_len; i++)
		g_free(cert_names[i]);

	g_free(config_path);
	if (style_names_len != 0)
		g_free(style_names);
	if (script_names_len != 0)
		g_free(script_names);
	if (cert_names_len != 0)
		g_free(cert_names);
}
