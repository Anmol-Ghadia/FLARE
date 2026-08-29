// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_origin_name16 at sqlite3.c:73214:24 in sqlite3.h
// sqlite3_column_database_name16 at sqlite3.c:73186:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_bytes16 at sqlite3.c:73031:16 in sqlite3.h
// sqlite3_column_bytes16 at sqlite3.c:73031:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_bytes16 at sqlite3.c:73031:16 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_origin_name16 at sqlite3.c:73214:24 in sqlite3.h
// sqlite3_column_database_name16 at sqlite3.c:73186:24 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_bytes16 at sqlite3.c:73031:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_enable_load_extension at sqlite3.c:114763:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
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

static const char *pick_sql(uint8_t selector) {
    switch (selector % 8) {
        case 0:
            return "SELECT x'00ff', 'text', 123, 45.67, NULL";
        case 1:
            return "SELECT zeroblob(0), zeroblob(8), CAST(123 AS TEXT), CAST(x'4142' AS BLOB)";
        case 2:
            return "CREATE TABLE IF NOT EXISTS t(a BLOB, b TEXT, c INTEGER, d REAL);"
                   "DELETE FROM t;"
                   "INSERT INTO t VALUES(x'010203', 'hello', 42, 3.14);"
                   "INSERT INTO t VALUES(zeroblob(0), '', -1, -0.0);"
                   "SELECT a, b, c, d, rowid FROM t";
        case 3:
            return "CREATE TABLE IF NOT EXISTS t2(x TEXT, y BLOB);"
                   "DELETE FROM t2;"
                   "INSERT INTO t2 VALUES('abc', x'61626300');"
                   "SELECT x AS alias_x, y AS alias_y FROM t2";
        case 4:
            return "ATTACH './dummy_file' AS aux;"
                   "CREATE TABLE IF NOT EXISTS aux.u(m TEXT, n BLOB);"
                   "DELETE FROM aux.u;"
                   "INSERT INTO aux.u VALUES('auxtext', x'102030');"
                   "SELECT m, n FROM aux.u";
        case 5:
            return "SELECT substr('unicode',1,4), hex(x'00010203'), CAST(NULL AS TEXT)";
        case 6:
            return "CREATE TABLE IF NOT EXISTS main.s(p TEXT, q INTEGER);"
                   "DELETE FROM s;"
                   "INSERT INTO s VALUES('name', 99);"
                   "SELECT p, q, p||q, q+1 FROM s";
        default:
            return "SELECT 1";
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_enable_load_extension(db, 0);

    const uint8_t *p = Data;
    size_t n = Size;

    const char *sql = pick_sql(n ? *p++ : 0);
    if (n) n--;

    sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    const char *prep_sql = "SELECT x'414243', 'xyz', 7, 8.5, NULL";
    if (n) {
        switch (*p % 6) {
            case 0:
                prep_sql = "SELECT x'414243', 'xyz', 7, 8.5, NULL";
                break;
            case 1:
                prep_sql = "SELECT a, b, c, d FROM t";
                break;
            case 2:
                prep_sql = "SELECT x AS cx, y AS cy FROM t2";
                break;
            case 3:
                prep_sql = "SELECT m, n FROM aux.u";
                break;
            case 4:
                prep_sql = "SELECT p, q, p||q, q+1 FROM s";
                break;
            default:
                prep_sql = "SELECT zeroblob(0), zeroblob(4), CAST(123 AS TEXT), x'00ff'";
                break;
        }
        p++;
        n--;
    }

    if (sqlite3_prepare_v2(db, prep_sql, -1, &stmt, NULL) != SQLITE_OK || stmt == NULL) {
        sqlite3_close(db);
        return 0;
    }

    int col_count = sqlite3_column_count(stmt);

    for (int i = 0; i < col_count + 2; i++) {
        const void *name16 = sqlite3_column_name16(stmt, i);
        (void)name16;
        const void *origin16 = sqlite3_column_origin_name16(stmt, i);
        (void)origin16;
        const void *dbname16 = sqlite3_column_database_name16(stmt, i);
        (void)dbname16;
    }

    int max_steps = 4;
    if (n) {
        max_steps = 1 + (*p % 8);
        p++;
        n--;
    }

    for (int step_iter = 0; step_iter < max_steps; step_iter++) {
        int rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW) {
            break;
        }

        int loops = 1;
        if (n) {
            loops = 1 + (*p % 8);
            p++;
            n--;
        }

        for (int j = 0; j < loops; j++) {
            int idx;
            if (n >= 4) {
                idx = read_int(&p, &n);
            } else {
                idx = j % (col_count > 0 ? col_count : 1);
            }

            if (col_count > 0) {
                idx %= col_count;
                if (idx < 0) idx += col_count;
            } else {
                idx = 0;
            }

            switch ((n ? *p++ : 0) % 6) {
                case 0: {
                    const void *blob = sqlite3_column_blob(stmt, idx);
                    (void)blob;
                    int err = sqlite3_errcode(db);
                    (void)err;
                    break;
                }
                case 1: {
                    const void *txt16 = sqlite3_column_text16(stmt, idx);
                    (void)txt16;
                    int bytes16 = sqlite3_column_bytes16(stmt, idx);
                    (void)bytes16;
                    break;
                }
                case 2: {
                    int bytes16 = sqlite3_column_bytes16(stmt, idx);
                    (void)bytes16;
                    const void *blob = sqlite3_column_blob(stmt, idx);
                    (void)blob;
                    int err = sqlite3_errcode(db);
                    (void)err;
                    break;
                }
                case 3: {
                    const void *blob = sqlite3_column_blob(stmt, idx);
                    (void)blob;
                    const void *txt16 = sqlite3_column_text16(stmt, idx);
                    (void)txt16;
                    int bytes16 = sqlite3_column_bytes16(stmt, idx);
                    (void)bytes16;
                    break;
                }
                case 4: {
                    const void *txt16 = sqlite3_column_text16(stmt, idx);
                    (void)txt16;
                    const void *blob = sqlite3_column_blob(stmt, idx);
                    (void)blob;
                    int err = sqlite3_errcode(db);
                    (void)err;
                    break;
                }
                default: {
                    const void *name16 = sqlite3_column_name16(stmt, idx);
                    (void)name16;
                    const void *origin16 = sqlite3_column_origin_name16(stmt, idx);
                    (void)origin16;
                    const void *dbname16 = sqlite3_column_database_name16(stmt, idx);
                    (void)dbname16;
                    const void *txt16 = sqlite3_column_text16(stmt, idx);
                    (void)txt16;
                    int bytes16 = sqlite3_column_bytes16(stmt, idx);
                    (void)bytes16;
                    break;
                }
            }

            if (n == 0) {
                continue;
            }
        }

        if (n && (*p & 1)) {
            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);
            sqlite3_step(stmt);
        }
        if (n) {
            p++;
            n--;
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}