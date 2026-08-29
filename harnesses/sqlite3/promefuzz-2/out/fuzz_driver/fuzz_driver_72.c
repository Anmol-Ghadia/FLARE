// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_total_changes at sqlite3.c:153360:16 in sqlite3.h
// sqlite3_db_status at sqlite3.c:9629:16 in sqlite3.h
// sqlite3_file_control at sqlite3.c:156077:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_total_changes at sqlite3.c:153360:16 in sqlite3.h
// sqlite3_db_status at sqlite3.c:9629:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_total_changes at sqlite3.c:153360:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *off) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4 && *off < size; ++i, ++(*off)) {
        v = (v << 8) | data[*off];
    }
    return v;
}

static int exec_callback(void *ctx, int argc, char **argv, char **colnames) {
    sqlite3 *db = (sqlite3 *)ctx;
    volatile int sink = 0;
    int i;

    sink ^= sqlite3_changes(db);
    sink ^= sqlite3_total_changes(db);

    for (i = 0; i < argc; ++i) {
        if (argv && argv[i]) sink ^= (unsigned char)argv[i][0];
        if (colnames && colnames[i]) sink ^= (unsigned char)colnames[i][0];
    }

    (void)sink;
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    int rc;
    size_t off = 0;
    char *errmsg = NULL;
    int cur = 0, hi = 0;
    int resetFlg;
    int use_callback;
    int sql_len;
    char *sql = NULL;
    const char *db_name;
    int op_choice;

    sqlite3_file *fc_file_ptr = NULL;
    sqlite3_file *fc_journal_ptr = NULL;
    sqlite3_vfs *fc_vfs_ptr = NULL;
    int fc_data_version = 0;
    int fc_lockstate = 0;
    int fc_dummy = 0;
    void *arg = NULL;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    rc = sqlite3_open("./dummy_file", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) {
            sqlite3_close(db);
        }
        return 0;
    }

    sqlite3_exec(db,
                 "PRAGMA foreign_keys=ON;"
                 "CREATE TABLE IF NOT EXISTS t1(a INTEGER, b TEXT);"
                 "CREATE TABLE IF NOT EXISTS t2(x INTEGER PRIMARY KEY, y BLOB);"
                 "CREATE TEMP TABLE IF NOT EXISTS tt(z TEXT);",
                 NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    use_callback = (off < Size) ? (Data[off++] & 1) : 0;
    resetFlg = (off < Size) ? (Data[off++] & 1) : 0;
    sql_len = (int)((off < Size) ? (Size - off) : 0);

    sql = (char *)malloc((size_t)sql_len + 1);
    if (!sql) {
        sqlite3_close(db);
        return 0;
    }
    if (sql_len > 0) {
        memcpy(sql, Data + off, (size_t)sql_len);
    }
    sql[sql_len] = '\0';

    rc = sqlite3_exec(db,
                      sql,
                      use_callback ? exec_callback : NULL,
                      use_callback ? db : NULL,
                      &errmsg);
    (void)rc;
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    (void)sqlite3_changes(db);
    (void)sqlite3_total_changes(db);

    {
        static const int dbstatus_ops[] = {
#ifdef SQLITE_DBSTATUS_LOOKASIDE_USED
            SQLITE_DBSTATUS_LOOKASIDE_USED,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_USED
            SQLITE_DBSTATUS_CACHE_USED,
#endif
#ifdef SQLITE_DBSTATUS_SCHEMA_USED
            SQLITE_DBSTATUS_SCHEMA_USED,
#endif
#ifdef SQLITE_DBSTATUS_STMT_USED
            SQLITE_DBSTATUS_STMT_USED,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_HIT
            SQLITE_DBSTATUS_CACHE_HIT,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_MISS
            SQLITE_DBSTATUS_CACHE_MISS,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_WRITE
            SQLITE_DBSTATUS_CACHE_WRITE,
#endif
#ifdef SQLITE_DBSTATUS_DEFERRED_FKS
            SQLITE_DBSTATUS_DEFERRED_FKS,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_USED_SHARED
            SQLITE_DBSTATUS_CACHE_USED_SHARED,
#endif
#ifdef SQLITE_DBSTATUS_CACHE_SPILL
            SQLITE_DBSTATUS_CACHE_SPILL,
#endif
        };
        size_t nops = sizeof(dbstatus_ops) / sizeof(dbstatus_ops[0]);
        if (nops > 0) {
            size_t idx = (size_t)(read_u32(Data, Size, &off) % nops);
            (void)sqlite3_db_status(db, dbstatus_ops[idx], &cur, &hi, resetFlg);
        }
    }

    switch ((off < Size) ? (Data[off++] % 4) : 0) {
        case 0: db_name = "main"; break;
        case 1: db_name = "temp"; break;
        case 2: db_name = NULL; break;
        default: db_name = "does_not_exist"; break;
    }

    switch ((off < Size) ? (Data[off++] % 6) : 0) {
#ifdef SQLITE_FCNTL_FILE_POINTER
        case 0:
            arg = &fc_file_ptr;
            op_choice = SQLITE_FCNTL_FILE_POINTER;
            break;
#else
        case 0:
#endif
#ifdef SQLITE_FCNTL_JOURNAL_POINTER
        case 1:
            arg = &fc_journal_ptr;
            op_choice = SQLITE_FCNTL_JOURNAL_POINTER;
            break;
#else
        case 1:
#endif
#ifdef SQLITE_FCNTL_VFS_POINTER
        case 2:
            arg = &fc_vfs_ptr;
            op_choice = SQLITE_FCNTL_VFS_POINTER;
            break;
#else
        case 2:
#endif
#ifdef SQLITE_FCNTL_DATA_VERSION
        case 3:
            arg = &fc_data_version;
            op_choice = SQLITE_FCNTL_DATA_VERSION;
            break;
#else
        case 3:
#endif
#ifdef SQLITE_FCNTL_LOCKSTATE
        case 4:
            arg = &fc_lockstate;
            op_choice = SQLITE_FCNTL_LOCKSTATE;
            break;
#else
        case 4:
#endif
        default:
            arg = &fc_dummy;
            op_choice = 0x7fffffff;
            break;
    }
    (void)sqlite3_file_control(db, db_name, op_choice, arg);

    sqlite3_exec(db,
                 "INSERT INTO t1(a,b) VALUES(1,'x');"
                 "INSERT OR REPLACE INTO t2(x,y) VALUES(1,X'00');"
                 "UPDATE t1 SET b='y' WHERE a=1;"
                 "DELETE FROM t1 WHERE a=2;",
                 NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    (void)sqlite3_changes(db);
    (void)sqlite3_total_changes(db);
    (void)sqlite3_db_status(db,
#ifdef SQLITE_DBSTATUS_CACHE_USED
                            SQLITE_DBSTATUS_CACHE_USED,
#else
                            0,
#endif
                            &cur, &hi, 1);

    free(sql);
    sqlite3_close(db);
    return 0;
}