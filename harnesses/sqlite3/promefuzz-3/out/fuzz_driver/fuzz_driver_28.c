// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static int consume_u32(const uint8_t **data, size_t *size) {
    int v = 0;
    size_t n = *size < 4 ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;

    int rc = sqlite3_open("./dummy_file", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    sqlite3_exec(db,
                 "CREATE TABLE IF NOT EXISTS t("
                 "id INTEGER PRIMARY KEY, "
                 "x TEXT"
                 ");"
                 "INSERT INTO t(x) VALUES('seed');",
                 NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    const uint8_t *p = Data;
    size_t remaining = Size;

    const char *sqls[] = {
        "INSERT INTO t(x) VALUES(?1);",
        "UPDATE t SET x=?1 WHERE id=1;",
        "DELETE FROM t WHERE x=?1;",
        "SELECT x FROM t WHERE x=?1;",
        "SELECT ?1;",
        "INSERT INTO t(x) VALUES(?1), (?1);"
    };
    int sql_index = 0;
    if (remaining > 0) {
        sql_index = p[0] % (int)(sizeof(sqls) / sizeof(sqls[0]));
        p++;
        remaining--;
    }

    rc = sqlite3_prepare_v2(db, sqls[sql_index], -1, &stmt, NULL);
    if (rc != SQLITE_OK || stmt == NULL) {
        (void)sqlite3_errmsg(db);
        sqlite3_close(db);
        return 0;
    }

    (void)sqlite3_reset(stmt);

    int bind_index = 1;
    int bind_len = 0;
    if (remaining > 0) {
        bind_len = (int)(p[0] % (remaining + 1));
        p++;
        remaining--;
        if ((size_t)bind_len > remaining) {
            bind_len = (int)remaining;
        }
    }

    const char *bind_ptr = (const char *)p;
    if (remaining == 0) {
        bind_ptr = "";
        bind_len = 0;
    }

    (void)sqlite3_bind_text(stmt, bind_index, bind_ptr, bind_len, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    (void)rc;

    (void)sqlite3_errmsg(db);

    (void)sqlite3_reset(stmt);

    (void)sqlite3_changes(db);

    if (remaining > 0) {
        int loops = (consume_u32(&p, &remaining) % 4);
        for (int i = 0; i < loops; ++i) {
            int loop_len = (remaining > 0) ? (int)remaining : 0;
            const char *loop_ptr = (remaining > 0) ? (const char *)p : "";
            (void)sqlite3_bind_text(stmt, 1, loop_ptr, loop_len, SQLITE_TRANSIENT);
            rc = sqlite3_step(stmt);
            (void)rc;
            (void)sqlite3_errmsg(db);
            (void)sqlite3_reset(stmt);
            (void)sqlite3_changes(db);
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}