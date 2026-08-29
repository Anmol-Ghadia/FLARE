// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    const char *tail = NULL;
    const char *errmsg = NULL;
    int rc;
    int col_count = 0;
    int col = 0;
    int sql_len;
    char *sql_buf = NULL;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open("./dummy_file", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db != NULL) {
            sqlite3_close(db);
        }
        return 0;
    }

    sql_buf = (char *)sqlite3_malloc64(Size + 1);
    if (sql_buf == NULL) {
        sqlite3_close(db);
        return 0;
    }

    if (Size > 0) {
        memcpy(sql_buf, Data, Size);
    }
    sql_buf[Size] = '\0';

    sql_len = (int)((Size > (size_t)0x7fffffff) ? 0x7fffffff : Size);

    rc = sqlite3_prepare_v2(db, sql_buf, sql_len, &stmt, &tail);
    (void)rc;

    errmsg = sqlite3_errmsg(db);
    (void)errmsg;

    if (stmt != NULL) {
        rc = sqlite3_step(stmt);
        (void)rc;

        rc = sqlite3_step(stmt);
        (void)rc;

        col_count = sqlite3_column_count(stmt);
        if (col_count > 0) {
            if (Size > 0) {
                col = Data[0] % (col_count + 1);
            } else {
                col = 0;
            }

            (void)sqlite3_column_type(stmt, col);
            (void)sqlite3_column_name(stmt, col);
            (void)sqlite3_column_text(stmt, col);
            (void)sqlite3_column_text(stmt, col);
            (void)sqlite3_column_text(stmt, col);
            (void)sqlite3_column_bytes(stmt, col);
        } else {
            (void)sqlite3_column_type(stmt, 0);
            (void)sqlite3_column_name(stmt, 0);
            (void)sqlite3_column_text(stmt, 0);
            (void)sqlite3_column_text(stmt, 0);
            (void)sqlite3_column_text(stmt, 0);
            (void)sqlite3_column_bytes(stmt, 0);
        }

        (void)sqlite3_finalize(stmt);
    } else {
        (void)sqlite3_finalize(NULL);
    }

    sqlite3_free(sql_buf);
    sqlite3_close(db);
    return 0;
}