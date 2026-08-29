// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

static uint32_t consume_u32(const uint8_t **data, size_t *size) {
    uint32_t v = 0;
    size_t n = *size < 4 ? *size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | (*data)[i];
    }
    *data += n;
    *size -= n;
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t remaining = Size;
    sqlite3 *db = NULL;
    char *formatted = NULL;
    const char *errmsg = NULL;
    int ec, eec;
    uint32_t mode = 0;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    if (remaining > 0) {
        mode = consume_u32(&p, &remaining);
    }

    switch (mode % 4U) {
        case 0: {
            int rc = sqlite3_open("./dummy_file", &db);
            if (rc == SQLITE_OK && db != NULL && remaining > 0) {
                char *name = sqlite3_mprintf("%.*w", (int)remaining, (const char *)p);
                if (name != NULL) {
                    char *sql = sqlite3_mprintf("SELECT * FROM \"%s\";", name);
                    if (sql != NULL) {
                        char *zErr = NULL;
                        (void)sqlite3_exec(db, sql, NULL, NULL, &zErr);
                        if (zErr != NULL) sqlite3_free(zErr);
                        sqlite3_free(sql);
                    }
                    sqlite3_free(name);
                }
            }
            break;
        }
        case 1: {
            int rc = sqlite3_open(":memory:", &db);
            if (rc == SQLITE_OK && db != NULL) {
                char *sql = sqlite3_mprintf("%.*s", (int)remaining, (const char *)p);
                if (sql != NULL) {
                    sqlite3_stmt *stmt = NULL;
                    int prc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
                    if (prc == SQLITE_OK && stmt != NULL) {
                        (void)sqlite3_step(stmt);
                    }
                    if (stmt != NULL) sqlite3_finalize(stmt);
                    sqlite3_free(sql);
                }
            }
            break;
        }
        case 2: {
            (void)sqlite3_open_v2("./dummy_file", &db, SQLITE_OPEN_READONLY, NULL);
            break;
        }
        default: {
            int rc = sqlite3_open(":memory:", &db);
            if (rc == SQLITE_OK && db != NULL) {
                (void)sqlite3_exec(db, "CREATE TABLE t(x);", NULL, NULL, NULL);
                if (remaining > 0) {
                    char *escaped = sqlite3_mprintf("%.*q", (int)remaining, (const char *)p);
                    if (escaped != NULL) {
                        char *sql = sqlite3_mprintf("INSERT INTO t(x) VALUES('%s');", escaped);
                        if (sql != NULL) {
                            (void)sqlite3_exec(db, sql, NULL, NULL, NULL);
                            sqlite3_free(sql);
                        }
                        sqlite3_free(escaped);
                    }
                }
                (void)sqlite3_exec(db, "SELECT * FROM no_such_table;", NULL, NULL, NULL);
            }
            break;
        }
    }

    ec = sqlite3_errcode(db);
    formatted = sqlite3_mprintf("ec=%d size=%d data=%.*q",
                                ec,
                                (int)remaining,
                                (int)remaining,
                                (const char *)p);
    errmsg = sqlite3_errmsg(db);
    eec = sqlite3_extended_errcode(db);

    if (formatted != NULL) {
        volatile size_t sink = strlen(formatted);
        (void)sink;
        sqlite3_free(formatted);
    }

    if (errmsg != NULL) {
        volatile unsigned char c = (unsigned char)errmsg[0];
        (void)c;
    }

    (void)ec;
    (void)eec;

    if (db != NULL) {
        sqlite3_close(db);
    }

    return 0;
}