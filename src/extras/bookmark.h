#ifndef _EXTRAS_BOOKMARK_H
#define _EXTRAS_BOOKMARK_H

extern void bookmark_init(char *file);
extern void bookmark_add(char *text);
extern void bookmark_remove(char *text);
extern char **bookmark_get(void);
extern char **books_get(void);
extern int bookmark_exists(char *book);
extern void bookmark_cleanup(void);

#endif /* _EXTRAS_BOOKMARK_H */
