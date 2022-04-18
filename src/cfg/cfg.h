#ifndef _CFG_CFG_H
#define _CFG_CFG_H

typedef enum {
	conf_cache_prefix, conf_ephemeral,
	conf_history_autoremove,
	conf_java, conf_javascript,
	conf_xss_auditor, conf_js_auto_popups,
	conf_font_family, conf_font_size,
	conf_charset, conf_developer_extras,
	conf_dns_prefetching, conf_caret_browsing,
	conf_media, conf_webaudio,
	conf_webgl, conf_site_quirks,
	conf_smooth_scrolling, conf_user_agent,
	conf_hardware_accel, conf_cookie_policy,
	conf_itp, conf_tls_error_policy,
	conf_search_engine, conf_secondary_search_engine,
	conf_dark_mode, conf_ddg_dark,
	conf_scrollbar, conf_debug,
	conf_len
} conf_opt_name;

typedef union {
	int i;
	float f;
	char *s;
} conf_opt;

conf_opt *cfg_get(void);
conf_opt *cfg_load(char *name);
void cfg_cleanup(void);

#endif /* _CFG_CFG_H */
