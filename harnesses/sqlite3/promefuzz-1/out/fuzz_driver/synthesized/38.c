// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_int at sqlite3.c:73041:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_int at sqlite3.c:73041:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static void udf_target(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
    int n = 0;
    const void *blob = NULL;
    int result = 0;

    if (argc > 0 && argv && argv[0]) {
        n = sqlite3_value_bytes(argv[0]);
        blob = sqlite3_value_blob(argv[0]);

        result = n;
        if (blob && n > 0) {
            const unsigned char *p = (const unsigned char *)blob;
            int mix = 0;
            int limit = n < 16 ? n : 16;
            for (int i = 0; i < limit; ++i) {
                mix = (mix * 33) ^ p[i];
            }
            result ^= mix;
        }
    }

    sqlite3_result_int(ctx, result);
}

int LLVMFuzzerTestOneInput_38(const uint8_t *Data, size_t Size) {
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

    sqlite3_create_function(db, "fuzzfunc", 1, SQLITE_UTF8, NULL, udf_target, NULL, NULL);

    sqlite3_exec(db, "CREATE TABLE t(x);", NULL, NULL, NULL);

    rc = sqlite3_prepare_v2(db, "INSERT INTO t(x) VALUES(?1);", -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
        if (Size == 0) {
            sqlite3_bind_null(stmt, 1);
        } else {
            switch (Data[0] % 6) {
                case 0:
                    sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
                    break;
                case 1:
                    sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_TRANSIENT);
                    break;
                case 2: {
                    sqlite3_int64 v = 0;
                    size_t copy = Size < sizeof(v) ? Size : sizeof(v);
                    memcpy(&v, Data, copy);
                    sqlite3_bind_int64(stmt, 1, v);
                    break;
                }
                case 3: {
                    double d = 0.0;
                    size_t copy = Size < sizeof(d) ? Size : sizeof(d);
                    memcpy(&d, Data, copy);
                    sqlite3_bind_double(stmt, 1, d);
                    break;
                }
                case 4:
                    sqlite3_bind_zeroblob(stmt, 1, (int)(Size % 1024));
                    break;
                default:
                    sqlite3_bind_null(stmt, 1);
                    break;
            }
        }
        sqlite3_step(stmt);
    }
    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(x) FROM t;", -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            (void)sqlite3_column_int(stmt, 0);
        }
    }
    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(?1);", -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
        if (Size == 0) {
            sqlite3_bind_null(stmt, 1);
        } else {
            switch ((Size > 1 ? Data[1] : Data[0]) % 6) {
                case 0:
                    sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
                    break;
                case 1:
                    sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_TRANSIENT);
                    break;
                case 2: {
                    sqlite3_int64 v = 0;
                    size_t copy = Size < sizeof(v) ? Size : sizeof(v);
                    memcpy(&v, Data, copy);
                    sqlite3_bind_int64(stmt, 1, v);
                    break;
                }
                case 3: {
                    double d = 0.0;
                    size_t copy = Size < sizeof(d) ? Size : sizeof(d);
                    memcpy(&d, Data, copy);
                    sqlite3_bind_double(stmt, 1, d);
                    break;
                }
                case 4:
                    sqlite3_bind_zeroblob(stmt, 1, (int)(Size % 2048));
                    break;
                default:
                    sqlite3_bind_null(stmt, 1);
                    break;
            }
        }
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            (void)sqlite3_column_int(stmt, 0);
        }
    }
    if (stmt) {
        sqlite3_finalize(stmt);
    }

    sqlite3_exec(db, "SELECT length(readfile('./dummy_file'));", NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
    }

    sqlite3_close(db);
    return 0;
}