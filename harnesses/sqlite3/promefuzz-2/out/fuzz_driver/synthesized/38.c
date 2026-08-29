// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
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

static void udf_target(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
    int n = 0;
    const void *blob = NULL;
    int out = 0;

    if (argc > 0 && argv[0] != NULL) {
        n = sqlite3_value_bytes(argv[0]);
        blob = sqlite3_value_blob(argv[0]);
        out = n;
        if (blob != NULL && n > 0) {
            const unsigned char *p = (const unsigned char *)blob;
            out ^= (int)p[0];
            out ^= (int)p[n - 1] << 8;
        }
    }

    sqlite3_result_int(ctx, out);
}

int LLVMFuzzerTestOneInput_38(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;
    int rc;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db != NULL) {
            sqlite3_close(db);
        }
        return 0;
    }

    sqlite3_create_function(db, "fuzzfunc", 1, SQLITE_UTF8, NULL, udf_target, NULL, NULL);

    sqlite3_exec(db, "CREATE TABLE t(x);", NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    sqlite3_exec(db, "INSERT INTO t(x) VALUES (zeroblob(0)), (NULL), (123), ('text');", NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    {
        const char *sql = "SELECT fuzzfunc(?1), fuzzfunc(x) FROM t;";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc == SQLITE_OK && stmt != NULL) {
            int mode = (Size > 0) ? (Data[0] % 6) : 0;
            switch (mode) {
                case 0:
                    sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
                    break;
                case 1:
                    sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_TRANSIENT);
                    break;
                case 2:
                        sqlite3_bind_int(stmt, 1, (Size >= 4)
                            ? (int)((uint32_t)Data[0] |
                                    ((uint32_t)Data[1] << 8) |
                                    ((uint32_t)Data[2] << 16) |
                                    ((uint32_t)Data[3] << 24))
                            : (int)Size);
                    break;
                case 3:
                    sqlite3_bind_null(stmt, 1);
                    break;
                case 4:
                    sqlite3_bind_zeroblob(stmt, 1, (int)(Size % 1024));
                    break;
                case 5:
                default:
                    sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
                    break;
            }

            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            }
        }
        if (stmt != NULL) {
            sqlite3_finalize(stmt);
            stmt = NULL;
        }
    }

    {
        const char *sql = "SELECT fuzzfunc(CAST(?1 AS TEXT)), fuzzfunc(CAST(?1 AS BLOB)), fuzzfunc(CAST(?1 AS INTEGER));";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc == SQLITE_OK && stmt != NULL) {
            sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            }
        }
        if (stmt != NULL) {
            sqlite3_finalize(stmt);
            stmt = NULL;
        }
    }

    sqlite3_close(db);
    return 0;
}