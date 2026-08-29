// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_column_decltype16 at sqlite3.c:73170:24 in sqlite3.h
// sqlite3_column_database_name16 at sqlite3.c:73186:24 in sqlite3.h
// sqlite3_column_origin_name16 at sqlite3.c:73214:24 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_table_name16 at sqlite3.c:73200:24 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
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
#include <stdio.h>
#include <sqlite3.h>

static int read_u32(const uint8_t *Data, size_t Size, size_t *Off, uint32_t *Out) {
    if (*Off + 4 > Size) return 0;
    *Out = ((uint32_t)Data[*Off]) |
           ((uint32_t)Data[*Off + 1] << 8) |
           ((uint32_t)Data[*Off + 2] << 16) |
           ((uint32_t)Data[*Off + 3] << 24);
    *Off += 4;
    return 1;
}

static int read_i32(const uint8_t *Data, size_t Size, size_t *Off, int *Out) {
    uint32_t v;
    if (!read_u32(Data, Size, Off, &v)) return 0;
    *Out = (int)v;
    return 1;
}

static void consume_column_metadata(sqlite3_stmt *stmt, int col) {
    const void *p;
    p = sqlite3_column_decltype16(stmt, col);
    (void)p;
    p = sqlite3_column_database_name16(stmt, col);
    (void)p;
    p = sqlite3_column_origin_name16(stmt, col);
    (void)p;
    p = sqlite3_column_name16(stmt, col);
    (void)p;
    p = sqlite3_column_table_name16(stmt, col);
    (void)p;
    p = sqlite3_column_text16(stmt, col);
    (void)p;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;
    size_t off = 0;
    int rc;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size) fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db, "PRAGMA foreign_keys=OFF;", NULL, NULL, NULL);
    sqlite3_exec(db, "CREATE TABLE t1(a INTEGER, b TEXT, c REAL, d BLOB);", NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    sqlite3_exec(db,
                 "INSERT INTO t1(a,b,c,d) VALUES "
                 "(1,'alpha',1.25,x'00ff'),"
                 "(2,'beta',2.50,x'1122'),"
                 "(3,NULL,NULL,NULL);",
                 NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    sqlite3_exec(db, "ATTACH DATABASE './dummy_file' AS aux;", NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS aux.t2(x TEXT, y INTEGER);", NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    sqlite3_exec(db,
                 "INSERT INTO aux.t2(x,y) VALUES ('gamma',10),('delta',20);",
                 NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    {
        static const char *queries[] = {
            "SELECT a, b, c, d FROM t1;",
            "SELECT a AS alias_a, b || '' AS expr_b, c, d FROM t1;",
            "SELECT t1.a, t1.b, aux.t2.x, aux.t2.y FROM t1 LEFT JOIN aux.t2 ON (t1.a = aux.t2.y/10);",
            "SELECT a, CAST(b AS TEXT), typeof(c), hex(d) FROM t1;",
            "SELECT 1 AS one, 'txt' AS two, NULL AS three;",
            "SELECT rowid, a, b FROM t1 WHERE a > 0;",
            "SELECT x, y FROM aux.t2;",
            "SELECT t1.b, aux.t2.x FROM t1, aux.t2 LIMIT 3;"
        };
        uint32_t qidx = 0;
        int col = 0;
        int step_count = 0;
        int col_count, i;

        if (!read_u32(Data, Size, &off, &qidx)) qidx = 0;
        qidx %= (uint32_t)(sizeof(queries) / sizeof(queries[0]));

        rc = sqlite3_prepare_v2(db, queries[qidx], -1, &stmt, NULL);
        if (rc == SQLITE_OK && stmt != NULL) {
            if (!read_i32(Data, Size, &off, &col)) col = 0;

            col_count = sqlite3_column_count(stmt);

            consume_column_metadata(stmt, col);
            if (col_count > 0) {
                for (i = 0; i < col_count; i++) {
                    consume_column_metadata(stmt, i);
                }
                consume_column_metadata(stmt, -1);
                consume_column_metadata(stmt, col_count);
                consume_column_metadata(stmt, col_count + 1);
            }

            while (step_count < 4) {
                rc = sqlite3_step(stmt);
                if (rc != SQLITE_ROW) break;

                consume_column_metadata(stmt, col);
                for (i = 0; i < col_count; i++) {
                    consume_column_metadata(stmt, i);
                }
                step_count++;
            }

            sqlite3_reset(stmt);

            consume_column_metadata(stmt, col);
            for (i = 0; i < col_count; i++) {
                consume_column_metadata(stmt, i);
            }

            rc = sqlite3_step(stmt);
            if (rc == SQLITE_ROW) {
                consume_column_metadata(stmt, col);
                for (i = 0; i < col_count; i++) {
                    consume_column_metadata(stmt, i);
                }
            }

            sqlite3_finalize(stmt);
            stmt = NULL;
        }
    }

    sqlite3_exec(db, "DETACH DATABASE aux;", NULL, NULL, NULL);
    sqlite3_close(db);
    return 0;
}