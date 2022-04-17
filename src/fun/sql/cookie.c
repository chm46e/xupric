#include <sqlite3.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "util/util.h"
#include "cookie.h"

static sqlite3 *db;
static struct cookie *cookies;
static int cookies_len;

void cookie_init(char *file)
{
	if (sqlite3_open(file, &db))
		die(1, "[ERROR] Unable to open the cookies database\n");
}

void cookie_remove(struct cookie *c)
{
	char *sql;
	char *err;
	char id[11];

	sql = ecalloc(150, sizeof(char));

	snprintf(id, 11, "%i", c->id);
	strcpy(sql, "DELETE FROM moz_cookies WHERE id = \'");
	strcat(sql, id);
	strcat(sql, "\';");

	sqlite3_exec(db, sql, NULL, NULL, &err);

	if (err)
		printf("Failed to remove cookie: %s\n", err);

	efree(sql);
}

void cookie_remove_all(void)
{
	char *sql;

	sql = "DELETE FROM moz_cookies;";

	sqlite3_exec(db, sql, NULL, NULL, NULL);
}

struct cookie *cookie_get(void)
{
	char *sql;
	sqlite3_stmt *stmt;
	int i;

	sql = "SELECT COUNT(*) AS count FROM moz_cookies;";
	if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL))
		die(1, "[ERROR] Unable to get cookies: %s\n", sqlite3_errmsg(db));

	sqlite3_step(stmt);
	/* +1 so it won't try to alloc a 0 size buffer (error) */
	cookies = erealloc(cookies, sqlite3_column_int(stmt, 0) * sizeof(struct cookie) + 1);
	sqlite3_finalize(stmt);

	sql = "SELECT id, host, name FROM moz_cookies;";
	if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL))
		die(1, "[ERROR] Unable to get cookies: %s\n", sqlite3_errmsg(db));

	for (i = 0; sqlite3_step(stmt) == SQLITE_ROW; i++) {
		cookies[i].id = sqlite3_column_int(stmt, 0);
		cookies[i].host = strdup(sqlite3_column_text(stmt, 1));
		cookies[i].name = strdup(sqlite3_column_text(stmt, 2));
	}

	cookies_len = i;
	sqlite3_finalize(stmt);
	return cookies;
}

struct cookie *cookie_p_get(void)
{
	return cookies;
}

int cookie_len_get(void)
{
	return cookies_len;
}

void cookie_cleanup(void)
{
	int i;

	for (i = 0; i < cookies_len; i++) {
		efree(cookies[i].host);
		efree(cookies[i].name);
	}
	efree(cookies);

	sqlite3_close(db);
}