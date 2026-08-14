#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static int progress_cb(void *p){
    const uint8_t *b = (const uint8_t *)p;
    return b ? (b[0] & 1) : 0;
}

static int exec_cb(void *p, int argc, char **argv, char **colv){
    (void)argc;
    (void)argv;
    (void)colv;
    const uint8_t *b = (const uint8_t *)p;
    return b ? (b[0] & 1) : 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
    sqlite3 *db = NULL;
    char *sql = NULL;
    char *errmsg = NULL;
    uint8_t *stable_data = NULL;
    FILE *fp = NULL;
    int rc;
    int id1, id2;
    int newVal1, newVal2;
    int nOps;
    int max_limit_id = SQLITE_LIMIT_WORKER_THREADS;
    sqlite3_int64 mem1, mem2;
    const char *ver;
    const char *sid;

    stable_data = (uint8_t *)malloc(Size ? Size : 1);
    if (stable_data == NULL) {
        sqlite3_memory_used();
        sqlite3_memory_used();
        sqlite3_libversion();
        sqlite3_sourceid();
        return 0;
    }
    if (Size > 0) {
        memcpy(stable_data, Data, Size);
    }

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(stable_data, 1, Size, fp);
        }
        fclose(fp);
        fp = NULL;
    }

    rc = sqlite3_open("./dummy_file", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db != NULL) {
            sqlite3_close(db);
        }
        sqlite3_memory_used();
        sqlite3_memory_used();
        sqlite3_libversion();
        sqlite3_sourceid();
        free(stable_data);
        return 0;
    }

    sql = (char *)malloc(Size + 1);
    if (sql == NULL) {
        sqlite3_close(db);
        sqlite3_memory_used();
        sqlite3_memory_used();
        sqlite3_libversion();
        sqlite3_sourceid();
        free(stable_data);
        return 0;
    }
    if (Size > 0) {
        memcpy(sql, stable_data, Size);
    }
    sql[Size] = '\0';

    if (max_limit_id <= 0) {
        max_limit_id = 1;
    }

    id1 = (Size > 0) ? (stable_data[0] % max_limit_id) : 0;
    newVal1 = (Size > 1) ? (int)((signed char)stable_data[1]) : -1;
    sqlite3_limit(db, id1, newVal1);

    id2 = (Size > 2) ? (stable_data[2] % max_limit_id) : 0;
    newVal2 = (Size > 3) ? ((int)stable_data[3] * 16) : -1;
    sqlite3_limit(db, id2, newVal2);

    sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESET);

    nOps = (Size > 4) ? ((int)(stable_data[4] % 32) - 8) : 1;
    sqlite3_progress_handler(db, nOps, progress_cb, stable_data);

    sqlite3_test_control(SQLITE_TESTCTRL_PENDING_BYTE, 0x40000000);

    rc = sqlite3_exec(db, sql, exec_cb, stable_data, &errmsg);
    if (errmsg != NULL) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (errmsg != NULL) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    sqlite3_progress_handler(db, 0, NULL, NULL);
    sqlite3_close(db);

    free(sql);
    sql = NULL;
    free(stable_data);
    stable_data = NULL;

    mem1 = sqlite3_memory_used();
    mem2 = sqlite3_memory_used();
    ver = sqlite3_libversion();
    sid = sqlite3_sourceid();

    (void)rc;
    (void)mem1;
    (void)mem2;
    (void)ver;
    (void)sid;

    return 0;
}