#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "cfg/cfg.h"
#include "util.h"

static opt_sev last_sev = D_DEBUG;

void debug(opt_sev sev, char *type, char *msg, ...)
{
	va_list ap;

	if (!(cfg_get()[conf_debug].i) && (sev == D_DEBUG ||
		(sev == D_FOLD && last_sev == D_DEBUG)))
		return;

	va_start(ap, msg);
	switch (sev) {
	case D_DEBUG:
		fprintf(stdout, "[D] %s: ", type);
		last_sev = D_DEBUG;
		break;
	case D_FOLD:
		fprintf(stdout, "      ");
		break;
	case D_WARN:
		fprintf(stdout, "[W] %s: ", type);
		last_sev = D_WARN;
		break;
	case D_ERR:
		fprintf(stderr, "PANIC! %s: ", type);
		vfprintf(stderr, msg, ap);
		va_end(ap);
		exit(1);
	default:
		return;
	}

	vfprintf(stdout, msg, ap);
	fprintf(stdout, "\n");

	va_end(ap);
}

void *emalloc(int size)
{
	void *p;

	if(!(p = malloc(size)))
		debug(D_ERR, "malloc", "failed to allocate memory");
	return p;
}

void *ecalloc(int num, int size)
{
	void *p;

	if (!(p = calloc(num, size)))
		debug(D_ERR, "calloc", "failed to allocate memory");
	return p;
}

void *erealloc(void *p, int size)
{
	if (!(p = realloc(p, size)))
		debug(D_ERR, "realloc", "failed to reallocate memory");
	return p;
}

void efree(void *p)
{
	free(p);
}
