// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_bind_zeroblob64 at sqlite3.c:73460:16 in sqlite3.h
// sqlite3_bind_blob64 at sqlite3.c:73327:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_stmt_status at sqlite3.c:73618:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_stmt_status at sqlite3.c:73618:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_blob64 at sqlite3.c:73327:16 in sqlite3.h
// sqlite3_bind_zeroblob64 at sqlite3.c:73460:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_stmt_status at sqlite3.c:73618:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_bind_zeroblob64 at sqlite3.c:73460:16 in sqlite3.h
// sqlite3_bind_blob64 at sqlite3.c:73327:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
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

static uint32_t read_u32(const uint8_t **p, size_t *n) {
    uint32_t v = 0;
    size_t take = *n < 4 ? *n : 4;
    for (size_t i = 0; i < take; ++i) {
        v = (v << 8) | (*p)[i];
    }
    *p += take;
    *n -= take;
    return v;
}

static uint64_t read_u64(const uint8_t **p, size_t *n) {
    uint64_t v = 0;
    size_t take = *n < 8 ? *n : 8;
    for (size_t i = 0; i < take; ++i) {
        v = (v << 8) | (*p)[i];
    }
    *p += take;
    *n -= take;
    return v;
}

static int read_i32(const uint8_t **p, size_t *n) {
    return (int)read_u32(p, n);
}

int LLVMFuzzerTestOneInput_71(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    sqlite3_stmt *stmt2 = NULL;
    char *errmsg = NULL;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    if (sqlite3_open(":memory:", &db) != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db,
                 "CREATE TABLE t(a BLOB, b BLOB, c TEXT, d INTEGER);"
                 "INSERT INTO t VALUES(x'00', x'01', 'x', 1);",
                 NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    const char *sql1 =
        "INSERT INTO t(a,b,c,d) VALUES(?1, ?2, :name, ?5);";
    if (sqlite3_prepare_v2(db, sql1, -1, &stmt, NULL) != SQLITE_OK) {
        sqlite3_close(db);
        return 0;
    }

    const uint8_t *p = Data;
    size_t n = Size;

    int param_count = sqlite3_bind_parameter_count(stmt);

    int idx1 = param_count > 0 ? ((read_i32(&p, &n) % (param_count + 2)) - 1) : 1;
    int idx2 = param_count > 0 ? ((read_i32(&p, &n) % (param_count + 2)) - 1) : 2;
    int idx3 = param_count > 0 ? ((read_i32(&p, &n) % (param_count + 2)) - 1) : 3;
    int idx4 = param_count > 0 ? ((read_i32(&p, &n) % (param_count + 2)) - 1) : 4;

    int zero_n = (int)(read_u32(&p, &n) % 1024);
    sqlite3_uint64 zero64_n = (sqlite3_uint64)(read_u64(&p, &n) % 4096);

    size_t blob_len = n > 512 ? 512 : n;
    sqlite3_uint64 blob64_len = (sqlite3_uint64)blob_len;

    (void)sqlite3_bind_null(stmt, idx1);
    (void)sqlite3_bind_zeroblob(stmt, idx2, zero_n);
    (void)sqlite3_bind_zeroblob64(stmt, idx3, zero64_n);
    (void)sqlite3_bind_blob64(stmt, idx4, p, blob64_len, SQLITE_STATIC);

    (void)sqlite3_step(stmt);
    (void)sqlite3_reset(stmt);

    if (param_count > 0) {
        for (int i = 1; i <= param_count; ++i) {
            switch ((i + (int)(Size & 3)) & 3) {
                case 0:
                    (void)sqlite3_bind_null(stmt, i);
                    break;
                case 1:
                    (void)sqlite3_bind_zeroblob(stmt, i, (int)((Size + (size_t)i) % 256));
                    break;
                case 2:
                    (void)sqlite3_bind_zeroblob64(stmt, i, (sqlite3_uint64)((Size + (size_t)i) % 1024));
                    break;
                default:
                    (void)sqlite3_bind_blob64(stmt, i, Data, (sqlite3_uint64)(Size > 256 ? 256 : Size), SQLITE_STATIC);
                    break;
            }
        }
        (void)sqlite3_step(stmt);
        (void)sqlite3_reset(stmt);
    }

    {
        static const int ops[] = {
#ifdef SQLITE_STMTSTATUS_FULLSCAN_STEP
            SQLITE_STMTSTATUS_FULLSCAN_STEP,
#endif
#ifdef SQLITE_STMTSTATUS_SORT
            SQLITE_STMTSTATUS_SORT,
#endif
#ifdef SQLITE_STMTSTATUS_AUTOINDEX
            SQLITE_STMTSTATUS_AUTOINDEX,
#endif
#ifdef SQLITE_STMTSTATUS_VM_STEP
            SQLITE_STMTSTATUS_VM_STEP,
#endif
#ifdef SQLITE_STMTSTATUS_REPREPARE
            SQLITE_STMTSTATUS_REPREPARE,
#endif
#ifdef SQLITE_STMTSTATUS_RUN
            SQLITE_STMTSTATUS_RUN,
#endif
#ifdef SQLITE_STMTSTATUS_FILTER_MISS
            SQLITE_STMTSTATUS_FILTER_MISS,
#endif
#ifdef SQLITE_STMTSTATUS_FILTER_HIT
            SQLITE_STMTSTATUS_FILTER_HIT,
#endif
#ifdef SQLITE_STMTSTATUS_MEMUSED
            SQLITE_STMTSTATUS_MEMUSED,
#endif
        };
        size_t ops_count = sizeof(ops) / sizeof(ops[0]);
        if (ops_count > 0) {
            for (size_t i = 0; i < ops_count; ++i) {
#ifdef SQLITE_STMTSTATUS_MEMUSED
                if (ops[i] == SQLITE_STMTSTATUS_MEMUSED) {
                    continue;
                }
#endif
                (void)sqlite3_stmt_status(stmt, ops[i], (int)(i & 1));
            }
        }
    }

    (void)sqlite3_bind_parameter_count(NULL);
    (void)sqlite3_stmt_status(NULL, 0, 0);

    const char *sql2 = "SELECT ?1, ?2, ?3, ?4, ?5;";
    if (sqlite3_prepare_v2(db, sql2, -1, &stmt2, NULL) == SQLITE_OK && stmt2) {
        int pc2 = sqlite3_bind_parameter_count(stmt2);
        for (int i = 1; i <= pc2; ++i) {
            if ((i & 1) == 0) {
                (void)sqlite3_bind_blob64(stmt2, i, Data, (sqlite3_uint64)(Size > 128 ? 128 : Size), SQLITE_STATIC);
            } else {
                (void)sqlite3_bind_zeroblob64(stmt2, i, (sqlite3_uint64)((Size + (size_t)i) % 2048));
            }
        }
        (void)sqlite3_step(stmt2);
        (void)sqlite3_reset(stmt2);

#ifdef SQLITE_STMTSTATUS_MEMUSED
        (void)sqlite3_stmt_status(stmt2, SQLITE_STMTSTATUS_MEMUSED, 0);
        stmt2 = NULL;
#endif
    }

    if (stmt2) {
        sqlite3_finalize(stmt2);
        stmt2 = NULL;
    }

    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    sqlite3_close(db);
    return 0;
}