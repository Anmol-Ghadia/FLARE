#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Data && Size) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
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
        int bind_len;
        int step_rc;
        int reset_rc;
        int changes_val;
        const char *errmsg_ptr;

        if (Size > 0) {
            choice = Data[0] % (sizeof(sqls) / sizeof(sqls[0]));
        }

        rc = sqlite3_prepare_v2(db, sqls[choice], -1, &stmt, &tail);
        if (rc == SQLITE_OK && stmt != NULL) {
            reset_rc = sqlite3_reset(stmt);
            (void)reset_rc;

            if (Size > 1) {
                if (Data[1] & 1) {
                    bind_len = -1;
                } else {
                    bind_len = (int)(Size > 2 ? (Size - 2) : 0);
                }
            } else {
                bind_len = 0;
            }

            rc = sqlite3_bind_text(
                stmt,
                1,
                (const char *)(Size > 2 ? Data + 2 : (const uint8_t *)""),
                bind_len,
                SQLITE_TRANSIENT
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

            if (step_rc == SQLITE_ROW) {
                while ((step_rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                }
            }
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