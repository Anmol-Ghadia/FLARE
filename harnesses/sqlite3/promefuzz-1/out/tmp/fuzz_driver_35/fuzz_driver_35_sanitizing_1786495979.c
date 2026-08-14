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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *dyn = NULL;
    const char *errmsg = NULL;
    int rc;
    int errcode_val;
    int exterr_val;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open("./dummy_file", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) {
            sqlite3_close(db);
        }
        return 0;
    }

    if (Size > 0) {
        char *sql = sqlite3_mprintf("%.*s", (int)Size, (const char *)Data);
        if (sql) {
            rc = sqlite3_exec(db, sql, NULL, NULL, NULL);
            sqlite3_free(sql);
        }
    } else {
        rc = sqlite3_exec(db, "SELECT * FROM no_such_table;", NULL, NULL, NULL);
    }

    if (rc == SQLITE_OK) {
        rc = sqlite3_prepare_v2(db, "SELECT * FROM no_such_table;", -1, &stmt, NULL);
        if (stmt) {
            sqlite3_finalize(stmt);
            stmt = NULL;
        }
    }

    errcode_val = sqlite3_errcode(db);
    (void)errcode_val;

    dyn = sqlite3_mprintf("rc=%d size=%d data='%.*q'", rc, (int)Size, (int)Size, (const char *)Data);

    errmsg = sqlite3_errmsg(db);
    (void)errmsg;

    exterr_val = sqlite3_extended_errcode(db);
    (void)exterr_val;

    if (dyn) {
        sqlite3_free(dyn);
    }

    sqlite3_close(db);
    return 0;
}