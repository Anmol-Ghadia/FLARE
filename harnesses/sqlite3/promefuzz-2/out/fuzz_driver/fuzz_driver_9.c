// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text64 at sqlite3.c:73396:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text64 at sqlite3.c:73396:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
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

static uint32_t read_u32(const uint8_t **data, size_t *size) {
    uint32_t v = 0;
    size_t n = *size < 4 ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static uint64_t read_u64(const uint8_t **data, size_t *size) {
    uint64_t v = 0;
    size_t n = *size < 8 ? *size : 8;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static double read_double(const uint8_t **data, size_t *size) {
    union {
        uint64_t u;
        double d;
    } conv;
    conv.u = read_u64(data, size);
    return conv.d;
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

    sqlite3_exec(db, "CREATE TABLE t(a,b,c,d,e);", NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    const char *sql = "INSERT INTO t(a,b,c,d,e) VALUES(?1,?2,?3,?4,?5);";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK || stmt == NULL) {
        sqlite3_close(db);
        return 0;
    }

    const uint8_t *p = Data;
    size_t remaining = Size;

    int idx1 = (int)(read_u32(&p, &remaining) % 8);
    sqlite3_bind_int(stmt, idx1, (int)read_u32(&p, &remaining));

    int idx2 = (int)(read_u32(&p, &remaining) % 8);
    sqlite3_bind_int64(stmt, idx2, (sqlite3_int64)read_u64(&p, &remaining));

    int idx3 = (int)(read_u32(&p, &remaining) % 8);
    sqlite3_bind_double(stmt, idx3, read_double(&p, &remaining));

    int idx4 = (int)(read_u32(&p, &remaining) % 8);
    sqlite3_uint64 text_len = (sqlite3_uint64)remaining;
    unsigned char enc;
    switch (remaining ? (p[0] % 4) : 0) {
        case 0: enc = SQLITE_UTF8; break;
        case 1: enc = SQLITE_UTF16; break;
        case 2: enc = SQLITE_UTF16LE; break;
        default: enc = SQLITE_UTF16BE; break;
    }
    sqlite3_bind_text64(stmt, idx4, (const char *)p, text_len, SQLITE_TRANSIENT, enc);

    int idx5 = (int)(read_u32(&p, &remaining) % 8);
    sqlite3_bind_int(stmt, idx5, (int)read_u32(&p, &remaining));

    sqlite3_step(stmt);
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);

    if (Size > 0) {
        int idx1b = (int)((Data[0] % 5) + 1);
        sqlite3_bind_int(stmt, idx1b, (int)(Size & 0x7fffffff));

        int idx2b = (int)(((Size > 1 ? Data[1] : 0) % 5) + 1);
        sqlite3_bind_int64(stmt, idx2b, (sqlite3_int64)(int64_t)Size);

        int idx3b = (int)(((Size > 2 ? Data[2] : 0) % 5) + 1);
        sqlite3_bind_double(stmt, idx3b, (double)Size / 3.0);

        int idx4b = (int)(((Size > 3 ? Data[3] : 0) % 5) + 1);
        sqlite3_bind_text64(stmt, idx4b, (const char *)Data, (sqlite3_uint64)Size, SQLITE_TRANSIENT, SQLITE_UTF8);

        int idx5b = (int)(((Size > 4 ? Data[4] : 0) % 5) + 1);
        sqlite3_bind_int(stmt, idx5b, (int)(Data[0]));
        sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}