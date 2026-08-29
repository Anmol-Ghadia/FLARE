// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_realloc at sqlite3.c:15947:18 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
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
#include <string.h>
#include <sqlite3.h>

static int read_u32(const uint8_t *Data, size_t Size, size_t *Off, uint32_t *Out) {
    if (*Off + 4 > Size) return 0;
    *Out = ((uint32_t)Data[*Off]) |
           ((uint32_t)Data[*Off + 1] << 8) |
           ((uint32_t)Data[*Off + 2] << 16) |
           ((uint32_t)Data[*Off + 3] << 24);
    *Off += 4;
    return 1;
}

int LLVMFuzzerTestOneInput_24(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    void *mem = NULL;
    char *fmt = NULL;
    int step_rc = SQLITE_OK;
    int col_count = 0;
    const unsigned char *txt = NULL;
    size_t off = 0;
    uint32_t v1 = 0, v2 = 0, v3 = 0, v4 = 0;

    sqlite3_initialize();

    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db, "CREATE TABLE t(a TEXT, b INTEGER);", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO t(a,b) VALUES('alpha',1),('beta',2),('gamma',3);", NULL, NULL, NULL);

    if (!read_u32(Data, Size, &off, &v1)) v1 = 16;
    if (!read_u32(Data, Size, &off, &v2)) v2 = 0;
    if (!read_u32(Data, Size, &off, &v3)) v3 = 0;
    if (!read_u32(Data, Size, &off, &v4)) v4 = 0;

    {
        int n = (int)(v1 % 4096);
        mem = sqlite3_malloc(1);
        mem = sqlite3_realloc(mem, n);
    }

    {
        const char *sqls[] = {
            "SELECT a,b FROM t;",
            "SELECT a FROM t WHERE b>=0;",
            "SELECT b FROM t;",
            "UPDATE t SET b=b+1 WHERE b<3;",
            "SELECT a,b FROM t WHERE a LIKE 'a%';",
            "SELECT zeroblob(4);"
        };
        const char *sql = sqls[v2 % (sizeof(sqls) / sizeof(sqls[0]))];
        sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    }

    if (stmt) {
        step_rc = sqlite3_step(stmt);
        col_count = sqlite3_column_count(stmt);

        {
            const uint8_t *tail = (off < Size) ? (Data + off) : (const uint8_t *)"";
            int tail_len = (int)((off < Size) ? (Size - off) : 0);
            fmt = sqlite3_mprintf("rc=%d cols=%d x=%u y=%u data=%.*q",
                                  step_rc, col_count, v3, v4, tail_len, tail);
        }

        if (col_count > 0) {
            int idx = (int)(v3 % (uint32_t)(col_count + 2)) - 1;
            txt = sqlite3_column_text(stmt, idx);
            (void)txt;
        }

        sqlite3_free(fmt);
        fmt = NULL;

        sqlite3_reset(stmt);
        sqlite3_finalize(stmt);
        stmt = NULL;
    } else {
        fmt = sqlite3_mprintf("prepare_failed_%u", v4);
        sqlite3_free(fmt);
        fmt = NULL;
    }

    sqlite3_free(mem);
    mem = NULL;

    sqlite3_close(db);
    return 0;
}