// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_int64 at sqlite3.c:73046:25 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

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

static const char *pick_sql(int selector) {
    static const char *sqls[] = {
        "SELECT CAST(?1 AS INTEGER)",
        "SELECT length(?1)",
        "SELECT abs(CAST(?1 AS INTEGER))",
        "SELECT ?1",
        "SELECT 1 WHERE ?1='x'",
        "SELECT substr(?1,1,8)",
        "SELECT unicode(substr(?1,1,1))",
        "SELECT CAST(?1 AS INTEGER) + 1"
    };
    return sqls[(unsigned)selector % (sizeof(sqls) / sizeof(sqls[0]))];
}

int LLVMFuzzerTestOneInput_27(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    FILE *fp = NULL;
    int rc;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
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

    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(x);", NULL, NULL, NULL);

    const uint8_t *p = Data;
    size_t remaining = Size;

    int selector = consume_u32(&p, &remaining);
    const char *sql = pick_sql(selector);

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK || stmt == NULL) {
        (void)sqlite3_errmsg(db);
        sqlite3_close(db);
        return 0;
    }

    size_t len1 = remaining ? (size_t)(consume_u32(&p, &remaining) % (remaining + 1)) : 0;
    const char *txt1 = (const char *)p;
    p += len1;
    remaining -= len1;

    (void)sqlite3_bind_text(stmt, 1, txt1, (int)len1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    (void)rc;
    (void)sqlite3_reset(stmt);

    size_t len2 = remaining ? (size_t)(consume_u32(&p, &remaining) % (remaining + 1)) : 0;
    const char *txt2 = (const char *)p;
    p += len2;
    remaining -= len2;

    (void)sqlite3_bind_text(stmt, 1, txt2, (int)len2, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    (void)sqlite3_errmsg(db);
    (void)sqlite3_reset(stmt);

    size_t len3 = remaining ? (size_t)(consume_u32(&p, &remaining) % (remaining + 1)) : 0;
    const char *txt3 = (const char *)p;
    p += len3;
    remaining -= len3;

    (void)sqlite3_bind_text(stmt, 1, txt3, (int)len3, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    (void)sqlite3_errmsg(db);
    (void)sqlite3_reset(stmt);

    (void)sqlite3_errmsg(db);

    size_t len4 = remaining;
    const char *txt4 = (const char *)p;

    (void)sqlite3_bind_text(stmt, 1, txt4, (int)len4, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        (void)sqlite3_column_int64(stmt, 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}