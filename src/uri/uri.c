/*
 * uri/uri.c:
 * Intended for uri related functions
 */

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <stdlib.h>

#include "util/atom.h"
#include "util/path.h"
#include "util/util.h"
#include "frame/frame.h"
#include "cfg/cfg.h"
#include "uri.h"

static char *search_engine;

static char *search_engine_get(char *opt);

extern void uri_init(void)
{
    search_engine = search_engine_get(cfg_get()[conf_search_engine].s);
}

extern void uri_reload(struct frame *f, int bypass)
{
    if (bypass)
        webkit_web_view_reload_bypass_cache(f->view);
    else
        webkit_web_view_reload(f->view);
}

extern void uri_stop(struct frame *f)
{
    webkit_web_view_stop_loading(f->view);
}

static char *search_engine_get(char *opt)
{
    if (g_str_has_prefix(opt, "https://") ||
        g_str_has_prefix(opt, "http://")) {
        return strdup(opt);
    }

    if (!(strcmp(opt, "duckduckgo")))
        return strdup("https://start.duckduckgo.com/");
    else if (!(strcmp(opt, "searx")))
        return strdup("https://searx.thegpm.org/");
    else if (!(strcmp(opt, "startpage")))
        return strdup("https://www.startpage.com/search");
    else if (!(strcmp(opt, "google")))
        return strdup("https://www.google.com/search");
    else if (!(strcmp(opt, "yandex")))
        return strdup("https://www.yandex.com/");
    else
        die(1, "[ERROR] Unknown search engine\n");

    return strdup("https://start.duckduckgo.com/");
}

extern char *uri_get(struct frame *f)
{
    char *uri;

    if (!(uri = (char *)webkit_web_view_get_uri(f->view)))
        uri = "about:blank";

    return uri;
}

extern void uri_search_engine_load(struct frame *f)
{
    atom_set(f, _sulfer_uri, search_engine);

    if (!(strcmp(search_engine, uri_get(f)))) {
        uri_reload(f, 0);
    } else {
        webkit_web_view_load_uri(f->view, search_engine);
    }
}

extern void uri_custom_load(struct frame *f, char *uri)
{
    char *url, *xpath, *rpath;

    if (!(strcmp(uri, "")))
        return;

    if (g_str_has_prefix(uri, "http://") ||
        g_str_has_prefix(uri, "https://") ||
        g_str_has_prefix(uri, "file://") ||
        g_str_has_prefix(uri, "about:")) {
        url = g_strdup(uri);
    } else {
        if (uri[0] == '~')
            xpath = home_path_expand(uri);
        else
            xpath = (char *)uri;

        if (!(access(xpath, R_OK)) && (rpath = realpath(xpath, NULL))) {
            url = g_strdup_printf("file://%s", rpath);
            free(rpath);
        } else {
            url = g_strdup_printf("%s?q=%s", search_engine_get(search_engine), uri);
        }
        if (xpath != uri)
            free(xpath);
    }

    atom_set(f, _sulfer_uri, url);

    if (!(strcmp(url, uri_get(f)))) {
        uri_reload(f, 0);
    } else {
        webkit_web_view_load_uri(f->view, url);
        //updatetitle;
    }

    g_free(url);
}

extern void uri_cleanup(void)
{
    free(search_engine);
}
