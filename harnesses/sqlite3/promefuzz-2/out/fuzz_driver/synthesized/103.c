// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text64 at sqlite3.c:73396:16 in sqlite3.h
// sqlite3_bind_text64 at sqlite3.c:73396:16 in sqlite3.h
// sqlite3_bind_text64 at sqlite3.c:73396:16 in sqlite3.h
// sqlite3_expanded_sql at sqlite3.c:73662:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_expanded_sql at sqlite3.c:73662:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text64 at sqlite3.c:73396:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
// sqlite3_expanded_sql at sqlite3.c:73662:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4; i++) {
        v <<= 8;
        if (*Off < Size) {
            v |= Data[*Off];
            (*Off)++;
        }
    }
    return v;
}

static int read_int(const uint8_t *Data, size_t Size, size_t *Off) {
    return (int)read_u32(Data, Size, Off);
}

static unsigned char read_u8(const uint8_t *Data, size_t Size, size_t *Off) {
    if (*Off < Size) return Data[(*Off)++];
    return 0;
}

static size_t take_bytes(const uint8_t *Data, size_t Size, size_t *Off, char *Dst, size_t MaxCopy) {
    size_t remain = (*Off < Size) ? (Size - *Off) : 0;
    size_t n = remain < MaxCopy ? remain : MaxCopy;
    if (n > 0) {
        memcpy(Dst, Data + *Off, n);
        *Off += n;
    }
    return n;
}

int LLVMFuzzerTestOneInput_103(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    sqlite3_stmt *stmt2 = NULL;
    char *errmsg = NULL;
    size_t off = 0;
    int rc;

    char *heap1 = NULL;
    char *heap2 = NULL;
    char *heap3 = NULL;
    size_t heap1_sz = 0;
    size_t heap2_sz = 0;
    size_t heap3_sz = 0;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db,
                 "CREATE TABLE t(a TEXT, b TEXT, c TEXT);"
                 "INSERT INTO t(a,b,c) VALUES('x','y','z');",
                 NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    {
        const char *sql =
            "SELECT :p1, @p2, $p3, ?1, ?, a FROM t "
            "WHERE a=?2 OR b=:p1 OR c=@p2;";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK || stmt == NULL) {
            sqlite3_close(db);
            return 0;
        }
    }

    {
        const char *orig = sqlite3_sql(stmt);
        (void)orig;
    }

    {
        int param_count = sqlite3_bind_parameter_count(stmt);
        int i;
        for (i = 0; i <= param_count + 2; i++) {
            const char *pname = sqlite3_bind_parameter_name(stmt, i);
            (void)pname;
        }
    }

    {
        char tmp1[256];
        char tmp2[256];
        size_t n1 = take_bytes(Data, Size, &off, tmp1, sizeof(tmp1) - 1);
        size_t n2 = take_bytes(Data, Size, &off, tmp2, sizeof(tmp2) - 1);
        sqlite3_uint64 len64;
        int len32;
        int idx1, idx2;
        unsigned char enc;

        tmp1[n1] = '\0';
        tmp2[n2] = '\0';

        heap1_sz = n1 + 1;
        heap2_sz = n2 + 1;
        heap1 = (char *)malloc(heap1_sz);
        heap2 = (char *)malloc(heap2_sz);
        if (!heap1 || !heap2) {
            free(heap1);
            free(heap2);
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return 0;
        }
        memcpy(heap1, tmp1, heap1_sz);
        memcpy(heap2, tmp2, heap2_sz);

        idx1 = (read_int(Data, Size, &off) % 10) - 2;
        idx2 = (read_int(Data, Size, &off) % 10) - 2;
        len32 = read_int(Data, Size, &off);
        enc = read_u8(Data, Size, &off);

        if ((len32 < -1) || ((size_t)len32 > n1 && len32 >= 0)) {
            if (read_u8(Data, Size, &off) & 1) {
                len32 = -1;
            } else {
                len32 = (int)n1;
            }
        }

        len64 = (sqlite3_uint64)read_u32(Data, Size, &off);
        if (len64 > n2) {
            if (read_u8(Data, Size, &off) & 1) {
                len64 = (sqlite3_uint64)n2;
            }
        }

        sqlite3_bind_text(stmt, idx1, heap1, len32, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 1, heap1, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text64(stmt, idx2, heap2, len64, SQLITE_TRANSIENT, enc);
        sqlite3_bind_text64(stmt, 2, heap2, (sqlite3_uint64)n2, SQLITE_TRANSIENT, SQLITE_UTF8);
        sqlite3_bind_text64(stmt, 3, heap2, 0, SQLITE_TRANSIENT, SQLITE_UTF16);
    }

    {
        char *expanded = sqlite3_expanded_sql(stmt);
        if (expanded) {
            sqlite3_free(expanded);
        }
    }

    rc = sqlite3_step(stmt);
    (void)rc;
    sqlite3_reset(stmt);

    {
        char *expanded = sqlite3_expanded_sql(stmt);
        if (expanded) {
            sqlite3_free(expanded);
        }
    }

    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);

    {
        const char *sql2 = "INSERT INTO t(a,b,c) VALUES(?1,:x,@y);";
        rc = sqlite3_prepare_v2(db, sql2, -1, &stmt2, NULL);
        if (rc == SQLITE_OK && stmt2 != NULL) {
            const char *orig2 = sqlite3_sql(stmt2);
            char *expanded2;
            const char *n1;
            const char *n2;
            const char *n3;
            char tmp3[64];
            size_t m1;

            (void)orig2;

            off = 0;
            m1 = take_bytes(Data, Size, &off, tmp3, sizeof(tmp3) - 1);
            tmp3[m1] = '\0';

            heap3_sz = m1 + 1;
            heap3 = (char *)malloc(heap3_sz);
            if (heap3) {
                memcpy(heap3, tmp3, heap3_sz);

                sqlite3_bind_text(stmt2, 1, heap3, -1, SQLITE_TRANSIENT);
                sqlite3_bind_text64(stmt2, 2, heap3, (sqlite3_uint64)m1, SQLITE_TRANSIENT, SQLITE_UTF8);
                sqlite3_bind_text(stmt2, 3, heap3, (int)m1, SQLITE_TRANSIENT);

                n1 = sqlite3_bind_parameter_name(stmt2, 1);
                n2 = sqlite3_bind_parameter_name(stmt2, 2);
                n3 = sqlite3_bind_parameter_name(stmt2, 3);
                (void)n1;
                (void)n2;
                (void)n3;

                expanded2 = sqlite3_expanded_sql(stmt2);
                if (expanded2) sqlite3_free(expanded2);

                rc = sqlite3_step(stmt2);
                (void)rc;
                sqlite3_reset(stmt2);
            }

            sqlite3_finalize(stmt2);
        }
    }

    free(heap1);
    free(heap2);
    free(heap3);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}