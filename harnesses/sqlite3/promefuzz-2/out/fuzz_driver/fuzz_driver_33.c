// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_blob_open at sqlite3.c:82675:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
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

static sqlite3_int64 read_i64(const uint8_t *p, size_t n) {
    sqlite3_int64 v = 0;
    size_t i;
    size_t limit = n < 8 ? n : 8;
    for (i = 0; i < limit; ++i) {
        v = (v << 8) | p[i];
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    sqlite3_blob *blob = NULL;
    char *errmsg = NULL;
    char *mp1 = NULL;
    char *mp2 = NULL;
    char *sql = NULL;
    char *table = NULL;
    char *column = NULL;
    const char *db_name = "main";
    sqlite3_int64 rowid = 1;
    int flags = 0;
    FILE *fp;
    int rc;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open("./dummy_file", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db != NULL) {
            sqlite3_close(db);
        }
        return 0;
    }

    if (Size > 0) {
        rowid = read_i64(Data, Size);
        flags = Data[0] & 1;
    }

    sqlite3_exec(db,
                 "CREATE TABLE IF NOT EXISTS t1(id INTEGER PRIMARY KEY, b BLOB, c TEXT);"
                 "INSERT OR IGNORE INTO t1(id,b,c) VALUES(1, x'00', 'seed');",
                 NULL, NULL, NULL);

    sqlite3_prepare_v2(db, "SELECT 1;", -1, &stmt, NULL);

    sqlite3_finalize(stmt);

    sql = sqlite3_mprintf("%.*s", (int)Size, (const char *)Data);
    rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);

    mp1 = sqlite3_mprintf("tbl_%d_%lld", (int)(Size & 0xffff), (long long)rowid);
    mp2 = sqlite3_mprintf("col_%d_%d", flags, rc);

    sqlite3_free(errmsg);
    sqlite3_free(sql);

    table = mp1 ? mp1 : (char *)"t1";
    column = mp2 ? mp2 : (char *)"b";

    sqlite3_blob_open(db, db_name, table, column, rowid, flags, &blob);

    if (blob != NULL) {
        sqlite3_blob_close(blob);
    }

    sqlite3_free(mp1);
    sqlite3_free(mp2);
    sqlite3_close(db);
    return 0;
}