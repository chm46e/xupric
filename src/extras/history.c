#include <sqlite3.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <util/util.h>
#include <stdlib.h>

static sqlite3 *db;
static char **history;
static int history_len;

extern void history_init(char *file)
{
    const char *sql;

    if (sqlite3_open(file, &db))
        die(1, "[ERROR] Unable to open the history database\n");

    sql = "CREATE TABLE history(uri TEXT NOT NULL);";

    /* ignore already exists errors */
    sqlite3_exec(db, sql, NULL, NULL, NULL);

    history = ecalloc(10000, sizeof(char));
}

extern void history_add(char *text)
{
    char *sql;
    char *err;

    sql = ecalloc(1000, sizeof(char *));

    strcpy(sql, "INSERT INTO history VALUES(\'");
    strcat(sql, text);
    strcat(sql, "\');");

    sqlite3_exec(db, sql, NULL, NULL, &err);

    if (err)
        printf("Failed to add history: %s\n", err);

    free(sql);
}

extern void history_remove(char *text)
{
    char *sql;
    char *err;

    sql = ecalloc(1000, sizeof(char *));

    strcpy(sql, "DELETE FROM history WHERE uri = \'");
    strcat(sql, text);
    strcat(sql, "\';");

    sqlite3_exec(db, sql, NULL, NULL, &err);

    if (err)
        printf("Failed to remove history: %s\n", err);

    free(sql);
}

extern void history_remove_all(void)
{
    char *sql;

    sql = "DELETE FROM history;";

    sqlite3_exec(db, sql, NULL, NULL, NULL);
}

extern char **history_get(void)
{
    char *sql;
    sqlite3_stmt *stmt;
    int i;

    sql = "SELECT uri FROM history;";

    if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL))
        die(1, "[ERROR] Unable to get history: %s\n", sqlite3_errmsg(db));

    for (i = 0; sqlite3_step(stmt) == SQLITE_ROW && i < 10000; i++)
        history[i] = strdup(sqlite3_column_text(stmt, 0));

    history_len = i;
    sqlite3_finalize(stmt);
    return history;
}

extern char **history_p_get(void)
{
    return history;
}

extern int history_len_get(void)
{
    return history_len;
}

extern void history_cleanup(void)
{
    int i;

    for (i = 0; i < history_len; i++)
        free(history[i]);
    free(history);

    sqlite3_close(db);
}
