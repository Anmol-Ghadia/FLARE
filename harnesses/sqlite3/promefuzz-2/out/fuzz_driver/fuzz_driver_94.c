// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_int at sqlite3.c:73041:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_table_name16 at sqlite3.c:73200:24 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_bytes16 at sqlite3.c:73031:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes16 at sqlite3.c:73031:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_int at sqlite3.c:73041:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_table_name16 at sqlite3.c:73200:24 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t **data, size_t *size) {
    uint32_t v = 0;
    size_t n = *size < 4 ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static int exec_sql(sqlite3 *db, const char *sql) {
    char *errmsg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
    }
    return rc;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    int rc;

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_extended_result_codes(db, 1);

    exec_sql(db, "PRAGMA foreign_keys=OFF;");
    exec_sql(db, "CREATE TABLE t(a INTEGER, b TEXT, c BLOB, d REAL, e);");

    {
        sqlite3_stmt *ins = NULL;
        rc = sqlite3_prepare_v2(
            db,
            "INSERT INTO t(a,b,c,d,e) VALUES(?1,?2,?3,?4,?5);",
            -1,
            &ins,
            NULL
        );
        if (rc == SQLITE_OK && ins != NULL) {
            const uint8_t *p = Data;
            size_t rem = Size;

            for (int row = 0; row < 4; ++row) {
                uint32_t v1 = read_u32(&p, &rem);
                uint32_t v2 = read_u32(&p, &rem);
                uint32_t v3 = read_u32(&p, &rem);
                uint32_t v4 = read_u32(&p, &rem);

                sqlite3_reset(ins);
                sqlite3_clear_bindings(ins);

                sqlite3_bind_int(ins, 1, (int)v1);

                if (rem > 0) {
                    int text_len = (int)(v2 % (rem + 1));
                    sqlite3_bind_text(ins, 2, (const char *)p, text_len, SQLITE_TRANSIENT);
                    p += text_len;
                    rem -= text_len;
                } else {
                    sqlite3_bind_null(ins, 2);
                }

                if (rem > 0) {
                    int blob_len = (int)(v3 % (rem + 1));
                    sqlite3_bind_blob(ins, 3, p, blob_len, SQLITE_TRANSIENT);
                    p += blob_len;
                    rem -= blob_len;
                } else {
                    sqlite3_bind_blob(ins, 3, "", 0, SQLITE_STATIC);
                }

                sqlite3_bind_double(ins, 4, (double)(int32_t)v4 / 17.0);

                switch (row & 3) {
                    case 0:
                        sqlite3_bind_null(ins, 5);
                        break;
                    case 1:
                        sqlite3_bind_int64(ins, 5, (sqlite3_int64)(int32_t)v1 * (sqlite3_int64)(int32_t)v4);
                        break;
                    case 2:
                        sqlite3_bind_text(ins, 5, "123xyz", -1, SQLITE_STATIC);
                        break;
                    default:
                        sqlite3_bind_blob(ins, 5, Data, (int)(Size > 16 ? 16 : Size), SQLITE_STATIC);
                        break;
                }

                sqlite3_step(ins);
            }
        }
        if (ins) sqlite3_finalize(ins);
    }

    {
        static const char *queries[] = {
            "SELECT a,b,c,d,e,rowid FROM t;",
            "SELECT b,a,c,e,d FROM t;",
            "SELECT c,b,a FROM t;",
            "SELECT a+1, substr(b,1,4), c, d*2, e FROM t;",
            "SELECT t.a, t.b, t.c FROM t;",
            "SELECT CAST(c AS TEXT), CAST(b AS BLOB), CAST(a AS TEXT) FROM t;",
            "SELECT NULL, a, b, c, d, e FROM t;"
        };
        const uint8_t *p = Data;
        size_t rem = Size;
        uint32_t qidx = read_u32(&p, &rem) % (sizeof(queries) / sizeof(queries[0]));

        rc = sqlite3_prepare_v2(db, queries[qidx], -1, &stmt, NULL);
        if (rc == SQLITE_OK && stmt != NULL) {
            int col_count = sqlite3_column_count(stmt);

            for (int iter = 0; iter < 8; ++iter) {
                rc = sqlite3_step(stmt);
                if (rc != SQLITE_ROW) {
                    break;
                }

                int loops = col_count > 0 ? col_count : 1;
                for (int i = 0; i < loops; ++i) {
                    int idx = i;
                    if (col_count > 0 && rem > 0) {
                        idx = (int)(p[0] % col_count);
                        p++;
                        rem--;
                    }

                    (void)sqlite3_column_count(stmt);
                    (void)sqlite3_column_type(stmt, idx);
                    (void)sqlite3_column_int(stmt, idx);

                    {
                        const void *blob = sqlite3_column_blob(stmt, idx);
                        (void)blob;
                    }

                    {
                        const void *tname16 = sqlite3_column_table_name16(stmt, idx);
                        (void)tname16;
                    }

                    if ((idx & 1) == 0) {
                        const void *txt16 = sqlite3_column_text16(stmt, idx);
                        (void)txt16;
                        (void)sqlite3_column_bytes16(stmt, idx);
                    } else {
                        const void *blob2 = sqlite3_column_blob(stmt, idx);
                        (void)blob2;
                        (void)sqlite3_column_bytes16(stmt, idx);
                    }

                    if (col_count > 0) {
                        int alt = (idx + 1) % col_count;
                        (void)sqlite3_column_type(stmt, alt);
                        (void)sqlite3_column_int(stmt, alt);
                        (void)sqlite3_column_blob(stmt, alt);
                        (void)sqlite3_column_table_name16(stmt, alt);
                    }
                }
            }
        }
    }

    if (stmt) {
        sqlite3_finalize(stmt);
    }
    sqlite3_close(db);
    return 0;
}