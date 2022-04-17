#ifndef _UTIL_UTIL_H
#define _UTIL_UTIL_H

void die(int exit_code, const char *err, ...);
void *emalloc(int size);
void *ecalloc(int num, int size);
void *erealloc(void *p, int size);
void efree(void *p);

#endif /* _UTIL_UTIL_H */
