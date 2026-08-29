// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

static int make_db_and_stmt(const uint8_t *Data, size_t Size, sqlite3 **db, sqlite3_stmt **stmt) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    *db = NULL;
    *stmt = NULL;

    if (sqlite3_open("./dummy_file", db) != SQLITE_OK || *db == NULL) {
        if (*db != NULL) {
            sqlite3_close(*db);
            *db = NULL;
        }
        return 0;
    }

    sqlite3_exec(*db, "CREATE TABLE IF NOT EXISTS t(x TEXT);", NULL, NULL, NULL);
    sqlite3_exec(*db, "DELETE FROM t;", NULL, NULL, NULL);

    {
        const char *sqls[] = {
            "INSERT INTO t(x) VALUES(?1);",
            "SELECT x FROM t WHERE x=?1;",
            "UPDATE t SET x=?1 WHERE rowid=1;",
            "DELETE FROM t WHERE x=?1;"
        };
        size_t idx = 0;
        if (Size > 0) {
            idx = Data[0] % (sizeof(sqls) / sizeof(sqls[0]));
        }
        if (sqlite3_prepare_v2(*db, sqls[idx], -1, stmt, NULL) != SQLITE_OK || *stmt == NULL) {
            sqlite3_close(*db);
            *db = NULL;
            *stmt = NULL;
            return 0;
        }
    }

    return 1;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    const char *errmsg_ptr;
    int rc;
    int changes_count;
    char *textbuf;

    if (!make_db_and_stmt(Data, Size, &db, &stmt)) {
        return 0;
    }

    textbuf = (char *)malloc(Size + 1);
    if (textbuf == NULL) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 0;
    }

    if (Size > 0) {
        memcpy(textbuf, Data, Size);
    }
    textbuf[Size] = '\0';

    rc = sqlite3_reset(stmt);
    (void)rc;

    rc = sqlite3_bind_text(
        stmt,
        1,
        textbuf,
        -1,
        SQLITE_TRANSIENT
    );

    rc = sqlite3_step(stmt);

    errmsg_ptr = sqlite3_errmsg(db);
    (void)errmsg_ptr;

    rc = sqlite3_reset(stmt);
    (void)rc;

    changes_count = sqlite3_changes(db);
    (void)changes_count;

    if (Size > 1) {
        int mode = Data[1] % 3;
        if (mode == 0) {
            sqlite3_reset(stmt);
            sqlite3_bind_text(stmt, 1, "", -1, SQLITE_STATIC);
            sqlite3_step(stmt);
            sqlite3_errmsg(db);
            sqlite3_reset(stmt);
            sqlite3_changes(db);
        } else if (mode == 1) {
            int bind_index = (Size > 2) ? ((Data[2] % 3) + 1) : 2;
            sqlite3_reset(stmt);
            sqlite3_bind_text(stmt, bind_index, textbuf, (int)(Size / 2), SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_errmsg(db);
            sqlite3_reset(stmt);
            sqlite3_changes(db);
        } else {
            sqlite3_reset(stmt);
            sqlite3_bind_text(stmt, 1, textbuf, 0, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_errmsg(db);
            sqlite3_reset(stmt);
            sqlite3_changes(db);
        }
    }

    free(textbuf);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}