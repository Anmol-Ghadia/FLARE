// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput_28(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    const char *tail = NULL;
    int rc;

    write_dummy_file(Data, Size);

    rc = sqlite3_open("./dummy_file", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db,
                 "CREATE TABLE IF NOT EXISTS t("
                 "id INTEGER PRIMARY KEY, "
                 "v TEXT UNIQUE"
                 ");",
                 NULL, NULL, NULL);
    sqlite3_exec(db,
                 "INSERT OR IGNORE INTO t(v) VALUES('seed');",
                 NULL, NULL, NULL);

    {
        const char *sqls[] = {
            "INSERT INTO t(v) VALUES(?1);",
            "UPDATE t SET v=?1 WHERE id=1;",
            "DELETE FROM t WHERE v=?1;",
            "SELECT ?1;",
            "INSERT INTO t(id,v) VALUES(1,?1);"
        };
        size_t choice = 0;
        int step_rc;
        int reset_rc;
        int changes_val;
        const char *errmsg_ptr;
        char *buf = NULL;
        int bind_len = 0;

        if (Size > 0) {
            choice = Data[0] % (sizeof(sqls) / sizeof(sqls[0]));
        }

        rc = sqlite3_prepare_v2(db, sqls[choice], -1, &stmt, &tail);
        if (rc == SQLITE_OK && stmt != NULL) {
            reset_rc = sqlite3_reset(stmt);
            (void)reset_rc;

            if (Size > 2) {
                bind_len = (int)(Size - 2);
                buf = (char *)malloc((size_t)bind_len + 1);
                if (buf != NULL) {
                    memcpy(buf, Data + 2, (size_t)bind_len);
                    buf[bind_len] = '\0';
                } else {
                    bind_len = 0;
                }
            } else {
                buf = (char *)malloc(1);
                if (buf != NULL) {
                    buf[0] = '\0';
                }
                bind_len = 0;
            }

            rc = sqlite3_bind_text(
                stmt,
                1,
                buf ? buf : "",
                bind_len,
                free
            );
            (void)rc;

            step_rc = sqlite3_step(stmt);
            (void)step_rc;

            errmsg_ptr = sqlite3_errmsg(db);
            if (errmsg_ptr) {
                volatile char c = errmsg_ptr[0];
                (void)c;
            }

            reset_rc = sqlite3_reset(stmt);
            (void)reset_rc;

            changes_val = sqlite3_changes(db);
            (void)changes_val;
        } else {
            errmsg_ptr = sqlite3_errmsg(db);
            if (errmsg_ptr) {
                volatile char c = errmsg_ptr[0];
                (void)c;
            }
            changes_val = sqlite3_changes(db);
            (void)changes_val;
        }
    }

    if (stmt) {
        sqlite3_finalize(stmt);
    }
    sqlite3_close(db);
    return 0;
}