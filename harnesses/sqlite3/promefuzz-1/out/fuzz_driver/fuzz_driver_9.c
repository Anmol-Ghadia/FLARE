// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_text64 at sqlite3.c:73396:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *off) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4 && *off < size; ++i, ++(*off)) {
        v |= ((uint32_t)data[*off]) << (i * 8);
    }
    return v;
}

static uint64_t read_u64(const uint8_t *data, size_t size, size_t *off) {
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8 && *off < size; ++i, ++(*off)) {
        v |= ((uint64_t)data[*off]) << (i * 8);
    }
    return v;
}

static double read_double(const uint8_t *data, size_t size, size_t *off) {
    union {
        uint64_t u;
        double d;
    } conv;
    conv.u = read_u64(data, size, off);
    return conv.d;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    size_t off = 0;
    int rc;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db != NULL) {
            sqlite3_close(db);
        }
        return 0;
    }

    rc = sqlite3_prepare_v2(
        db,
        "SELECT ?1, ?2, ?3, ?4, ?5;",
        -1,
        &stmt,
        NULL
    );
    if (rc != SQLITE_OK || stmt == NULL) {
        sqlite3_close(db);
        return 0;
    }

    {
        int idx1 = (int)(read_u32(Data, Size, &off) % 8);
        int val1 = (int)read_u32(Data, Size, &off);
        (void)sqlite3_bind_int(stmt, idx1, val1);
    }

    {
        int idx2 = (int)(read_u32(Data, Size, &off) % 8);
        sqlite3_int64 val2 = (sqlite3_int64)read_u64(Data, Size, &off);
        (void)sqlite3_bind_int64(stmt, idx2, val2);
    }

    {
        int idx3 = (int)(read_u32(Data, Size, &off) % 8);
        double val3 = read_double(Data, Size, &off);
        (void)sqlite3_bind_double(stmt, idx3, val3);
    }

    {
        int idx4 = (int)(read_u32(Data, Size, &off) % 8);
        unsigned char enc_choices[] = {
            SQLITE_UTF8,
            SQLITE_UTF16LE,
            SQLITE_UTF16BE,
            SQLITE_UTF16
        };
        unsigned char enc = enc_choices[(Size > off) ? (Data[off++] % 4) : 0];
        sqlite3_uint64 text_len = (sqlite3_uint64)((Size > off) ? (Size - off) : 0);
        const char *text_ptr = (const char *)(Data + off);
        (void)sqlite3_bind_text64(stmt, idx4, text_ptr, text_len, SQLITE_TRANSIENT, enc);
        off = Size;
    }

    {
        int idx5 = (int)(read_u32(Data, Size, &off) % 8);
        int val5 = (int)read_u32(Data, Size, &off);
        (void)sqlite3_bind_int(stmt, idx5, val5);
    }

    (void)sqlite3_step(stmt);
    (void)sqlite3_reset(stmt);
    (void)sqlite3_clear_bindings(stmt);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}