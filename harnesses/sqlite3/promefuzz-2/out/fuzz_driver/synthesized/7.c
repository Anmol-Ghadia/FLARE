// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_hard_heap_limit64 at sqlite3.c:15573:26 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4; ++i) {
        v <<= 8;
        if (*Off < Size) {
            v |= Data[*Off];
            (*Off)++;
        }
    }
    return v;
}

static sqlite3_int64 read_i64(const uint8_t *Data, size_t Size, size_t *Off) {
    sqlite3_int64 v = 0;
    size_t i;
    for (i = 0; i < 8; ++i) {
        v <<= 8;
        if (*Off < Size) {
            v |= (sqlite3_int64)Data[*Off];
            (*Off)++;
        }
    }
    return v;
}

static int progress_cb(void *pArg) {
    const uint8_t *b = (const uint8_t *)pArg;
    if (b == NULL) return 0;
    return (b[0] & 1) ? 1 : 0;
}

int LLVMFuzzerTestOneInput_7(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    sqlite3 *db = NULL;
    int rc;
    FILE *fp;
    int flags;
    const char *vfs = NULL;
    int nOps;
    int limit_id1, limit_val1;
    sqlite3_int64 heap_limit;
    int limit_id2, limit_val2;
    int dbcfg_op;
    int out_int = 0;
    char main_db_name[32];

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_initialize();
    if (rc != SQLITE_OK) {
        return 0;
    }

    flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
#ifdef SQLITE_OPEN_URI
    if (Size > off && (Data[off++] & 1)) flags |= SQLITE_OPEN_URI;
#endif
#ifdef SQLITE_OPEN_MEMORY
    if (Size > off && (Data[off++] & 1)) flags |= SQLITE_OPEN_MEMORY;
#endif
#ifdef SQLITE_OPEN_NOMUTEX
    if (Size > off && (Data[off++] & 1)) flags |= SQLITE_OPEN_NOMUTEX;
#endif
#ifdef SQLITE_OPEN_FULLMUTEX
    if (Size > off && (Data[off++] & 1)) flags |= SQLITE_OPEN_FULLMUTEX;
#endif
#ifdef SQLITE_OPEN_PRIVATECACHE
    if (Size > off && (Data[off++] & 1)) flags |= SQLITE_OPEN_PRIVATECACHE;
#endif
#ifdef SQLITE_OPEN_SHAREDCACHE
    if (Size > off && (Data[off++] & 1)) flags |= SQLITE_OPEN_SHAREDCACHE;
#endif
#ifdef SQLITE_OPEN_EXRESCODE
    if (Size > off && (Data[off++] & 1)) flags |= SQLITE_OPEN_EXRESCODE;
#endif

    if (Size > off && (Data[off++] & 1)) {
        vfs = "invalid_vfs_name";
    }

    rc = sqlite3_open_v2("./dummy_file", &db, flags, vfs);

    if (db != NULL) {
        nOps = (int)(read_u32(Data, Size, &off) % 1024);
        sqlite3_progress_handler(db, nOps, progress_cb, (void *)Data);

        limit_id1 = (int)(read_u32(Data, Size, &off) % (SQLITE_LIMIT_WORKER_THREADS + 8));
        limit_val1 = (int)read_u32(Data, Size, &off);
        if (Size > off && (Data[off++] & 1)) {
            limit_val1 = -1;
        } else if (Size > off && (Data[off++] & 1)) {
            limit_val1 = -limit_val1;
        }
        (void)sqlite3_limit(db, limit_id1, limit_val1);

        heap_limit = read_i64(Data, Size, &off);
        if (Size > off && (Data[off++] & 1)) {
            heap_limit = -1;
        }
        (void)sqlite3_hard_heap_limit64(heap_limit);

        limit_id2 = (int)(read_u32(Data, Size, &off) % (SQLITE_LIMIT_WORKER_THREADS + 8));
        limit_val2 = (int)read_u32(Data, Size, &off);
        if (Size > off && (Data[off++] & 1)) {
            limit_val2 = -1;
        } else if (Size > off && (Data[off++] & 1)) {
            limit_val2 = -limit_val2;
        }
        (void)sqlite3_limit(db, limit_id2, limit_val2);

        dbcfg_op = (int)(read_u32(Data, Size, &off) % 6);
        switch (dbcfg_op) {
            case 0:
#ifdef SQLITE_DBCONFIG_ENABLE_FKEY
                (void)sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY,
                                        (int)(read_u32(Data, Size, &off) & 1), &out_int);
#endif
                break;
            case 1:
#ifdef SQLITE_DBCONFIG_ENABLE_TRIGGER
                (void)sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_TRIGGER,
                                        (int)(read_u32(Data, Size, &off) & 1), &out_int);
#endif
                break;
            case 2:
#ifdef SQLITE_DBCONFIG_LOOKASIDE
                (void)sqlite3_db_config(db, SQLITE_DBCONFIG_LOOKASIDE,
                                        NULL,
                                        (int)(read_u32(Data, Size, &off) % 4096),
                                        (int)(read_u32(Data, Size, &off) % 128));
#endif
                break;
            case 3:
#ifdef SQLITE_DBCONFIG_MAINDBNAME
                memset(main_db_name, 0, sizeof(main_db_name));
                if (Size > off) {
                    size_t copy = Size - off;
                    if (copy >= sizeof(main_db_name)) copy = sizeof(main_db_name) - 1;
                    memcpy(main_db_name, Data + off, copy);
                } else {
                    strcpy(main_db_name, "main");
                }
                (void)sqlite3_db_config(db, SQLITE_DBCONFIG_MAINDBNAME, main_db_name);
#endif
                break;
            case 4:
#ifdef SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION
                (void)sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION,
                                        (int)(read_u32(Data, Size, &off) & 1), &out_int);
#endif
                break;
            case 5:
#ifdef SQLITE_DBCONFIG_DEFENSIVE
                (void)sqlite3_db_config(db, SQLITE_DBCONFIG_DEFENSIVE,
                                        (int)(read_u32(Data, Size, &off) & 1), &out_int);
#endif
                break;
            default:
                break;
        }

        sqlite3_progress_handler(db, 0, NULL, NULL);
        sqlite3_close(db);
    }

    return 0;
}