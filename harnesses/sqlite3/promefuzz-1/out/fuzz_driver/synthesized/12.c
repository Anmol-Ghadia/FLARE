// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_blob_write at sqlite3.c:83009:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_blob_read at sqlite3.c:83002:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_blob_open at sqlite3.c:82675:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_blob_bytes at sqlite3.c:83019:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static int read_u32(const uint8_t *Data, size_t Size, size_t *off, uint32_t *out) {
    if (*off + 4 > Size) return 0;
    *out = ((uint32_t)Data[*off]) |
           ((uint32_t)Data[*off + 1] << 8) |
           ((uint32_t)Data[*off + 2] << 16) |
           ((uint32_t)Data[*off + 3] << 24);
    *off += 4;
    return 1;
}

static int read_i32(const uint8_t *Data, size_t Size, size_t *off, int *out) {
    uint32_t v;
    if (!read_u32(Data, Size, off, &v)) return 0;
    *out = (int)v;
    return 1;
}

static void fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
    int sum = 0;
    int i;
    for (i = 0; i < argc; i++) {
        sum += sqlite3_value_bytes(argv[i]);
    }
    sqlite3_result_int(ctx, sum);
}

int LLVMFuzzerTestOneInput_12(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_blob *blob = NULL;
    sqlite3_stmt *stmt = NULL;
    const char *tail = NULL;
    char *sqlbuf = NULL;
    char funcname[256];
    unsigned char readbuf[256];
    size_t off = 0;
    int rc;
    int writable = 1;
    int blob_bytes = 0;
    int write_n = 0, write_off = 0;
    int read_n = 0, read_off = 0;
    int nArg = -1;
    int encflag = SQLITE_UTF8;
    uint32_t tmp = 0;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open("./dummy_file", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db, "PRAGMA journal_mode=OFF;", NULL, NULL, NULL);
    sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(id INTEGER PRIMARY KEY, b BLOB);", NULL, NULL, NULL);
    sqlite3_exec(db, "DELETE FROM t;", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO t(b) VALUES(zeroblob(64));", NULL, NULL, NULL);

    if (read_u32(Data, Size, &off, &tmp)) {
        writable = (tmp & 1U) ? 1 : 0;
    }

    rc = sqlite3_blob_open(db, "main", "t", "b", 1, writable, &blob);
    if (rc != SQLITE_OK || blob == NULL) {
        (void)sqlite3_errmsg(db);
        sqlite3_close(db);
        return 0;
    }

    blob_bytes = sqlite3_blob_bytes(blob);

    if (!read_i32(Data, Size, &off, &write_n)) write_n = (int)(Size & 0x7f);
    if (!read_i32(Data, Size, &off, &write_off)) write_off = 0;
    if (!read_i32(Data, Size, &off, &read_n)) read_n = (int)(Size & 0x7f);
    if (!read_i32(Data, Size, &off, &read_off)) read_off = 0;
    if (!read_i32(Data, Size, &off, &nArg)) nArg = -1;
    if (nArg < -1) nArg = -1;
    if (nArg > 127) nArg = 127;

    if (read_u32(Data, Size, &off, &tmp)) {
        switch (tmp % 4U) {
            case 0: encflag = SQLITE_UTF8; break;
            case 1: encflag = SQLITE_UTF16LE; break;
            case 2: encflag = SQLITE_UTF16BE; break;
            default: encflag = SQLITE_UTF16; break;
        }
        if (tmp & 0x10U) encflag |= SQLITE_DETERMINISTIC;
        if (tmp & 0x20U) encflag |= SQLITE_DIRECTONLY;
    }

    if (write_n < 0) write_n = -(write_n % 32);
    else write_n %= 128;
    if (write_off < 0) write_off = -(abs(write_off) % 32);
    else if (blob_bytes > 0) write_off %= (blob_bytes + 32);

    if (read_n < 0) read_n = -(read_n % 32);
    else read_n %= (int)sizeof(readbuf);
    if (read_off < 0) read_off = -(abs(read_off) % 32);
    else if (blob_bytes > 0) read_off %= (blob_bytes + 32);

    memset(readbuf, 0, sizeof(readbuf));

    /* Required order: sqlite3_blob_write -> sqlite3_errmsg -> sqlite3_blob_read -> sqlite3_errmsg -> sqlite3_create_function -> sqlite3_prepare_v2 */
    rc = sqlite3_blob_write(blob,
                            off < Size ? (const void *)(Data + off) : (const void *)"",
                            write_n,
                            write_off);
    (void)rc;
    (void)sqlite3_errmsg(db);

    rc = sqlite3_blob_read(blob, readbuf, read_n, read_off);
    (void)rc;
    (void)sqlite3_errmsg(db);

    {
        size_t remain = (off < Size) ? (Size - off) : 0;
        size_t fnlen = remain > 255 ? 255 : remain;
        if (fnlen == 0) {
            memcpy(funcname, "f", 2);
        } else {
            size_t i;
            for (i = 0; i < fnlen; i++) {
                unsigned char c = Data[off + i];
                if ((c >= 'a' && c <= 'z') ||
                    (c >= 'A' && c <= 'Z') ||
                    (c >= '0' && c <= '9') ||
                    c == '_') {
                    funcname[i] = (char)c;
                } else {
                    funcname[i] = (char)('a' + (c % 26));
                }
            }
            funcname[fnlen] = '\0';
        }
    }

    rc = sqlite3_create_function(db, funcname, nArg, encflag, NULL,
                                 fuzz_scalar, NULL, NULL);
    (void)rc;

    sqlbuf = (char *)malloc(Size + 1);
    if (sqlbuf) {
        memcpy(sqlbuf, Data, Size);
        sqlbuf[Size] = '\0';
        rc = sqlite3_prepare_v2(db, sqlbuf, (int)Size, &stmt, &tail);
        (void)rc;
    }

    if (stmt) {
        sqlite3_finalize(stmt);
    }
    if (blob) {
        sqlite3_blob_close(blob);
    }
    if (db) {
        sqlite3_close(db);
    }
    free(sqlbuf);
    return 0;
}