// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_realloc at sqlite3.c:15947:18 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <sqlite3.h>

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;
    void *mem = NULL;
    int rc;

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) {
            sqlite3_close(db);
        }
        return 0;
    }

    sqlite3_exec(
        db,
        "CREATE TABLE t(a TEXT, b INTEGER);"
        "INSERT INTO t(a,b) VALUES('x',1),('y',2),('z',3);",
        NULL,
        NULL,
        &errmsg
    );
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    {
        const char *sql_select = "SELECT a,b FROM t WHERE a IS NOT NULL;";
        const char *sql_nonselect = "UPDATE t SET b=b+1 WHERE 0;";
        const char *sql = (Size > 0 && (Data[0] & 1)) ? sql_select : sql_nonselect;

        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK || stmt == NULL) {
            sqlite3_close(db);
            return 0;
        }
    }

    {
        int n = 0;
        if (Size > 1) {
            n = (int)Data[1];
            if (Size > 2 && (Data[2] & 1)) {
                n = -n;
            }
        }

        mem = sqlite3_realloc(NULL, n);

        rc = sqlite3_step(stmt);

        {
            int col_count = sqlite3_column_count(stmt);
            int idx = 0;
            char *formatted;

            if (col_count > 0 && Size > 3) {
                idx = (int)(Data[3] % (uint8_t)col_count);
            }

            formatted = sqlite3_mprintf("rc=%d size=%lld idx=%d first=%d",
                                        rc,
                                        (long long)Size,
                                        idx,
                                        (int)(Size ? Data[0] : 0));

            if (rc == SQLITE_ROW && idx >= 0 && idx < col_count) {
                const unsigned char *txt = sqlite3_column_text(stmt, idx);
                if (txt && formatted) {
                    volatile unsigned char c = txt[0];
                    volatile char d = formatted[0];
                    (void)c;
                    (void)d;
                }
            } else if (col_count > 0) {
                (void)sqlite3_column_text(stmt, 0);
            }

            sqlite3_free(formatted);
        }

        sqlite3_free(mem);
        mem = NULL;

        (void)sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}