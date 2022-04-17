#ifndef _FUN_SQL_COOKIE_H
#define _FUN_SQL_COOKIE_H

struct cookie {
	int id;
	char *host;
	char *name;
};

void cookie_init(char *file);
void cookie_remove(struct cookie *c);
void cookie_remove_all(void);
struct cookie *cookie_get(void);
struct cookie *cookie_p_get(void);
int cookie_len_get(void);
void cookie_ddg_dark_theme_set(void);
void cookie_cleanup(void);

#endif /* _FUN_SQL_COOKIE_H */
