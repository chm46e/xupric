#ifndef _EXTRAS_HISTORY_H
#define _EXTRAS_HISTORY_H

extern void history_init(char *file);
extern void history_add(char *text);
extern void history_remove(char *text);
extern void history_remove_all(void);
extern char **history_get(void);
extern char **history_p_get(void);
extern int history_len_get(void);
extern void history_cleanup(void);

#endif /* _EXTRAS_HISTORY_H */
