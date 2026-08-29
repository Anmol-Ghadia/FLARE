// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text64 at sqlite3.c:73396:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text64 at sqlite3.c:73396:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
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

static uint32_t read_u32(const uint8_t **p, size_t *n) {
    uint32_t v = 0;
    size_t take = *n < 4 ? *n : 4;
    for (size_t i = 0; i < take; i++) {
        v = (v << 8) | (*p)[i];
    }
    *p += take;
    *n -= take;
    return v;
}

static uint64_t read_u64(const uint8_t **p, size_t *n) {
    uint64_t v = 0;
    size_t take = *n < 8 ? *n : 8;
    for (size_t i = 0; i < take; i++) {
        v = (v << 8) | (*p)[i];
    }
    *p += take;
    *n -= take;
    return v;
}

static double read_double(const uint8_t **p, size_t *n) {
    union {
        uint64_t u;
        double d;
    } conv;
    conv.u = read_u64(p, n);
    return conv.d;
}

int LLVMFuzzerTestOneInput_9(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t remaining = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;

    if (sqlite3_open(":memory:", &db) != SQLITE_OK || db == NULL) {
        if (db) {
            sqlite3_close(db);
        }
        return 0;
    }

    sqlite3_exec(db, "CREATE TABLE t(a,b,c,d,e);", NULL, NULL, NULL);

    const char *sql = "INSERT INTO t(a,b,c,d,e) VALUES(?1,?2,?3,?4,?5);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK || stmt == NULL) {
        if (stmt) {
            sqlite3_finalize(stmt);
        }
        sqlite3_close(db);
        return 0;
    }

    int idx1 = (int)((read_u32(&p, &remaining) % 5) + 1);
    int v1 = (int)read_u32(&p, &remaining);
    (void)sqlite3_bind_int(stmt, idx1, v1);

    int idx2 = (int)((read_u32(&p, &remaining) % 5) + 1);
    sqlite3_int64 v2 = (sqlite3_int64)read_u64(&p, &remaining);
    (void)sqlite3_bind_int64(stmt, idx2, v2);

    int idx3 = (int)((read_u32(&p, &remaining) % 5) + 1);
    double v3 = read_double(&p, &remaining);
    (void)sqlite3_bind_double(stmt, idx3, v3);

    int idx4 = (int)((read_u32(&p, &remaining) % 5) + 1);
    sqlite3_uint64 text_len = (sqlite3_uint64)remaining;
    unsigned char enc = (remaining > 0 && (p[0] & 1)) ? SQLITE_UTF16 : SQLITE_UTF8;
    (void)sqlite3_bind_text64(stmt, idx4, (const char *)p, text_len, SQLITE_TRANSIENT, enc);

    int idx5 = (int)((read_u32(&p, &remaining) % 5) + 1);
    int v5 = (int)read_u32(&p, &remaining);
    (void)sqlite3_bind_int(stmt, idx5, v5);

    (void)sqlite3_step(stmt);
    (void)sqlite3_reset(stmt);
    (void)sqlite3_clear_bindings(stmt);

    if (Size > 0) {
        int idx1b = (int)((Data[0] % 5) + 1);
        (void)sqlite3_bind_int(stmt, idx1b, -(int)Size);

        int idx2b = (int)(((Size > 1 ? Data[1] : 0) % 5) + 1);
        (void)sqlite3_bind_int64(stmt, idx2b, (sqlite3_int64)(int64_t)Size);

        int idx3b = (int)(((Size > 2 ? Data[2] : 0) % 5) + 1);
        (void)sqlite3_bind_double(stmt, idx3b, (double)Size / 3.0);

        int idx4b = (int)(((Size > 3 ? Data[3] : 0) % 5) + 1);
        (void)sqlite3_bind_text64(stmt, idx4b, "", 0, SQLITE_STATIC, SQLITE_UTF8);

        int idx5b = (int)(((Size > 4 ? Data[4] : 0) % 5) + 1);
        (void)sqlite3_bind_int(stmt, idx5b, 0);

        (void)sqlite3_step(stmt);
        (void)sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}