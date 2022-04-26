#ifndef _UTIL_UTIL_H
#define _UTIL_UTIL_H

typedef enum {
	D_DEBUG,
	D_WARN,
	D_ERR,
} opt_sev;

void die(int exit_code, const char *err, ...);
void debug(opt_sev sev, char *type, char *msg, ...);
void *emalloc(int size);
void *ecalloc(int num, int size);
void *erealloc(void *p, int size);
void efree(void *p);

#endif /* _UTIL_UTIL_H */
