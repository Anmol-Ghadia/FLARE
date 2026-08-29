// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_decltype16 at sqlite3.c:73170:24 in sqlite3.h
// sqlite3_column_database_name16 at sqlite3.c:73186:24 in sqlite3.h
// sqlite3_column_table_name16 at sqlite3.c:73200:24 in sqlite3.h
// sqlite3_column_origin_name16 at sqlite3.c:73214:24 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
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

static int pick_int(const uint8_t **p, size_t *n) {
    int v = 0;
    size_t take = *n < sizeof(int) ? *n : sizeof(int);
    for (size_t i = 0; i < take; ++i) {
        v = (v << 8) ^ (*p)[i];
    }
    *p += take;
    *n -= take;
    return v;
}

static sqlite3_int64 pick_i64(const uint8_t **p, size_t *n) {
    sqlite3_int64 v = 0;
    size_t take = *n < sizeof(sqlite3_int64) ? *n : sizeof(sqlite3_int64);
    for (size_t i = 0; i < take; ++i) {
        v = (v << 8) ^ (*p)[i];
    }
    *p += take;
    *n -= take;
    return v;
}

static void exec_sql(sqlite3 *db, const char *sql) {
    char *errmsg = NULL;
    sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (errmsg) sqlite3_free(errmsg);
}

static void touch_metadata(sqlite3_stmt *stmt, int idx) {
    const void *p1 = sqlite3_column_name16(stmt, idx);
    const void *p2 = sqlite3_column_decltype16(stmt, idx);
    const void *p3 = sqlite3_column_database_name16(stmt, idx);
    const void *p4 = sqlite3_column_table_name16(stmt, idx);
    const void *p5 = sqlite3_column_origin_name16(stmt, idx);
    (void)p1; (void)p2; (void)p3; (void)p4; (void)p5;
}

static void touch_row(sqlite3 *db, sqlite3_stmt *stmt, int idx) {
    const void *b = sqlite3_column_blob(stmt, idx);
    (void)b;
    (void)sqlite3_errcode(db);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    const uint8_t *p = Data;
    size_t n = Size;

    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_extended_result_codes(db, 1);

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp) {
            if (Size) fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    exec_sql(db, "PRAGMA foreign_keys=OFF;");
    exec_sql(db, "CREATE TABLE t1(a INTEGER, b TEXT, c BLOB, d REAL, e);");
    exec_sql(db, "ATTACH DATABASE './dummy_file' AS aux;");
    exec_sql(db, "CREATE TABLE IF NOT EXISTS aux.t2(x TEXT, y BLOB);");

    sqlite3_stmt *ins1 = NULL;
    if (sqlite3_prepare_v2(db, "INSERT INTO t1(a,b,c,d,e) VALUES(?,?,?,?,?);", -1, &ins1, NULL) == SQLITE_OK) {
        sqlite3_bind_int(ins1, 1, pick_int(&p, &n));
        sqlite3_bind_text(ins1, 2, (const char *)p, (int)n, SQLITE_TRANSIENT);
        sqlite3_bind_blob(ins1, 3, Data, (int)Size, SQLITE_TRANSIENT);
        sqlite3_bind_double(ins1, 4, (double)(pick_int(&p, &n) % 100000) / 7.0);
        sqlite3_bind_int64(ins1, 5, pick_i64(&p, &n));
        sqlite3_step(ins1);
    }
    if (ins1) sqlite3_finalize(ins1);

    sqlite3_stmt *ins2 = NULL;
    if (sqlite3_prepare_v2(db, "INSERT INTO aux.t2(x,y) VALUES(?,?);", -1, &ins2, NULL) == SQLITE_OK) {
        sqlite3_bind_text(ins2, 1, (const char *)Data, (int)Size, SQLITE_TRANSIENT);
        sqlite3_bind_blob(ins2, 2, Data, (int)Size, SQLITE_TRANSIENT);
        sqlite3_step(ins2);
    }
    if (ins2) sqlite3_finalize(ins2);

    const char *queries[] = {
        "SELECT a, b, c, d, e FROM t1;",
        "SELECT rowid, a AS aa, b, c FROM t1;",
        "SELECT t1.a, t1.b, aux.t2.x, aux.t2.y FROM t1 LEFT JOIN aux.t2 ON 1;",
        "SELECT a+1, upper(b), c, d, e FROM t1;",
        "SELECT zeroblob(0), zeroblob(4), x'414243', NULL FROM t1;",
        "SELECT * FROM (SELECT a, b, c FROM t1);",
        "SELECT a, b, c FROM t1 UNION ALL SELECT NULL, x, y FROM aux.t2;",
        "SELECT ?1, ?2, ?3, a, b, c FROM t1;"
    };

    int qidx = 0;
    if (Size > 0) qidx = Data[0] % (int)(sizeof(queries) / sizeof(queries[0]));

    if (sqlite3_prepare_v2(db, queries[qidx], -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, pick_int(&p, &n));
        sqlite3_bind_blob(stmt, 2, Data, (int)Size, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, (const char *)Data, (int)Size, SQLITE_TRANSIENT);

        int col_count = sqlite3_column_count(stmt);

        for (int i = -2; i < col_count + 2; ++i) {
            touch_metadata(stmt, i);
        }

        int rc;
        int steps = 0;
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW && steps < 8) {
            int current_cols = sqlite3_column_count(stmt);
            for (int i = -2; i < current_cols + 2; ++i) {
                touch_metadata(stmt, i);
                touch_row(db, stmt, i);
            }
            steps++;
        }

        sqlite3_reset(stmt);

        for (int i = -2; i < col_count + 2; ++i) {
            touch_metadata(stmt, i);
        }

        sqlite3_clear_bindings(stmt);
        sqlite3_bind_null(stmt, 1);
        sqlite3_bind_blob(stmt, 2, "", 0, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, "", 0, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            int current_cols = sqlite3_column_count(stmt);
            for (int i = -1; i <= current_cols; ++i) {
                touch_row(db, stmt, i);
                touch_metadata(stmt, i);
            }
        }
    }

    if (stmt) sqlite3_finalize(stmt);

    exec_sql(db, "DETACH DATABASE aux;");
    sqlite3_close(db);
    return 0;
}