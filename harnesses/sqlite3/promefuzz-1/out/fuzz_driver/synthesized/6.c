// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
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

static int read_u32(const uint8_t *data, size_t size, size_t *off) {
    int v = 0;
    if (*off + 4 <= size) {
        v = (int)(
            ((unsigned int)data[*off]) |
            ((unsigned int)data[*off + 1] << 8) |
            ((unsigned int)data[*off + 2] << 16) |
            ((unsigned int)data[*off + 3] << 24));
        *off += 4;
    } else if (*off < size) {
        v = data[(*off)++];
    }
    return v;
}

int LLVMFuzzerTestOneInput_6(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt_select = NULL;
    sqlite3_stmt *stmt_bind = NULL;
    sqlite3_stmt *stmt_extra = NULL;
    char sqlbuf1[256];
    char sqlbuf2[256];
    char bindbuf[256];
    int rc;
    size_t off = 0;

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

    sqlite3_exec(db, "CREATE TABLE t(a TEXT, b INTEGER);", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO t(a,b) VALUES('seed',1),('x',2),('',3);", NULL, NULL, NULL);

    sqlite3_snprintf((int)sizeof(sqlbuf1), sqlbuf1,
                     "SELECT a FROM t WHERE b >= %d ORDER BY b LIMIT 1;",
                     read_u32(Data, Size, &off));
    sqlite3_snprintf((int)sizeof(sqlbuf2), sqlbuf2,
                     "INSERT INTO t(a,b) VALUES(?1,%d);",
                     read_u32(Data, Size, &off));

    sqlite3_prepare_v2(db, sqlbuf1, -1, &stmt_select, NULL);
    sqlite3_prepare_v2(db, sqlbuf2, -1, &stmt_bind, NULL);
    sqlite3_prepare_v2(db, "SELECT 1;", -1, &stmt_extra, NULL);

    if (stmt_select) {
        rc = sqlite3_step(stmt_select);
        if (rc == SQLITE_ROW) {
            int col = 0;
            if (off < Size) {
                col = Data[off++] % 2;
            }
            (void)sqlite3_column_text(stmt_select, col);
        }
    }

    memset(bindbuf, 0, sizeof(bindbuf));
    if (Size > off) {
        size_t copy_len = Size - off;
        if (copy_len > sizeof(bindbuf) - 1) copy_len = sizeof(bindbuf) - 1;
        memcpy(bindbuf, Data + off, copy_len);
        bindbuf[copy_len] = '\0';
    }

    sqlite3_snprintf((int)sizeof(bindbuf), bindbuf, "%s", bindbuf);
    sqlite3_snprintf((int)sizeof(sqlbuf1), sqlbuf1, "len=%d:%s",
                     (int)strlen(bindbuf), bindbuf);

    if (stmt_bind) {
        int bind_len;
        if (off < Size) {
            bind_len = ((int)Data[off++] % ((int)strlen(sqlbuf1) + 2)) - 1;
        } else {
            bind_len = -1;
        }
        sqlite3_bind_text(stmt_bind, 1, sqlbuf1, bind_len, SQLITE_TRANSIENT);
        sqlite3_step(stmt_bind);
        sqlite3_reset(stmt_bind);
        (void)sqlite3_sql(stmt_bind);
    }

    sqlite3_finalize(stmt_select);
    sqlite3_finalize(stmt_bind);
    sqlite3_finalize(stmt_extra);

    sqlite3_close(db);
    return 0;
}