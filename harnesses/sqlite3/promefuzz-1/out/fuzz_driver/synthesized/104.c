// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_normalized_sql at sqlite3.c:73682:24 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
// sqlite3_expanded_sql at sqlite3.c:73662:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_normalized_sql at sqlite3.c:73682:24 in sqlite3.h
// sqlite3_expanded_sql at sqlite3.c:73662:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_normalized_sql at sqlite3.c:73682:24 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_expanded_sql at sqlite3.c:73662:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
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
#include <stdio.h>

#include "sqlite3.h"

static void bind_from_data(sqlite3_stmt *stmt, const uint8_t *data, size_t size) {
    int nparam, i;
    size_t off = 0;

    if (!stmt) return;

    nparam = sqlite3_bind_parameter_count(stmt);
    for (i = 1; i <= nparam; i++) {
        unsigned char tag = 0;
        int len = 0;
        sqlite3_int64 v64 = 0;
        double d = 0.0;

        if (off < size) tag = data[off++];

        switch (tag % 6) {
            case 0:
                sqlite3_bind_null(stmt, i);
                break;
            case 1:
                if (off + sizeof(v64) <= size) {
                    memcpy(&v64, data + off, sizeof(v64));
                    off += sizeof(v64);
                } else if (off < size) {
                    memcpy(&v64, data + off, size - off);
                    off = size;
                }
                sqlite3_bind_int64(stmt, i, v64);
                break;
            case 2:
                if (off + sizeof(d) <= size) {
                    memcpy(&d, data + off, sizeof(d));
                    off += sizeof(d);
                } else if (off < size) {
                    memcpy(&d, data + off, size - off);
                    off = size;
                }
                sqlite3_bind_double(stmt, i, d);
                break;
            case 3:
                if (off < size) {
                    len = (int)(data[off++] % (size - off + 1));
                }
                sqlite3_bind_text(stmt, i, (const char *)(data + off), len, SQLITE_TRANSIENT);
                off += (size_t)len;
                break;
            case 4:
                if (off < size) {
                    len = (int)(data[off++] % (size - off + 1));
                }
                sqlite3_bind_blob(stmt, i, data + off, len, SQLITE_TRANSIENT);
                off += (size_t)len;
                break;
            default:
                if (off < size) {
                    len = (int)(data[off++] % (size - off + 1));
                }
                sqlite3_bind_text(stmt, i, (const char *)(data + off), len, SQLITE_STATIC);
                off += (size_t)len;
                break;
        }
    }
}

int LLVMFuzzerTestOneInput_104(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *sqlbuf = NULL;
    const char *tail = NULL;
    const char *orig_sql;
    const char *norm_sql;
    char *expanded_sql;
    int rc;
    int i;
    int step_count = 0;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size) fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    if (sqlite3_open(":memory:", &db) != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db,
                 "CREATE TABLE IF NOT EXISTS t(a,b,c);"
                 "INSERT INTO t(a,b,c) VALUES(1,'x',X'00');"
                 "CREATE TABLE IF NOT EXISTS u(x);",
                 NULL, NULL, NULL);

    sqlbuf = (char *)malloc(Size + 1);
    if (!sqlbuf) {
        sqlite3_close(db);
        return 0;
    }
    if (Size) memcpy(sqlbuf, Data, Size);
    sqlbuf[Size] = '\0';

    rc = sqlite3_prepare_v2(db, sqlbuf, (int)Size, &stmt, &tail);
    if (rc != SQLITE_OK || stmt == NULL) {
        sqlite3_prepare_v2(db, sqlbuf, (int)Size + 1, &stmt, &tail);
    }

    if (stmt) {
        orig_sql = sqlite3_sql(stmt);
        (void)orig_sql;

        norm_sql = sqlite3_normalized_sql(stmt);
        (void)norm_sql;

        bind_from_data(stmt, Data, Size);

        for (i = -2; i <= sqlite3_bind_parameter_count(stmt) + 2; i++) {
            const char *pname = sqlite3_bind_parameter_name(stmt, i);
            (void)pname;
        }

        expanded_sql = sqlite3_expanded_sql(stmt);
        if (expanded_sql) {
            sqlite3_free(expanded_sql);
        }

        for (i = 0; i < 3; i++) {
            rc = sqlite3_step(stmt);
            step_count++;
            if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
                break;
            }
            if (rc == SQLITE_DONE) {
                break;
            }
        }

        rc = sqlite3_reset(stmt);
        (void)rc;

        orig_sql = sqlite3_sql(stmt);
        (void)orig_sql;

        norm_sql = sqlite3_normalized_sql(stmt);
        (void)norm_sql;

        expanded_sql = sqlite3_expanded_sql(stmt);
        if (expanded_sql) {
            sqlite3_free(expanded_sql);
        }

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);

        sqlite3_finalize(stmt);
    }

    sqlite3_normalized_sql(NULL);
    sqlite3_sql(NULL);
    sqlite3_bind_parameter_name(NULL, 1);
    sqlite3_reset(NULL);
    expanded_sql = sqlite3_expanded_sql(NULL);
    if (expanded_sql) {
        sqlite3_free(expanded_sql);
    }

    free(sqlbuf);
    sqlite3_close(db);
    (void)step_count;
    return 0;
}