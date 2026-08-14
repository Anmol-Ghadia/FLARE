// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_decltype16 at sqlite3.c:73170:24 in sqlite3.h
// sqlite3_column_table_name16 at sqlite3.c:73200:24 in sqlite3.h
// sqlite3_column_database_name16 at sqlite3.c:73186:24 in sqlite3.h
// sqlite3_column_origin_name16 at sqlite3.c:73214:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_database_name16 at sqlite3.c:73186:24 in sqlite3.h
// sqlite3_column_origin_name16 at sqlite3.c:73214:24 in sqlite3.h
// sqlite3_column_decltype16 at sqlite3.c:73170:24 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_table_name16 at sqlite3.c:73200:24 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_decltype16 at sqlite3.c:73170:24 in sqlite3.h
// sqlite3_column_table_name16 at sqlite3.c:73200:24 in sqlite3.h
// sqlite3_column_database_name16 at sqlite3.c:73186:24 in sqlite3.h
// sqlite3_column_origin_name16 at sqlite3.c:73214:24 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    sqlite3_stmt *create_stmt = NULL;
    char *errmsg = NULL;
    size_t off = 0;
    uint32_t v = 0;
    int rc;

    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db,
                 "PRAGMA foreign_keys=OFF;"
                 "CREATE TABLE t1(a INTEGER, b TEXT, c REAL, d BLOB);"
                 "INSERT INTO t1 VALUES(1,'alpha',3.14,x'414243');"
                 "INSERT INTO t1 VALUES(2,'beta',2.71,NULL);"
                 "INSERT INTO t1 VALUES(NULL,'gamma',-1.0,x'00ff');",
                 NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    if (Size > 0) {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp) {
            fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    {
        const char *create_sql =
            "CREATE TEMP TABLE IF NOT EXISTS fuzzfile(x TEXT);";
        rc = sqlite3_prepare_v2(db, create_sql, -1, &create_stmt, NULL);
        if (rc == SQLITE_OK && create_stmt) {
            sqlite3_step(create_stmt);
        }
        if (create_stmt) {
            sqlite3_finalize(create_stmt);
            create_stmt = NULL;
        }
        sqlite3_exec(db,
                     "DELETE FROM fuzzfile;"
                     "INSERT INTO fuzzfile(x) VALUES('./dummy_file');",
                     NULL, NULL, &errmsg);
        if (errmsg) {
            sqlite3_free(errmsg);
            errmsg = NULL;
        }
    }

    {
        static const char *queries[] = {
            "SELECT a, b, c, d FROM t1;",
            "SELECT a AS col_a, b AS col_b FROM t1 WHERE a IS NOT NULL;",
            "SELECT t1.a, t1.b, t1.c FROM t1 ORDER BY a;",
            "SELECT b, length(b), typeof(d) FROM t1;",
            "SELECT x FROM fuzzfile;",
            "SELECT a + 1, upper(b), c * 2 FROM t1;",
            "SELECT * FROM t1 WHERE b LIKE 'a%';",
            "SELECT a, b, c, d FROM t1 WHERE rowid = 1;"
        };
        size_t num_queries = sizeof(queries) / sizeof(queries[0]);
        const char *sql = queries[0];
        int step_count = 0;
        int i, col_count;

        if (read_u32(Data, Size, &off, &v)) {
            sql = queries[v % num_queries];
        }

        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK || stmt == NULL) {
            if (stmt) sqlite3_finalize(stmt);
            sqlite3_close(db);
            return 0;
        }

        col_count = sqlite3_column_count(stmt);

        for (i = -2; i < col_count + 2; i++) {
            const void *p1 = sqlite3_column_name16(stmt, i);
            const void *p2 = sqlite3_column_decltype16(stmt, i);
            const void *p3 = sqlite3_column_table_name16(stmt, i);
            const void *p4 = sqlite3_column_database_name16(stmt, i);
            const void *p5 = sqlite3_column_origin_name16(stmt, i);
            (void)p1;
            (void)p2;
            (void)p3;
            (void)p4;
            (void)p5;
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW && step_count < 8) {
            int idx1 = 0, idx2 = 0, idx3 = 0, idx4 = 0, idx5 = 0, idx6 = 0;

            if (read_u32(Data, Size, &off, &v)) idx1 = (int)(v % (uint32_t)(col_count + 4)) - 2;
            else idx1 = step_count - 1;
            if (read_u32(Data, Size, &off, &v)) idx2 = (int)(v % (uint32_t)(col_count + 4)) - 2;
            else idx2 = step_count;
            if (read_u32(Data, Size, &off, &v)) idx3 = (int)(v % (uint32_t)(col_count + 4)) - 2;
            else idx3 = step_count + 1;
            if (read_u32(Data, Size, &off, &v)) idx4 = (int)(v % (uint32_t)(col_count + 4)) - 2;
            else idx4 = 0;
            if (read_u32(Data, Size, &off, &v)) idx5 = (int)(v % (uint32_t)(col_count + 4)) - 2;
            else idx5 = col_count - 1;
            if (read_u32(Data, Size, &off, &v)) idx6 = (int)(v % (uint32_t)(col_count + 4)) - 2;
            else idx6 = col_count;

            {
                const void *p1 = sqlite3_column_database_name16(stmt, idx1);
                const void *p2 = sqlite3_column_origin_name16(stmt, idx2);
                const void *p3 = sqlite3_column_decltype16(stmt, idx3);
                const void *p4 = sqlite3_column_text16(stmt, idx4);
                const void *p5 = sqlite3_column_table_name16(stmt, idx5);
                const void *p6 = sqlite3_column_name16(stmt, idx6);
                (void)p1;
                (void)p2;
                (void)p3;
                (void)p4;
                (void)p5;
                (void)p6;
            }

            for (i = -1; i <= col_count; i++) {
                const void *p = sqlite3_column_text16(stmt, i);
                (void)p;
            }

            step_count++;
        }

        sqlite3_reset(stmt);

        for (i = -2; i < col_count + 2; i++) {
            const void *p1 = sqlite3_column_name16(stmt, i);
            const void *p2 = sqlite3_column_decltype16(stmt, i);
            const void *p3 = sqlite3_column_table_name16(stmt, i);
            const void *p4 = sqlite3_column_database_name16(stmt, i);
            const void *p5 = sqlite3_column_origin_name16(stmt, i);
            const void *p6 = sqlite3_column_text16(stmt, i);
            (void)p1;
            (void)p2;
            (void)p3;
            (void)p4;
            (void)p5;
            (void)p6;
        }

        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    sqlite3_close(db);
    return 0;
}