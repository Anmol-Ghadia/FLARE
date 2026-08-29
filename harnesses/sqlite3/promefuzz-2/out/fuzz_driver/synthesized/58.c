// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
// sqlite3_bind_parameter_index at sqlite3.c:73505:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

static uint64_t read_u64(const uint8_t *data, size_t size, size_t *off) {
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8 && *off < size; ++i, ++(*off)) {
        v = (v << 8) | data[*off];
    }
    return v;
}

static int read_int(const uint8_t *data, size_t size, size_t *off) {
    if (*off >= size) return 0;
    return (int)data[(*off)++];
}

static void write_dummy_file(const uint8_t *data, size_t size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (size > 0) {
        fwrite(data, 1, size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput_58(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    const char *tail = NULL;
    char *sql = NULL;
    int rc;
    size_t off = 0;
    int open_flags;
    int nByte;
    int step_count = 0;
    int i;

    write_dummy_file(Data, Size);

    open_flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    if (Size > 0 && (Data[0] & 1)) {
        open_flags |= SQLITE_OPEN_URI;
    }

    rc = sqlite3_open_v2(":memory:", &db, open_flags, NULL);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db,
                 "CREATE TABLE IF NOT EXISTS t(a INTEGER, b INTEGER, c TEXT);"
                 "INSERT INTO t VALUES(1,2,'x');"
                 "INSERT INTO t VALUES(3,4,'y');",
                 NULL, NULL, NULL);

    sql = (char *)sqlite3_malloc64(Size + 1);
    if (!sql) {
        sqlite3_close(db);
        return 0;
    }
    if (Size > 0) {
        memcpy(sql, Data, Size);
    }
    sql[Size] = '\0';

    nByte = (Size > 0 && (Data[0] & 2)) ? (int)Size : -1;
    rc = sqlite3_prepare_v2(db, sql, nByte, &stmt, &tail);

    if (stmt) {
        int count = sqlite3_bind_parameter_count(stmt);

        (void)sqlite3_bind_parameter_count(NULL);

        for (i = -2; i <= count + 2; ++i) {
            const char *name = sqlite3_bind_parameter_name(stmt, i);
            if (name) {
                (void)sqlite3_bind_parameter_index(stmt, name);
            }
        }

        if (count > 0) {
            for (i = 0; i < count + 3; ++i) {
                int idx;
                sqlite3_int64 val;

                if ((Size > 0) && (Data[0] & 4)) {
                    idx = i;
                } else {
                    idx = (read_int(Data, Size, &off) % (count + 4)) - 1;
                }

                val = (sqlite3_int64)read_u64(Data, Size, &off);
                (void)sqlite3_bind_int64(stmt, idx, val);
            }

            for (i = 1; i <= count; ++i) {
                const char *name = sqlite3_bind_parameter_name(stmt, i);
                if (name) {
                    int idx = sqlite3_bind_parameter_index(stmt, name);
                    sqlite3_int64 val = (sqlite3_int64)read_u64(Data, Size, &off);
                    if (idx > 0) {
                        (void)sqlite3_bind_int64(stmt, idx, val);
                    }
                }
            }

            (void)sqlite3_bind_parameter_index(stmt, ":nonexistent");
            (void)sqlite3_bind_parameter_index(stmt, "@missing");
            (void)sqlite3_bind_parameter_index(stmt, "$x");
            (void)sqlite3_bind_parameter_index(stmt, "?1");
        }

        step_count = (Size > 1) ? (Data[1] % 8) : 2;
        for (i = 0; i < step_count; ++i) {
            rc = sqlite3_step(stmt);
            if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
                break;
            }
        }

        (void)sqlite3_reset(stmt);

        if (count > 0) {
            for (i = 1; i <= count; ++i) {
                sqlite3_int64 val = (sqlite3_int64)read_u64(Data, Size, &off);
                (void)sqlite3_bind_int64(stmt, i, val);
            }
        }

        step_count = (Size > 2) ? (Data[2] % 8) : 2;
        for (i = 0; i < step_count; ++i) {
            rc = sqlite3_step(stmt);
            if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
                break;
            }
        }

        (void)sqlite3_reset(stmt);
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    if (tail && *tail) {
        sqlite3_stmt *stmt2 = NULL;
        rc = sqlite3_prepare_v2(db, tail, -1, &stmt2, NULL);
        if (stmt2) {
            int count2 = sqlite3_bind_parameter_count(stmt2);
            for (i = 1; i <= count2; ++i) {
                const char *name = sqlite3_bind_parameter_name(stmt2, i);
                if (name) {
                    (void)sqlite3_bind_parameter_index(stmt2, name);
                }
                (void)sqlite3_bind_int64(stmt2, i, (sqlite3_int64)read_u64(Data, Size, &off));
            }
            (void)sqlite3_step(stmt2);
            (void)sqlite3_reset(stmt2);
            sqlite3_finalize(stmt2);
        }
    }

    sqlite3_free(sql);
    sqlite3_close(db);
    return 0;
}