// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_stmt_readonly at sqlite3.c:73571:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_stmt_isexplain at sqlite3.c:73579:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_stmt_readonly at sqlite3.c:73571:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_stmt_isexplain at sqlite3.c:73579:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_stmt_readonly at sqlite3.c:73571:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_stmt_isexplain at sqlite3.c:73579:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_transfer_bindings at sqlite3.c:73539:16 in sqlite3.h
// sqlite3_transfer_bindings at sqlite3.c:73539:16 in sqlite3.h
// sqlite3_transfer_bindings at sqlite3.c:73539:16 in sqlite3.h
// sqlite3_stmt_readonly at sqlite3.c:73571:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_stmt_isexplain at sqlite3.c:73579:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_stmt_readonly at sqlite3.c:73571:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_stmt_isexplain at sqlite3.c:73579:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_stmt_readonly at sqlite3.c:73571:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_stmt_isexplain at sqlite3.c:73579:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
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

static int consume_u8(const uint8_t **data, size_t *size) {
    if (*size == 0) return 0;
    int v = **data;
    (*data)++;
    (*size)--;
    return v;
}

static int consume_u32(const uint8_t **data, size_t *size) {
    uint32_t v = 0;
    for (int i = 0; i < 4; i++) {
        v = (v << 8) | (uint32_t)consume_u8(data, size);
    }
    return (int)v;
}

static void write_dummy_file(const uint8_t *data, size_t size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (size > 0) {
        fwrite(data, 1, size, fp);
    }
    fclose(fp);
}

static sqlite3_stmt *prepare_sql(sqlite3 *db, const char *sql) {
    sqlite3_stmt *stmt = NULL;
    if (!db || !sql) return NULL;
    (void)sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    return stmt;
}

static void maybe_bind(sqlite3_stmt *stmt, int seed) {
    if (!stmt) return;
    int n = sqlite3_bind_parameter_count(stmt);
    for (int i = 1; i <= n; i++) {
        switch ((seed + i) % 4) {
            case 0:
                (void)sqlite3_bind_int(stmt, i, seed * i);
                break;
            case 1:
                (void)sqlite3_bind_int64(stmt, i, ((sqlite3_int64)seed << 32) ^ i);
                break;
            case 2:
                (void)sqlite3_bind_text(stmt, i, "fuzz", -1, SQLITE_STATIC);
                break;
            default:
                (void)sqlite3_bind_null(stmt, i);
                break;
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t remaining = Size;

    write_dummy_file(Data, Size);

    sqlite3 *db = NULL;
    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return 0;
    }

    (void)sqlite3_exec(db,
                       "CREATE TABLE IF NOT EXISTS t(a INTEGER, b TEXT);"
                       "INSERT INTO t(a,b) VALUES(1,'x');"
                       "INSERT INTO t(a,b) VALUES(2,'y');",
                       NULL, NULL, NULL);

    sqlite3_stmt *stmt1 = NULL;
    sqlite3_stmt *stmt2 = NULL;
    sqlite3_stmt *stmt3 = NULL;

    static const char *sqls[] = {
        "SELECT * FROM t WHERE a=?1;",
        "UPDATE t SET b=?1 WHERE a=?2;",
        "INSERT INTO t(a,b) VALUES(?1,?2);",
        "DELETE FROM t WHERE a=?1;",
        "BEGIN;",
        "BEGIN IMMEDIATE;",
        "COMMIT;",
        "ROLLBACK;",
        "ATTACH './dummy_file' AS aux;",
        "DETACH aux;",
        "EXPLAIN SELECT * FROM t WHERE a=?1;",
        "EXPLAIN QUERY PLAN SELECT * FROM t WHERE a=?1;",
        "CREATE TABLE IF NOT EXISTS u(x);",
        "SELECT 1;",
        "PRAGMA schema_version;"
    };

    int idx1 = consume_u8(&p, &remaining) % (int)(sizeof(sqls) / sizeof(sqls[0]));
    int idx2 = consume_u8(&p, &remaining) % (int)(sizeof(sqls) / sizeof(sqls[0]));
    int idx3 = consume_u8(&p, &remaining) % (int)(sizeof(sqls) / sizeof(sqls[0]));

    stmt1 = prepare_sql(db, sqls[idx1]);
    stmt2 = prepare_sql(db, sqls[idx2]);
    stmt3 = prepare_sql(db, sqls[idx3]);

    maybe_bind(stmt1, consume_u32(&p, &remaining));
    maybe_bind(stmt2, consume_u32(&p, &remaining));
    maybe_bind(stmt3, consume_u32(&p, &remaining));

    if (stmt1) {
        (void)sqlite3_stmt_readonly(stmt1);
        (void)sqlite3_expired(stmt1);
        (void)sqlite3_stmt_isexplain(stmt1);
        (void)sqlite3_stmt_busy(stmt1);
    }
    if (stmt2) {
        (void)sqlite3_stmt_readonly(stmt2);
        (void)sqlite3_expired(stmt2);
        (void)sqlite3_stmt_isexplain(stmt2);
        (void)sqlite3_stmt_busy(stmt2);
    }
    if (stmt3) {
        (void)sqlite3_stmt_readonly(stmt3);
        (void)sqlite3_expired(stmt3);
        (void)sqlite3_stmt_isexplain(stmt3);
        (void)sqlite3_stmt_busy(stmt3);
    }

    int steps1 = consume_u8(&p, &remaining) % 4;
    int steps2 = consume_u8(&p, &remaining) % 4;
    int steps3 = consume_u8(&p, &remaining) % 4;

    for (int i = 0; i < steps1 && stmt1; i++) {
        int rc = sqlite3_step(stmt1);
        (void)rc;
        (void)sqlite3_stmt_busy(stmt1);
        (void)sqlite3_expired(stmt1);
        if (consume_u8(&p, &remaining) & 1) {
            (void)sqlite3_reset(stmt1);
        }
    }

    for (int i = 0; i < steps2 && stmt2; i++) {
        int rc = sqlite3_step(stmt2);
        (void)rc;
        (void)sqlite3_stmt_busy(stmt2);
        (void)sqlite3_expired(stmt2);
        if (consume_u8(&p, &remaining) & 1) {
            (void)sqlite3_reset(stmt2);
        }
    }

    for (int i = 0; i < steps3 && stmt3; i++) {
        int rc = sqlite3_step(stmt3);
        (void)rc;
        (void)sqlite3_stmt_busy(stmt3);
        (void)sqlite3_expired(stmt3);
        if (consume_u8(&p, &remaining) & 1) {
            (void)sqlite3_reset(stmt3);
        }
    }

    if ((consume_u8(&p, &remaining) & 1) && stmt1 && stmt2) {
        (void)sqlite3_transfer_bindings(stmt1, stmt2);
    }
    if ((consume_u8(&p, &remaining) & 1) && stmt2 && stmt3) {
        (void)sqlite3_transfer_bindings(stmt2, stmt3);
    }
    if ((consume_u8(&p, &remaining) & 1) && stmt1 && stmt3) {
        (void)sqlite3_transfer_bindings(stmt1, stmt3);
    }

    if (stmt1) {
        (void)sqlite3_stmt_readonly(stmt1);
        (void)sqlite3_expired(stmt1);
        (void)sqlite3_stmt_isexplain(stmt1);
        (void)sqlite3_stmt_busy(stmt1);
    }
    if (stmt2) {
        (void)sqlite3_stmt_readonly(stmt2);
        (void)sqlite3_expired(stmt2);
        (void)sqlite3_stmt_isexplain(stmt2);
        (void)sqlite3_stmt_busy(stmt2);
    }
    if (stmt3) {
        (void)sqlite3_stmt_readonly(stmt3);
        (void)sqlite3_expired(stmt3);
        (void)sqlite3_stmt_isexplain(stmt3);
        (void)sqlite3_stmt_busy(stmt3);
    }

    if (consume_u8(&p, &remaining) & 1) {
        int rc = sqlite3_finalize(stmt1);
        (void)rc;
        stmt1 = NULL;
    }
    if (consume_u8(&p, &remaining) & 1) {
        int rc = sqlite3_finalize(stmt2);
        (void)rc;
        stmt2 = NULL;
    }
    if (consume_u8(&p, &remaining) & 1) {
        int rc = sqlite3_finalize(stmt3);
        (void)rc;
        stmt3 = NULL;
    }

    (void)sqlite3_finalize(NULL);

    if (stmt1) {
        int rc = sqlite3_finalize(stmt1);
        (void)rc;
        stmt1 = NULL;
    }
    if (stmt2) {
        int rc = sqlite3_finalize(stmt2);
        (void)rc;
        stmt2 = NULL;
    }
    if (stmt3) {
        int rc = sqlite3_finalize(stmt3);
        (void)rc;
        stmt3 = NULL;
    }

    sqlite3_close(db);
    return 0;
}