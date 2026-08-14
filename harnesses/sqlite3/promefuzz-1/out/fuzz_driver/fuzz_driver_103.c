// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text16be at sqlite3.c:72409:17 in sqlite3.h
// sqlite3_result_text16be at sqlite3.c:72409:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_result_pointer at sqlite3.c:72356:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_pointer at sqlite3.c:72356:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

static void udf_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
    const unsigned char *txt = NULL;
    const void *blob = NULL;
    int txt_len = 0;
    int blob_len = 0;
    int mode = 0;
    int aux = 0;

    if (argc > 0) {
        txt = sqlite3_value_text(argv[0]);
        txt_len = sqlite3_value_bytes(argv[0]);
    }
    if (argc > 1) {
        blob = sqlite3_value_blob(argv[1]);
        blob_len = sqlite3_value_bytes(argv[1]);
    }
    if (argc > 2) {
        mode = sqlite3_value_int(argv[2]);
    }
    if (argc > 3) {
        aux = sqlite3_value_int(argv[3]);
    }

    switch ((unsigned)mode % 6U) {
        case 0:
            sqlite3_result_int(ctx, aux);
            break;
        case 1:
            if (txt) {
                sqlite3_result_text(ctx, (const char *)txt, txt_len, SQLITE_TRANSIENT);
            } else {
                sqlite3_result_text(ctx, "", 0, SQLITE_STATIC);
            }
            break;
        case 2:
            if (blob && blob_len >= 0) {
                sqlite3_result_text16be(ctx, blob, blob_len, SQLITE_TRANSIENT);
            } else {
                static const unsigned char empty16be[2] = {0, 0};
                sqlite3_result_text16be(ctx, empty16be, 0, SQLITE_STATIC);
            }
            break;
        case 3: {
            char *p = (char *)sqlite3_malloc(16);
            if (p) {
                memcpy(p, "fuzzptr", 8);
            }
            sqlite3_result_pointer(ctx, p, "fuzz.ptr", sqlite3_free);
            break;
        }
        case 4:
            if (txt) {
                sqlite3_result_error(ctx, (const char *)txt, txt_len >= 0 ? txt_len : 0);
            } else {
                sqlite3_result_error(ctx, "err", 3);
            }
            break;
        default:
            sqlite3_result_text(ctx, "", -1, SQLITE_STATIC);
            break;
    }
}

static void udf_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
    int req = 0;
    void *agg;

    if (argc > 0) {
        req = sqlite3_value_int(argv[0]);
    }
    if (req < -32) req = -32;
    if (req > 8192) req = 8192;

    agg = sqlite3_aggregate_context(ctx, req);
    if (agg && req > 0) {
        unsigned char *p = (unsigned char *)agg;
        p[0] ^= 0xA5;
        if (argc > 1) {
            p[0] ^= (unsigned char)(sqlite3_value_int(argv[1]) & 0xFF);
        }
    }

    (void)argv;
}

static void udf_final(sqlite3_context *ctx) {
    void *agg0 = sqlite3_aggregate_context(ctx, 0);
    if (agg0) {
        sqlite3_result_pointer(ctx, agg0, "agg.ptr", NULL);
    } else {
        sqlite3_result_int(ctx, 0);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;
    int rc;
    size_t o = 0;
    int i1 = 0, i2 = 0, i3 = 0, i4 = 0;
    size_t rem, part1, part2, part3;

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    (void)sqlite3_create_function(db, "fuzz_scalar", 4, SQLITE_UTF8, NULL, udf_scalar, NULL, NULL);
    (void)sqlite3_create_function(db, "fuzz_agg", 4, SQLITE_UTF8, NULL, NULL, udf_step, udf_final);

    rc = sqlite3_exec(db, "CREATE TABLE t(a,b,c,d);", NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return 0;
    }

    rc = sqlite3_exec(db, "INSERT INTO t VALUES(1,2,3,4),(5,6,7,8),(9,10,11,12);", NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return 0;
    }

    rc = sqlite3_prepare_v2(
        db,
        "SELECT fuzz_scalar(?1, ?2, ?3, ?4), fuzz_agg(?5, ?6, ?7, ?8) FROM t;",
        -1,
        &stmt,
        NULL
    );
    if (rc != SQLITE_OK || stmt == NULL) {
        sqlite3_close(db);
        return 0;
    }

    if (Size - o >= 4) { memcpy(&i1, Data + o, 4); o += 4; }
    if (Size - o >= 4) { memcpy(&i2, Data + o, 4); o += 4; }
    if (Size - o >= 4) { memcpy(&i3, Data + o, 4); o += 4; }
    if (Size - o >= 4) { memcpy(&i4, Data + o, 4); o += 4; }

    rem = (o <= Size) ? (Size - o) : 0;
    part1 = rem / 3;
    part2 = rem / 3;
    part3 = rem - part1 - part2;

    (void)sqlite3_bind_text(stmt, 1, (const char *)(Data + o), (int)part1, SQLITE_TRANSIENT);
    (void)sqlite3_bind_blob(stmt, 2, Data + o + part1, (int)part2, SQLITE_TRANSIENT);
    (void)sqlite3_bind_int(stmt, 3, i1);
    (void)sqlite3_bind_int(stmt, 4, i2);
    (void)sqlite3_bind_int(stmt, 5, i3);
    (void)sqlite3_bind_int(stmt, 6, i4);
    (void)sqlite3_bind_text(stmt, 7, (const char *)(Data + o + part1 + part2), (int)part3, SQLITE_TRANSIENT);
    (void)sqlite3_bind_int(stmt, 8, (int)Size);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        (void)sqlite3_column_type(stmt, 0);
        (void)sqlite3_column_type(stmt, 1);
        (void)sqlite3_column_bytes(stmt, 0);
        (void)sqlite3_column_bytes(stmt, 1);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}