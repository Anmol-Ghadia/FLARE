// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_normalized_sql at sqlite3.c:73682:24 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_normalized_sql at sqlite3.c:73682:24 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_normalized_sql at sqlite3.c:73682:24 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static int consume_u8(const uint8_t **data, size_t *size) {
    if (*size == 0) return 0;
    int v = **data;
    (*data)++;
    (*size)--;
    return v;
}

static uint32_t consume_u32(const uint8_t **data, size_t *size) {
    uint32_t v = 0;
    int i;
    for (i = 0; i < 4; i++) {
        v = (v << 8) | (uint32_t)consume_u8(data, size);
    }
    return v;
}

static size_t consume_size_bounded(const uint8_t **data, size_t *size, size_t maxv) {
    if (maxv == 0) return 0;
    return (size_t)(consume_u32(data, size) % (maxv + 1));
}

static char *make_cstring(const uint8_t *data, size_t size) {
    char *s = (char *)malloc(size + 1);
    if (!s) return NULL;
    if (size > 0) memcpy(s, data, size);
    s[size] = '\0';
    return s;
}

static void write_dummy_file(const uint8_t *data, size_t size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (size > 0) fwrite(data, 1, size, fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    sqlite3_stmt *stmt2 = NULL;
    const char *tail = NULL;
    char *sql = NULL;
    char *param_name = NULL;
    int rc;

    write_dummy_file(Data, Size);

    rc = sqlite3_open("./dummy_file", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db,
                 "CREATE TABLE IF NOT EXISTS t(a,b,c);"
                 "INSERT INTO t(a,b,c) VALUES(1,2,3);",
                 NULL, NULL, NULL);

    {
        const uint8_t *p = Data;
        size_t remaining = Size;

        size_t sql_len = consume_size_bounded(&p, &remaining, remaining);
        if (sql_len > remaining) sql_len = remaining;
        sql = make_cstring(p, sql_len);
        if (!sql) {
            sqlite3_close(db);
            return 0;
        }
        p += sql_len;
        remaining -= sql_len;

        rc = sqlite3_prepare_v2(db, sql, (int)sql_len, &stmt, &tail);
        if (rc == SQLITE_OK && stmt != NULL) {
            int i;
            int col_count = sqlite3_column_count(stmt);
            int param_count = sqlite3_bind_parameter_count(stmt);

            (void)sqlite3_sql(stmt);
            (void)sqlite3_normalized_sql(stmt);

            for (i = -2; i < col_count + 3; i++) {
                (void)sqlite3_column_name(stmt, i);
            }

            for (i = -1; i < param_count + 3; i++) {
                (void)sqlite3_bind_parameter_name(stmt, i);
            }

            if (remaining > 0) {
                size_t pname_len = consume_size_bounded(&p, &remaining, remaining);
                if (pname_len > remaining) pname_len = remaining;
                param_name = make_cstring(p, pname_len);
                if (param_name) {
                    (void)sqlite3_bind_parameter_index(stmt, param_name);
                    free(param_name);
                    param_name = NULL;
                }
                p += pname_len;
                remaining -= pname_len;
            } else {
                (void)sqlite3_bind_parameter_index(stmt, ":x");
                (void)sqlite3_bind_parameter_index(stmt, "@y");
                (void)sqlite3_bind_parameter_index(stmt, "$z");
                (void)sqlite3_bind_parameter_index(stmt, "?1");
            }

            for (i = 0; i < 3; i++) {
                int step_rc = sqlite3_step(stmt);
                (void)sqlite3_sql(stmt);
                (void)sqlite3_normalized_sql(stmt);
                if (col_count > 0) {
                    int idx = (int)(consume_u32(&p, &remaining) % (uint32_t)(col_count + 4)) - 2;
                    (void)sqlite3_column_name(stmt, idx);
                } else {
                    (void)sqlite3_column_name(stmt, 0);
                }
                if (step_rc != SQLITE_ROW && step_rc != SQLITE_DONE) {
                    break;
                }
            }

            sqlite3_reset(stmt);
        }

        if (tail && *tail) {
            sqlite3_prepare_v2(db, tail, -1, &stmt2, NULL);
            if (stmt2) {
                int col_count2 = sqlite3_column_count(stmt2);
                int param_count2 = sqlite3_bind_parameter_count(stmt2);
                int i;

                (void)sqlite3_sql(stmt2);
                (void)sqlite3_normalized_sql(stmt2);

                for (i = -1; i < col_count2 + 2; i++) {
                    (void)sqlite3_column_name(stmt2, i);
                }
                for (i = 0; i < param_count2 + 2; i++) {
                    const char *n = sqlite3_bind_parameter_name(stmt2, i);
                    if (n) {
                        (void)sqlite3_bind_parameter_index(stmt2, n);
                    }
                }

                (void)sqlite3_step(stmt2);
            }
        }
    }

    if (stmt2) sqlite3_finalize(stmt2);
    if (stmt) sqlite3_finalize(stmt);
    free(sql);
    sqlite3_close(db);
    return 0;
}