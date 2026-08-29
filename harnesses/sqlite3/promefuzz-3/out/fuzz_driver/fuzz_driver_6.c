// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static int read_u32(const uint8_t *data, size_t size, size_t *off) {
    int v = 0;
    if (*off + 4 <= size) {
        v = (int)(
            ((unsigned)data[*off]) |
            ((unsigned)data[*off + 1] << 8) |
            ((unsigned)data[*off + 2] << 16) |
            ((unsigned)data[*off + 3] << 24));
        *off += 4;
    } else {
        while (*off < size) {
            v = (v << 8) ^ data[*off];
            (*off)++;
        }
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt_select = NULL;
    sqlite3_stmt *stmt_insert = NULL;
    sqlite3_stmt *stmt_extra = NULL;
    char *errmsg = NULL;
    size_t off = 0;
    int rc;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open("./dummy_file", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) {
            sqlite3_close(db);
        }
        return 0;
    }

    sqlite3_exec(db, "PRAGMA temp_store=MEMORY;", NULL, NULL, NULL);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(a TEXT, b INTEGER);", NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    sqlite3_exec(db, "DELETE FROM t;", NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    if (Size > 0) {
        char *hex = (char *)malloc(Size * 2 + 1);
        if (hex) {
            static const char h[] = "0123456789abcdef";
            size_t i;
            for (i = 0; i < Size; i++) {
                hex[i * 2] = h[(Data[i] >> 4) & 0xF];
                hex[i * 2 + 1] = h[Data[i] & 0xF];
            }
            hex[Size * 2] = '\0';

            {
                char sqlbuf[256];
                sqlite3_snprintf((int)sizeof(sqlbuf), sqlbuf,
                                 "INSERT INTO t(a,b) VALUES('%q',%d);",
                                 hex, (int)Size);
                sqlite3_exec(db, sqlbuf, NULL, NULL, &errmsg);
                if (errmsg) {
                    sqlite3_free(errmsg);
                    errmsg = NULL;
                }
            }
            free(hex);
        }
    } else {
        sqlite3_exec(db, "INSERT INTO t(a,b) VALUES('',0);", NULL, NULL, &errmsg);
        if (errmsg) {
            sqlite3_free(errmsg);
            errmsg = NULL;
        }
    }

    sqlite3_prepare_v2(db, "SELECT a FROM t;", -1, &stmt_select, NULL);
    sqlite3_prepare_v2(db, "INSERT INTO t(a,b) VALUES(?1, ?2);", -1, &stmt_insert, NULL);

    {
        const char *extra_sqls[] = {
            "SELECT ?1;",
            "SELECT upper(?1);",
            "INSERT INTO t(a,b) VALUES(?1, 123);"
        };
        int idx = 0;
        if (Size > 0) {
            idx = (unsigned)Data[0] % 3;
        }
        sqlite3_prepare_v2(db, extra_sqls[idx], -1, &stmt_extra, NULL);
    }

    if (stmt_select) {
        int s = sqlite3_step(stmt_select);
        if (s == SQLITE_ROW) {
            int col = 0;
            if (Size > 1) {
                col = (int)(Data[1] % 2);
            }
            (void)sqlite3_column_text(stmt_select, col);
        } else {
            (void)sqlite3_column_text(stmt_select, 0);
        }
    } else {
        (void)sqlite3_column_text(NULL, 0);
    }

    {
        char buf1[64];
        char buf2[128];
        int v1 = read_u32(Data, Size, &off);
        int v2 = read_u32(Data, Size, &off);

        sqlite3_snprintf((int)sizeof(buf1), buf1, "%d", v1);
        sqlite3_snprintf((int)sizeof(buf2), buf2, "%s", buf1);
        sqlite3_snprintf((int)sizeof(buf2), buf2, "%s:%d", buf2, v2);

        if (stmt_insert) {
            int bind_len;
            if (Size > 0 && off < Size) {
                size_t maxlen = strlen(buf2);
                bind_len = (int)(Data[off] % (maxlen + 1));
                off++;
            } else {
                bind_len = -1;
            }
            (void)sqlite3_bind_text(stmt_insert, 1, buf2, bind_len, SQLITE_TRANSIENT);
            (void)sqlite3_step(stmt_insert);
            (void)sqlite3_reset(stmt_insert);
            (void)sqlite3_sql(stmt_insert);
        } else {
            (void)sqlite3_bind_text(NULL, 1, buf2, -1, SQLITE_TRANSIENT);
            (void)sqlite3_step(NULL);
            (void)sqlite3_reset(NULL);
            (void)sqlite3_sql(NULL);
        }
    }

    (void)sqlite3_finalize(stmt_select);
    stmt_select = NULL;

    (void)sqlite3_finalize(stmt_insert);
    stmt_insert = NULL;

    (void)sqlite3_finalize(stmt_extra);
    stmt_extra = NULL;

    sqlite3_close(db);
    return 0;
}