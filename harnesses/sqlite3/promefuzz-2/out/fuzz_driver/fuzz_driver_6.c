// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
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
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt_select = NULL;
    sqlite3_stmt *stmt_insert = NULL;
    sqlite3_stmt *stmt_extra = NULL;
    char buf1[128];
    char buf2[128];
    int rc;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Data && Size) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db, "CREATE TABLE t(a TEXT);", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO t(a) VALUES('seed');", NULL, NULL, NULL);

    if (sqlite3_prepare_v2(db, "SELECT a FROM t;", -1, &stmt_select, NULL) == SQLITE_OK) {
        rc = sqlite3_step(stmt_select);
        if (rc == SQLITE_ROW) {
            const unsigned char *txt = sqlite3_column_text(stmt_select, 0);
            sqlite3_snprintf((int)sizeof(buf1), buf1, "%s",
                             txt ? (const char *)txt : "");
            sqlite3_snprintf((int)sizeof(buf2), buf2, "%.*s",
                             (int)((Size < 64) ? Size : 64),
                             Data ? (const char *)Data : "");
        } else {
            sqlite3_snprintf((int)sizeof(buf1), buf1, "%s", "");
            sqlite3_snprintf((int)sizeof(buf2), buf2, "%s", "");
        }
    } else {
        sqlite3_snprintf((int)sizeof(buf1), buf1, "%s", "");
        sqlite3_snprintf((int)sizeof(buf2), buf2, "%s", "");
    }

    if (sqlite3_prepare_v2(db, "INSERT INTO t(a) VALUES(?1);", -1, &stmt_insert, NULL) == SQLITE_OK) {
        const char *bind_ptr = buf1;
        if (Size > 0 && (Data[0] & 1)) {
            bind_ptr = buf2;
        }

        sqlite3_bind_text(stmt_insert, 1, bind_ptr, -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt_insert);
        sqlite3_reset(stmt_insert);
        (void)sqlite3_sql(stmt_insert);
    }

    if (Size > 1) {
        const char *sql = "SELECT 1;";
        sqlite3_prepare_v2(db, sql, -1, &stmt_extra, NULL);
    }

    sqlite3_finalize(stmt_select);
    sqlite3_finalize(stmt_insert);
    sqlite3_finalize(stmt_extra);

    sqlite3_close(db);
    return 0;
}