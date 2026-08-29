// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
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
    for (size_t i = 0; i < take; ++i) {
        v = (v << 8) | (*p)[i];
    }
    *p += take;
    *n -= take;
    return v;
}

static int choose_int(const uint8_t **p, size_t *n, int fallback) {
    if (*n == 0) return fallback;
    int v = (int)(*(*p)++);
    (*n)--;
    return v;
}

static void write_dummy_file(const uint8_t *data, size_t size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (size) fwrite(data, 1, size, fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t n = Size;
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    sqlite3_stmt *stmt2 = NULL;
    char *errmsg = NULL;
    int rc;

    write_dummy_file(Data, Size);

    if (sqlite3_open(":memory:", &db) != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db,
                 "CREATE TABLE t(a INTEGER, b TEXT, c BLOB, d REAL);"
                 "INSERT INTO t VALUES(1,'alpha',x'001122',1.25);"
                 "INSERT INTO t VALUES(2,'beta',x'33445566',2.5);"
                 "INSERT INTO t VALUES(NULL,'',x'',-3.0);"
                 "CREATE TABLE u(x TEXT);"
                 "INSERT INTO u VALUES('hello');",
                 NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    const char *sqls[] = {
        "SELECT a,b,c,d FROM t;",
        "SELECT b,a FROM t WHERE a IS NOT NULL;",
        "SELECT length(b), c, d FROM t;",
        "UPDATE t SET b=b WHERE a=1;",
        "PRAGMA table_info(t);",
        "SELECT readfile('./dummy_file');",
        "SELECT quote(b), typeof(c), a FROM t;",
        "SELECT x FROM u;"
    };

    int idx1 = choose_int(&p, &n, 0) % (int)(sizeof(sqls) / sizeof(sqls[0]));
    int idx2 = choose_int(&p, &n, 1) % (int)(sizeof(sqls) / sizeof(sqls[0]));

    rc = sqlite3_prepare_v2(db, sqls[idx1], -1, &stmt, NULL);
    if (rc != SQLITE_OK) stmt = NULL;

    rc = sqlite3_prepare_v2(db, sqls[idx2], -1, &stmt2, NULL);
    if (rc != SQLITE_OK) stmt2 = NULL;

    (void)sqlite3_stmt_busy(NULL);
    (void)sqlite3_column_count(NULL);
    (void)sqlite3_data_count(NULL);

    if (stmt) {
        (void)sqlite3_column_count(stmt);
        (void)sqlite3_data_count(stmt);
        (void)sqlite3_stmt_busy(stmt);

        int steps = (int)(read_u32(&p, &n) % 8U);
        for (int i = 0; i < steps; ++i) {
            rc = sqlite3_step(stmt);

            (void)sqlite3_stmt_busy(stmt);
            (void)sqlite3_column_count(stmt);
            (void)sqlite3_data_count(stmt);

            int colcount = sqlite3_column_count(stmt);
            int datacount = sqlite3_data_count(stmt);

            int fuzz_col = choose_int(&p, &n, 0);
            int col = colcount > 0 ? (fuzz_col % (colcount + 2)) - 1 : fuzz_col - 1;

            if (rc == SQLITE_ROW) {
                if (datacount > 0) {
                    (void)sqlite3_column_type(stmt, col);
                    (void)sqlite3_column_bytes(stmt, col);
                    (void)sqlite3_column_text16(stmt, col);

                    for (int j = 0; j < datacount && j < 4; ++j) {
                        (void)sqlite3_column_type(stmt, j);
                        (void)sqlite3_column_text16(stmt, j);
                        (void)sqlite3_column_bytes(stmt, j);
                    }
                }
            } else if (rc == SQLITE_DONE) {
                (void)sqlite3_column_type(stmt, col);
                (void)sqlite3_column_bytes(stmt, col);
                (void)sqlite3_column_text16(stmt, col);
                break;
            } else {
                break;
            }

            if ((choose_int(&p, &n, 0) & 1) != 0) {
                sqlite3_reset(stmt);
                (void)sqlite3_stmt_busy(stmt);
                (void)sqlite3_data_count(stmt);
            }
        }

        if ((choose_int(&p, &n, 0) & 1) != 0) {
            sqlite3_reset(stmt);
            (void)sqlite3_stmt_busy(stmt);
        }
    }

    if (stmt2) {
        (void)sqlite3_column_count(stmt2);
        (void)sqlite3_data_count(stmt2);
        (void)sqlite3_stmt_busy(stmt2);

        rc = sqlite3_step(stmt2);
        (void)sqlite3_stmt_busy(stmt2);
        (void)sqlite3_column_count(stmt2);
        (void)sqlite3_data_count(stmt2);

        int colcount2 = sqlite3_column_count(stmt2);
        int col2 = colcount2 > 0 ? (int)(read_u32(&p, &n) % (uint32_t)(colcount2 + 2)) - 1 : 0;

        (void)sqlite3_column_type(stmt2, col2);
        (void)sqlite3_column_bytes(stmt2, col2);
        (void)sqlite3_column_text16(stmt2, col2);

        if ((choose_int(&p, &n, 0) & 1) != 0) {
            sqlite3_reset(stmt2);
            (void)sqlite3_stmt_busy(stmt2);
        }
    }

    sqlite3_stmt *it = NULL;
    while ((it = sqlite3_next_stmt(db, it)) != NULL) {
        (void)sqlite3_stmt_busy(it);
        (void)sqlite3_column_count(it);
        (void)sqlite3_data_count(it);
    }

    if (stmt) sqlite3_finalize(stmt);
    if (stmt2) sqlite3_finalize(stmt2);
    sqlite3_close(db);
    return 0;
}