#ifndef _FUN_SQL_HISTORY_H
#define _FUN_SQL_HISTORY_H

struct history {
	int id;
	char *uri;
};

void history_init(char *file);
void history_add(char *text);
void history_remove(struct history *h);
void history_remove_all(void);
struct history *history_get(void);
struct history *history_p_get(void);
int history_len_get(void);
void history_cleanup(void);

#endif /* _FUN_SQL_HISTORY_H */
