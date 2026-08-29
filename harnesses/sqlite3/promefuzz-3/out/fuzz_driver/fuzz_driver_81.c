// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_int64 at sqlite3.c:73046:25 in sqlite3.h
// sqlite3_column_double at sqlite3.c:73036:19 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_expanded_sql at sqlite3.c:73662:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_normalized_sql at sqlite3.c:73682:24 in sqlite3.h
// sqlite3_expanded_sql at sqlite3.c:73662:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_expanded_sql at sqlite3.c:73662:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_normalized_sql at sqlite3.c:73682:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_expanded_sql at sqlite3.c:73662:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_normalized_sql at sqlite3.c:73682:24 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
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

#ifndef SQLITE_PREPARE_PERSISTENT
#define SQLITE_PREPARE_PERSISTENT 0
#endif

static int read_u32(const uint8_t *data, size_t size, size_t *off, uint32_t *out) {
    if (*off + 4 > size) return 0;
    *out = ((uint32_t)data[*off]) |
           ((uint32_t)data[*off + 1] << 8) |
           ((uint32_t)data[*off + 2] << 16) |
           ((uint32_t)data[*off + 3] << 24);
    *off += 4;
    return 1;
}

static int exec_sql(sqlite3 *db, const char *sql) {
    char *errmsg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (errmsg) sqlite3_free(errmsg);
    return rc;
}

static void bind_from_data(sqlite3_stmt *stmt, const uint8_t *data, size_t size, size_t *off) {
    int nparam = sqlite3_bind_parameter_count(stmt);
    int i;

    for (i = 1; i <= nparam; i++) {
        if (*off >= size) break;

        uint8_t kind = data[(*off)++] % 6;
        switch (kind) {
            case 0: {
                sqlite3_bind_null(stmt, i);
                break;
            }
            case 1: {
                if (*off + 8 <= size) {
                    sqlite3_int64 v = 0;
                    int j;
                    for (j = 0; j < 8; j++) {
                        v |= ((sqlite3_int64)data[*off + j]) << (8 * j);
                    }
                    *off += 8;
                    sqlite3_bind_int64(stmt, i, v);
                } else {
                    sqlite3_bind_null(stmt, i);
                }
                break;
            }
            case 2: {
                if (*off + 8 <= size) {
                    uint64_t bits = 0;
                    double d;
                    int j;
                    for (j = 0; j < 8; j++) {
                        bits |= ((uint64_t)data[*off + j]) << (8 * j);
                    }
                    *off += 8;
                    memcpy(&d, &bits, sizeof(d));
                    sqlite3_bind_double(stmt, i, d);
                } else {
                    sqlite3_bind_null(stmt, i);
                }
                break;
            }
            case 3: {
                uint32_t len = 0;
                if (!read_u32(data, size, off, &len)) {
                    sqlite3_bind_null(stmt, i);
                    break;
                }
                if (*off > size) {
                    sqlite3_bind_null(stmt, i);
                    break;
                }
                len %= (uint32_t)(size - *off + 1);
                sqlite3_bind_text(stmt, i, (const char *)(data + *off), (int)len, SQLITE_TRANSIENT);
                *off += len;
                break;
            }
            case 4: {
                uint32_t len = 0;
                if (!read_u32(data, size, off, &len)) {
                    sqlite3_bind_null(stmt, i);
                    break;
                }
                if (*off > size) {
                    sqlite3_bind_null(stmt, i);
                    break;
                }
                len %= (uint32_t)(size - *off + 1);
                sqlite3_bind_blob(stmt, i, data + *off, (int)len, SQLITE_TRANSIENT);
                *off += len;
                break;
            }
            case 5:
            default: {
                sqlite3_bind_zeroblob(stmt, i, (int)((*off < size) ? (data[(*off)++] % 64) : 0));
                break;
            }
        }
    }
}

static void consume_columns(sqlite3_stmt *stmt) {
    int cols = sqlite3_column_count(stmt);
    int i;
    for (i = 0; i < cols; i++) {
        int t = sqlite3_column_type(stmt, i);
        switch (t) {
            case SQLITE_INTEGER:
                (void)sqlite3_column_int64(stmt, i);
                break;
            case SQLITE_FLOAT:
                (void)sqlite3_column_double(stmt, i);
                break;
            case SQLITE_TEXT:
                (void)sqlite3_column_text(stmt, i);
                (void)sqlite3_column_bytes(stmt, i);
                break;
            case SQLITE_BLOB:
                (void)sqlite3_column_blob(stmt, i);
                (void)sqlite3_column_bytes(stmt, i);
                break;
            case SQLITE_NULL:
            default:
                break;
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *sql = NULL;
    const char *tail = NULL;
    size_t off = 0;
    uint32_t sql_len32 = 0;
    int rc;
    int step_budget;
    int reset_count;
    int i;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size) fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_extended_result_codes(db, 1);
    sqlite3_busy_timeout(db, 1);

    exec_sql(db, "PRAGMA temp_store=MEMORY;");
    exec_sql(db, "PRAGMA journal_mode=OFF;");
    exec_sql(db, "CREATE TABLE IF NOT EXISTS t1(a,b,c);");
    exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(x INTEGER PRIMARY KEY, y TEXT);");
    exec_sql(db, "INSERT INTO t1 VALUES(1,'one',x'00');");
    exec_sql(db, "INSERT INTO t1 VALUES(2,'two',x'0102');");
    exec_sql(db, "INSERT OR IGNORE INTO t2 VALUES(1,'alpha');");
    exec_sql(db, "ATTACH DATABASE './dummy_file' AS aux;");

    if (Size == 0) {
        sqlite3_close(db);
        return 0;
    }

    if (!read_u32(Data, Size, &off, &sql_len32)) {
        sql_len32 = (uint32_t)Size;
    }

    {
        size_t remain = (off <= Size) ? (Size - off) : 0;
        size_t sql_len = remain ? (sql_len32 % remain) : 0;
        sql = (char *)malloc(sql_len + 1);
        if (!sql) {
            sqlite3_close(db);
            return 0;
        }
        if (sql_len) memcpy(sql, Data + off, sql_len);
        sql[sql_len] = '\0';
        off += sql_len;

        rc = sqlite3_prepare_v2(db, sql, (int)sql_len, &stmt, &tail);
    }

    if (rc == SQLITE_OK && stmt != NULL) {
        (void)sqlite3_sql(stmt);

        {
            char *expanded = sqlite3_expanded_sql(stmt);
            if (expanded) sqlite3_free(expanded);
        }

        (void)sqlite3_normalized_sql(stmt);

        bind_from_data(stmt, Data, Size, &off);

        {
            char *expanded = sqlite3_expanded_sql(stmt);
            if (expanded) sqlite3_free(expanded);
        }

        step_budget = 1;
        if (off < Size) step_budget += (Data[off++] % 32);

        for (i = 0; i < step_budget; i++) {
            rc = sqlite3_step(stmt);
            if (rc == SQLITE_ROW) {
                consume_columns(stmt);
                continue;
            }
            if (rc == SQLITE_BUSY) {
                continue;
            }
            break;
        }

        (void)sqlite3_sql(stmt);
        {
            char *expanded = sqlite3_expanded_sql(stmt);
            if (expanded) sqlite3_free(expanded);
        }
        (void)sqlite3_normalized_sql(stmt);

        reset_count = 1;
        if (off < Size) reset_count += (Data[off++] % 4);

        for (i = 0; i < reset_count; i++) {
            (void)sqlite3_reset(stmt);
            if (off < Size && (Data[off++] & 1)) {
                sqlite3_clear_bindings(stmt);
                bind_from_data(stmt, Data, Size, &off);
            }

            rc = sqlite3_step(stmt);
            if (rc == SQLITE_ROW) {
                consume_columns(stmt);
                while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                    consume_columns(stmt);
                }
            } else if (rc == SQLITE_BUSY) {
                (void)sqlite3_reset(stmt);
            } else {
                (void)sqlite3_reset(stmt);
            }

            (void)sqlite3_sql(stmt);
            {
                char *expanded = sqlite3_expanded_sql(stmt);
                if (expanded) sqlite3_free(expanded);
            }
            (void)sqlite3_normalized_sql(stmt);
        }
    }

    if (stmt) sqlite3_finalize(stmt);
    free(sql);
    sqlite3_close(db);
    return 0;
}