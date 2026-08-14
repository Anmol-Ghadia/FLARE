// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare at sqlite3.c:119055:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v3 at sqlite3.c:119085:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_name at sqlite3.c:73143:24 in sqlite3.h
// sqlite3_column_database_name at sqlite3.c:73182:24 in sqlite3.h
// sqlite3_column_table_name at sqlite3.c:73196:24 in sqlite3.h
// sqlite3_column_origin_name at sqlite3.c:73210:24 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
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

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Data && Size) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static void exec_sql(sqlite3 *db, const char *sql) {
    char *errmsg = NULL;
    sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (errmsg) {
        sqlite3_free(errmsg);
    }
}

static void inspect_stmt(sqlite3_stmt *stmt, int extra_index) {
    int col_count = sqlite3_column_count(stmt);
    int indices[8];
    int n = 0;

    indices[n++] = -1;
    indices[n++] = 0;
    indices[n++] = col_count > 0 ? col_count - 1 : 0;
    indices[n++] = col_count;
    indices[n++] = extra_index;
    indices[n++] = extra_index ^ 1;
    indices[n++] = 1000;
    indices[n++] = -1000;

    for (int i = 0; i < n; ++i) {
        int idx = indices[i];
        const char *a = sqlite3_column_name(stmt, idx);
        const char *b = sqlite3_column_database_name(stmt, idx);
        const char *c = sqlite3_column_table_name(stmt, idx);
        const char *d = sqlite3_column_origin_name(stmt, idx);
        (void)a;
        (void)b;
        (void)c;
        (void)d;
    }
}

int LLVMFuzzerTestOneInput_87(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt1 = NULL;
    sqlite3_stmt *stmt2 = NULL;
    const char *tail1 = NULL;
    const char *tail2 = NULL;
    char *sql = NULL;
    int rc;

    write_dummy_file(Data, Size);

    rc = sqlite3_open("./dummy_file", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    exec_sql(db, "PRAGMA writable_schema=OFF;");
    exec_sql(db, "CREATE TABLE IF NOT EXISTS t1(a INTEGER, b TEXT, c REAL);");
    exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(x INTEGER, y TEXT);");
    exec_sql(db, "INSERT INTO t1(a,b,c) VALUES (1,'one',1.5),(2,'two',2.5),(3,'three',3.5);");
    exec_sql(db, "INSERT INTO t2(x,y) VALUES (10,'ten'),(20,'twenty');");

    sql = (char *)malloc(Size + 1);
    if (sql) {
        if (Size) memcpy(sql, Data, Size);
        sql[Size] = '\0';
    }

    if (!sql) {
        sqlite3_close(db);
        return 0;
    }

    {
        int nByte1;
        if (Size == 0) {
            nByte1 = 0;
        } else {
            switch (Data[0] % 4) {
                case 0: nByte1 = -1; break;
                case 1: nByte1 = (int)Size; break;
                case 2: nByte1 = (int)Size + 1; break;
                default: nByte1 = (int)(Size / 2); break;
            }
        }

        rc = sqlite3_prepare(db, sql, nByte1, &stmt1, &tail1);
        if (stmt1) {
            inspect_stmt(stmt1, (Size > 1) ? (int)((int8_t)Data[1]) : 0);
            sqlite3_step(stmt1);
            inspect_stmt(stmt1, (Size > 2) ? (int)((int8_t)Data[2]) : 1);
            sqlite3_reset(stmt1);
            inspect_stmt(stmt1, (Size > 3) ? (int)((int8_t)Data[3]) : 2);
            sqlite3_finalize(stmt1);
            stmt1 = NULL;
        }
        (void)rc;
        (void)tail1;
    }

    {
        unsigned int flags = 0;
#ifdef SQLITE_PREPARE_PERSISTENT
        if (Size > 4 && (Data[4] & 1)) flags |= SQLITE_PREPARE_PERSISTENT;
#endif
#ifdef SQLITE_PREPARE_NORMALIZE
        if (Size > 4 && (Data[4] & 2)) flags |= SQLITE_PREPARE_NORMALIZE;
#endif
#ifdef SQLITE_PREPARE_NO_VTAB
        if (Size > 4 && (Data[4] & 4)) flags |= SQLITE_PREPARE_NO_VTAB;
#endif
#ifdef SQLITE_PREPARE_DONT_LOG
        if (Size > 4 && (Data[4] & 8)) flags |= SQLITE_PREPARE_DONT_LOG;
#endif

        int nByte2;
        if (Size == 0) {
            nByte2 = -1;
        } else {
            switch ((Size > 5 ? Data[5] : 0) % 4) {
                case 0: nByte2 = -1; break;
                case 1: nByte2 = (int)Size; break;
                case 2: nByte2 = (int)Size + 1; break;
                default: nByte2 = (int)(Size / 3); break;
            }
        }

        rc = sqlite3_prepare_v3(db, sql, nByte2, flags, &stmt2, &tail2);
        if (stmt2) {
            inspect_stmt(stmt2, (Size > 6) ? (int)((int8_t)Data[6]) : -2);
            sqlite3_step(stmt2);
            inspect_stmt(stmt2, (Size > 7) ? (int)((int8_t)Data[7]) : 3);
            sqlite3_reset(stmt2);
            sqlite3_step(stmt2);
            inspect_stmt(stmt2, (Size > 8) ? (int)((int8_t)Data[8]) : 4);
            sqlite3_finalize(stmt2);
            stmt2 = NULL;
        }
        (void)rc;
        (void)tail2;
    }

    free(sql);
    sqlite3_close(db);
    return 0;
}