/*
 * cfg/config.c:
 * Intended for creating the config/cache files/dirs
 */

#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include "util/util.h"
#include "util/path.h"
#include "util/macros.h"
#include "cfg.h"

static conf_opt *config;
static char *config_path = "~/.config/sulfer";
char *config_names[] = {
    "sulfer.conf",
    "scripts/",
    "scripts/script.js",
    "styles/",
    "certs/",
    "styles/dark_mode.css",
    "styles/scrollbar.css",
    "styles/main.css",
    "main.ui"
};

char *cache_names[] = {
    "cookies",
    "data/",
    "itp/",
    "offline/",
    "hsts/"
};

static void sulfer_conf_build(void);
static void dark_mode_css_build(void);
static void scrollbar_css_build(void);
static void main_css_build(void);

extern void config_cache_create(void)
{
    struct stat st;

    config_path = home_path_expand(config_path);

    for (int i = 0; i < (int)LENGTH(config_names); i++) {
        config_names[i] = g_build_filename(config_path, config_names[i], NULL);
        if (config_names[i][strlen(config_names[i])-1] == '/') {
            path_create(config_names[i]);
        } else {
            file_create(config_names[i]);
        }
    }

    config = cfg_load(config_names[0]);

    config[conf_cache_prefix].s = home_path_expand(config[conf_cache_prefix].s);
    for (int i = 0; i < (int)LENGTH(cache_names); i++) {
        cache_names[i] = g_build_filename(config[conf_cache_prefix].s, cache_names[i], NULL);
        if (cache_names[i][strlen(cache_names[i])-1] == '/')
            path_create(cache_names[i]);
        else
            file_create(cache_names[i]);
    }

    stat(config_names[0], &st);
    if (st.st_size == 0)
        sulfer_conf_build();

    stat(config_names[5], &st);
    if (st.st_size == 0)
        dark_mode_css_build();

    stat(config_names[6], &st);
    if (st.st_size == 0)
        scrollbar_css_build();

    stat(config_names[7], &st);
    if (st.st_size == 0)
        main_css_build();
}

extern void config_cache_cleanup(void)
{
    for (int i = 0; i < (int)LENGTH(cache_names); i++)
        g_free(cache_names[i]);
    for (int i = 0; i < (int)LENGTH(config_names); i++)
        g_free(config_names[i]);

    g_free(config_path);
}

/*
 * This was used for file to str conversion:
 * https://tomeko.net/online_tools/cpp_text_escape.php?lang=en
 * (it's a super hacky way, but it'll do for now)
 */

static void sulfer_conf_build(void)
{
    FILE *file;
    char *content;

    content =
"# Path to cache storage\n"
"# default: \"~/.cache/sulfer\"\n"
"cache_prefix = \"~/.cache/sulfer\"\n"
"\n"
"# Default Ephemeral (private browsing mode)\n"
"# default: false\n"
"ephemeral = false\n"
"\n"
"# Allow Java execution\n"
"# default: true\n"
"java = true\n"
"\n"
"# Allow JavaScript execution\n"
"# default: true\n"
"javascript = true\n"
"\n"
"# XSS auditor (filter XSS attacks)\n"
"# default: true\n"
"xss_auditor = true\n"
"\n"
"# Allow opening JavaScript popups automatically\n"
"# default: false\n"
"js_auto_popups = false\n"
"\n"
"# Default font family\n"
"# default: \"sans-serif\"\n"
"font_family = \"sans-serif\"\n"
"\n"
"# Default font size\n"
"# default: 16\n"
"font_size = 16\n"
"\n"
"# Default charset\n"
"# default: \"UTF-8\"\n"
"charset = \"UTF-8\"\n"
"\n"
"# Enable developer tools like Web Inspector\n"
"# default: false\n"
"developer_extras = false\n"
"\n"
"# Prefetch domain names (attempt to resolve before following a link)\n"
"# default: false\n"
"dns_prefetching = false\n"
"\n"
"# Enable caret browsing\n"
"# default: false\n"
"caret_browsing = false\n"
"\n"
"# Media playback support (<audio>, <track>, <video>)\n"
"# default: true\n"
"media = true\n"
"\n"
"# WebAudio support\n"
"# default: true\n"
"webaudio = true\n"
"\n"
"# WebGL support\n"
"# default: true\n"
"webgl = true\n"
"\n"
"# Site-specific workarounds for better web compatibility\n"
"# default: true\n"
"site_quirks = true\n"
"\n"
"# Smooth scrolling\n"
"# default: true\n"
"smooth_scrolling = true\n"
"\n"
"# Set a custom user-agent (leave empty for default)\n"
"# default: \"\"\n"
"user_agent = \"\"\n"
"\n"
"# Hardware acceleration policy (2 - on demand, 1 - always, 0 - never)\n"
"# default: 2\n"
"hardware_accel = 2\n"
"\n"
"# Cookie accept policy (2 - no third party, 1 - always, 0 - never)\n"
"# default: 2\n"
"cookie_policy = 2\n"
"\n"
"# Intelligent Tracking Prevention (ITP)\n"
"# about: https://webkit.org/blog/7675/intelligent-tracking-prevention/\n"
"# default: true\n"
"itp = true\n"
"\n"
"# Handle TLS errors (false - ignore all TLS errors)\n"
"# default: true\n"
"tls_error_policy = true\n"
"\n"
"# Default search engine (duckduckgo, searx, startpage, google, yandex or full url)\n"
"# default: \"duckduckgo\"\n"
"search_engine = \"duckduckgo\"\n"
"\n"
"# Dark mode through ~/.config/sulfer/styles/dark_mode.css\n"
"# default: true\n"
"dark_mode = true\n"
"\n"
"# Custom scrollbar through ~/.config/sulfer/styles/scrollbar.css\n"
"# default: true\n"
"scrollbar = true";

    file = fopen(config_names[0], "w");
    fprintf(file, content);
    fclose(file);
}

static void dark_mode_css_build(void)
{
    FILE *file;
    char *content;

    content =
"html,\n"
"body,\n"
":not([style*=\"background-color:\"]) {\n"
"\t/* background color */\t\t\n"
"\tbackground-color: #1d1f21 !important;\n"
"}\n"
"\n"
"* {\n"
"\tbox-shadow: none !important;\n"
"}\n"
"\n"
"html,\n"
"body,\n"
":not([style*=\"color:\"]),\n"
".sr-reader *:not([class*='sr-pivot']) {\n"
"\t/* general text color */\n"
"\tcolor: #c5c8c6 !important;\n"
"}\n"
"h1:not([style*=\"color:\"]),\n"
"h2:not([style*=\"color:\"]),\n"
"h3:not([style*=\"color:\"]),\n"
"h4:not([style*=\"color:\"]),\n"
"h5:not([style*=\"color:\"]),\n"
"h6:not([style*=\"color:\"]) {\n"
"\t/* header text color */\n"
"\tcolor: #c5c8c6 !important;\n"
"}\n"
"cite:not([style*=\"color:\"]) {\n"
"\t/* some wierd cite idk */\n"
"\tcolor: #b5bd68 !important;\n"
"}\n"
"input:not([style*=\"background-color:\"]),\n"
"textarea:not([style*=\"background-color:\"]),\n"
"button:not([style*=\"background-color:\"]),\n"
"[role=\"button\"] {\n"
"\t/* input/btn/txt background color (looks wierd if different) */\n"
"\tbackground-color: #1d1f21 !important;\n"
"}\n"
"input:not([style*=\"background-color:\"]):hover,\n"
"textarea:not([style*=\"background-color:\"]):hover,\n"
"button:not([style*=\"background-color:\"]):hover,\n"
"[role=\"button\"]:hover {\n"
"\t/* input/btn/txt hover color */\n"
"\tbackground-color: rgba(29, 31, 33, 0.8) !important;\n"
"}\n"
"input:not([style*=\"background-color:\"]):active, input:not([style*=\"background-color:\"]):focus,\n"
"textarea:not([style*=\"background-color:\"]):active, textarea:not([style*=\"background-color:\"]):focus,\n"
"button:not([style*=\"background-color:\"]):active, button:not([style*=\"background-color:\"]):focus,\n"
"[role=\"button\"]:active, [role=\"button\"]:focus {\n"
"\t/* input/btn/txt active color */\n"
"\tbackground-color: #1d1f21 !important;\n"
"}\n"
"a:not([style*=\"color:\"]) {\n"
"\t/* link color */\n"
"\tcolor: #e3e5e3 !important;\n"
"}\n"
"a:not([style*=\"color:\"]):hover {\n"
"\t/* link hover color */\n"
"\tcolor: #e3e5e3 !important;\n"
"}\n"
"a:not([style*=\"color:\"]):active, a:not([style*=\"color:\"]):focus {\n"
"\t/* link active color */\n"
"\tcolor: #e3e5e3 !important;\n"
"}\n"
"input:not([style*=\"background-image:\"]),\n"
"textarea:not([style*=\"background-image:\"]) {\n"
"\tbackground-image: none !important;\n"
"}";

    file = fopen(config_names[5], "w");
    fprintf(file, content);
    fclose(file);
}

static void scrollbar_css_build(void)
{
    FILE *file;
    char *content;

    content =
"::-webkit-scrollbar {\n"
"\twidth: 5px;\n"
"\theight: 5px;\n"
"\tbackground-color: #1d1f21;\n"
"}\n"
"\n"
"::-webkit-scrollbar-track {\n"
"\tbackground-color: #282a2e;\n"
"\tborder-radius: 5px;\n"
"}\n"
"\n"
"::-webkit-scrollbar-thumb {\n"
"\twidth: 5px;\n"
"\tbackground-color: #4b5159;\n"
"\tborder-radius: 5px;\n"
"\tbox-shadow: 0 0 2px #41454c; \n"
"}\n"
"\n"
"::-webkit-scrollbar-thumb:hover {\n"
"\tbackground-color: #41454c;\n"
"}\n"
"\n"
"::-webkit-scrollbar-thumb:active {\n"
"\tbackground-color: #373b41;\n"
"} \n"
"\n"
"::-webkit-scrollbar-corner {\n"
"\tbackground-color: #1d1f21;\n"
"}";

    file = fopen(config_names[6], "w");
    fprintf(file, content);
    fclose(file);
}

static void main_css_build(void)
{
    FILE *file;
    char *content;

    content =
"@define-color main rgba(29, 31, 33, 1); /* #1d1f21 */\n"
"@define-color main_hover rgba(40, 43, 45, 1); /* #282b2d */\n"
"@define-color main_active rgba(51, 55, 58, 1); /* #33373a */\n"
"@define-color uri rgba(40, 42, 46, 1); /* #282a2e */\n"
"@define-color uri_hover rgba(53, 55, 61, 1); /* #35373d */\n"
"@define-color text rgba(171, 173, 172, 1); /* #abadac */\n"
"@define-color text_hl rgba(70, 71, 70, 1); /* #464746 */\n"
"\n"
"* {\n"
"\tcolor: @text;\n"
"\tbackground-color: @main;\n"
"\tmin-width: 0px;\n"
"\tmin-height: 0px;\n"
"}\n"
"\n"
"#bar_uri_entry {\n"
"\tbackground-color: @uri;\n"
"\tbox-shadow: 0 0 1px @uri_hover;\n"
"\tborder: 0px;\n"
"\tborder-radius: 7px;\n"
"\n"
"\tcolor: @text;\n"
"\tfont-family: \"JetBrainsMono Nerd Font\", sans-serif;\n"
"\tfont-weight: bold;\n"
"\tcaret-color: @text;\n"
"}\n"
"\n"
"*:disabled {\n"
"\tbackground-color: #cc6666;\n"
"}\n"
"\n"
"button {\n"
"\t-gtk-icon-shadow: none;\n"
"\tbackground-image: none;\n"
"\tborder: none;\n"
"}\n"
"\n"
"button:hover {\n"
"\tbox-shadow: 0 0 3px @main_active;\n"
"}\n"
"\n"
"button:active {\n"
"\tbox-shadow: 0 0 10px @main_active;\n"
"}\n"
"\n"
"menu, menu * {\n"
"\tbackground-color: @uri;\n"
"\tborder: 0px;\n"
"}\n"
"\n"
"selection {\n"
"\tbackground-color: @text_hl;\n"
"}";

    file = fopen(config_names[7], "w");
    fprintf(file, content);
    fclose(file);
}
