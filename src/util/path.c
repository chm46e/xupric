#include <glib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>

#include "util/util.h"
#include "path.h"

char *home_path_get(void)
{
	char *hdir;

	if (!(hdir = getenv("HOME")))
		hdir = getpwuid(getuid())->pw_dir;

	if (!(strcmp(hdir, "")))
		debug(D_ERR, "path", "failed to get the home path");

	return hdir;
}

char *home_path_expand(char *path)
{
	char *hdir;

	if (!(path[0] == '~'))
		return g_strdup(path);

	if (!(path[1] == '/' || path[1] == '\0'))
		debug(D_ERR, "path", "invalid path: %s", path);

	hdir = home_path_get();
	return g_build_filename(hdir, (char *)&path[1], NULL);
}

char *path_create(char *path)
{
	char *xpath, *rpath;

	if (path[0] == '~')
		xpath = home_path_expand(path);
	else
		xpath = g_strdup(path);

	if (g_mkdir_with_parents(xpath, 0700) < 0)
		debug(D_WARN, "path", "failed to create path: %s", xpath);

	/* realpath: ~/../../../tmp/./ -> /tmp */
	rpath = realpath(xpath, NULL);

	g_free(xpath);
	return rpath;
}

void file_create(char *path)
{
	char *dname, *bname, *bpath, *fpath;
	FILE *file;

	dname = g_path_get_dirname(path);
	bname = g_path_get_basename(path);

	bpath = path_create(dname);
	g_free(dname);

	fpath = g_build_filename(bpath, bname, NULL);
	g_free(bpath);
	g_free(bname);

	if (!(file = fopen(fpath, "a")))
		debug(D_WARN, "path", "failed to create file: %s", fpath);

	if (chmod(fpath, 0600) == -1)
		debug(D_WARN, "path", "failed to change file mode to 0600: %s", fpath);

	fclose(file);
	g_free(fpath);
}
