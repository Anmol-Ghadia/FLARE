// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
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
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4 && *Off < Size; ++i, ++(*Off)) {
        v = (v << 8) | Data[*Off];
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    FILE *fp;
    int rc;
    int flags;
    int open_flags[] = {
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
        SQLITE_OPEN_READONLY,
        SQLITE_OPEN_READWRITE,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY
    };
    const char *sql = "SELECT 1";

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    if (Size > 0) {
        switch (Data[off++] % 6) {
            case 0:
                (void)sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
                break;
            case 1:
                (void)sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
                break;
            case 2:
                (void)sqlite3_config(SQLITE_CONFIG_SERIALIZED);
                break;
            case 3:
                (void)sqlite3_config(SQLITE_CONFIG_MEMSTATUS, (int)(Data[(off - 1) % Size] & 1));
                break;
            case 4:
                (void)sqlite3_config(SQLITE_CONFIG_URI, (int)(Data[(off - 1) % Size] & 1));
                break;
            default:
                (void)sqlite3_config(0x7fffffff);
                break;
        }
    }

    flags = open_flags[(Size > 1 ? Data[1] : 0) % (sizeof(open_flags) / sizeof(open_flags[0]))];
    rc = sqlite3_open_v2("./dummy_file", &db, flags, NULL);
    if (rc == SQLITE_OK && db != NULL) {
        if (Size > off) {
            size_t remaining = Size - off;
            size_t sql_len = remaining > 4096 ? 4096 : remaining;
            char sqlbuf[4097];
            memcpy(sqlbuf, Data + off, sql_len);
            sqlbuf[sql_len] = '\0';
            sql = sqlbuf;
            (void)sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
            if (stmt != NULL) {
                if (Size > 2 && (Data[2] & 1)) {
                    (void)sqlite3_step(stmt);
                }
                if (Size > 3 && (Data[3] & 1)) {
                    (void)sqlite3_reset(stmt);
                }
            }
        } else {
            (void)sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
            if (stmt != NULL) {
                (void)sqlite3_step(stmt);
            }
        }
    }

    (void)sqlite3_finalize(stmt);
    stmt = NULL;

    (void)sqlite3_close(db);
    db = NULL;

    (void)sqlite3_memory_used();

    (void)sqlite3_shutdown();

    if (Size > 4) {
        switch (read_u32(Data, Size, &off) % 6) {
            case 0:
                (void)sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
                break;
            case 1:
                (void)sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
                break;
            case 2:
                (void)sqlite3_config(SQLITE_CONFIG_SERIALIZED);
                break;
            case 3:
                (void)sqlite3_config(SQLITE_CONFIG_MEMSTATUS, (int)(Size & 1));
                break;
            case 4:
                (void)sqlite3_config(SQLITE_CONFIG_URI, (int)((Size >> 1) & 1));
                break;
            default:
                (void)sqlite3_config(0x7fffffff);
                break;
        }
    } else {
        (void)sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
    }

    return 0;
}