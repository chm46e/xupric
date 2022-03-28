#include <sqlite3.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <util/util.h>

static sqlite3 *db;
static char **books;
static int books_len;

extern void bookmark_init(char *file)
{
    const char *sql;

    if (sqlite3_open(file, &db))
        die(1, "[ERROR] Unable to open the bookmarks database\n");

    sql = "CREATE TABLE books(uri TEXT NOT NULL);";

    /* ignore already exists errors */
    sqlite3_exec(db, sql, NULL, NULL, NULL);

    books = ecalloc(1000, sizeof(char));
}

extern void bookmark_add(char *text)
{
    char *sql;
    char *err;

    sql = ecalloc(1000, sizeof(char *));

    strcpy(sql, "INSERT INTO books VALUES(\'");
    strcat(sql, text);
    strcat(sql, "\');");

    sqlite3_exec(db, sql, NULL, NULL, &err);

    if (err)
        printf("Failed to add bookmark: %s\n", err);

    free(sql);
}

extern void bookmark_remove(char *text)
{
    char *sql;
    char *err;

    sql = ecalloc(1000, sizeof(char *));

    strcpy(sql, "DELETE FROM books WHERE uri = \'");
    strcat(sql, text);
    strcat(sql, "\';");

    sqlite3_exec(db, sql, NULL, NULL, &err);

    if (err)
        printf("Failed to remove bookmark: %s\n", err);

    free(sql);
}

extern char **bookmark_get(void)
{
    char *sql;
    sqlite3_stmt *stmt;
    int i;

    sql = "SELECT uri FROM books;";

    if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL))
        die(1, "[ERROR] Unable to get bookmarks: %s\n", sqlite3_errmsg(db));

    for (i = 0; sqlite3_step(stmt) == SQLITE_ROW && i < 1000; i++)
        books[i] = strdup(sqlite3_column_text(stmt, 0));

    books_len = i;
    sqlite3_finalize(stmt);
    return books;
}

extern char **books_get(void)
{
    return books;
}

extern int bookmark_exists(char *book)
{
    char *sql;
    sqlite3_stmt *stmt;
    int exists = 0;

    sql = ecalloc(1000, sizeof(char *));

    strcpy(sql, "SELECT uri FROM books WHERE uri = \'");
    strcat(sql, book);
    strcat(sql, "\';");

    if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL))
        die(1, "[ERROR] Unable to get bookmarks: %s\n", sqlite3_errmsg(db));

    if (sqlite3_step(stmt) == SQLITE_ROW)
        exists = 1;

    sqlite3_finalize(stmt);
    free(sql);
    return exists;
}

extern void bookmark_cleanup(void)
{
    int i;

    for (i = 0; i < books_len; i++)
        free(books[i]);
    free(books);

    sqlite3_close(db);
}
