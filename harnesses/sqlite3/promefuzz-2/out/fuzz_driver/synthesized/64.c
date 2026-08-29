// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_pointer at sqlite3.c:73369:16 in sqlite3.h
// sqlite3_bind_text64 at sqlite3.c:73396:16 in sqlite3.h
// sqlite3_bind_text16 at sqlite3.c:73409:16 in sqlite3.h
// sqlite3_bind_blob64 at sqlite3.c:73327:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_text64 at sqlite3.c:73396:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4; i++) {
        v <<= 8;
        if (*Off < Size) {
            v |= Data[*Off];
            (*Off)++;
        }
    }
    return v;
}

static uint64_t read_u64(const uint8_t *Data, size_t Size, size_t *Off) {
    uint64_t hi = read_u32(Data, Size, Off);
    uint64_t lo = read_u32(Data, Size, Off);
    return (hi << 32) | lo;
}

static int read_int_range(const uint8_t *Data, size_t Size, size_t *Off, int minv, int maxv) {
    uint32_t v;
    if (maxv <= minv) return minv;
    v = read_u32(Data, Size, Off);
    return minv + (int)(v % (uint32_t)(maxv - minv + 1));
}

static size_t take_bytes(const uint8_t *Data, size_t Size, size_t *Off, size_t want, const uint8_t **Out) {
    size_t remain = (*Off < Size) ? (Size - *Off) : 0;
    size_t n = want < remain ? want : remain;
    *Out = Data + *Off;
    *Off += n;
    return n;
}

static void free_destructor(void *p) {
    free(p);
}

int LLVMFuzzerTestOneInput_64(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;
    size_t off = 0;
    int rc;
    int i;
    int param_count;

    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return 0;
    }

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp) {
            if (Size > 0) fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    rc = sqlite3_exec(
        db,
        "CREATE TABLE IF NOT EXISTS t(a TEXT, b BLOB, c TEXT, d INTEGER);"
        "INSERT INTO t(a,b,c,d) VALUES('x', X'010203', 'y', 1);",
        NULL, NULL, &errmsg
    );
    if (rc != SQLITE_OK) {
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return 0;
    }

    {
        const char *sqls[] = {
            "SELECT ?1, ?2, ?3, ?4, ?5",
            "INSERT INTO t(a,b,c,d) VALUES(?1, ?2, ?3, ?4)",
            "UPDATE t SET a=?1, b=?2, c=?3 WHERE d=?4",
            "SELECT ?1, ?5, :named, @other, $third",
            "SELECT length(?1), hex(?2), ?3, ?4, ?10"
        };
        int idx = 0;
        if (Size > 0) idx = Data[0] % (int)(sizeof(sqls) / sizeof(sqls[0]));
        rc = sqlite3_prepare_v2(db, sqls[idx], -1, &stmt, NULL);
        if (rc != SQLITE_OK || stmt == NULL) {
            sqlite3_close(db);
            return 0;
        }
    }

    param_count = sqlite3_bind_parameter_count(stmt);

    for (i = 0; i < 8; i++) {
        int which;
        int idx;
        const uint8_t *chunk = NULL;
        size_t chunk_len = 0;
        void *heapbuf = NULL;
        size_t alloc_len = 0;

        which = read_int_range(Data, Size, &off, 0, 5);

        if (param_count > 0) {
            idx = read_int_range(Data, Size, &off, 0, param_count + 2);
        } else {
            idx = read_int_range(Data, Size, &off, 0, 3);
        }

        switch (which) {
            case 0: {
                int *pint = (int *)malloc(sizeof(int));
                if (pint) {
                    *pint = (int)read_u32(Data, Size, &off);
                }
                rc = sqlite3_bind_pointer(stmt, idx, pint, "fuzz.ptr", free_destructor);
                if (rc != SQLITE_OK && pint) {
                    /* Destructor may already have been called by SQLite on failure. */
                }
                break;
            }

            case 1: {
                sqlite3_uint64 n64 = (sqlite3_uint64)read_u64(Data, Size, &off);
                unsigned char encs[] = {
                    SQLITE_UTF8,
                    SQLITE_UTF16,
                    SQLITE_UTF16BE,
                    SQLITE_UTF16LE,
                    0xff
                };
                unsigned char enc = encs[read_int_range(Data, Size, &off, 0, 4)];
                chunk_len = take_bytes(Data, Size, &off, (size_t)(n64 % 64), &chunk);
                alloc_len = chunk_len + 1;
                heapbuf = malloc(alloc_len);
                if (heapbuf) {
                    if (chunk_len) memcpy(heapbuf, chunk, chunk_len);
                    ((char *)heapbuf)[chunk_len] = '\0';
                    rc = sqlite3_bind_text64(
                        stmt, idx, (const char *)heapbuf,
                        (sqlite3_uint64)chunk_len,
                        free_destructor, enc
                    );
                    if (rc != SQLITE_OK) {
                        /* SQLite handles destructor semantics. */
                    }
                }
                break;
            }

            case 2: {
                int nData;
                chunk_len = take_bytes(Data, Size, &off, (size_t)read_int_range(Data, Size, &off, 0, 64), &chunk);
                alloc_len = chunk_len + 2;
                heapbuf = malloc(alloc_len);
                if (heapbuf) {
                    if (chunk_len) memcpy(heapbuf, chunk, chunk_len);
                    ((uint8_t *)heapbuf)[chunk_len] = 0;
                    ((uint8_t *)heapbuf)[chunk_len + 1] = 0;
                    nData = (read_int_range(Data, Size, &off, 0, 1) == 0) ? -1 : (int)chunk_len;
                    rc = sqlite3_bind_text16(stmt, idx, heapbuf, nData, free_destructor);
                    if (rc != SQLITE_OK) {
                        /* SQLite handles destructor semantics. */
                    }
                }
                break;
            }

            case 3: {
                chunk_len = take_bytes(Data, Size, &off, (size_t)read_int_range(Data, Size, &off, 0, 64), &chunk);
                alloc_len = chunk_len ? chunk_len : 1;
                heapbuf = malloc(alloc_len);
                if (heapbuf) {
                    if (chunk_len) memcpy(heapbuf, chunk, chunk_len);
                    rc = sqlite3_bind_blob64(
                        stmt, idx, heapbuf, (sqlite3_uint64)chunk_len, free_destructor
                    );
                    if (rc != SQLITE_OK) {
                        /* SQLite handles destructor semantics. */
                    }
                }
                break;
            }

            case 4: {
                rc = sqlite3_step(stmt);
                (void)rc;
                break;
            }

            case 5: {
                rc = sqlite3_reset(stmt);
                (void)rc;
                break;
            }

            default:
                break;
        }
    }

    rc = sqlite3_step(stmt);
    (void)rc;
    rc = sqlite3_reset(stmt);
    (void)rc;

    if (param_count > 0) {
        const char *txt = "final";
        unsigned char enc = SQLITE_UTF8;
        sqlite3_bind_text64(stmt, 1, txt, 5, SQLITE_STATIC, enc);
        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}