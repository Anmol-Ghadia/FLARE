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
    int rc;
    int id1, id2;
    int newVal1, newVal2;
    int nOps;
    int max_limit_id = SQLITE_LIMIT_WORKER_THREADS;
    sqlite3_int64 mem1, mem2;
    const char *ver;
    const char *sid;
    const char *open_path = "./dummy_file";

    FILE *fp = fopen(open_path, "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open(open_path, &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db != NULL) {
            sqlite3_close(db);
        }
        mem1 = sqlite3_memory_used();
        mem2 = sqlite3_memory_used();
        ver = sqlite3_libversion();
        sid = sqlite3_sourceid();
        (void)mem1;
        (void)mem2;
        (void)ver;
        (void)sid;
        return 0;
    }

    sql = (char *)malloc(Size + 1);
    if (sql == NULL) {
        sqlite3_close(db);
        mem1 = sqlite3_memory_used();
        mem2 = sqlite3_memory_used();
        ver = sqlite3_libversion();
        sid = sqlite3_sourceid();
        (void)mem1;
        (void)mem2;
        (void)ver;
        (void)sid;
        return 0;
    }
    if (Size > 0) {
        memcpy(sql, Data, Size);
    }
    sql[Size] = '\0';

    if (max_limit_id <= 0) {
        max_limit_id = 1;
    }

    id1 = (Size > 0) ? (Data[0] % max_limit_id) : 0;
    newVal1 = (Size > 1) ? (int)((signed char)Data[1]) : -1;
    sqlite3_limit(db, id1, newVal1);

    id2 = (Size > 2) ? (Data[2] % max_limit_id) : 0;
    newVal2 = (Size > 3) ? ((int)Data[3] * 16) : -1;
    sqlite3_limit(db, id2, newVal2);

    sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESET);

    nOps = (Size > 4) ? ((int)(Data[4] % 32) - 8) : 1;
    sqlite3_progress_handler(db, nOps, progress_cb, (void *)Data);

    sqlite3_test_control(SQLITE_TESTCTRL_PENDING_BYTE, 0x40000000);

    rc = sqlite3_exec(db, sql, exec_cb, (void *)Data, &errmsg);
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

    free(sql);
    sql = NULL;

    sqlite3_close(db);
    db = NULL;

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