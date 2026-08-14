#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static void agg_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
    int n = 0;
    void *p;

    if (argc > 0 && argv && argv[0]) {
        n = sqlite3_value_bytes(argv[0]);
    }

    if (n < 0) n = 0;
    if (n > 4096) n = 4096;

    p = sqlite3_aggregate_context(ctx, n);
    (void)p;

    if (argc > 0 && argv && argv[0]) {
        (void)sqlite3_value_bytes(argv[0]);
        (void)sqlite3_value_text(argv[0]);
    }
}

static void agg_final(sqlite3_context *ctx) {
    void *p = sqlite3_aggregate_context(ctx, 0);
    (void)p;
    sqlite3_result_null(ctx);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;
    int rc;
    size_t sql_len;
    char *sql = NULL;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_create_function(db, "fuzzagg", 1, SQLITE_UTF8, NULL, NULL, agg_step, agg_final);

    sqlite3_exec(db, "CREATE TABLE t(x);", NULL, NULL, NULL);

    rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS tf(y);", NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    rc = sqlite3_prepare_v2(db, "INSERT INTO t(x) VALUES(?1);", -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
        sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
        sqlite3_step(stmt);
    }
    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    rc = sqlite3_prepare_v2(db, "INSERT INTO t(x) VALUES(CAST(?1 AS TEXT));", -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
        sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_STATIC);
        sqlite3_step(stmt);
    }
    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    rc = sqlite3_prepare_v2(db, "INSERT INTO t(x) VALUES(?1);", -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
        sqlite3_int64 v = 0;
        size_t copy = Size < sizeof(v) ? Size : sizeof(v);
        memcpy(&v, Data, copy);
        sqlite3_bind_int64(stmt, 1, v);
        sqlite3_step(stmt);
    }
    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    sqlite3_exec(db, "SELECT fuzzagg(x) FROM t;", NULL, NULL, NULL);
    sqlite3_exec(db, "SELECT fuzzagg(CAST(x AS TEXT)) FROM t;", NULL, NULL, NULL);
    sqlite3_exec(db, "SELECT fuzzagg(CAST(length(x) AS TEXT)) FROM t;", NULL, NULL, NULL);
    sqlite3_exec(db, "SELECT fuzzagg(x) FROM t WHERE 0;", NULL, NULL, NULL);

    sql_len = Size + 64;
    sql = (char *)malloc(sql_len);
    if (sql) {
        size_t copy = Size;
        if (copy > sql_len - 32) copy = sql_len - 32;
        memcpy(sql, "SELECT fuzzagg('", 16);
        memcpy(sql + 16, Data, copy);
        memcpy(sql + 16 + copy, "')", 2);
        sql[18 + copy] = '\0';
        sqlite3_exec(db, sql, NULL, NULL, NULL);
        free(sql);
    }

    sqlite3_close(db);
    return 0;
}