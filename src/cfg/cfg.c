#include <confuse.h>
#include <string.h>

#include "util/util.h"
#include "cfg.h"

static conf_opt *config;
static int filter_len;
static int workspace_len[] = {
	0,0,0,0,0
};

conf_opt *cfg_get(void)
{
	return config;
}

conf_opt *cfg_load(char *name)
{
	cfg_t *cfg;
	int len, i;

	config = ecalloc(conf_len, sizeof(conf_opt));

	cfg_opt_t opts[] = {
		CFG_STR("cache_prefix", "~/.cache/xupric", CFGF_NONE),
		CFG_BOOL("ephemeral", cfg_false, CFGF_NONE),
		CFG_BOOL("history_autoremove", cfg_true, CFGF_NONE),
		CFG_BOOL("java", cfg_true, CFGF_NONE),
		CFG_BOOL("javascript", cfg_true, CFGF_NONE),
		CFG_BOOL("xss_auditor", cfg_true, CFGF_NONE),
		CFG_BOOL("js_auto_popups", cfg_false, CFGF_NONE),
		CFG_STR("font_family", "sans-serif", CFGF_NONE),
		CFG_INT("font_size", 16, CFGF_NONE),
		CFG_STR("charset", "UTF_8", CFGF_NONE),
		CFG_BOOL("developer_extras", cfg_true, CFGF_NONE),
		CFG_BOOL("dns_prefetching", cfg_false, CFGF_NONE),
		CFG_BOOL("caret_browsing", cfg_false, CFGF_NONE),
		CFG_BOOL("media", cfg_true, CFGF_NONE),
		CFG_BOOL("webaudio", cfg_true, CFGF_NONE),
		CFG_BOOL("webgl", cfg_true, CFGF_NONE),
		CFG_BOOL("site_quirks", cfg_true, CFGF_NONE),
		CFG_BOOL("smooth_scrolling", cfg_true, CFGF_NONE),
		CFG_STR("user_agent", "", CFGF_NONE),
		CFG_INT("hardware_accel", 2, CFGF_NONE),
		CFG_INT("cookie_policy", 2, CFGF_NONE),
		CFG_BOOL("cookie_autofilter", cfg_true, CFGF_NONE),
		CFG_STR_LIST("cookie_filter", "{\"start.duckduckgo.com\"}", CFGF_NONE),
		CFG_BOOL("itp", cfg_true, CFGF_NONE),
		CFG_BOOL("tls_error_policy", cfg_true, CFGF_NONE),
		CFG_INT("permission_geolocation", 2, CFGF_NONE),
		CFG_INT("permission_notification", 2, CFGF_NONE),
		CFG_INT("permission_microphone", 2, CFGF_NONE),
		CFG_INT("permission_camera", 2, CFGF_NONE),
		CFG_STR("search_engine", "duckduckgo", CFGF_NONE),
		CFG_STR("secondary_search_engine", "searx", CFGF_NONE),
		CFG_BOOL("dark_mode", cfg_true, CFGF_NONE),
		CFG_BOOL("ddg_dark", cfg_true, CFGF_NONE),
		CFG_BOOL("scrollbar", cfg_true, CFGF_NONE),
		CFG_BOOL("debug", cfg_false, CFGF_NONE),
		CFG_STR_LIST("workspace_1", "{}", CFGF_NONE),
		CFG_STR_LIST("workspace_2", "{}", CFGF_NONE),
		CFG_STR_LIST("workspace_3", "{}", CFGF_NONE),
		CFG_STR_LIST("workspace_4", "{}", CFGF_NONE),
		CFG_STR_LIST("workspace_5", "{}", CFGF_NONE),
		CFG_END()
	};

	cfg = cfg_init(opts, CFGF_NOCASE);
	if(cfg_parse(cfg, name) == CFG_PARSE_ERROR)
		die(1, "[ERROR] Unable to parse config: %s\n", name);

	config[conf_cache_prefix].s = strdup(cfg_getstr(cfg, "cache_prefix"));
	config[conf_ephemeral].i = cfg_getbool(cfg, "ephemeral");
	config[conf_history_autoremove].i = cfg_getbool(cfg, "history_autoremove");
	config[conf_java].i = cfg_getbool(cfg, "java");
	config[conf_javascript].i = cfg_getbool(cfg, "javascript");
	config[conf_xss_auditor].i = cfg_getbool(cfg, "xss_auditor");
	config[conf_js_auto_popups].i = cfg_getbool(cfg, "js_auto_popups");
	config[conf_font_family].s = strdup(cfg_getstr(cfg, "font_family"));
	config[conf_font_size].i = cfg_getint(cfg, "font_size");
	config[conf_charset].s = strdup(cfg_getstr(cfg, "charset"));
	config[conf_developer_extras].i = cfg_getbool(cfg, "developer_extras");
	config[conf_dns_prefetching].i = cfg_getbool(cfg, "dns_prefetching");
	config[conf_caret_browsing].i = cfg_getbool(cfg, "caret_browsing");
	config[conf_media].i = cfg_getbool(cfg, "media");
	config[conf_webaudio].i = cfg_getbool(cfg, "webaudio");
	config[conf_webgl].i = cfg_getbool(cfg, "webgl");
	config[conf_site_quirks].i = cfg_getbool(cfg, "site_quirks");
	config[conf_smooth_scrolling].i = cfg_getbool(cfg, "smooth_scrolling");
	config[conf_user_agent].s = strdup(cfg_getstr(cfg, "user_agent"));
	config[conf_hardware_accel].i = cfg_getint(cfg, "hardware_accel");
	config[conf_cookie_policy].i = cfg_getint(cfg, "cookie_policy");
	config[conf_cookie_autofilter].i = cfg_getbool(cfg, "cookie_autofilter");
	config[conf_itp].i = cfg_getbool(cfg, "itp");
	config[conf_tls_error_policy].i = cfg_getbool(cfg, "tls_error_policy");
	config[conf_permission_geolocation].i = cfg_getint(cfg, "permission_geolocation");
	config[conf_permission_notification].i = cfg_getint(cfg, "permission_notification");
	config[conf_permission_microphone].i = cfg_getint(cfg, "permission_microphone");
	config[conf_permission_camera].i = cfg_getint(cfg, "permission_camera");
	config[conf_search_engine].s = strdup(cfg_getstr(cfg, "search_engine"));
	config[conf_secondary_search_engine].s = strdup(cfg_getstr(cfg, "secondary_search_engine"));
	config[conf_dark_mode].i = cfg_getbool(cfg, "dark_mode");
	config[conf_ddg_dark].i = cfg_getbool(cfg, "ddg_dark");
	config[conf_scrollbar].i = cfg_getbool(cfg, "scrollbar");
	config[conf_debug].i = cfg_getbool(cfg, "debug");

	len = cfg_size(cfg, "cookie_filter");
	config[conf_cookie_filter].p = ecalloc(len, sizeof(*config[conf_cookie_filter].p));
	for (i = 0; i < len; i++)
		config[conf_cookie_filter].p[i] = strdup(cfg_getnstr(cfg, "cookie_filter", i));
	filter_len = i;

	len = cfg_size(cfg, "workspace_1");
	config[conf_workspace_1].p = ecalloc(len+1, sizeof(*config[conf_workspace_1].p));
	for (i = 0; i < len; i++)
		config[conf_workspace_1].p[i] = strdup(cfg_getnstr(cfg, "workspace_1", i));
	workspace_len[0] = i;

	len = cfg_size(cfg, "workspace_2");
	config[conf_workspace_2].p = ecalloc(len+1, sizeof(*config[conf_workspace_2].p));
	for (i = 0; i < len; i++)
		config[conf_workspace_2].p[i] = strdup(cfg_getnstr(cfg, "workspace_2", i));
	workspace_len[1] = i;

	len = cfg_size(cfg, "workspace_3");
	config[conf_workspace_3].p = ecalloc(len+1, sizeof(*config[conf_workspace_3].p));
	for (i = 0; i < len; i++)
		config[conf_workspace_3].p[i] = strdup(cfg_getnstr(cfg, "workspace_3", i));
	workspace_len[2] = i;

	len = cfg_size(cfg, "workspace_4");
	config[conf_workspace_4].p = ecalloc(len+1, sizeof(*config[conf_workspace_4].p));
	for (i = 0; i < len; i++)
		config[conf_workspace_4].p[i] = strdup(cfg_getnstr(cfg, "workspace_4", i));
	workspace_len[3] = i;

	len = cfg_size(cfg, "workspace_5");
	config[conf_workspace_5].p = ecalloc(len+1, sizeof(*config[conf_workspace_5].p));
	for (i = 0; i < len; i++)
		config[conf_workspace_5].p[i] = strdup(cfg_getnstr(cfg, "workspace_5", i));
	workspace_len[4] = i;

	cfg_free(cfg);

	return config;
}

int cfg_filter_len_get(void)
{
	return filter_len;
}

int *cfg_workspace_len_get(void)
{
	return workspace_len;
}

void cfg_cleanup(void)
{
	int i;

	efree(config[conf_cache_prefix].s);
	efree(config[conf_font_family].s);
	efree(config[conf_charset].s);
	efree(config[conf_user_agent].s);
	efree(config[conf_search_engine].s);
	efree(config[conf_secondary_search_engine].s);
	for (i = 0; i < filter_len; i++)
		efree(config[conf_cookie_filter].p[i]);
	efree(config[conf_cookie_filter].p);

	for (i = 0; i < workspace_len[0]; i++)
		efree(config[conf_workspace_1].p[i]);
	efree(config[conf_workspace_1].p);
	for (i = 0; i < workspace_len[1]; i++)
		efree(config[conf_workspace_2].p[i]);
	efree(config[conf_workspace_2].p);
	for (i = 0; i < workspace_len[2]; i++)
		efree(config[conf_workspace_3].p[i]);
	efree(config[conf_workspace_3].p);
	for (i = 0; i < workspace_len[3]; i++)
		efree(config[conf_workspace_4].p[i]);
	efree(config[conf_workspace_4].p);
	for (i = 0; i < workspace_len[4]; i++)
		efree(config[conf_workspace_5].p[i]);
	efree(config[conf_workspace_5].p);

	efree(config);
}
