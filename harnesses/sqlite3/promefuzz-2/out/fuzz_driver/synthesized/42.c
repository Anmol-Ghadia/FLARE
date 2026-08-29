// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v3 at sqlite3.c:119085:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_stmt_readonly at sqlite3.c:73571:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_stmt_readonly at sqlite3.c:73571:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_database_name16 at sqlite3.c:73186:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_stmt_readonly at sqlite3.c:73571:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_database_name16 at sqlite3.c:73186:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static int consume_u8(const uint8_t **data, size_t *size) {
    if (*size == 0) return 0;
    int v = **data;
    (*data)++;
    (*size)--;
    return v;
}

static int consume_i32(const uint8_t **data, size_t *size) {
    int v = 0;
    size_t n = *size < 4 ? *size : 4;
    for (size_t i = 0; i < n; i++) {
        v = (v << 8) | **data;
        (*data)++;
        (*size)--;
    }
    return v;
}

static void write_dummy_file(const uint8_t *data, size_t size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (size > 0) fwrite(data, 1, size, fp);
    fclose(fp);
}

static void exec_sql(sqlite3 *db, const char *sql) {
    char *errmsg = NULL;
    sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (errmsg) sqlite3_free(errmsg);
}

static void exercise_stmt(sqlite3 *db, sqlite3_stmt *stmt, const uint8_t *data, size_t size) {
    (void)db;

    if (!stmt) {
        sqlite3_stmt_readonly(NULL);
        sqlite3_expired(NULL);
        return;
    }

    sqlite3_stmt_readonly(stmt);
    sqlite3_expired(stmt);

    int step_count = 0;
    while (step_count < 3) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            int col_count = sqlite3_column_count(stmt);
            int extra = (size > 0) ? (int)(data[0] % 3) : 0;
            for (int i = -1; i < col_count + extra; i++) {
                const void *blob = sqlite3_column_blob(stmt, i);
                const void *txt16 = sqlite3_column_text16(stmt, i);
                const void *dbn16 = sqlite3_column_database_name16(stmt, i);
                (void)blob;
                (void)txt16;
                (void)dbn16;
            }
        } else {
            break;
        }
        step_count++;
    }

    sqlite3_reset(stmt);
    sqlite3_stmt_readonly(stmt);
    sqlite3_expired(stmt);

    int rc2 = sqlite3_step(stmt);
    if (rc2 == SQLITE_ROW) {
        int col_count = sqlite3_column_count(stmt);
        for (int i = 0; i < col_count; i++) {
            const void *blob = sqlite3_column_blob(stmt, i);
            const void *txt16 = sqlite3_column_text16(stmt, i);
            const void *dbn16 = sqlite3_column_database_name16(stmt, i);
            (void)blob;
            (void)txt16;
            (void)dbn16;
        }
    }
    sqlite3_reset(stmt);
}

int LLVMFuzzerTestOneInput_42(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t remaining = Size;

    write_dummy_file(Data, Size);

    sqlite3 *db = NULL;
    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return 0;
    }

    exec_sql(db, "PRAGMA foreign_keys=OFF;");
    exec_sql(db, "CREATE TABLE IF NOT EXISTS t1(a INTEGER, b TEXT, c BLOB);");
    exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(x TEXT);");
    exec_sql(db, "INSERT INTO t1(a,b,c) VALUES(1,'alpha',x'00ff'),(2,'beta',x''),(3,NULL,x'414243');");
    exec_sql(db, "INSERT INTO t2(x) VALUES('row1'),('row2');");
    exec_sql(db, "ATTACH './dummy_file' AS aux;");
    exec_sql(db, "DETACH aux;");

    static const char *templates[] = {
        "SELECT a,b,c FROM t1;",
        "SELECT c,b,a FROM t1 WHERE a>=0;",
        "SELECT zeroblob(0), zeroblob(4), NULL;",
        "SELECT CAST(a AS BLOB), CAST(b AS BLOB), c FROM t1;",
        "SELECT b, length(c), a FROM t1;",
        "PRAGMA schema_version;",
        "BEGIN;",
        "BEGIN IMMEDIATE;",
        "COMMIT;",
        "ROLLBACK;",
        "SAVEPOINT s1;",
        "RELEASE s1;",
        "ATTACH './dummy_file' AS fuzzaux;",
        "DETACH fuzzaux;",
        "CREATE TABLE IF NOT EXISTS t3(y);",
        "INSERT INTO t1(a,b,c) VALUES(99,'ins',x'01');",
        "UPDATE t1 SET b='upd' WHERE a=1;",
        "DELETE FROM t1 WHERE a=123456;",
        "SELECT * FROM t1 WHERE b LIKE '%a%';",
        "SELECT x FROM t2;"
    };

    int iterations = 1 + (consume_u8(&p, &remaining) % 4);
    for (int it = 0; it < iterations; it++) {
        const char *sql = templates[consume_u8(&p, &remaining) % (sizeof(templates) / sizeof(templates[0]))];
        sqlite3_stmt *stmt = NULL;
        const char *tail = NULL;

        int prep_flags = 0;
#ifdef SQLITE_PREPARE_PERSISTENT
        if (consume_u8(&p, &remaining) & 1) {
            prep_flags |= SQLITE_PREPARE_PERSISTENT;
        }
#endif

#if SQLITE_VERSION_NUMBER >= 3020000
        int rc = sqlite3_prepare_v3(db, sql, -1, prep_flags, &stmt, &tail);
#else
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, &tail);
#endif
        (void)tail;

        if (rc == SQLITE_OK || stmt != NULL) {
            exercise_stmt(db, stmt, p, remaining);
        }

        if (stmt) {
            sqlite3_finalize(stmt);
        }
    }

    if (remaining > 0) {
        int sql_len = consume_i32(&p, &remaining);
        if (sql_len < 0) sql_len = -sql_len;
        if ((size_t)sql_len > remaining) sql_len = (int)remaining;

        char sqlbuf[512];
        size_t copy_len = (size_t)sql_len;
        if (copy_len >= sizeof(sqlbuf)) copy_len = sizeof(sqlbuf) - 1;
        memcpy(sqlbuf, p, copy_len);
        sqlbuf[copy_len] = '\0';

        sqlite3_stmt *stmt = NULL;
        sqlite3_prepare_v2(db, sqlbuf, (int)copy_len, &stmt, NULL);
        if (stmt) {
            exercise_stmt(db, stmt, p, remaining);
            sqlite3_finalize(stmt);
        }
    }

    sqlite3_close(db);
    return 0;
}