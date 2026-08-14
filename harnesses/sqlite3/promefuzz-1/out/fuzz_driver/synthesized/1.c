// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
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
#include <sqlite3.h>

int LLVMFuzzerTestOneInput_1(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    const char *tail = NULL;

    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        if (db) {
            sqlite3_close(db);
        }
        return 0;
    }

    sqlite3_exec(db,
                 "CREATE TABLE t(a,b,c);"
                 "INSERT INTO t VALUES(1,'x',NULL);"
                 "INSERT INTO t VALUES(2,'y','z');",
                 NULL, NULL, NULL);

    char *sql = (char *)malloc(Size + 1);
    if (!sql) {
        sqlite3_close(db);
        return 0;
    }
    memcpy(sql, Data, Size);
    sql[Size] = '\0';

    (void)sqlite3_prepare_v2(db, sql, (int)Size, &stmt, &tail);
    (void)sqlite3_errmsg(db);

    if (stmt) {
        int rc1 = sqlite3_step(stmt);
        int rc2 = sqlite3_step(stmt);
        (void)rc1;
        (void)rc2;

        int col_count = sqlite3_column_count(stmt);
        int col = 0;
        if (col_count > 0) {
            if (Size > 0) {
                col = (int)(Data[0] % (size_t)col_count);
            }
        }

        (void)sqlite3_column_count(stmt);
        (void)sqlite3_column_type(stmt, col);
        (void)sqlite3_column_name(stmt, col);
        (void)sqlite3_column_text(stmt, col);
        (void)sqlite3_column_text(stmt, col);
        (void)sqlite3_column_text(stmt, col);
        (void)sqlite3_column_bytes(stmt, col);
        (void)sqlite3_finalize(stmt);
    }

    free(sql);
    sqlite3_close(db);
    return 0;
}