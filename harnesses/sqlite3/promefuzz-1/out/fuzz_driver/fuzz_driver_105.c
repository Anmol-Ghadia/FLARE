// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_text64 at sqlite3.c:73396:16 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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

static int write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return 0;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
    return 1;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t n = Size;

    (void)write_dummy_file(Data, Size);

    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;

    if (sqlite3_open(":memory:", &db) != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    (void)sqlite3_exec(db,
        "CREATE TABLE t(a INTEGER, b TEXT, c TEXT);"
        "INSERT INTO t VALUES(1,'x','y');"
        "INSERT INTO t VALUES(2,'hello','world');",
        NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    const char *sqls[] = {
        "SELECT ?1, ?2, :x, @y, $z, ? FROM t WHERE a=?3;",
        "INSERT INTO t(a,b,c) VALUES(?1,:b,@c);",
        "UPDATE t SET b=?2, c=$name WHERE a=?1;",
        "SELECT :alpha, ?5, ?, @beta, $gamma;",
        "DELETE FROM t WHERE a=?1 OR b=:bname;",
        "SELECT * FROM t WHERE a=?1 AND b LIKE ?2;",
        "SELECT ?999, :n, @m, $k, ?;"
    };

    uint32_t which = read_u32(&p, &n);
    const char *sql = sqls[which % (sizeof(sqls) / sizeof(sqls[0]))];

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqlite3_close(db);
        return 0;
    }

    int count = sqlite3_bind_parameter_count(stmt);
    (void)sqlite3_bind_parameter_count(NULL);

    for (int i = 0; i <= count + 2; i++) {
        const char *name = sqlite3_bind_parameter_name(stmt, i);
        if (name) {
            (void)sqlite3_bind_parameter_index(stmt, name);
        }
    }

    (void)sqlite3_bind_parameter_name(NULL, 1);
    (void)sqlite3_bind_parameter_index(NULL, ":x");

    const char *candidate_names[] = {
        ":x", "@y", "$z", "?1", "?2", "?3", "?5", "?999",
        ":b", "@c", "$name", ":alpha", "@beta", "$gamma",
        ":bname", ":n", "@m", "$k", "?", ""
    };

    for (size_t i = 0; i < sizeof(candidate_names) / sizeof(candidate_names[0]); i++) {
        (void)sqlite3_bind_parameter_index(stmt, candidate_names[i]);
    }

    int iterations = (n > 0) ? (int)(p[0] % 8 + 1) : 1;
    if (n > 0) {
        p++;
        n--;
    }

    for (int it = 0; it < iterations; it++) {
        int local_count = sqlite3_bind_parameter_count(stmt);
        int max_index = local_count > 0 ? local_count : 4;

        uint32_t op = read_u32(&p, &n);
        int idx;
        if ((op & 1U) && local_count > 0) {
            idx = (int)((op % (uint32_t)local_count) + 1);
        } else {
            idx = (int)(op % (uint32_t)(max_index + 3));
        }

        if ((op & 2U) != 0) {
            sqlite3_int64 v = (sqlite3_int64)read_u64(&p, &n);
            (void)sqlite3_bind_int64(stmt, idx, v);
        } else {
            sqlite3_uint64 len = (sqlite3_uint64)(n > 0 ? (read_u32(&p, &n) % (n + 1)) : 0);
            const char *text = (const char *)p;
            if (len > n) len = (sqlite3_uint64)n;

            unsigned char encodings[] = {
                SQLITE_UTF8,
                SQLITE_UTF16LE,
                SQLITE_UTF16BE,
                SQLITE_UTF16
            };
            unsigned char enc = encodings[op % (sizeof(encodings) / sizeof(encodings[0]))];

            (void)sqlite3_bind_text64(stmt, idx, text, len, SQLITE_TRANSIENT, enc);

            p += (size_t)len;
            n -= (size_t)len;
        }

        for (int j = 1; j <= local_count + 1; j++) {
            const char *nm = sqlite3_bind_parameter_name(stmt, j);
            if (nm) {
                (void)sqlite3_bind_parameter_index(stmt, nm);
            }
        }

        int step_rc = sqlite3_step(stmt);
        (void)step_rc;
        (void)sqlite3_reset(stmt);

        if ((op & 4U) != 0) {
            (void)sqlite3_clear_bindings(stmt);
        }
    }

    (void)sqlite3_step(stmt);
    (void)sqlite3_reset(stmt);
    (void)sqlite3_reset(stmt);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}