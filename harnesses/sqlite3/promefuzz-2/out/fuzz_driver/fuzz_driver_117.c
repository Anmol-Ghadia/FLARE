// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_unlock_notify at sqlite3.c:157112:16 in sqlite3.h
// sqlite3_unlock_notify at sqlite3.c:157112:16 in sqlite3.h
// sqlite3_unlock_notify at sqlite3.c:157112:16 in sqlite3.h
// sqlite3_unlock_notify at sqlite3.c:157112:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_unlock_notify at sqlite3.c:157112:16 in sqlite3.h
// sqlite3_unlock_notify at sqlite3.c:157112:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_unlock_notify at sqlite3.c:157112:16 in sqlite3.h
// sqlite3_unlock_notify at sqlite3.c:157112:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
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

static int read_i32(const uint8_t **p, size_t *n) {
    return (int)read_u32(p, n);
}

static void exec_sql(sqlite3 *db, const char *sql) {
    char *errmsg = NULL;
    sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
    }
}

static void unlock_notify_cb(void **apArg, int nArg) {
    (void)apArg;
    (void)nArg;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t n = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    sqlite3 *db1 = NULL;
    sqlite3 *db2 = NULL;
    sqlite3_stmt *stmt1 = NULL;
    sqlite3_stmt *stmt2 = NULL;
    int rc1, rc2;

    rc1 = sqlite3_open("./dummy_file", &db1);
    if (db1) {
        sqlite3_busy_timeout(db1, read_i32(&p, &n));
        sqlite3_errcode(db1);
        sqlite3_extended_errcode(db1);
    }

    rc2 = sqlite3_open("./dummy_file", &db2);
    if (db2) {
        sqlite3_busy_timeout(db2, read_i32(&p, &n));
        sqlite3_errcode(db2);
        sqlite3_extended_errcode(db2);
    }

    if (!db1) {
        if (db2) sqlite3_close(db2);
        return 0;
    }

    exec_sql(db1, "PRAGMA journal_mode=DELETE;");
    exec_sql(db1, "PRAGMA synchronous=OFF;");
    exec_sql(db1, "CREATE TABLE IF NOT EXISTS t(a BLOB, b TEXT);");
    exec_sql(db1, "CREATE TABLE IF NOT EXISTS t2(x INTEGER);");

    if (db2) {
        exec_sql(db2, "PRAGMA journal_mode=DELETE;");
        exec_sql(db2, "PRAGMA synchronous=OFF;");
        exec_sql(db2, "CREATE TABLE IF NOT EXISTS t(a BLOB, b TEXT);");
        exec_sql(db2, "CREATE TABLE IF NOT EXISTS t2(x INTEGER);");
    }

    {
        const char *insert_sql = "INSERT INTO t(a,b) VALUES(?1,?2);";
        if (sqlite3_prepare_v2(db1, insert_sql, -1, &stmt1, NULL) == SQLITE_OK) {
            sqlite3_bind_blob(stmt1, 1, Data, (int)Size, SQLITE_STATIC);
            sqlite3_bind_text(stmt1, 2, (const char *)Data, (int)Size, SQLITE_STATIC);
            sqlite3_step(stmt1);
            sqlite3_reset(stmt1);
        }
    }

    exec_sql(db1, "BEGIN IMMEDIATE;");
    if (stmt1) {
        sqlite3_bind_blob(stmt1, 1, Data, (int)Size, SQLITE_STATIC);
        sqlite3_bind_text(stmt1, 2, (const char *)Data, (int)Size, SQLITE_STATIC);
        sqlite3_step(stmt1);
        sqlite3_reset(stmt1);
    }

    sqlite3_db_cacheflush(db1);
    sqlite3_errcode(db1);
    sqlite3_extended_errcode(db1);

    if (db2) {
        exec_sql(db2, "BEGIN;");
        if (sqlite3_prepare_v2(db2, "SELECT * FROM t;", -1, &stmt2, NULL) == SQLITE_OK) {
            sqlite3_step(stmt2);
        }

        if (sqlite3_prepare_v2(db2, "INSERT INTO t2(x) VALUES(1);", -1, &stmt2, NULL) == SQLITE_OK) {
            int step_rc = sqlite3_step(stmt2);
            if (step_rc == SQLITE_LOCKED || step_rc == SQLITE_BUSY) {
                sqlite3_unlock_notify(db2, unlock_notify_cb, db1);
                sqlite3_unlock_notify(db2, NULL, NULL);
            } else {
                sqlite3_unlock_notify(db2, unlock_notify_cb, db1);
                sqlite3_unlock_notify(db2, NULL, NULL);
            }
            sqlite3_errcode(db2);
            sqlite3_extended_errcode(db2);
        } else {
            sqlite3_unlock_notify(db2, unlock_notify_cb, db1);
            sqlite3_unlock_notify(db2, NULL, NULL);
            sqlite3_errcode(db2);
            sqlite3_extended_errcode(db2);
        }

        sqlite3_db_cacheflush(db2);
        sqlite3_errcode(db2);
        sqlite3_extended_errcode(db2);
    }

    for (int i = 0; i < 3 && n > 0; ++i) {
        int choice = *p++;
        n--;

        switch (choice % 6) {
            case 0:
                sqlite3_busy_timeout(db1, read_i32(&p, &n));
                sqlite3_db_cacheflush(db1);
                break;
            case 1:
                if (db2) {
                    sqlite3_busy_timeout(db2, read_i32(&p, &n));
                    sqlite3_db_cacheflush(db2);
                }
                break;
            case 2:
                exec_sql(db1, "SAVEPOINT s1;");
                sqlite3_db_cacheflush(db1);
                exec_sql(db1, "RELEASE s1;");
                break;
            case 3:
                if (db2) {
                    sqlite3_unlock_notify(db2, unlock_notify_cb, db1);
                    sqlite3_unlock_notify(db2, NULL, NULL);
                }
                break;
            case 4:
                sqlite3_errcode(db1);
                sqlite3_extended_errcode(db1);
                if (db2) {
                    sqlite3_errcode(db2);
                    sqlite3_extended_errcode(db2);
                }
                break;
            case 5:
                exec_sql(db1, "INSERT INTO t2(x) VALUES(42);");
                sqlite3_db_cacheflush(db1);
                break;
        }
    }

    if (stmt2) sqlite3_finalize(stmt2);
    if (stmt1) sqlite3_finalize(stmt1);

    exec_sql(db1, "ROLLBACK;");
    if (db2) exec_sql(db2, "ROLLBACK;");

    sqlite3_close(db2);
    sqlite3_close(db1);
    (void)rc1;
    (void)rc2;
    return 0;
}