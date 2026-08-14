// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_stmt_readonly at sqlite3.c:73571:16 in sqlite3.h
// sqlite3_stmt_isexplain at sqlite3.c:73579:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_stmt_isexplain at sqlite3.c:73579:16 in sqlite3.h
// sqlite3_stmt_readonly at sqlite3.c:73571:16 in sqlite3.h
// sqlite3_stmt_isexplain at sqlite3.c:73579:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

static int consume_u8(const uint8_t **p, size_t *n) {
    if (*n == 0) return 0;
    int v = **p;
    (*p)++;
    (*n)--;
    return v;
}

static uint32_t consume_u32(const uint8_t **p, size_t *n) {
    uint32_t v = 0;
    for (int i = 0; i < 4; i++) {
        v = (v << 8) | (uint32_t)consume_u8(p, n);
    }
    return v;
}

static int consume_range(const uint8_t **p, size_t *n, int maxv) {
    if (maxv <= 0) return 0;
    return (int)(consume_u32(p, n) % (uint32_t)maxv);
}

static char *make_sql_from_data(const uint8_t *data, size_t size) {
    char *sql = (char *)malloc(size + 1);
    if (!sql) return NULL;
    for (size_t i = 0; i < size; i++) {
        unsigned char c = data[i];
        if (c == 0) {
            sql[i] = ' ';
        } else if (c < 32 || c > 126) {
            sql[i] = (char)('A' + (c % 26));
        } else {
            sql[i] = (char)c;
        }
    }
    sql[size] = '\0';
    return sql;
}

static void exercise_stmt(sqlite3 *db, sqlite3_stmt *stmt, const uint8_t **p, size_t *n) {
    if (!stmt) {
        (void)sqlite3_stmt_isexplain(NULL);
        return;
    }

    (void)sqlite3_stmt_readonly(stmt);
    (void)sqlite3_stmt_isexplain(stmt);

    int col_count = sqlite3_column_count(stmt);

    for (int i = -2; i < col_count + 2; i++) {
        (void)sqlite3_column_name(stmt, i);
    }

    int max_steps = 1 + consume_range(p, n, 4);
    for (int s = 0; s < max_steps; s++) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            int cc = sqlite3_column_count(stmt);
            for (int i = -1; i < cc + 1; i++) {
                (void)sqlite3_column_name(stmt, i);
            }

            for (int i = 0; i < cc; i++) {
                int mode = consume_range(p, n, 3);
                if (mode == 0) {
                    const void *blob = sqlite3_column_blob(stmt, i);
                    (void)blob;
                    (void)sqlite3_column_bytes(stmt, i);
                } else if (mode == 1) {
                    (void)sqlite3_column_bytes(stmt, i);
                    (void)sqlite3_column_blob(stmt, i);
                    (void)sqlite3_column_bytes(stmt, i);
                } else {
                    const void *blob1 = sqlite3_column_blob(stmt, i);
                    int bytes1 = sqlite3_column_bytes(stmt, i);
                    const void *blob2 = sqlite3_column_blob(stmt, i);
                    int bytes2 = sqlite3_column_bytes(stmt, i);
                    (void)blob1;
                    (void)bytes1;
                    (void)blob2;
                    (void)bytes2;
                }
            }
        } else {
            break;
        }
    }

    (void)sqlite3_reset(stmt);
    (void)sqlite3_stmt_readonly(stmt);
    (void)sqlite3_stmt_isexplain(stmt);
    (void)sqlite3_column_count(stmt);

    for (int i = -1; i < col_count + 1; i++) {
        (void)sqlite3_column_name(stmt, i);
    }

    sqlite3_finalize(stmt);

    (void)db;
}

int LLVMFuzzerTestOneInput_88(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t n = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    sqlite3 *db = NULL;
    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db,
                 "CREATE TABLE IF NOT EXISTS t1(a BLOB, b TEXT, c INTEGER);"
                 "DELETE FROM t1;"
                 "INSERT INTO t1 VALUES(x'00', 'alpha', 1);"
                 "INSERT INTO t1 VALUES(x'414243', 'beta', 2);"
                 "INSERT INTO t1 VALUES(zeroblob(4), '', NULL);",
                 NULL, NULL, NULL);

    char *sql = make_sql_from_data(p, n);
    if (!sql) {
        sqlite3_close(db);
        return 0;
    }

    for (int iter = 0; iter < 3; iter++) {
        const char *tail = sql;
        while (tail && *tail) {
            sqlite3_stmt *stmt = NULL;
            const char *next = NULL;
            int rc = sqlite3_prepare_v2(db, tail, -1, &stmt, &next);

            if (rc != SQLITE_OK) {
                break;
            }

            if (stmt) {
                exercise_stmt(db, stmt, &p, &n);
            }

            if (!next || next == tail) {
                break;
            }
            tail = next;
        }

        const char *fallbacks[] = {
            "SELECT a AS colA, b AS colB, c AS colC FROM t1;",
            "EXPLAIN SELECT a, b FROM t1 WHERE c > 0;",
            "EXPLAIN QUERY PLAN SELECT * FROM t1 WHERE b LIKE 'a%';",
            "UPDATE t1 SET b='x' WHERE c=999;",
            "BEGIN IMMEDIATE;",
            "COMMIT;"
        };

        int idx = consume_range(&p, &n, (int)(sizeof(fallbacks) / sizeof(fallbacks[0])));
        sqlite3_stmt *stmt = NULL;
        if (sqlite3_prepare_v2(db, fallbacks[idx], -1, &stmt, NULL) == SQLITE_OK && stmt) {
            exercise_stmt(db, stmt, &p, &n);
        }
    }

    free(sql);
    sqlite3_close(db);
    return 0;
}