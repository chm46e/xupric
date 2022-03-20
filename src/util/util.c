/*
 * util/util.c:
 * Intended for simple utilities
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

extern void die(int exit_code, const char *err, ...)
{
	va_list ap;

	va_start(ap, err);
	vfprintf(stderr, err, ap);
	va_end(ap);
	exit(exit_code);
}

extern void *emalloc(int size)
{
	void *p;

	if(!(p = malloc(size)))
		die(2, "[ERROR] Failed to allocate memory");
	return p;
}

extern void *ecalloc(int num, int size)
{
	void *p;

	if (!(p = calloc(num, size)))
		die(2, "[ERROR] Failed to allocate memory");
	return p;
}
