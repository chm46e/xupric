#ifndef _PATH_H
#define _PATH_H

extern char *home_path_get(void);
extern char *home_path_expand(char *path);
extern char *path_create(char *path);
extern char *file_create(char *path);

#endif /* _PATH_H */
