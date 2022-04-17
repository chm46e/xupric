#ifndef _FUN_SQL_BOOKMARK_H
#define _FUN_SQL_BOOKMARK_H

struct bookmark {
	int id;
	char *uri;
};

void bookmark_init(char *file);
void bookmark_add(char *text);
void bookmark_remove(struct bookmark *b);
void bookmark_remove_by_uri(char *uri);
struct bookmark *bookmark_get(void);
struct bookmark *bookmark_p_get(void);
int books_len_get(void);
int bookmark_exists(char *book);
void bookmark_cleanup(void);

#endif /* _FUN_SQL_BOOKMARK_H */
