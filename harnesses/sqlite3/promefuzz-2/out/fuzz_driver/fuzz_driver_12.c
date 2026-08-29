// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_blob_read at sqlite3.c:83002:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_blob_read at sqlite3.c:83002:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_blob_open at sqlite3.c:82675:16 in sqlite3.h
// sqlite3_blob_bytes at sqlite3.c:83019:16 in sqlite3.h
// sqlite3_blob_write at sqlite3.c:83009:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4 && *Off < Size; i++, (*Off)++) {
        v = (v << 8) | Data[*Off];
    }
    return v;
}

static int read_int_range(const uint8_t *Data, size_t Size, size_t *Off, int minv, int maxv) {
    uint32_t v;
    if (minv >= maxv) return minv;
    v = read_u32(Data, Size, Off);
    return minv + (int)(v % (uint32_t)(maxv - minv + 1));
}

static void fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
    if (argc > 0 && argv && argv[0]) {
        const unsigned char *txt = sqlite3_value_text(argv[0]);
        if (txt) {
            sqlite3_result_text(ctx, (const char *)txt, -1, SQLITE_TRANSIENT);
            return;
        }
    }
    sqlite3_result_null(ctx);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_blob *blob = NULL;
    sqlite3_stmt *stmt = NULL;
    const char *tail = NULL;
    char *errmsg = NULL;
    size_t off = 0;
    int rc;
    int writable = 1;
    int blob_bytes;
    int write_n, write_off;
    int read_n, read_off;
    int nArg;
    int enc;
    char funcname[256];
    char sqlbuf[512];
    unsigned char readbuf[256];
    int i;

    FILE *fp = fopen("./dummy_file", "wb");
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

    sqlite3_exec(db,
                 "PRAGMA journal_mode=OFF;"
                 "PRAGMA synchronous=OFF;"
                 "CREATE TABLE IF NOT EXISTS t(id INTEGER PRIMARY KEY, b BLOB);"
                 "DELETE FROM t;"
                 "INSERT INTO t(b) VALUES(zeroblob(64));",
                 NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    writable = (Size > 0) ? (Data[0] & 1) : 1;
    rc = sqlite3_blob_open(db, "main", "t", "b", 1, writable, &blob);
    if (rc == SQLITE_OK && blob) {
        blob_bytes = sqlite3_blob_bytes(blob);
        if (blob_bytes < 0) blob_bytes = 0;

        write_n = read_int_range(Data, Size, &off, 0, 128);
        write_off = read_int_range(Data, Size, &off, -16, 96);
        if (write_n > (int)(Size - off)) {
            write_n = (int)(Size - off);
        }
        if (write_n < 0) write_n = 0;

        rc = sqlite3_blob_write(blob, Data + off, write_n, write_off);
        (void)sqlite3_errmsg(db);

        read_n = read_int_range(Data, Size, &off, 0, 128);
        read_off = read_int_range(Data, Size, &off, -16, 96);
        if (read_n > (int)sizeof(readbuf)) {
            read_n = (int)sizeof(readbuf);
        }
        memset(readbuf, 0, sizeof(readbuf));
        rc = sqlite3_blob_read(blob, readbuf, read_n, read_off);
        (void)sqlite3_errmsg(db);
    } else {
        (void)sqlite3_errmsg(db);
        memset(readbuf, 0, sizeof(readbuf));
        rc = sqlite3_blob_read(blob, readbuf, 0, 0);
        (void)sqlite3_errmsg(db);
    }

    memset(funcname, 0, sizeof(funcname));
    {
        int fnlen = read_int_range(Data, Size, &off, 1, 255);
        if ((size_t)fnlen > Size - off) fnlen = (int)(Size - off);
        if (fnlen <= 0) {
            memcpy(funcname, "f", 2);
        } else {
            for (i = 0; i < fnlen && i < 255; i++) {
                unsigned char c = Data[off + i];
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                    (c >= '0' && c <= '9') || c == '_') {
                    funcname[i] = (char)c;
                } else {
                    funcname[i] = (char)('a' + (c % 26));
                }
            }
            funcname[fnlen] = '\0';
        }
        off += (size_t)fnlen <= Size - off ? (size_t)fnlen : (Size - off);
    }

    nArg = read_int_range(Data, Size, &off, -1, 4);
    switch (read_int_range(Data, Size, &off, 0, 3)) {
        case 0: enc = SQLITE_UTF8; break;
        case 1: enc = SQLITE_UTF16LE; break;
        case 2: enc = SQLITE_UTF16BE; break;
        default: enc = SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_DIRECTONLY; break;
    }

    rc = sqlite3_create_function(db, funcname, nArg, enc, NULL, fuzz_scalar, NULL, NULL);

    memset(sqlbuf, 0, sizeof(sqlbuf));
    if (Size > off && (Data[off] & 1)) {
        snprintf(sqlbuf, sizeof(sqlbuf), "SELECT %s(%d);", funcname,
                 (Size > off + 1) ? (int)Data[off + 1] : 0);
    } else {
        int sql_len = (int)((Size > off) ? (Size - off) : 0);
        if (sql_len > (int)sizeof(sqlbuf) - 1) sql_len = (int)sizeof(sqlbuf) - 1;
        if (sql_len > 0) {
            memcpy(sqlbuf, Data + off, (size_t)sql_len);
            sqlbuf[sql_len] = '\0';
        } else {
            memcpy(sqlbuf, "SELECT 1;", 10);
        }
    }

    rc = sqlite3_prepare_v2(db, sqlbuf, -1, &stmt, &tail);

    if (stmt) {
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    if (blob) {
        sqlite3_blob_close(blob);
        blob = NULL;
    }

    sqlite3_close(db);
    return 0;
}