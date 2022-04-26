#include <sqlite3.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "cfg/cfg.h"
#include "util/util.h"
#include "history.h"

static sqlite3 *db;
static struct history *history;
static int history_len;

void history_init(char *file)
{
	char *sql;
	conf_opt *config;

	if (sqlite3_open(file, &db))
		debug(D_ERR, "history", "failed to open the history database: %s", file);

	sql = "CREATE TABLE history(id INTEGER NOT NULL UNIQUE" \
		", uri TEXT NOT NULL, PRIMARY KEY (id AUTOINCREMENT));";

	/* ignore already exists errors */
	sqlite3_exec(db, sql, NULL, NULL, NULL);

	config = cfg_get();
	if (config[conf_history_autoremove].i)
		history_remove_all();
}

void history_add(char *text)
{
	char *sql;
	char *err;

	sql = ecalloc(strlen(text) + 40, sizeof(char));

	strcpy(sql, "INSERT INTO history(uri) VALUES(\'");
	strcat(sql, text);
	strcat(sql, "\');");

	sqlite3_exec(db, sql, NULL, NULL, &err);

	if (err)
		debug(D_WARN, "history", "failed to add history: %s", err);

	efree(sql);
}

void history_remove(struct history *h)
{
	char *sql;
	char *err;
	char id[11];

	sql = ecalloc(60, sizeof(char));

	snprintf(id, 11, "%i", h->id);
	strcpy(sql, "DELETE FROM history WHERE id = \'");
	strcat(sql, id);
	strcat(sql, "\';");

	sqlite3_exec(db, sql, NULL, NULL, &err);

	if (err)
		debug(D_WARN, "history", "failed to remove history: %s", err);

	efree(sql);
}

void history_remove_all(void)
{
	char *sql;

	sql = "DELETE FROM history;";

	sqlite3_exec(db, sql, NULL, NULL, NULL);
}

struct history *history_get(void)
{
	char *sql;
	sqlite3_stmt *stmt;
	int i;

	sql = "SELECT COUNT(*) AS count FROM history;";
	if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL))
		debug(D_ERR, "history", "failed to get history len: %s", sqlite3_errmsg(db));

	sqlite3_step(stmt);
	/* +1 so it won't try to alloc a 0 size buffer (error) */
	history = erealloc(history, sqlite3_column_int(stmt, 0) * sizeof(struct history) + 1);
	sqlite3_finalize(stmt);

	sql = "SELECT * FROM history;";
	if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL))
		debug(D_ERR, "history", "failed to get history: %s", sqlite3_errmsg(db));

	for (i = 0; sqlite3_step(stmt) == SQLITE_ROW; i++) {
		history[i].id = sqlite3_column_int(stmt, 0);
		history[i].uri = strdup(sqlite3_column_text(stmt, 1));
	}

	history_len = i;
	sqlite3_finalize(stmt);
	return history;
}

struct history *history_p_get(void)
{
	return history;
}

int history_len_get(void)
{
	return history_len;
}

void history_cleanup(void)
{
	int i;

	for (i = 0; i < history_len; i++)
		efree(history[i].uri);
	efree(history);

	sqlite3_close(db);
}
