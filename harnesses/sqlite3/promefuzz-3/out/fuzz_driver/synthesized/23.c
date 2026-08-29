// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_libversion at sqlite3.c:152357:24 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput_23(const uint8_t *Data, size_t Size) {
    sqlite3_blob *blob = NULL;
    sqlite3 *db1 = NULL;
    sqlite3 *db2 = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;
    const char *tail = NULL;
    const char *version = NULL;
    int rc;

    write_dummy_file(Data, Size);

    rc = sqlite3_blob_close(blob);
    (void)rc;

    rc = sqlite3_open(":memory:", &db1);
    if (db1 != NULL) {
        size_t sql_len = Size > 4096 ? 4096 : Size;
        char *sql = (char *)malloc(sql_len + 1);
        if (sql != NULL) {
            if (sql_len > 0) {
                memcpy(sql, Data, sql_len);
            }
            sql[sql_len] = '\0';
            rc = sqlite3_exec(db1, sql, NULL, NULL, &errmsg);
            (void)rc;
            free(sql);
        } else {
            rc = sqlite3_exec(db1, "", NULL, NULL, &errmsg);
            (void)rc;
        }
        if (errmsg != NULL) {
            sqlite3_free(errmsg);
            errmsg = NULL;
        }

        rc = sqlite3_close(db1);
        (void)rc;
        if (rc == SQLITE_OK) {
            db1 = NULL;
        }
    }

    if (db1 != NULL) {
        rc = sqlite3_close(db1);
        (void)rc;
        if (rc == SQLITE_OK) {
            db1 = NULL;
        }
    }

    version = sqlite3_libversion();
    (void)version;

    rc = sqlite3_open("./dummy_file", &db2);
    (void)rc;
    if (db2 != NULL) {
        size_t sql_len = Size > 4096 ? 4096 : Size;
        char *sql = (char *)malloc(sql_len + 1);
        if (sql != NULL) {
            if (sql_len > 0) {
                memcpy(sql, Data, sql_len);
            }
            sql[sql_len] = '\0';

            rc = sqlite3_prepare_v2(db2, sql, (int)sql_len, &stmt, &tail);
            (void)tail;
            if (stmt != NULL) {
                rc = sqlite3_step(stmt);
                if (rc == SQLITE_ROW) {
                    int iCol = (Size > 0) ? (int)(Data[0] % 4) : 0;
                    (void)sqlite3_column_text(stmt, iCol);
                } else {
                    (void)sqlite3_column_text(stmt, 0);
                }
            }
            rc = sqlite3_finalize(stmt);
            (void)rc;
            stmt = NULL;

            free(sql);
        } else {
            rc = sqlite3_prepare_v2(db2, "", 0, &stmt, &tail);
            (void)tail;
            if (stmt != NULL) {
                rc = sqlite3_step(stmt);
                (void)rc;
                (void)sqlite3_column_text(stmt, 0);
            }
            rc = sqlite3_finalize(stmt);
            (void)rc;
            stmt = NULL;
        }

        rc = sqlite3_close(db2);
        (void)rc;
        if (rc == SQLITE_OK) {
            db2 = NULL;
        }
    }

    return 0;
}