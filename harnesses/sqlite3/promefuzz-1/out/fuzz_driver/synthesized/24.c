// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_realloc at sqlite3.c:15947:18 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static int read_u32(const uint8_t *data, size_t size, size_t *off, uint32_t *out) {
    if (*off + 4 > size) return 0;
    *out = ((uint32_t)data[*off]) |
           ((uint32_t)data[*off + 1] << 8) |
           ((uint32_t)data[*off + 2] << 16) |
           ((uint32_t)data[*off + 3] << 24);
    *off += 4;
    return 1;
}

int LLVMFuzzerTestOneInput_24(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;
    void *mem = NULL;
    char *formatted = NULL;
    int rc;
    size_t off = 0;
    uint32_t v1 = 0, v2 = 0, v3 = 0;
    int realloc_size;
    int col_index = 0;

    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db, "CREATE TABLE t(a,b,c);", NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    sqlite3_exec(db,
                 "INSERT INTO t(a,b,c) VALUES "
                 "(1,'alpha',x'414243'),"
                 "(2,NULL,12345),"
                 "(3,'gamma',3.14);",
                 NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    if (Size > 0) {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp) {
            fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    if (!read_u32(Data, Size, &off, &v1)) v1 = 0;
    if (!read_u32(Data, Size, &off, &v2)) v2 = 0;
    if (!read_u32(Data, Size, &off, &v3)) v3 = 0;

    realloc_size = (int)(v1 % 4096);
    col_index = (int)(v2 % 8) - 2;

    mem = sqlite3_realloc(NULL, realloc_size);

    {
        const char *sqls[] = {
            "SELECT a,b,c FROM t;",
            "SELECT b,a FROM t WHERE a>=1;",
            "SELECT c FROM t;",
            "UPDATE t SET a=a+1 WHERE a=1;",
            "SELECT quote(b), typeof(c), a FROM t;"
        };
        const char *sql = sqls[v3 % (sizeof(sqls) / sizeof(sqls[0]))];
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc == SQLITE_OK && stmt) {
            rc = sqlite3_step(stmt);
            (void)sqlite3_column_count(stmt);
            formatted = sqlite3_mprintf("rc=%d col=%d tail=%.*s",
                                        rc,
                                        col_index,
                                        (int)((Size - off) > 32 ? 32 : (Size - off)),
                                        (const char *)(Data + off));
            (void)sqlite3_column_text(stmt, col_index);
            sqlite3_free(formatted);
            formatted = NULL;
            (void)sqlite3_reset(stmt);

            if ((v3 & 1) == 0) {
                rc = sqlite3_step(stmt);
                (void)sqlite3_column_count(stmt);
                formatted = sqlite3_mprintf("again:%d", rc);
                (void)sqlite3_column_text(stmt, 0);
                sqlite3_free(formatted);
                formatted = NULL;
                (void)sqlite3_reset(stmt);
            }
        }
    }

    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    sqlite3_free(mem);
    sqlite3_close(db);
    return 0;
}