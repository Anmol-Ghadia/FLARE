// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_blob64 at sqlite3.c:73327:16 in sqlite3.h
// sqlite3_bind_zeroblob64 at sqlite3.c:73460:16 in sqlite3.h
// sqlite3_bind_blob64 at sqlite3.c:73327:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_stmt_status at sqlite3.c:73618:16 in sqlite3.h
// sqlite3_stmt_status at sqlite3.c:73618:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_zeroblob64 at sqlite3.c:73460:16 in sqlite3.h
// sqlite3_bind_blob64 at sqlite3.c:73327:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
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

static size_t take_bytes(const uint8_t **p, size_t *n, size_t want) {
    size_t got = (*n < want) ? *n : want;
    *p += got;
    *n -= got;
    return got;
}

int LLVMFuzzerTestOneInput_97(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t n = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    sqlite3 *db = NULL;
    if (sqlite3_open(":memory:", &db) != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db,
                 "CREATE TABLE t(a INTEGER, b BLOB, c TEXT);"
                 "INSERT INTO t VALUES(1, X'', '');"
                 "INSERT INTO t VALUES(2, X'414243', 'abc');"
                 "INSERT INTO t VALUES(-3, X'00010203', 'def');",
                 NULL, NULL, NULL);

    sqlite3_stmt *stmt = NULL;
    const char *sqls[] = {
        "SELECT ?1, ?2, ?3;",
        "SELECT a, b, c FROM t WHERE a=?1 OR length(b)=?1 OR c=?3;",
        "INSERT INTO t(a,b,c) VALUES(?1, ?2, ?3);",
        "SELECT b FROM t WHERE rowid=?1;",
        "SELECT zeroblob(?1), ?2, ?3;"
    };
    size_t sql_count = sizeof(sqls) / sizeof(sqls[0]);
    const char *sql = sqls[(n ? p[0] : 0) % sql_count];
    if (n) {
        p++;
        n--;
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK || stmt == NULL) {
        sqlite3_close(db);
        return 0;
    }

    int param_count = sqlite3_bind_parameter_count(stmt);
    for (int i = 1; i <= param_count; ++i) {
        uint32_t choice = read_u32(&p, &n) % 4;
        if (choice == 0) {
            int v = read_i32(&p, &n);
            sqlite3_bind_int(stmt, i, v);
        } else if (choice == 1) {
            uint64_t blob_len = read_u64(&p, &n);
            size_t avail = n;
            size_t actual = (size_t)(blob_len % (avail + 1));
            const void *blob_ptr = p;
            take_bytes(&p, &n, actual);
            sqlite3_bind_blob64(stmt, i, blob_ptr, (sqlite3_uint64)actual, SQLITE_TRANSIENT);
        } else if (choice == 2) {
            uint64_t zlen = read_u64(&p, &n);
            zlen %= 4096;
            sqlite3_bind_zeroblob64(stmt, i, (sqlite3_uint64)zlen);
        } else {
            size_t tlen = n ? (read_u32(&p, &n) % (n + 1)) : 0;
            const char *txt = (const char *)p;
            take_bytes(&p, &n, tlen);
            sqlite3_bind_blob64(stmt, i, txt, (sqlite3_uint64)tlen, SQLITE_TRANSIENT);
        }
    }

    for (int iter = 0; iter < 3; ++iter) {
        int rc;
        int stepped = 0;
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            stepped = 1;
            int cols = sqlite3_column_count(stmt);
            if (cols > 0) {
                int idx = cols ? (int)(read_u32(&p, &n) % (uint32_t)cols) : 0;
                const void *blob = sqlite3_column_blob(stmt, idx);
                int bytes = sqlite3_column_bytes(stmt, idx);
                (void)blob;
                (void)bytes;
                (void)sqlite3_errcode(db);

                if (cols > 1) {
                    int idx2 = (idx + 1) % cols;
                    const void *blob2 = sqlite3_column_blob(stmt, idx2);
                    int bytes2 = sqlite3_column_bytes(stmt, idx2);
                    (void)blob2;
                    (void)bytes2;
                    (void)sqlite3_errcode(db);
                }
            }

            int ops[] = {
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
            int opcount = (int)(sizeof(ops) / sizeof(ops[0]));
            if (opcount > 0) {
                int op = ops[read_u32(&p, &n) % (uint32_t)opcount];
                if (op != SQLITE_STMTSTATUS_MEMUSED) {
                    (void)sqlite3_stmt_status(stmt, op, read_u32(&p, &n) & 1);
                }
            }
        }

        if (!stepped && rc != SQLITE_ROW) {
            int ops[] = {
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
            };
            int opcount = (int)(sizeof(ops) / sizeof(ops[0]));
            if (opcount > 0) {
                int op = ops[read_u32(&p, &n) % (uint32_t)opcount];
                (void)sqlite3_stmt_status(stmt, op, read_u32(&p, &n) & 1);
            }
        }

        (void)sqlite3_reset(stmt);

        if (iter < 2) {
            for (int i = 1; i <= param_count; ++i) {
                uint32_t choice = read_u32(&p, &n) % 3;
                if (choice == 0) {
                    sqlite3_bind_int(stmt, i, read_i32(&p, &n));
                } else if (choice == 1) {
                    uint64_t zlen = read_u64(&p, &n) % 4096;
                    sqlite3_bind_zeroblob64(stmt, i, (sqlite3_uint64)zlen);
                } else {
                    size_t avail = n;
                    size_t len = avail ? (read_u32(&p, &n) % (avail + 1)) : 0;
                    const void *blob_ptr = p;
                    take_bytes(&p, &n, len);
                    sqlite3_bind_blob64(stmt, i, blob_ptr, (sqlite3_uint64)len, SQLITE_TRANSIENT);
                }
            }
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}