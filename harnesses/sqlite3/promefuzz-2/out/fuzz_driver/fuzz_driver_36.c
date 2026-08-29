// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static void fuzz_sql_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
    int i;
    sqlite3_int64 acc = 0;

    for (i = 0; i < argc; i++) {
        const unsigned char *txt;
        const void *blob;
        int n;

        txt = sqlite3_value_text(argv[i]);
        blob = sqlite3_value_blob(argv[i]);
        n = sqlite3_value_bytes(argv[i]);

        if (txt && n > 0) {
            int j;
            for (j = 0; j < n; j++) {
                acc += txt[j];
            }
        }

        if (blob && n > 0) {
            const unsigned char *b = (const unsigned char *)blob;
            int j;
            for (j = 0; j < n; j++) {
                acc ^= ((sqlite3_int64)b[j] << (j & 7));
            }
        }

        acc += (sqlite3_int64)n;
    }

    sqlite3_result_int64(ctx, acc);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    FILE *fp = NULL;
    int rc;
    int mode;
    int split1, split2;
    int intv = 0;
    sqlite3_int64 i64v = 0;
    double dblv = 0.0;

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
        fp = NULL;
    }

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_create_function(db, "fuzzfunc", -1, SQLITE_UTF8, NULL, fuzz_sql_func, NULL, NULL);

    if (Size == 0) {
        sqlite3_prepare_v2(db, "SELECT fuzzfunc(NULL);", -1, &stmt, NULL);
        if (stmt) {
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
        sqlite3_close(db);
        return 0;
    }

    mode = Data[0] % 6;
    split1 = (int)(Size > 1 ? (Data[1] % Size) : 0);
    split2 = (int)(Size > 2 ? (Data[2] % Size) : split1);
    if (split1 > split2) {
        int t = split1;
        split1 = split2;
        split2 = t;
    }

    if (Size >= 5) {
        memcpy(&intv, Data + 1, sizeof(intv) <= Size - 1 ? sizeof(intv) : Size - 1);
    }
    if (Size >= 9) {
        memcpy(&i64v, Data + 1, sizeof(i64v) <= Size - 1 ? sizeof(i64v) : Size - 1);
    }
    if (Size >= 9) {
        memcpy(&dblv, Data + 1, sizeof(dblv) <= Size - 1 ? sizeof(dblv) : Size - 1);
    }

    switch (mode) {
        case 0:
            rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(?1);", -1, &stmt, NULL);
            if (rc == SQLITE_OK && stmt) {
                sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
            }
            break;
        case 1:
            rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(?1, ?2);", -1, &stmt, NULL);
            if (rc == SQLITE_OK && stmt) {
                sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_STATIC);
                sqlite3_bind_blob(stmt, 2, Data + split1, (int)(Size - (size_t)split1), SQLITE_STATIC);
            }
            break;
        case 2:
            rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(?1, ?2, ?3);", -1, &stmt, NULL);
            if (rc == SQLITE_OK && stmt) {
                sqlite3_bind_int(stmt, 1, intv);
                sqlite3_bind_int64(stmt, 2, i64v);
                sqlite3_bind_double(stmt, 3, dblv);
            }
            break;
        case 3:
            rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(?1, ?2, ?3, ?4);", -1, &stmt, NULL);
            if (rc == SQLITE_OK && stmt) {
                sqlite3_bind_null(stmt, 1);
                sqlite3_bind_text(stmt, 2, (const char *)Data, split1, SQLITE_STATIC);
                sqlite3_bind_blob(stmt, 3, Data + split1, split2 - split1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 4, (const char *)(Data + split2), (int)(Size - (size_t)split2), SQLITE_STATIC);
            }
            break;
        case 4:
            rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(readfile('./dummy_file'), ?1);", -1, &stmt, NULL);
            if (rc == SQLITE_OK && stmt) {
                sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_STATIC);
            }
            break;
        default:
            rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(CAST(?1 AS TEXT), CAST(?2 AS BLOB), CAST(?3 AS TEXT));", -1, &stmt, NULL);
            if (rc == SQLITE_OK && stmt) {
                sqlite3_bind_blob(stmt, 1, Data, split1, SQLITE_STATIC);
                sqlite3_bind_blob(stmt, 2, Data + split1, split2 - split1, SQLITE_STATIC);
                sqlite3_bind_blob(stmt, 3, Data + split2, (int)(Size - (size_t)split2), SQLITE_STATIC);
            }
            break;
    }

    if (stmt) {
        sqlite3_step(stmt);
        sqlite3_reset(stmt);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);
    return 0;
}