#ifndef _UTIL_H
#define _UTIL_H

extern void die(int exit_code, const char *err, ...);
extern void *emalloc(int size);
extern void *ecalloc(int num, int size);

#endif /* _UTIL_H */
