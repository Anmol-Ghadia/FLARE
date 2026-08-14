#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

static void udf_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
    (void)argc;
    (void)argv;

    const unsigned char *txt = sqlite3_value_text(argv[0]);
    int txt_len = sqlite3_value_bytes(argv[0]);

    const void *blob = sqlite3_value_blob(argv[1]);
    int blob_len = sqlite3_value_bytes(argv[1]);

    int mode = sqlite3_value_int(argv[2]);
    int aux = sqlite3_value_int(argv[3]);

    int agg_n = aux;
    if (agg_n < -16) agg_n = -16;
    if (agg_n > 4096) agg_n = 4096;

    void *agg = sqlite3_aggregate_context(ctx, agg_n);
    if (agg && agg_n > 0) {
        unsigned char *p = (unsigned char *)agg;
        p[0] ^= (unsigned char)(mode & 0xFF);
        if (agg_n > 1) p[1] ^= (unsigned char)(txt_len & 0xFF);
    }

    switch ((unsigned)mode % 8U) {
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
        case 5:
            sqlite3_result_text(ctx, (const char *)"", -1, SQLITE_STATIC);
            sqlite3_result_int(ctx, aux ^ txt_len);
            break;
        case 6:
            sqlite3_result_pointer(ctx, NULL, "null.ptr", NULL);
            sqlite3_result_text16be(ctx, blob, blob_len >= 0 ? blob_len : 0, SQLITE_TRANSIENT);
            break;
        default:
            sqlite3_result_error(ctx, "forced", 6);
            sqlite3_result_pointer(ctx, NULL, "after.error", NULL);
            break;
    }
}

static void udf_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
    int req = 0;
    if (argc > 0) {
        req = sqlite3_value_int(argv[0]);
    }
    if (req < -32) req = -32;
    if (req > 8192) req = 8192;

    void *agg = sqlite3_aggregate_context(ctx, req);
    if (agg && req > 0) {
        unsigned char *p = (unsigned char *)agg;
        p[0] ^= 0xA5;
        if (argc > 1) {
            p[0] ^= (unsigned char)(sqlite3_value_int(argv[1]) & 0xFF);
        }
    }

    if (argc > 2) {
        const unsigned char *txt = sqlite3_value_text(argv[2]);
        int len = sqlite3_value_bytes(argv[2]);
        if (txt) {
            sqlite3_result_text(ctx, (const char *)txt, len, SQLITE_TRANSIENT);
        }
    }
    if (argc > 3) {
        sqlite3_result_int(ctx, sqlite3_value_int(argv[3]));
    }
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

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_create_function(db, "fuzz_scalar", 4, SQLITE_UTF8, NULL, udf_scalar, NULL, NULL);
    sqlite3_create_function(db, "fuzz_agg", 4, SQLITE_UTF8, NULL, NULL, udf_step, udf_final);

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

    size_t o = 0;

    int i1 = 0, i2 = 0, i3 = 0, i4 = 0;
    if (Size - o >= 4) { memcpy(&i1, Data + o, 4); o += 4; }
    if (Size - o >= 4) { memcpy(&i2, Data + o, 4); o += 4; }
    if (Size - o >= 4) { memcpy(&i3, Data + o, 4); o += 4; }
    if (Size - o >= 4) { memcpy(&i4, Data + o, 4); o += 4; }

    size_t rem = (o <= Size) ? (Size - o) : 0;
    size_t part1 = rem / 3;
    size_t part2 = rem / 3;
    size_t part3 = rem - part1 - part2;

    sqlite3_bind_text(stmt, 1, (const char *)(Data + o), (int)part1, SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 2, Data + o + part1, (int)part2, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, i1);
    sqlite3_bind_int(stmt, 4, i2);
    sqlite3_bind_int(stmt, 5, i3);
    sqlite3_bind_int(stmt, 6, i4);
    sqlite3_bind_text(stmt, 7, (const char *)(Data + o + part1 + part2), (int)part3, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 8, (int)Size);

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