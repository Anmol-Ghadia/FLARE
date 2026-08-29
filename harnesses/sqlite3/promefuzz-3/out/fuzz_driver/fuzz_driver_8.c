// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_blob_bytes at sqlite3.c:83019:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_realloc64 at sqlite3.c:15954:18 in sqlite3.h
// sqlite3_realloc64 at sqlite3.c:15954:18 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_blob_open at sqlite3.c:82675:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_blob_reopen at sqlite3.c:83034:16 in sqlite3.h
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

static uint64_t read_u64(const uint8_t *data, size_t size, size_t *off) {
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8 && *off < size; ++i, ++(*off)) {
        v |= ((uint64_t)data[*off]) << (i * 8);
    }
    return v;
}

static int read_i32(const uint8_t *data, size_t size, size_t *off) {
    int v = 0;
    size_t i;
    for (i = 0; i < 4 && *off < size; ++i, ++(*off)) {
        v |= ((int)data[*off]) << (i * 8);
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_blob *blob = NULL;
    sqlite3_stmt *stmt = NULL;
    void *mem = NULL;
    FILE *fp;
    size_t off = 0;
    int rc;
    sqlite3_int64 rowid1 = 1, rowid2 = 2;
    int bytes = 0;
    const char *msg;
    int rnd_n;
    unsigned char rnd_buf[256];

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open("./dummy_file", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) {
            msg = sqlite3_errmsg(db);
            (void)msg;
            sqlite3_close(db);
        } else {
            msg = sqlite3_errmsg(NULL);
            (void)msg;
        }
        return 0;
    }

    sqlite3_exec(db, "PRAGMA journal_mode=OFF;", NULL, NULL, NULL);
    sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(id INTEGER PRIMARY KEY, b BLOB);", NULL, NULL, NULL);
    sqlite3_exec(db, "DELETE FROM t;", NULL, NULL, NULL);

    rc = sqlite3_prepare_v2(db, "INSERT INTO t(id,b) VALUES(?,?);", -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
        int split = (int)(Size / 2);

        sqlite3_bind_int64(stmt, 1, 1);
        sqlite3_bind_blob(stmt, 2, Data, split, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);

        sqlite3_bind_int64(stmt, 1, 2);
        sqlite3_bind_blob(stmt, 2, Data + split, (int)(Size - (size_t)split), SQLITE_STATIC);
        sqlite3_step(stmt);
    }
    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    rc = sqlite3_blob_open(db, "main", "t", "b", 1, 0, &blob);

    msg = sqlite3_errmsg(db);
    (void)msg;

    if (blob) {
        if (Size > 0) {
            rowid2 = (sqlite3_int64)(read_u64(Data, Size, &off) % 4);
        }
        rc = sqlite3_blob_reopen(blob, rowid2);
        (void)rc;

        bytes = sqlite3_blob_bytes(blob);
        (void)bytes;
    }

    {
        sqlite3_uint64 initial = 1;
        sqlite3_uint64 newsize;

        if (Size > off) {
            initial = (sqlite3_uint64)((Data[off] % 64) + 1);
            off++;
        }

        mem = sqlite3_malloc64(initial);
        if (mem) {
            memset(mem, 0xA5, (size_t)initial);
            newsize = (sqlite3_uint64)(read_u64(Data, Size, &off) % 4096);
            mem = sqlite3_realloc64(mem, newsize);
        } else {
            newsize = (sqlite3_uint64)(read_u64(Data, Size, &off) % 4096);
            mem = sqlite3_realloc64(NULL, newsize);
        }
    }

    rnd_n = read_i32(Data, Size, &off);
    if (rnd_n < 0) rnd_n = -rnd_n;
    rnd_n %= (int)(sizeof(rnd_buf) + 32);

    if ((off < Size) && (Data[off] & 1)) {
        sqlite3_randomness(rnd_n, NULL);
    } else {
        int use_n = rnd_n;
        if (use_n > (int)sizeof(rnd_buf)) use_n = (int)sizeof(rnd_buf);
        sqlite3_randomness(use_n, rnd_buf);
    }

    if (mem) {
        sqlite3_free(mem);
    }
    if (blob) {
        sqlite3_blob_close(blob);
    }
    sqlite3_close(db);
    return 0;
}