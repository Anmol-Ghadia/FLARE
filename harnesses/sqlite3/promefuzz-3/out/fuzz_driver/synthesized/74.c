// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_bind_text16 at sqlite3.c:73409:16 in sqlite3.h
// sqlite3_bind_blob64 at sqlite3.c:73327:16 in sqlite3.h
// sqlite3_bind_pointer at sqlite3.c:73369:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_bind_text16 at sqlite3.c:73409:16 in sqlite3.h
// sqlite3_bind_blob64 at sqlite3.c:73327:16 in sqlite3.h
// sqlite3_bind_pointer at sqlite3.c:73369:16 in sqlite3.h
// sqlite3_transfer_bindings at sqlite3.c:73539:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_transfer_bindings at sqlite3.c:73539:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_bind_pointer at sqlite3.c:73369:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
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

static int read_int(const uint8_t **p, size_t *n) {
    return (int)read_u32(p, n);
}

static size_t read_size(const uint8_t **p, size_t *n, size_t maxv) {
    if (*n == 0) return 0;
    uint32_t v = read_u32(p, n);
    if (maxv == 0) return 0;
    return (size_t)(v % (maxv + 1));
}

static void noop_destructor(void *p) {
    (void)p;
}

int LLVMFuzzerTestOneInput_74(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt1 = NULL;
    sqlite3_stmt *stmt2 = NULL;
    sqlite3_stmt *stmt3 = NULL;
    FILE *fp = NULL;

    const uint8_t *p = Data;
    size_t n = Size;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
        fp = NULL;
    }

    if (sqlite3_open(":memory:", &db) != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(a BLOB, b TEXT, c BLOB, d TEXT);", NULL, NULL, NULL);

    {
        const char *sql1 = "SELECT ?1, ?2, ?3, ?4;";
        const char *sql2 = "SELECT ?1, ?2, ?3, ?4;";
        const char *sql3 = "SELECT ?1, ?3, ?5;";

        sqlite3_prepare_v2(db, sql1, -1, &stmt1, NULL);
        sqlite3_prepare_v2(db, sql2, -1, &stmt2, NULL);
        sqlite3_prepare_v2(db, sql3, -1, &stmt3, NULL);
    }

    if (stmt1) {
        int param_count = sqlite3_bind_parameter_count(stmt1);
        int idx1 = param_count > 0 ? ((read_int(&p, &n) % (param_count + 2)) - 1) : 1;
        int idx2 = param_count > 0 ? ((read_int(&p, &n) % (param_count + 2)) - 1) : 1;
        int idx3 = param_count > 0 ? ((read_int(&p, &n) % (param_count + 2)) - 1) : 1;
        int idx4 = param_count > 0 ? ((read_int(&p, &n) % (param_count + 2)) - 1) : 1;

        int blob_n = read_int(&p, &n);
        int text16_n = read_int(&p, &n);
        sqlite3_uint64 blob64_n = (sqlite3_uint64)read_u32(&p, &n);

        size_t remain = n;
        size_t part1 = read_size(&p, &n, remain);
        const void *blob_data = p;
        if (part1 > n) part1 = n;
        p += part1;
        n -= part1;

        remain = n;
        size_t part2 = read_size(&p, &n, remain);
        const void *text16_data = p;
        if (part2 > n) part2 = n;
        p += part2;
        n -= part2;

        remain = n;
        size_t part3 = n;
        const void *blob64_data = p;
        if (part3 > n) part3 = n;

        if ((uint64_t)blob64_n > (uint64_t)part3) {
            blob64_n = (sqlite3_uint64)part3;
        }

        sqlite3_bind_parameter_count(stmt1);
        sqlite3_bind_zeroblob(stmt1, idx1, blob_n);
        sqlite3_bind_text16(stmt1, idx2, text16_data, text16_n, SQLITE_STATIC);
        sqlite3_bind_blob64(stmt1, idx3, blob64_data, blob64_n, SQLITE_STATIC);
        sqlite3_bind_pointer(stmt1, idx4, (void *)blob_data, "fuzz_ptr", noop_destructor);

        sqlite3_step(stmt1);
        sqlite3_reset(stmt1);

        sqlite3_bind_parameter_count(stmt1);
        sqlite3_bind_zeroblob(stmt1, 1, (int)part1);

        /* Avoid passing -1 for arbitrary fuzz data. Provide an explicit,
           bounded byte length for UTF-16 input and keep it even-sized. */
        {
            int safe_text16_len = (int)(part2 & ~(size_t)1);
            sqlite3_bind_text16(stmt1, 2, text16_data, safe_text16_len, SQLITE_STATIC);
        }

        sqlite3_bind_blob64(stmt1, 3, blob64_data, (sqlite3_uint64)part3, SQLITE_STATIC);
        sqlite3_bind_pointer(stmt1, 4, (void *)text16_data, "alt_ptr", NULL);
    }

    if (stmt1 && stmt2) {
        sqlite3_transfer_bindings(stmt1, stmt2);
        sqlite3_step(stmt2);
        sqlite3_reset(stmt2);
    }

    if (stmt1 && stmt3) {
        sqlite3_transfer_bindings(stmt1, stmt3);
        sqlite3_step(stmt3);
        sqlite3_reset(stmt3);
    }

    if (stmt3) {
        int pc3 = sqlite3_bind_parameter_count(stmt3);
        int idx = pc3 > 0 ? ((read_int(&p, &n) % (pc3 + 2)) - 1) : 1;
        sqlite3_bind_zeroblob(stmt3, idx, read_int(&p, &n));
        sqlite3_bind_pointer(stmt3, idx, db, "sqlite3*", NULL);
        sqlite3_step(stmt3);
        sqlite3_reset(stmt3);
    }

    if (stmt1) sqlite3_finalize(stmt1);
    if (stmt2) sqlite3_finalize(stmt2);
    if (stmt3) sqlite3_finalize(stmt3);
    sqlite3_close(db);
    return 0;
}