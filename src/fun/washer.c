#include <jansson.h>
#include <pcre.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>

#include "frame/frame.h"
#include "cfg/cfg.h"
#include "cfg/config.h"
#include "util/util.h"
#include "filter.h"

struct provider {
	pcre *urlPattern;
	int completeProvider;
	pcre **rules;
	int rules_len;
	pcre **referralMarketing;
	int referralMarketing_len;
	pcre **exceptions;
	int exceptions_len;
	pcre **redirections;
	int redirections_len;
};

static struct provider *pros;
static int pros_len;

static void regex_build(pcre **reg, char *value)
{
	const char *err;
	int erro;

	*reg = pcre_compile(value, 0, &err, &erro, NULL);

	if (*reg == NULL)
		debug(D_ERR, "washer", "failed to compile: %s\n%i: %s", value, erro, err);
}

void washer_init(void)
{
	json_t *root, *providers, *pr, *f, *data;
	json_error_t err;
	void *iter;
	char s[200];
	int i, j, len, arlen;

	if (!(cfg_get()[conf_url_filter].i))
		return;

	if (!(root = json_loads(clearurls_filter, 0, &err)))
		debug(D_ERR, "washer", "failed to parse json: %s", err.text);

	providers = json_object_get(root, "providers");
	len = json_object_size(providers);
	pros_len = len;
	pros = ecalloc(len, sizeof(struct provider));

	iter = json_object_iter(providers);
	for (i = 0; i < len-1; i++) {
		iter = json_object_iter_next(providers, iter);
		pr = json_object_iter_value(iter);

		/* no checking - clearurls filters have a strict guideline */
		f = json_object_get(pr, "urlPattern");
		strcpy(s, json_string_value(f));
		regex_build(&pros[i].urlPattern, s);

		f = json_object_get(pr, "completeProvider");
		pros[i].completeProvider = json_integer_value(f);

		f = json_object_get(pr, "rules");
		arlen = json_array_size(f);
		pros[i].rules_len = arlen;
		pros[i].rules = ecalloc(arlen, sizeof(pcre *));
		for (j = 0; j < arlen; j++) {
			data = json_array_get(f, j);

			strcpy(s, "(?:&amp;|[/?#&])(?:");
			strcat(s, json_string_value(data));
			strcat(s, "=[^&]*)");
			regex_build(&pros[i].rules[j], s);
		}

		f = json_object_get(pr, "referralMarketing");
		arlen = json_array_size(f);
		pros[i].referralMarketing_len = arlen;
		pros[i].referralMarketing = ecalloc(arlen, sizeof(pcre *));
		for (j = 0; j < arlen; j++) {
			data = json_array_get(f, j);

			strcpy(s, "(?:&amp;|[/?#&])(?:");
			strcat(s, json_string_value(data));
			strcat(s, "=[^&]*)");
			regex_build(&pros[i].referralMarketing[j], s);
		}

		f = json_object_get(pr, "exceptions");
		arlen = json_array_size(f);
		pros[i].exceptions_len = arlen;
		pros[i].exceptions = ecalloc(arlen, sizeof(pcre *));
		for (j = 0; j < arlen; j++) {
			data = json_array_get(f, j);
			strcpy(s, json_string_value(data));
			regex_build(&pros[i].exceptions[j], s);
		}

		f = json_object_get(pr, "redirections");
		arlen = json_array_size(f);
		pros[i].redirections_len = arlen;
		pros[i].redirections = ecalloc(arlen, sizeof(pcre *));
		for (j = 0; j < arlen; j++) {
			data = json_array_get(f, j);
			strcpy(s, json_string_value(data));
			regex_build(&pros[i].redirections[j], s);
		}
	}

	json_decref(root);
}

char *washer_clean(char *uri)
{
	int r, i, j, k, palen;
	char dm[2] = "&";
	int ov[300];
	char *tok, **pa;
	char *newuri;

	if (!(cfg_get()[conf_url_filter].i))
		return uri;

	if (!(strcmp(uri, "")))
		return uri;

	newuri = strdup(uri);
	for (i = 0; i < pros_len-1; i++) {
		/* check if urlPattern matches */
		r = pcre_exec(pros[i].urlPattern, NULL, newuri, strlen(newuri), 0, 0, NULL, 0);
		if (r == -1) {
			goto next_provider;
		} else if (r < -1) {
			debug(D_WARN, "washer", "failed to execute regex: %i", r);
			goto next_provider;
		}

		/* completeProvider - block all filtering */
		if (pros[i].completeProvider)
			break;

		/* if matches any exceptions, then skip filtering */
		for (j = 0; j < pros[i].exceptions_len; j++) {
			if (pcre_exec(pros[i].exceptions[j], NULL, newuri, strlen(newuri),
				0, 0, NULL, 0) == 0) {
				goto next_provider;
			}
		}

		/* I have no clue if redirections work */
		for (j = 0; j < pros[i].redirections_len; j++) {
			if (pcre_exec(pros[i].redirections[j], NULL, newuri, strlen(newuri),
				0, 0, ov, 300) == 0) {
				newuri = newuri+ov[1];
			}
		}

		/* build uri field array */
		tok = strtok(newuri, dm);
		pa = ecalloc(10, sizeof(char *));
		for (j = 0; tok != NULL; j++) {
			if (j > 9)
				erealloc(pa, (j+2) * sizeof(char *));
			if (j == 0)
				pa[j] = tok;
			else
				pa[j] = g_strdup_printf("&%s", tok);
			tok = strtok(NULL, dm);
		}
		palen = j;
		if (palen == 1)
			goto next_provider;

		/* Check for rules */
		for (j = 0; j < palen; j++) {
			for (k = 0; k < pros[i].rules_len; k++) {
				if (pcre_exec(pros[i].rules[k], NULL, pa[j], strlen(pa[j]),
					0, 0, NULL, 0) == 0) {
					if (j != 0)
						pa[j] = NULL;
					goto next_field;
				}
			}
			for (k = 0; k < pros[i].referralMarketing_len; k++) {
				if (pcre_exec(pros[i].referralMarketing[k], NULL, pa[j], strlen(pa[j]),
					0, 0, NULL, 0) == 0) {
					if (j != 0)
						pa[j] = NULL;
					break;
				}
			}
next_field:
		}

		/* rebuild uri */
		strcpy(newuri, pa[0]);
		for (j = 1; j < palen; j++) {
			if (pa[j] != NULL) {
				strcat(newuri, pa[j]);
				g_free(pa[j]);
			}
		}

		efree(pa);
next_provider:
	}
	return newuri;
}

void washer_cleanup(void)
{
	int i, j;

	if (!(cfg_get()[conf_url_filter].i))
		return;

	for (i = 0; i < pros_len; i++) {
		pcre_free(pros[i].urlPattern);
		for (j = 0; j < pros[i].rules_len; j++)
			pcre_free(pros[i].rules[j]);
		efree(pros[i].rules);
		for (j = 0; j < pros[i].referralMarketing_len; j++)
			pcre_free(pros[i].referralMarketing[j]);
		efree(pros[i].referralMarketing);
		for (j = 0; j < pros[i].exceptions_len; j++)
			pcre_free(pros[i].exceptions[j]);
		efree(pros[i].exceptions);
		for (j = 0; j < pros[i].redirections_len; j++)
			pcre_free(pros[i].redirections[j]);
		efree(pros[i].redirections);
	}

	efree(pros);
}
