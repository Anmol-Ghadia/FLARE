// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_column_decltype at sqlite3.c:73166:24 in sqlite3.h
// sqlite3_column_origin_name at sqlite3.c:73210:24 in sqlite3.h
// sqlite3_column_database_name at sqlite3.c:73182:24 in sqlite3.h
// sqlite3_column_table_name at sqlite3.c:73196:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_column_decltype at sqlite3.c:73166:24 in sqlite3.h
// sqlite3_column_origin_name at sqlite3.c:73210:24 in sqlite3.h
// sqlite3_column_database_name at sqlite3.c:73182:24 in sqlite3.h
// sqlite3_column_table_name at sqlite3.c:73196:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_column_decltype at sqlite3.c:73166:24 in sqlite3.h
// sqlite3_column_origin_name at sqlite3.c:73210:24 in sqlite3.h
// sqlite3_column_database_name at sqlite3.c:73182:24 in sqlite3.h
// sqlite3_column_table_name at sqlite3.c:73196:24 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_column_decltype at sqlite3.c:73166:24 in sqlite3.h
// sqlite3_column_origin_name at sqlite3.c:73210:24 in sqlite3.h
// sqlite3_column_database_name at sqlite3.c:73182:24 in sqlite3.h
// sqlite3_column_table_name at sqlite3.c:73196:24 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_column_decltype at sqlite3.c:73166:24 in sqlite3.h
// sqlite3_column_origin_name at sqlite3.c:73210:24 in sqlite3.h
// sqlite3_column_database_name at sqlite3.c:73182:24 in sqlite3.h
// sqlite3_column_table_name at sqlite3.c:73196:24 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
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

static uint32_t read_u32(const uint8_t **data, size_t *size) {
    uint32_t v = 0;
    size_t n = *size < 4 ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static int read_int(const uint8_t **data, size_t *size) {
    return (int)read_u32(data, size);
}

static void exec_sql(sqlite3 *db, const char *sql) {
    char *errmsg = NULL;
    sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
    }
}

int LLVMFuzzerTestOneInput_85(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *sql = NULL;
    int rc;

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    exec_sql(db, "PRAGMA foreign_keys=OFF;");
    exec_sql(db, "CREATE TABLE t1(a INTEGER, b TEXT, c REAL, d BLOB);");
    exec_sql(db, "CREATE TABLE t2(x TEXT, y INTEGER);");
    exec_sql(db, "INSERT INTO t1 VALUES(1,'one',1.5,x'0102');");
    exec_sql(db, "INSERT INTO t1 VALUES(2,'two',2.5,x'0304');");
    exec_sql(db, "INSERT INTO t2 VALUES('alpha',10);");
    exec_sql(db, "INSERT INTO t2 VALUES('beta',20);");
    exec_sql(db, "ATTACH DATABASE './dummy_file' AS aux;");
    exec_sql(db, "CREATE TABLE IF NOT EXISTS aux.t3(m TEXT, n INTEGER);");
    exec_sql(db, "INSERT INTO aux.t3 VALUES('aux1',100);");

    sql = (char *)malloc(Size + 1);
    if (!sql) {
        sqlite3_close(db);
        return 0;
    }
    memcpy(sql, Data, Size);
    sql[Size] = '\0';

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK || stmt == NULL) {
        free(sql);
        sqlite3_close(db);
        return 0;
    }

    int col_count = sqlite3_column_count(stmt);

    for (int i = -2; i < col_count + 3; ++i) {
        const char *name = sqlite3_column_name(stmt, i);
        const char *decl = sqlite3_column_decltype(stmt, i);
        const char *origin = sqlite3_column_origin_name(stmt, i);
        const char *dbname = sqlite3_column_database_name(stmt, i);
        const char *tname = sqlite3_column_table_name(stmt, i);

        (void)name;
        (void)decl;
        (void)origin;
        (void)dbname;
        (void)tname;
    }

    {
        const uint8_t *p = Data;
        size_t remaining = Size;
        int iterations = (int)(remaining > 0 ? (remaining % 8) + 1 : 1);

        for (int it = 0; it < iterations; ++it) {
            int action = read_int(&p, &remaining) % 4;

            if (action == 0) {
                for (int i = -1; i < col_count + 2; ++i) {
                    (void)sqlite3_column_name(stmt, i);
                    (void)sqlite3_column_decltype(stmt, i);
                    (void)sqlite3_column_origin_name(stmt, i);
                    (void)sqlite3_column_database_name(stmt, i);
                    (void)sqlite3_column_table_name(stmt, i);
                }
            } else if (action == 1) {
                rc = sqlite3_step(stmt);
                if (rc == SQLITE_ROW || rc == SQLITE_DONE || rc == SQLITE_ERROR || rc == SQLITE_MISUSE) {
                    for (int i = -1; i < col_count + 2; ++i) {
                        (void)sqlite3_column_name(stmt, i);
                        (void)sqlite3_column_decltype(stmt, i);
                        (void)sqlite3_column_origin_name(stmt, i);
                        (void)sqlite3_column_database_name(stmt, i);
                        (void)sqlite3_column_table_name(stmt, i);
                    }
                }
            } else if (action == 2) {
                sqlite3_reset(stmt);
                sqlite3_clear_bindings(stmt);
                for (int i = -1; i < col_count + 2; ++i) {
                    (void)sqlite3_column_name(stmt, i);
                    (void)sqlite3_column_decltype(stmt, i);
                    (void)sqlite3_column_origin_name(stmt, i);
                    (void)sqlite3_column_database_name(stmt, i);
                    (void)sqlite3_column_table_name(stmt, i);
                }
            } else {
                int idx = read_int(&p, &remaining);
                if (col_count > 0) {
                    idx = (idx % (col_count + 4)) - 2;
                } else {
                    idx = (idx % 5) - 2;
                }
                (void)sqlite3_column_count(stmt);
                (void)sqlite3_column_name(stmt, idx);
                (void)sqlite3_column_decltype(stmt, idx);
                (void)sqlite3_column_origin_name(stmt, idx);
                (void)sqlite3_column_database_name(stmt, idx);
                (void)sqlite3_column_table_name(stmt, idx);
            }
        }
    }

    sqlite3_finalize(stmt);
    free(sql);
    sqlite3_close(db);
    return 0;
}