#include <sqlite3.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <util/util.h>
#include <stdlib.h>

static sqlite3 *db;
static char **cookies;
static int cookies_len;

extern void cookie_init(char *file)
{
    if (sqlite3_open(file, &db))
        die(1, "[ERROR] Unable to open the cookies database\n");

    cookies = ecalloc(1000, sizeof(char));
}

extern void cookie_remove(char *text)
{
    char *sql;
    char *err;

    sql = ecalloc(1000, sizeof(char *));

    strcpy(sql, "DELETE FROM moz_cookies WHERE name = \'");
    strcat(sql, text);
    strcat(sql, "\';");

    sqlite3_exec(db, sql, NULL, NULL, &err);

    if (err)
        printf("Failed to remove cookie: %s\n", err);

    free(sql);
}

extern void cookie_remove_all(void)
{
    char *sql;

    sql = "DELETE FROM moz_cookies;";

    sqlite3_exec(db, sql, NULL, NULL, NULL);
}

extern char **cookie_get(void)
{
    char *sql, *val;
    sqlite3_stmt *stmt;
    int i;

    sql = "SELECT host, name FROM moz_cookies;";

    val = ecalloc(1000, sizeof(char));

    if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL))
        die(1, "[ERROR] Unable to get cookies: %s\n", sqlite3_errmsg(db));

    for (i = 0; sqlite3_step(stmt) == SQLITE_ROW && i < 1000; i++) {
        strcpy(val, sqlite3_column_text(stmt, 0));
        strcat(val, ": ");
        strcat(val, sqlite3_column_text(stmt, 1));
        cookies[i] = strdup(val);
    }

    cookies_len = i;
    sqlite3_finalize(stmt);
    free(val);
    return cookies;
}

extern char **cookie_p_get(void)
{
    return cookies;
}

extern int cookie_len_get(void)
{
    return cookies_len;
}

extern void cookie_cleanup(void)
{
    int i;

    for (i = 0; i < cookies_len; i++)
        free(cookies[i]);
    free(cookies);

    sqlite3_close(db);
}
