// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

static void fuzz_sql_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
    int i;
    sqlite3_int64 acc = 0;

    for (i = 0; i < argc; i++) {
        const unsigned char *txt = sqlite3_value_text(argv[i]);
        const void *blob = sqlite3_value_blob(argv[i]);
        int n = sqlite3_value_bytes(argv[i]);

        if (txt) {
            acc += (sqlite3_int64)txt[0];
        }
        if (blob && n > 0) {
            acc += (sqlite3_int64)((const unsigned char *)blob)[0];
        }
        acc += (sqlite3_int64)n;
    }

    sqlite3_result_int64(ctx, acc);
}

int LLVMFuzzerTestOneInput_36(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;
    int rc;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
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

    sqlite3_create_function(db, "fuzzfunc", -1, SQLITE_UTF8, NULL, fuzz_sql_func, NULL, NULL);

    sqlite3_exec(db,
                 "CREATE TABLE t(a);"
                 "CREATE TABLE t2(x);",
                 NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    sqlite3_exec(db,
                 "INSERT INTO t(a) VALUES (NULL), (0), (1), (-1), (3.14159), "
                 "(''), ('text'), (x''), (x'00'), (x'414243');",
                 NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    {
        const char *sqls[] = {
            "SELECT fuzzfunc(?1);",
            "SELECT fuzzfunc(?1, ?2);",
            "SELECT fuzzfunc(a) FROM t;",
            "SELECT fuzzfunc(CAST(?1 AS TEXT));",
            "SELECT fuzzfunc(CAST(?1 AS BLOB));",
            "SELECT fuzzfunc(CAST(?1 AS INTEGER));",
            "SELECT fuzzfunc(substr(?1,1,8), zeroblob(length(?1)%32));",
            "SELECT fuzzfunc(readfile('./dummy_file'));"
        };
        size_t idx, count = sizeof(sqls) / sizeof(sqls[0]);

        for (idx = 0; idx < count; idx++) {
            rc = sqlite3_prepare_v2(db, sqls[idx], -1, &stmt, NULL);
            if (rc != SQLITE_OK || stmt == NULL) {
                if (stmt) {
                    sqlite3_finalize(stmt);
                    stmt = NULL;
                }
                continue;
            }

            sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, (const char *)Data, (int)Size, SQLITE_STATIC);

            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            }

            sqlite3_finalize(stmt);
            stmt = NULL;
        }
    }

    sqlite3_close(db);
    return 0;
}