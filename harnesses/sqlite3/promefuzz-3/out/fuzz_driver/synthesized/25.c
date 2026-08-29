// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_str_new at sqlite3.c:17391:25 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_appendf at sqlite3.c:17580:17 in sqlite3.h
// sqlite3_str_finish at sqlite3.c:17325:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

int LLVMFuzzerTestOneInput_25(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    const char *tail = NULL;
    int rc;
    char *mstr = NULL;
    sqlite3_str *sacc = NULL;

    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return 0;
    }

    rc = sqlite3_exec(
        db,
        "CREATE TABLE t(a TEXT, b INTEGER);"
        "INSERT INTO t(a,b) VALUES('alpha',1),('beta',2),('gamma',3);",
        NULL, NULL, NULL
    );
    (void)rc;

    {
        size_t sql_len = Size;
        char sqlbuf[4096];
        if (sql_len >= sizeof(sqlbuf)) {
            sql_len = sizeof(sqlbuf) - 1;
        }
        memcpy(sqlbuf, Data, sql_len);
        sqlbuf[sql_len] = '\0';

        rc = sqlite3_prepare_v2(db, sqlbuf, (int)sql_len, &stmt, &tail);
        if (stmt != NULL) {
            int step_rc = sqlite3_step(stmt);

            if (step_rc == SQLITE_ROW) {
                const unsigned char *coltxt = sqlite3_column_text(stmt, 0);
                mstr = sqlite3_mprintf("rc=%d tail=%s col=%s",
                                       step_rc,
                                       tail ? tail : "",
                                       coltxt ? (const char *)coltxt : "(null)");
            } else {
                mstr = sqlite3_mprintf("rc=%d tail=%s err=%s",
                                       step_rc,
                                       tail ? tail : "",
                                       sqlite3_errmsg(db));
            }

            sacc = sqlite3_str_new(db);
            if (sacc != NULL) {
                sqlite3_str_appendf(sacc, "sql=%s;", sqlbuf);
                sqlite3_str_appendf(sacc, "msg=%s;", mstr ? mstr : "(null)");
                sqlite3_str_finish(sacc);
                sacc = NULL;
            }

            sqlite3_free(mstr);
            mstr = NULL;

            sqlite3_finalize(stmt);
            stmt = NULL;
        }
    }

    sqlite3_close(db);
    return 0;
}