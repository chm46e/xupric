#ifndef _EXTRAS_COOKIE_H
#define _EXTRAS_COOKIE_H

extern void cookie_init(char *file);
extern void cookie_remove(char *text);
extern void cookie_remove_all(void);
extern char **cookie_get(void);
extern char **cookie_p_get(void);
extern int cookie_len_get(void);
extern void cookie_cleanup(void);

#endif /* _EXTRAS_COOKIE_H */
