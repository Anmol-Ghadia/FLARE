// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <sqlite3.h>

int LLVMFuzzerTestOneInput_32(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Data && Size) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    int rc;

    rc = sqlite3_open("./dummy_file", &db);
    if (rc == SQLITE_OK && db != NULL) {
        const char *sqls[] = {
            "SELECT 1;",
            "CREATE TABLE IF NOT EXISTS t(x);",
            "INSERT INTO t(x) VALUES(1);",
            "SELECT * FROM t;",
            "PRAGMA integrity_check;"
        };
        size_t idx = 0;
        if (Size > 0) {
            idx = Data[0] % (sizeof(sqls) / sizeof(sqls[0]));
        }
        (void)sqlite3_prepare_v2(db, sqls[idx], -1, &stmt, NULL);
        if (stmt) {
            (void)sqlite3_step(stmt);
        }
    }

    (void)sqlite3_finalize(stmt);
    (void)sqlite3_close(db);
    (void)sqlite3_memory_used();
    (void)sqlite3_shutdown();

    if (Size > 1) {
        switch (Data[1] % 4) {
            case 0:
                (void)sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
                break;
            case 1:
                (void)sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
                break;
            case 2:
                (void)sqlite3_config(SQLITE_CONFIG_SERIALIZED);
                break;
            default:
                (void)sqlite3_config(-1);
                break;
        }
    } else {
        (void)sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
    }

    return 0;
}