// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_double at sqlite3.c:73036:19 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_double at sqlite3.c:73036:19 in sqlite3.h
// sqlite3_column_double at sqlite3.c:73036:19 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t **p, size_t *n) {
    uint32_t v = 0;
    size_t take = *n < 4 ? *n : 4;
    for (size_t i = 0; i < take; ++i) {
        v = (v << 8) | (*p)[i];
    }
    *p += take;
    *n -= take;
    return v;
}

static double read_double_like(const uint8_t **p, size_t *n) {
    union {
        uint64_t u;
        double d;
    } cvt;
    cvt.u = 0;
    size_t take = *n < 8 ? *n : 8;
    for (size_t i = 0; i < take; ++i) {
        cvt.u = (cvt.u << 8) | (*p)[i];
    }
    *p += take;
    *n -= take;
    return cvt.d;
}

static void write_dummy_file(const uint8_t *data, size_t size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (size) {
        fwrite(data, 1, size, fp);
    }
    fclose(fp);
}

static void exec_sql(sqlite3 *db, const char *sql) {
    char *errmsg = NULL;
    sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
    }
}

static void exercise_row(sqlite3_stmt *stmt, int cols, int selector) {
    if (cols <= 0) return;

    int base = selector % (cols + 3);
    if (base < 0) base = -base;

    for (int i = 0; i < cols + 2; ++i) {
        int col = (base + i) % (cols + 2);
        int t = sqlite3_column_type(stmt, col);
        (void)t;

        if ((selector + i) & 1) {
            const void *blob = sqlite3_column_blob(stmt, col);
            (void)blob;
        } else {
            double d = sqlite3_column_double(stmt, col);
            (void)d;
        }

        if (((selector >> (i % 8)) & 1) != 0) {
            const void *blob2 = sqlite3_column_blob(stmt, col);
            (void)blob2;
            double d2 = sqlite3_column_double(stmt, col);
            (void)d2;
        } else {
            double d3 = sqlite3_column_double(stmt, col);
            (void)d3;
            const void *blob3 = sqlite3_column_blob(stmt, col);
            (void)blob3;
        }
    }
}

static void run_stmt(sqlite3 *db, sqlite3_stmt *stmt, const uint8_t **p, size_t *n) {
    if (!stmt) return;

    int bind_count = sqlite3_bind_parameter_count(stmt);
    for (int i = 1; i <= bind_count; ++i) {
        double v = read_double_like(p, n);
        sqlite3_bind_double(stmt, i, v);
    }

    int max_steps = 1;
    if (*n > 0) {
        max_steps += (int)((**p) % 8);
        (*p)++;
        (*n)--;
    }

    for (int iter = 0; iter < 3; ++iter) {
        int steps = 0;
        while (steps < max_steps) {
            int rc = sqlite3_step(stmt);
            if (rc == SQLITE_ROW) {
                int cols = sqlite3_column_count(stmt);
                int selector = 0;
                if (*n > 0) {
                    selector = **p;
                    (*p)++;
                    (*n)--;
                }
                exercise_row(stmt, cols, selector);
            } else if (rc == SQLITE_SCHEMA) {
                break;
            } else if (rc == SQLITE_BUSY) {
                continue;
            } else {
                break;
            }
            steps++;
        }

        int rrc = sqlite3_reset(stmt);
        (void)rrc;

        if (iter != 2) {
            for (int i = 1; i <= bind_count; ++i) {
                double v = read_double_like(p, n);
                sqlite3_bind_double(stmt, i, v);
            }
        }
    }

    sqlite3_finalize(stmt);
    (void)db;
}

int LLVMFuzzerTestOneInput_114(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t n = Size;

    write_dummy_file(Data, Size);

    sqlite3 *db = NULL;
    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return 0;
    }

    exec_sql(db, "PRAGMA foreign_keys=ON;");
    exec_sql(db, "PRAGMA journal_mode=OFF;");
    exec_sql(db, "CREATE TABLE t(a REAL, b BLOB, c TEXT, d INTEGER);");
    exec_sql(db, "CREATE TABLE t2(x REAL, y BLOB);");

    sqlite3_stmt *ins = NULL;
    if (sqlite3_prepare_v2(db,
                           "INSERT INTO t(a,b,c,d) VALUES(?1, ?2, ?3, ?4);",
                           -1, &ins, NULL) == SQLITE_OK) {
        double a = read_double_like(&p, &n);
        sqlite3_bind_double(ins, 1, a);

        if (n > 0) {
            int blob_len = (int)(n > 32 ? 32 : n);
            sqlite3_bind_blob(ins, 2, p, blob_len, SQLITE_TRANSIENT);
            p += blob_len;
            n -= blob_len;
        } else {
            sqlite3_bind_blob(ins, 2, "", 0, SQLITE_STATIC);
        }

        sqlite3_bind_text(ins, 3, "txt", -1, SQLITE_STATIC);
        sqlite3_bind_int(ins, 4, (int)read_u32(&p, &n));

        sqlite3_step(ins);
        sqlite3_reset(ins);

        sqlite3_bind_double(ins, 1, read_double_like(&p, &n));
        sqlite3_bind_blob(ins, 2, Data, (int)(Size > 16 ? 16 : Size), SQLITE_TRANSIENT);
        sqlite3_bind_text(ins, 3, "123.456", -1, SQLITE_STATIC);
        sqlite3_bind_int(ins, 4, (int)read_u32(&p, &n));
        sqlite3_step(ins);
        sqlite3_finalize(ins);
    }

    exec_sql(db, "INSERT INTO t(a,b,c,d) VALUES(NULL, x'', '', 0);");
    exec_sql(db, "INSERT INTO t(a,b,c,d) VALUES(1.5, x'010203', 'hello', 42);");
    exec_sql(db, "INSERT INTO t2(x,y) VALUES(3.14, readfile('./dummy_file'));");

    sqlite3_stmt *stmt1 = NULL;
    if (sqlite3_prepare_v2(db,
                           "SELECT a,b,c,d, a+0.25, CAST(b AS REAL), CAST(c AS REAL), zeroblob(0) FROM t WHERE a>?1 OR d<?2;",
                           -1, &stmt1, NULL) == SQLITE_OK) {
        run_stmt(db, stmt1, &p, &n);
    }

    sqlite3_stmt *stmt2 = NULL;
    if (sqlite3_prepare_v2(db,
                           "SELECT x,y, CAST(y AS REAL), typeof(y) FROM t2 WHERE x>?1;",
                           -1, &stmt2, NULL) == SQLITE_OK) {
        run_stmt(db, stmt2, &p, &n);
    }

    sqlite3_stmt *stmt3 = NULL;
    if (sqlite3_prepare_v2(db,
                           "SELECT ?1 AS p1, ?2 AS p2, CAST(?2 AS BLOB), CAST(?1 AS TEXT);",
                           -1, &stmt3, NULL) == SQLITE_OK) {
        run_stmt(db, stmt3, &p, &n);
    }

    sqlite3_close(db);
    return 0;
}