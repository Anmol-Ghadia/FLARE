// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_parameter_name at sqlite3.c:73490:24 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
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

static uint64_t read_u64(const uint8_t **data, size_t *size) {
    uint64_t v = 0;
    size_t n = *size < 8 ? *size : 8;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static int read_i32(const uint8_t **data, size_t *size) {
    int v = 0;
    size_t n = *size < 4 ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

static char *make_sql(const uint8_t *data, size_t size) {
    static const char *templates[] = {
        "SELECT ?1, :a, @b, $c, ?5;",
        "SELECT ?, ?, ?;",
        "SELECT :x, :x, ?2, @y, $z;",
        "INSERT INTO t(a,b,c) VALUES(?1,:name,@v);",
        "UPDATE t SET a=?3 WHERE b=:k AND c=@m;",
        "SELECT $p, ?10, :q;"
    };
    size_t idx = 0;
    if (size > 0) {
        idx = data[0] % (sizeof(templates) / sizeof(templates[0]));
    }
    return sqlite3_mprintf("%s", templates[idx]);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t remaining = Size;
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *sql = NULL;
    char *formatted = NULL;
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
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db, "CREATE TABLE t(a,b,c);", NULL, NULL, NULL);

    sql = make_sql(p, remaining);
    if (sql == NULL) {
        sqlite3_close(db);
        return 0;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt != NULL) {
        int count = sqlite3_bind_parameter_count(stmt);

        int idx = 1;
        if (count > 0) {
            int chooser = read_i32(&p, &remaining);
            if (chooser < 0) chooser = -chooser;
            idx = (chooser % (count + 2)) + 1;
        } else {
            idx = 1;
        }

        const char *name = sqlite3_bind_parameter_name(stmt, idx);

        sqlite3_int64 value = (sqlite3_int64)read_u64(&p, &remaining);
        (void)sqlite3_bind_int64(stmt, idx, value);

        formatted = sqlite3_mprintf("cnt=%d idx=%d name=%s val=%lld sql=%s",
                                    count,
                                    idx,
                                    name ? name : "(null)",
                                    (long long)value,
                                    sql ? sql : "(null)");
        if (formatted != NULL) {
            sqlite3_free(formatted);
            formatted = NULL;
        }

        if (count > 0) {
            for (int i = 1; i <= count; ++i) {
                const char *n2 = sqlite3_bind_parameter_name(stmt, i);
                sqlite3_int64 v2 = (sqlite3_int64)read_u64(&p, &remaining);
                (void)n2;
                (void)sqlite3_bind_int64(stmt, i, v2);

                formatted = sqlite3_mprintf("loop i=%d n=%s v=%lld",
                                            i,
                                            n2 ? n2 : "(null)",
                                            (long long)v2);
                if (formatted != NULL) {
                    sqlite3_free(formatted);
                    formatted = NULL;
                }
            }
        }
    }

    if (stmt) {
        sqlite3_finalize(stmt);
    }
    sqlite3_free(sql);
    sqlite3_close(db);
    return 0;
}