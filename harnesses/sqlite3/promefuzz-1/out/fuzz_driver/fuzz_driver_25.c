// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *off) {
    uint32_t v = 0;
    if (*off < size) v |= (uint32_t)data[(*off)++];
    if (*off < size) v |= (uint32_t)data[(*off)++] << 8;
    if (*off < size) v |= (uint32_t)data[(*off)++] << 16;
    if (*off < size) v |= (uint32_t)data[(*off)++] << 24;
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    const char *tail = NULL;
    char *mprintf_buf = NULL;
    sqlite3_str *str = NULL;
    char *sql = NULL;
    int rc;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    (void)sqlite3_exec(db,
                       "CREATE TABLE t(a,b,c);"
                       "INSERT INTO t VALUES('alpha', 123, x'4142');"
                       "INSERT INTO t VALUES(NULL, -1, 'text');"
                       "INSERT INTO t VALUES('dummy_file', 0, 'zzz');",
                       NULL, NULL, NULL);

    sql = (char *)sqlite3_malloc64(Size + 1);
    if (!sql) {
        sqlite3_close(db);
        return 0;
    }
    if (Size > 0) {
        memcpy(sql, Data, Size);
    }
    sql[Size] = '\0';

    rc = sqlite3_prepare_v2(db, sql, (int)Size, &stmt, &tail);

    if (stmt != NULL) {
        int step_rc = sqlite3_step(stmt);

        if (step_rc == SQLITE_ROW) {
            int col = 0;
            size_t off = 0;
            if (sqlite3_column_count(stmt) > 0) {
                col = (int)(read_u32(Data, Size, &off) % (uint32_t)sqlite3_column_count(stmt));
            }

            {
                const unsigned char *txt = sqlite3_column_text(stmt, col);

                mprintf_buf = sqlite3_mprintf("rc=%d col=%d txt=%Q tail=%Q",
                                              step_rc,
                                              col,
                                              (const char *)txt,
                                              tail ? tail : "");

                str = sqlite3_str_new(db);
                if (str) {
                    sqlite3_str_appendf(str, "size=%d;", (int)Size);
                    sqlite3_str_appendf(str, "sql=%q|m=%q",
                                        sql ? sql : "",
                                        mprintf_buf ? mprintf_buf : "");
                    (void)sqlite3_str_finish(str);
                    str = NULL;
                }
            }
        } else {
            mprintf_buf = sqlite3_mprintf("step=%d sql=%Q", step_rc, sql ? sql : "");

            str = sqlite3_str_new(db);
            if (str) {
                sqlite3_str_appendf(str, "tail=%q;", tail ? tail : "");
                sqlite3_str_appendf(str, "msg=%q", mprintf_buf ? mprintf_buf : "");
                (void)sqlite3_str_finish(str);
                str = NULL;
            }
        }

        sqlite3_free(mprintf_buf);
        mprintf_buf = NULL;

        (void)sqlite3_finalize(stmt);
        stmt = NULL;
    }

    sqlite3_free(sql);
    sqlite3_close(db);
    return 0;
}