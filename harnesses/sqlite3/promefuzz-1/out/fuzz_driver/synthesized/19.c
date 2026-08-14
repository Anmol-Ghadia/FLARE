// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

static int read_u32(const uint8_t *Data, size_t Size, size_t *Offset, uint32_t *Out) {
    if (*Offset + 4 > Size) return 0;
    *Out = ((uint32_t)Data[*Offset]) |
           ((uint32_t)Data[*Offset + 1] << 8) |
           ((uint32_t)Data[*Offset + 2] << 16) |
           ((uint32_t)Data[*Offset + 3] << 24);
    *Offset += 4;
    return 1;
}

static int64_t read_i64(const uint8_t *Data, size_t Size, size_t *Offset) {
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8 && *Offset < Size; i++, (*Offset)++) {
        v |= ((uint64_t)Data[*Offset]) << (8 * i);
    }
    return (int64_t)v;
}

int LLVMFuzzerTestOneInput_19(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;
    int rc;
    size_t off = 0;
    uint32_t sql_len_u32 = 0;
    size_t sql_len;
    char *sql = NULL;
    int param_count, i;
    int bind_index;
    sqlite3_int64 bind_value;
    char *formatted = NULL;

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

    sqlite3_exec(db,
                 "CREATE TABLE IF NOT EXISTS t(a INTEGER, b TEXT);"
                 "INSERT INTO t(a,b) VALUES(1,'x');",
                 NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    if (!read_u32(Data, Size, &off, &sql_len_u32)) {
        sql_len_u32 = (uint32_t)(Size > off ? (Size - off) : 0);
    }
    sql_len = (size_t)sql_len_u32;
    if (sql_len > Size - off) sql_len = Size - off;

    sql = (char *)sqlite3_malloc64(sql_len + 1);
    if (!sql) {
        sqlite3_close(db);
        return 0;
    }

    if (sql_len > 0) {
        memcpy(sql, Data + off, sql_len);
    }
    sql[sql_len] = '\0';
    off += sql_len;

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK || stmt == NULL) {
        sqlite3_free(sql);
        sqlite3_close(db);
        return 0;
    }

    param_count = sqlite3_bind_parameter_count(stmt);

    for (i = 0; i <= param_count + 2; i++) {
        const char *pname = sqlite3_bind_parameter_name(stmt, i);

        bind_index = i;
        if (off < Size) {
            bind_index = (int)(Data[off++] % (param_count + 3)) - 1;
        }

        bind_value = read_i64(Data, Size, &off);
        sqlite3_bind_int64(stmt, bind_index, bind_value);

        formatted = sqlite3_mprintf("pc=%d i=%d name=%s idx=%d val=%lld sql=%q",
                                    param_count,
                                    i,
                                    pname ? pname : "(null)",
                                    bind_index,
                                    (long long)bind_value,
                                    sql ? sql : "");
        if (formatted) {
            sqlite3_free(formatted);
            formatted = NULL;
        }
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    sqlite3_free(sql);
    sqlite3_close(db);
    return 0;
}