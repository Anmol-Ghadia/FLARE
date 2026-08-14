// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_int at sqlite3.c:73041:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
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

int LLVMFuzzerTestOneInput_22(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    const char *tail = NULL;
    char *sql1 = NULL;
    char *sql2 = NULL;
    char *input = NULL;
    int rc;

    FILE *fp = fopen("./dummy_file", "wb");
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

    input = (char *)sqlite3_malloc64(Size + 1);
    if (!input) {
        sqlite3_close(db);
        return 0;
    }
    if (Size > 0) {
        memcpy(input, Data, Size);
    }
    input[Size] = '\0';

    sql1 = sqlite3_mprintf("%s", input ? input : "");
    sql2 = sqlite3_mprintf("%s; %s", sql1 ? sql1 : "", input ? input : "");

    rc = sqlite3_prepare_v2(db, sql2 ? sql2 : input, sql2 ? -1 : (int)Size, &stmt, &tail);

    sqlite3_free(sql1);
    sql1 = NULL;

    (void)sqlite3_errmsg(db);

    if (stmt) {
        int step_rc = sqlite3_step(stmt);
        if (step_rc == SQLITE_ROW) {
            int col = 0;
            (void)sqlite3_column_bytes(stmt, col);
            (void)sqlite3_column_int(stmt, col);
            (void)sqlite3_column_blob(stmt, col);
        }
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    sqlite3_free(sql2);
    sqlite3_free(input);
    sqlite3_close(db);
    return 0;
}