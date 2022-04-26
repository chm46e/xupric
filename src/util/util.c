#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "cfg/cfg.h"
#include "util.h"

void die(int exit_code, const char *err, ...)
{
	va_list ap;

	va_start(ap, err);
	vfprintf(stderr, err, ap);
	va_end(ap);
	exit(exit_code);
}

void debug(opt_sev sev, char *type, char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	switch (sev) {
	case D_DEBUG:
		if (!(cfg_get()[conf_debug].i))
			return;
		fprintf(stdout, "[D] %s: ", type);
		break;
	case D_WARN:
		fprintf(stdout, "[W] %s: ", type);
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
		die(2, "[ERROR] Failed to allocate memory");
	return p;
}

void *ecalloc(int num, int size)
{
	void *p;

	if (!(p = calloc(num, size)))
		die(2, "[ERROR] Failed to allocate memory");
	return p;
}

void *erealloc(void *p, int size)
{
	if (!(p = realloc(p, size)))
		die(2, "[ERROR] Failed to reallocate memory");
	return p;
}

void efree(void *p)
{
	free(p);
}
