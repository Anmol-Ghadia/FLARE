// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_errcode at sqlite3.c:17337:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
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

static int consume_u32(const uint8_t **data, size_t *size) {
    int v = 0;
    size_t n = *size < 4 ? *size : 4;
    for (size_t i = 0; i < n; i++) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

int LLVMFuzzerTestOneInput_25(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    const char *tail = NULL;
    int rc;
    char *mprintf_buf = NULL;
    sqlite3_str *str = NULL;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db, "CREATE TABLE t(x TEXT, y INTEGER);", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO t(x,y) VALUES('alpha',1),('beta',2),('gamma',3);", NULL, NULL, NULL);

    const uint8_t *p = Data;
    size_t remaining = Size;

    char *sql = (char *)sqlite3_malloc64(remaining + 1);
    if (sql == NULL) {
        sqlite3_close(db);
        return 0;
    }
    if (remaining > 0) {
        memcpy(sql, p, remaining);
    }
    sql[remaining] = '\0';

    int nByte;
    if (remaining > 0) {
        nByte = consume_u32(&p, &remaining);
        if (nByte <= 0 || (size_t)nByte > Size) {
            nByte = -1;
        }
    } else {
        nByte = -1;
    }

    rc = sqlite3_prepare_v2(db, sql, nByte, &stmt, &tail);

    if (stmt != NULL) {
        int step_rc = sqlite3_step(stmt);

        if (step_rc == SQLITE_ROW) {
            int col = 0;
            if (remaining > 0) {
                col = (int)(p[0] % 4);
            }
            const unsigned char *txt = sqlite3_column_text(stmt, col);

            mprintf_buf = sqlite3_mprintf("rc=%d col=%d txt=%Q tail=%Q",
                                          step_rc,
                                          col,
                                          txt ? (const char *)txt : NULL,
                                          tail ? tail : NULL);

            str = sqlite3_str_new(db);
            if (str != NULL) {
                sqlite3_str_appendf(str, "A:%s|", mprintf_buf ? mprintf_buf : "");
                sqlite3_str_appendf(str, "B:%Q|%d", txt ? (const char *)txt : NULL, sqlite3_str_errcode(str));
                sqlite3_free(sqlite3_str_finish(str));
                str = NULL;
            }

            sqlite3_free(mprintf_buf);
            mprintf_buf = NULL;
        } else {
            mprintf_buf = sqlite3_mprintf("step=%d sql=%Q", step_rc, sql ? sql : NULL);

            str = sqlite3_str_new(db);
            if (str != NULL) {
                sqlite3_str_appendf(str, "E1:%s|", mprintf_buf ? mprintf_buf : "");
                sqlite3_str_appendf(str, "E2:%Q|%d", sqlite3_errmsg(db), step_rc);
                sqlite3_free(sqlite3_str_finish(str));
                str = NULL;
            }

            sqlite3_free(mprintf_buf);
            mprintf_buf = NULL;
        }
    }

    sqlite3_free(sql);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}