#ifndef _CFG_CONFIG_H
#define _CFG_CONFIG_H

extern char *config_names[];
extern char *cache_names[];

extern char **style_names;
extern int style_names_len;
extern char **script_names;
extern int script_names_len;
extern char **cert_names;
extern int cert_names_len;

void config_cache_create(void);
void config_cache_cleanup(void);

#endif /* _CFG_CONFIG_H */
