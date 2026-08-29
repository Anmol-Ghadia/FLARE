// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare at sqlite3.c:119055:16 in sqlite3.h
// sqlite3_prepare at sqlite3.c:119055:16 in sqlite3.h
// sqlite3_prepare at sqlite3.c:119055:16 in sqlite3.h
// sqlite3_prepare at sqlite3.c:119055:16 in sqlite3.h
// sqlite3_prepare at sqlite3.c:119055:16 in sqlite3.h
// sqlite3_prepare at sqlite3.c:119055:16 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_backup_init at sqlite3.c:64113:28 in sqlite3.h
// sqlite3_backup_step at sqlite3.c:64293:16 in sqlite3.h
// sqlite3_backup_finish at sqlite3.c:64547:16 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
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

static int consume_u8(const uint8_t **data, size_t *size) {
    if (*size == 0) return 0;
    int v = **data;
    (*data)++;
    (*size)--;
    return v;
}

static int consume_int(const uint8_t **data, size_t *size) {
    int v = 0;
    int i;
    for (i = 0; i < 4; i++) {
        v = (v << 8) | consume_u8(data, size);
    }
    return v;
}

static void exec_sql(sqlite3 *db, const char *sql) {
    char *errmsg = NULL;
    if (db && sql) {
        sqlite3_exec(db, sql, NULL, NULL, &errmsg);
        sqlite3_free(errmsg);
    }
}

int LLVMFuzzerTestOneInput_55(const uint8_t *Data, size_t Size) {
    sqlite3 *db1 = NULL;
    sqlite3 *db2 = NULL;
    sqlite3_stmt *stmts[16];
    int stmt_count = 0;
    int i;

    memset(stmts, 0, sizeof(stmts));

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp) {
            if (Size) fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    if (sqlite3_open(":memory:", &db1) != SQLITE_OK) {
        if (db1) sqlite3_close_v2(db1);
        return 0;
    }
    if (sqlite3_open("./dummy_file", &db2) != SQLITE_OK) {
        if (db2) sqlite3_close_v2(db2);
        sqlite3_close_v2(db1);
        return 0;
    }

    exec_sql(db1, "CREATE TABLE IF NOT EXISTS t(a,b,c);");
    exec_sql(db1, "CREATE TABLE IF NOT EXISTS u(x);");
    exec_sql(db1, "INSERT INTO t VALUES(1,'one',x'00');");
    exec_sql(db1, "INSERT INTO t VALUES(2,'two',x'0102');");
    exec_sql(db1, "INSERT INTO u VALUES(123);");
    exec_sql(db2, "CREATE TABLE IF NOT EXISTS t(a,b,c);");

    while (Size > 0 && stmt_count < 16) {
        int mode = consume_u8(&Data, &Size) % 6;
        int nbyte_mode;
        int nByte;
        sqlite3_stmt *stmt = NULL;
        const char *tail = NULL;
        size_t sql_len;
        char sqlbuf[256];

        if (Size == 0) break;

        sql_len = consume_u8(&Data, &Size);
        if (sql_len > sizeof(sqlbuf) - 1) sql_len = sizeof(sqlbuf) - 1;
        if (sql_len > Size) sql_len = Size;
        memcpy(sqlbuf, Data, sql_len);
        sqlbuf[sql_len] = '\0';
        Data += sql_len;
        Size -= sql_len;

        nbyte_mode = consume_u8(&Data, &Size) % 4;
        switch (nbyte_mode) {
            case 0:
                nByte = -1;
                break;
            case 1:
                nByte = 0;
                break;
            case 2:
                nByte = (int)sql_len;
                break;
            default:
                nByte = (int)sql_len + 1;
                break;
        }

        if (mode == 0) {
            sqlite3_prepare(db1, sqlbuf, nByte, &stmt, &tail);
        } else if (mode == 1) {
            sqlite3_prepare(db2, sqlbuf, nByte, &stmt, &tail);
        } else if (mode == 2) {
            sqlite3_prepare(db1, "SELECT * FROM t; SELECT * FROM u;", -1, &stmt, &tail);
        } else if (mode == 3) {
            sqlite3_prepare(db1, "/*comment*/", -1, &stmt, &tail);
        } else if (mode == 4) {
            sqlite3_prepare(db1, "", 0, &stmt, &tail);
        } else {
            sqlite3_prepare(db1, "INSERT INTO t VALUES(3,'three',x'03');", -1, &stmt, &tail);
        }

        if (stmt) {
            sqlite3 *owner = sqlite3_db_handle(stmt);
            if (owner) {
                int steps = consume_u8(&Data, &Size) % 4;
                int j;
                for (j = 0; j < steps; j++) {
                    int rc;
                    if ((consume_u8(&Data, &Size) & 1) != 0) {
                        sqlite3_interrupt(owner);
                    }
                    rc = sqlite3_step(stmt);
                    if (rc != SQLITE_ROW && rc != SQLITE_DONE && rc != SQLITE_BUSY &&
                        rc != SQLITE_LOCKED && rc != SQLITE_INTERRUPT) {
                        break;
                    }
                    if (rc == SQLITE_DONE) break;
                }
            }
            stmts[stmt_count++] = stmt;
        }

        if ((consume_u8(&Data, &Size) & 1) != 0) {
            sqlite3_stmt *it = NULL;
            int limit = 0;
            while ((it = sqlite3_next_stmt(db1, it)) != NULL && limit < 32) {
                sqlite3_db_handle(it);
                limit++;
            }
            it = NULL;
            limit = 0;
            while ((it = sqlite3_next_stmt(db2, it)) != NULL && limit < 32) {
                sqlite3_db_handle(it);
                limit++;
            }
        }
    }

    {
        sqlite3_backup *bk;
        const char *destName = (consume_u8(&Data, &Size) & 1) ? "main" : "temp";
        const char *srcName = (consume_u8(&Data, &Size) & 1) ? "main" : "temp";
        sqlite3 *src = (consume_u8(&Data, &Size) & 1) ? db1 : db2;
        sqlite3 *dst = (consume_u8(&Data, &Size) & 1) ? db2 : db1;

        if ((consume_u8(&Data, &Size) & 1) != 0) {
            exec_sql(dst, "BEGIN;");
        }

        bk = sqlite3_backup_init(dst, destName, src, srcName);
        if (bk) {
            int loops = (consume_u8(&Data, &Size) % 8) + 1;
            for (i = 0; i < loops; i++) {
                int pages;
                int rc;
                switch (consume_u8(&Data, &Size) % 4) {
                    case 0: pages = -1; break;
                    case 1: pages = 0; break;
                    case 2: pages = 1; break;
                    default: pages = consume_int(&Data, &Size) % 16; break;
                }
                rc = sqlite3_backup_step(bk, pages);
                if (rc == SQLITE_DONE) break;
                if (rc != SQLITE_OK && rc != SQLITE_BUSY && rc != SQLITE_LOCKED) break;
            }
            sqlite3_backup_finish(bk);
        }

        exec_sql(dst, "ROLLBACK;");
    }

    {
        sqlite3_stmt *it = NULL;
        int limit = 0;
        while ((it = sqlite3_next_stmt(db1, it)) != NULL && limit < 64) {
            sqlite3_db_handle(it);
            limit++;
        }
        it = NULL;
        limit = 0;
        while ((it = sqlite3_next_stmt(db2, it)) != NULL && limit < 64) {
            sqlite3_db_handle(it);
            limit++;
        }
    }

    sqlite3_interrupt(db1);
    sqlite3_interrupt(db2);

    for (i = 0; i < stmt_count; i++) {
        if (stmts[i]) {
            sqlite3_finalize(stmts[i]);
            stmts[i] = NULL;
        }
    }

    sqlite3_close_v2(db1);
    sqlite3_close_v2(db2);
    return 0;
}