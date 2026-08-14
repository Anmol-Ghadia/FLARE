// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
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

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *off) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4 && *off < size; ++i, ++(*off)) {
        v = (v << 8) | data[*off];
    }
    return v;
}

static int bounded_int(uint32_t v, int limit) {
    if (limit <= 0) return 0;
    return (int)(v % (uint32_t)limit);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    int rc;
    size_t off = 0;
    int busy_ms;
    int sleep_ms;
    int step_count;
    int col_index;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open("./dummy_file", &db);
    if (db == NULL) {
        return 0;
    }

    busy_ms = bounded_int(read_u32(Data, Size, &off), 1000) - 250;
    sqlite3_busy_timeout(db, busy_ms);

    {
        static const char *queries[] = {
            "SELECT 1;",
            "SELECT 42, -1, 0;",
            "SELECT length('abc'), abs(-7);",
            "SELECT sqlite_version();",
            "PRAGMA schema_version;",
            "SELECT 1 WHERE 0;",
            "SELECT x FROM no_such_table;",
            "CREATE TABLE IF NOT EXISTS t(a);",
            "INSERT INTO t(a) VALUES(1);",
            "SELECT a FROM t;",
            "SELECT * FROM sqlite_master;"
        };
        const char *sql = queries[bounded_int(read_u32(Data, Size, &off),
                                             (int)(sizeof(queries) / sizeof(queries[0])))];
        sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    }

    if (stmt != NULL) {
        step_count = 1 + bounded_int(read_u32(Data, Size, &off), 4);
        col_index = bounded_int(read_u32(Data, Size, &off), 8) - 2;

        for (int i = 0; i < step_count; ++i) {
            rc = sqlite3_step(stmt);
            if (rc == SQLITE_ROW) {
                (void)sqlite3_column_int(stmt, col_index);
            } else if (rc == SQLITE_DONE || rc == SQLITE_BUSY || rc == SQLITE_ERROR) {
                break;
            } else {
                break;
            }
        }
    }

    sqlite3_finalize(stmt);
    stmt = NULL;

    sleep_ms = bounded_int(read_u32(Data, Size, &off), 20);
    sqlite3_sleep(sleep_ms);

    sqlite3_close(db);
    return 0;
}