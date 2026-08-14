// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_int64 at sqlite3.c:73046:25 in sqlite3.h
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

static int consume_u32(const uint8_t **data, size_t *size) {
    int v = 0;
    size_t n = *size < 4 ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    int rc;

    rc = sqlite3_open("./dummy_file", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) {
            sqlite3_close(db);
        }
        return 0;
    }

    sqlite3_exec(db,
                 "CREATE TABLE IF NOT EXISTS t(x TEXT, y INTEGER);"
                 "DELETE FROM t;"
                 "INSERT INTO t(x,y) VALUES('0',0),('1',1),('2',2);",
                 NULL, NULL, NULL);

    const char *sql = "SELECT CAST(?1 AS INTEGER)";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK || stmt == NULL) {
        sqlite3_close(db);
        return 0;
    }

    const uint8_t *p = Data;
    size_t remaining = Size;

    int len1 = remaining ? (consume_u32(&p, &remaining) % (remaining + 1)) : 0;
    const char *txt1 = (const char *)p;
    p += len1;
    remaining -= len1;

    sqlite3_bind_text(stmt, 1, txt1, len1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    (void)rc;
    sqlite3_reset(stmt);

    int len2 = remaining ? (consume_u32(&p, &remaining) % (remaining + 1)) : 0;
    const char *txt2 = (const char *)p;
    p += len2;
    remaining -= len2;

    sqlite3_bind_text(stmt, 1, txt2, len2, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    (void)sqlite3_errmsg(db);
    sqlite3_reset(stmt);

    int len3 = remaining ? (consume_u32(&p, &remaining) % (remaining + 1)) : 0;
    const char *txt3 = (const char *)p;
    p += len3;
    remaining -= len3;

    sqlite3_bind_text(stmt, 1, txt3, len3, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    (void)sqlite3_errmsg(db);
    sqlite3_reset(stmt);

    (void)sqlite3_errmsg(db);

    int len4 = (int)remaining;
    const char *txt4 = (const char *)p;

    sqlite3_bind_text(stmt, 1, txt4, len4, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        (void)sqlite3_column_int64(stmt, 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}