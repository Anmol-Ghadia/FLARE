// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_int at sqlite3.c:73041:16 in sqlite3.h
// sqlite3_column_int at sqlite3.c:73041:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_sleep at sqlite3.c:156048:16 in sqlite3.h
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

static int read_u32(const uint8_t *Data, size_t Size, size_t *off) {
    int v = 0;
    size_t i;
    for (i = 0; i < 4 && *off < Size; ++i, ++(*off)) {
        v = (v << 8) | Data[*off];
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    FILE *fp;
    int rc;
    int step_rc = SQLITE_OK;
    int timeout_ms = 0;
    int sleep_ms = 0;
    int col = 0;
    size_t off = 0;
    char sqlbuf[1024];
    size_t sql_len = 0;
    const char *db_name = "./dummy_file";

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open(db_name, &db);
    if (db == NULL) {
        return 0;
    }

    if (Size > 0) {
        timeout_ms = read_u32(Data, Size, &off) % 1000;
        sleep_ms = read_u32(Data, Size, &off) % 50;
        col = read_u32(Data, Size, &off) % 8;
    }

    sqlite3_busy_timeout(db, timeout_ms);

    rc = sqlite3_exec(db,
                      "CREATE TABLE IF NOT EXISTS t(a INTEGER, b TEXT);"
                      "DELETE FROM t;"
                      "INSERT INTO t(a,b) VALUES(1,'x'),(2,'y'),(3,'z');",
                      NULL, NULL, NULL);

    if (off < Size) {
        sql_len = Size - off;
        if (sql_len >= sizeof(sqlbuf)) {
            sql_len = sizeof(sqlbuf) - 1;
        }
        memcpy(sqlbuf, Data + off, sql_len);
        sqlbuf[sql_len] = '\0';
    } else {
        memcpy(sqlbuf, "SELECT a FROM t;", sizeof("SELECT a FROM t;"));
    }

    rc = sqlite3_prepare_v2(db, sqlbuf, -1, &stmt, NULL);
    if (rc != SQLITE_OK || stmt == NULL) {
        stmt = NULL;
        goto cleanup;
    }

    step_rc = sqlite3_step(stmt);
    if (step_rc == SQLITE_ROW) {
        (void)sqlite3_column_int(stmt, col);
    } else {
        (void)sqlite3_column_int(stmt, col);
    }

cleanup:
    (void)sqlite3_finalize(stmt);
    sqlite3_sleep(sleep_ms);
    (void)sqlite3_close(db);
    return 0;
}