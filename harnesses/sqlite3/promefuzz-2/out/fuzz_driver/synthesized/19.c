// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static int read_u32(const uint8_t *data, size_t size, size_t *off, uint32_t *out) {
    if (*off + 4 > size) return 0;
    *out = ((uint32_t)data[*off]) |
           ((uint32_t)data[*off + 1] << 8) |
           ((uint32_t)data[*off + 2] << 16) |
           ((uint32_t)data[*off + 3] << 24);
    *off += 4;
    return 1;
}

static int64_t read_i64(const uint8_t *data, size_t size, size_t *off) {
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8 && *off < size; ++i, ++(*off)) {
        v |= ((uint64_t)data[*off]) << (8 * i);
    }
    return (int64_t)v;
}

int LLVMFuzzerTestOneInput_19(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *sql = NULL;
    char *formatted = NULL;
    int rc;
    size_t off = 0;
    uint32_t selector = 0;
    int param_count, idx;
    const char *param_name;
    sqlite3_int64 bind_value;

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    if (!read_u32(Data, Size, &off, &selector)) {
        selector = 0;
    }

    switch (selector % 6) {
        case 0:
            sql = sqlite3_mprintf("SELECT ?1, :name, @other, $x, ?");
            break;
        case 1:
            sql = sqlite3_mprintf("SELECT ?5, ?2, :abc, ?");
            break;
        case 2:
            sql = sqlite3_mprintf("VALUES(:x),(@y),($z),(?3),(?)");
            break;
        case 3: {
            const char *tail = (const char *)(Data + off);
            int tail_len = (int)(Size - off);
            sql = sqlite3_mprintf("SELECT %.*q, ?1, :p, @q, $r, ?", tail_len, tail);
            break;
        }
        case 4:
            sql = sqlite3_mprintf("WITH c(v) AS (SELECT ?10) SELECT :n, @m, $k, ?, ?2 FROM c");
            break;
        default:
            sql = sqlite3_mprintf("SELECT ?");
            break;
    }

    if (sql == NULL) {
        sqlite3_close(db);
        return 0;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK || stmt == NULL) {
        sqlite3_free(sql);
        sqlite3_close(db);
        return 0;
    }

    param_count = sqlite3_bind_parameter_count(stmt);

    if (param_count <= 0) {
        idx = 1;
    } else {
        uint32_t idx_selector = 0;
        if (!read_u32(Data, Size, &off, &idx_selector)) {
            idx_selector = 0;
        }
        idx = (int)(idx_selector % (uint32_t)(param_count + 3));
    }

    param_name = sqlite3_bind_parameter_name(stmt, idx);

    bind_value = (sqlite3_int64)read_i64(Data, Size, &off);
    (void)sqlite3_bind_int64(stmt, idx, bind_value);

    formatted = sqlite3_mprintf("sql=%q|count=%d|idx=%d|name=%s|value=%lld",
                                sql,
                                param_count,
                                idx,
                                param_name ? param_name : "(null)",
                                (long long)bind_value);
    if (formatted) {
        sqlite3_free(formatted);
    }

    sqlite3_finalize(stmt);
    sqlite3_free(sql);
    sqlite3_close(db);
    return 0;
}