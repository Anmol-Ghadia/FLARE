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

static char *make_sql_from_data(const uint8_t *data, size_t size) {
    char *sql = (char *)malloc(size + 1);
    if (!sql) return NULL;
    for (size_t i = 0; i < size; i++) {
        unsigned char c = data[i];
        if (c == 0) {
            sql[i] = ' ';
        } else {
            sql[i] = (char)c;
        }
    }
    sql[size] = '\0';
    return sql;
}

static void touch_column_metadata(sqlite3_stmt *stmt, int idx) {
    const char *s1 = sqlite3_column_name(stmt, idx);
    const char *s2 = sqlite3_column_database_name(stmt, idx);
    const char *s3 = sqlite3_column_table_name(stmt, idx);
    const char *s4 = sqlite3_column_decltype(stmt, idx);
    const char *s5 = sqlite3_column_origin_name(stmt, idx);

    (void)s1;
    (void)s2;
    (void)s3;
    (void)s4;
    (void)s5;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *sql = NULL;
    const uint8_t *p = Data;
    size_t n = Size;

    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db,
                 "PRAGMA foreign_keys=OFF;"
                 "CREATE TABLE IF NOT EXISTS t1(a INTEGER, b TEXT, c REAL, d BLOB);"
                 "CREATE TABLE IF NOT EXISTS t2(x TEXT, y INTEGER);"
                 "INSERT INTO t1 VALUES(1,'one',1.5,x'010203');"
                 "INSERT INTO t1 VALUES(2,'two',2.5,x'040506');"
                 "INSERT INTO t2 VALUES('alpha',10);"
                 "INSERT INTO t2 VALUES('beta',20);",
                 NULL, NULL, NULL);

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp) {
            if (Size > 0) fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    if (n == 0) {
        sql = strdup("SELECT a AS col_a, b, c, d, a+b, (SELECT y FROM t2 LIMIT 1) FROM t1;");
    } else {
        uint32_t mode = read_u32(&p, &n) % 6;
        if (mode == 0) {
            sql = make_sql_from_data(p, n);
        } else if (mode == 1) {
            sql = strdup("SELECT a AS aa, b AS bb, c, d FROM t1;");
        } else if (mode == 2) {
            sql = strdup("SELECT t1.a, t1.b, t2.x, t2.y FROM t1 JOIN t2 ON 1;");
        } else if (mode == 3) {
            sql = strdup("SELECT a+1, upper(b), c*2, randomblob(4) FROM t1;");
        } else if (mode == 4) {
            sql = strdup("SELECT * FROM t1 UNION ALL SELECT y, x, y*1.0, x'00' FROM t2;");
        } else {
            sql = strdup("SELECT a, (SELECT x FROM t2 LIMIT 1) AS subq, b FROM t1;");
        }
    }

    if (!sql) {
        sqlite3_close(db);
        return 0;
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK && stmt) {
        int col_count = sqlite3_column_count(stmt);

        for (int i = -2; i < col_count + 3; i++) {
            touch_column_metadata(stmt, i);
        }

        {
            int rc;
            int steps = 0;
            do {
                rc = sqlite3_step(stmt);
                if (rc == SQLITE_ROW) {
                    for (int i = -2; i < col_count + 3; i++) {
                        touch_column_metadata(stmt, i);
                        (void)sqlite3_column_type(stmt, i);
                        (void)sqlite3_column_int(stmt, i);
                        (void)sqlite3_column_int64(stmt, i);
                        (void)sqlite3_column_double(stmt, i);
                        (void)sqlite3_column_text(stmt, i);
                        (void)sqlite3_column_blob(stmt, i);
                        (void)sqlite3_column_bytes(stmt, i);
                    }
                }
                steps++;
            } while (rc == SQLITE_ROW && steps < 8);
        }

        sqlite3_reset(stmt);

        for (int i = col_count + 2; i >= -2; i--) {
            touch_column_metadata(stmt, i);
        }

        {
            int rc = sqlite3_step(stmt);
            if (rc == SQLITE_ROW) {
                for (int i = 0; i < col_count; i++) {
                    (void)sqlite3_column_text(stmt, i);
                    (void)sqlite3_column_bytes(stmt, i);
                    touch_column_metadata(stmt, i);
                }
            }
        }

        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    free(sql);
    sqlite3_close(db);
    return 0;
}