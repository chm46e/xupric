#ifndef _CFG_CONFIG_H
#define _CFG_CONFIG_H

extern char *config_names[];
extern char *cache_names[];

void config_cache_create(void);
void config_cache_cleanup(void);

#endif /* _CFG_CONFIG_H */
