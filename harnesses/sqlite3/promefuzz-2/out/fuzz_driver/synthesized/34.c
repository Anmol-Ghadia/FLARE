// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_libversion_number at sqlite3.c:152370:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_libversion_number at sqlite3.c:152370:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **data, size_t *size) {
    uint32_t v = 0;
    size_t n = (*size < 4) ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static int read_int(const uint8_t **data, size_t *size) {
    return (int)read_u32(data, size);
}

static sqlite3_int64 read_i64(const uint8_t **data, size_t *size) {
    sqlite3_int64 v = 0;
    size_t n = (*size < 8) ? *size : 8;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static const char *take_text(const uint8_t **data, size_t *size, int *len_out) {
    static const char empty[] = "";
    size_t len;
    uint32_t want;

    if (*size == 0) {
        *len_out = 0;
        return empty;
    }

    want = read_u32(data, size);
    len = (*size == 0) ? 0 : (want % (*size + 1));
    *len_out = (int)len;

    if (len == 0) {
        return empty;
    }

    {
        const char *s = (const char *)(*data);
        *data += len;
        *size -= len;
        return s;
    }
}

int LLVMFuzzerTestOneInput_34(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    FILE *fp;
    int rc;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
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

    {
        const char *sql =
            "SELECT "
            "?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,"
            "?11,?12,?13,?14,?15,?16,?17,?18,?19,?20,"
            "?21,?22,?23,?24,?25,?26,?27,?28,?29,?30,"
            "?31,?32,?33,?34,?35";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK || stmt == NULL) {
            sqlite3_close(db);
            return 0;
        }
    }

    {
        const uint8_t *p = Data;
        size_t remaining = Size;
        int tlen;
        const char *txt;
        int version;

        sqlite3_bind_int64(stmt, 1, read_i64(&p, &remaining));
        sqlite3_bind_int(stmt, 2, read_int(&p, &remaining));
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 3, txt, tlen, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, read_int(&p, &remaining));
        sqlite3_bind_int64(stmt, 5, read_i64(&p, &remaining));
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 6, txt, tlen, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 7, read_int(&p, &remaining));
        sqlite3_bind_int64(stmt, 8, read_i64(&p, &remaining));
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 9, txt, tlen, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 10, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 11, read_int(&p, &remaining));
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 12, txt, tlen, SQLITE_TRANSIENT);
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 13, txt, tlen, SQLITE_TRANSIENT);
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 14, txt, tlen, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 15, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 16, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 17, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 18, read_int(&p, &remaining));
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 19, txt, tlen, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 20, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 21, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 22, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 23, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 24, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 25, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 26, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 27, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 28, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 29, read_int(&p, &remaining));
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 30, txt, tlen, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 31, read_int(&p, &remaining));
        version = sqlite3_libversion_number();
        sqlite3_bind_int(stmt, 32, read_int(&p, &remaining) ^ version);
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 33, txt, tlen, SQLITE_TRANSIENT);
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 34, txt, tlen, SQLITE_TRANSIENT);

        (void)sqlite3_step(stmt);
        (void)sqlite3_reset(stmt);
        (void)sqlite3_clear_bindings(stmt);

        p = Data;
        remaining = Size;

        sqlite3_bind_int64(stmt, 1, -(read_i64(&p, &remaining)));
        sqlite3_bind_int(stmt, 2, ~read_int(&p, &remaining));
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 3, txt, tlen, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, ~read_int(&p, &remaining));
        sqlite3_bind_int64(stmt, 5, read_i64(&p, &remaining) ^ (sqlite3_int64)version);
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 6, txt, tlen, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 7, read_int(&p, &remaining) ^ version);
        sqlite3_bind_int64(stmt, 8, -(read_i64(&p, &remaining)));
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 9, txt, tlen, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 10, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 11, read_int(&p, &remaining));
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 12, txt, tlen, SQLITE_TRANSIENT);
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 13, txt, tlen, SQLITE_TRANSIENT);
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 14, txt, tlen, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 15, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 16, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 17, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 18, read_int(&p, &remaining));
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 19, txt, tlen, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 20, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 21, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 22, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 23, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 24, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 25, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 26, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 27, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 28, read_int(&p, &remaining));
        sqlite3_bind_int(stmt, 29, read_int(&p, &remaining));
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 30, txt, tlen, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 31, read_int(&p, &remaining));
        (void)sqlite3_libversion_number();
        sqlite3_bind_int(stmt, 32, version);
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 33, txt, tlen, SQLITE_TRANSIENT);
        txt = take_text(&p, &remaining, &tlen);
        sqlite3_bind_text(stmt, 34, txt, tlen, SQLITE_TRANSIENT);

        (void)sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}