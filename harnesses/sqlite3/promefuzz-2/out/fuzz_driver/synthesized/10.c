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

static int consume_u32(const uint8_t **data, size_t *size) {
    int v = 0;
    size_t n = *size < 4 ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static int consume_int_range(const uint8_t **data, size_t *size, int min, int max) {
    if (min >= max) return min;
    int span = max - min + 1;
    int v = consume_u32(data, size);
    if (v < 0) v = -v;
    return min + (v % span);
}

int LLVMFuzzerTestOneInput_10(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    FILE *fp = NULL;
    int rc;
    int busy_ms;
    int sleep_ms;
    int col;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
        fp = NULL;
    }

    rc = sqlite3_open("./dummy_file", &db);
    if (db == NULL) {
        return 0;
    }

    busy_ms = consume_int_range(&Data, &Size, -1000, 5000);
    (void)sqlite3_busy_timeout(db, busy_ms);

    {
        const char *sql1 = "SELECT 1;";
        const char *sql2 = "SELECT CAST('123' AS INTEGER);";
        const char *sql3 = "SELECT length('abc');";
        const char *sql4 = "SELECT abs(-42);";
        const char *sql = sql1;

        if (Size > 0) {
            switch (Data[0] % 4) {
                case 0: sql = sql1; break;
                case 1: sql = sql2; break;
                case 2: sql = sql3; break;
                default: sql = sql4; break;
            }
            Data++;
            Size--;
        }

        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc == SQLITE_OK && stmt != NULL) {
            rc = sqlite3_step(stmt);
            if (rc == SQLITE_ROW) {
                col = consume_int_range(&Data, &Size, -4, 8);
                (void)sqlite3_column_int(stmt, col);
            }
            (void)sqlite3_finalize(stmt);
            stmt = NULL;
        }
    }

    sleep_ms = consume_int_range(&Data, &Size, 0, 20);
    (void)sqlite3_sleep(sleep_ms);

    (void)sqlite3_close(db);
    return 0;
}