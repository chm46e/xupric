#ifndef _UTIL_PATH_H
#define _UTIL_PATH_H

char *home_path_get(void);
char *home_path_expand(char *path);
char *path_create(char *path);
char *file_create(char *path);

#endif /* _UTIL_PATH_H */
