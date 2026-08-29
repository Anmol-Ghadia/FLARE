// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_column_origin_name at sqlite3.c:73210:24 in sqlite3.h
// sqlite3_column_table_name at sqlite3.c:73196:24 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_column_origin_name at sqlite3.c:73210:24 in sqlite3.h
// sqlite3_column_table_name at sqlite3.c:73196:24 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
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

static int read_int(const uint8_t **p, size_t *n) {
    return (int)read_u32(p, n);
}

static void exercise_metadata(sqlite3_stmt *stmt, int extra_idx) {
    int col_count = sqlite3_column_count(stmt);

    (void)sqlite3_column_name(stmt, extra_idx);
    (void)sqlite3_column_origin_name(stmt, extra_idx);
    (void)sqlite3_column_table_name(stmt, extra_idx);

    for (int i = -1; i <= col_count + 1; i++) {
        (void)sqlite3_column_name(stmt, i);
        (void)sqlite3_column_origin_name(stmt, i);
        (void)sqlite3_column_table_name(stmt, i);
    }
}

static void exercise_row(sqlite3 *db, sqlite3_stmt *stmt, int extra_idx) {
    int col_count = sqlite3_column_count(stmt);

    (void)sqlite3_column_blob(stmt, extra_idx);
    (void)sqlite3_column_type(stmt, extra_idx);
    (void)sqlite3_errcode(db);

    for (int i = -1; i <= col_count + 1; i++) {
        (void)sqlite3_column_type(stmt, i);
        (void)sqlite3_column_blob(stmt, i);
        (void)sqlite3_errcode(db);
    }

    for (int i = 0; i < col_count; i++) {
        (void)sqlite3_column_type(stmt, i);
        (void)sqlite3_column_blob(stmt, i);
        (void)sqlite3_errcode(db);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;
    int rc;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) {
            sqlite3_close(db);
        }
        return 0;
    }

    (void)sqlite3_exec(
        db,
        "CREATE TABLE t(a BLOB, b TEXT, c INTEGER, d REAL);"
        "INSERT INTO t VALUES(x'00', 'alpha', 1, 1.5);"
        "INSERT INTO t VALUES(x'', '', NULL, -2.25);"
        "INSERT INTO t VALUES(x'414243', 'beta', 123456, 3.14159);",
        NULL, NULL, &errmsg
    );
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    const uint8_t *p = Data;
    size_t n = Size;

    {
        int mode_raw = read_int(&p, &n);
        int idx_seed = read_int(&p, &n);
        int limit = (int)(n > 512 ? 512 : n);

        char sqlbuf[2048];
        static const char *const prefixes[] = {
            "SELECT a AS x, b AS y, c AS z, d AS w FROM t",
            "SELECT b, a, c, d FROM t",
            "SELECT a, length(b) AS lb, c+1 AS cp1, d FROM t",
            "SELECT t.a, t.b, t.c, t.d FROM t",
            "SELECT randomblob(4) AS rb, upper(b) AS ub, c, d FROM t"
        };
        size_t prefix_count = sizeof(prefixes) / sizeof(prefixes[0]);
        size_t mode = 0;
        const char *prefix;
        int written;

        if (prefix_count > 0) {
            mode = ((unsigned int)mode_raw) % prefix_count;
        }
        prefix = prefixes[mode];

        written = snprintf(sqlbuf, sizeof(sqlbuf), "%s", prefix);
        if (written < 0 || (size_t)written >= sizeof(sqlbuf)) {
            sqlite3_close(db);
            return 0;
        }

        for (int i = 0; i < limit && (size_t)written + 2 < sizeof(sqlbuf); i++) {
            unsigned char ch = p[i];
            if (ch >= 32 && ch <= 126 && ch != ';') {
                sqlbuf[written++] = (char)ch;
            }
        }
        sqlbuf[written] = '\0';

        rc = sqlite3_prepare_v2(db, sqlbuf, -1, &stmt, NULL);
        if (rc != SQLITE_OK || stmt == NULL) {
            const char *fallback =
                "SELECT a AS alias_a, b, c, d FROM t UNION ALL "
                "SELECT x'0102', 'txt', 42, 9.5";
            rc = sqlite3_prepare_v2(db, fallback, -1, &stmt, NULL);
        }

        if (stmt) {
            int extra_idx = idx_seed % 16;
            if (idx_seed & 1) {
                extra_idx = -extra_idx;
            }

            exercise_metadata(stmt, extra_idx);

            for (int iter = 0; iter < 3; iter++) {
                rc = sqlite3_step(stmt);
                if (rc == SQLITE_ROW) {
                    exercise_row(db, stmt, extra_idx);
                } else {
                    break;
                }
            }

            sqlite3_reset(stmt);

            exercise_metadata(stmt, extra_idx);

            rc = sqlite3_step(stmt);
            if (rc == SQLITE_ROW) {
                exercise_row(db, stmt, extra_idx);
            }

            sqlite3_finalize(stmt);
            stmt = NULL;
        }
    }

    sqlite3_close(db);
    return 0;
}