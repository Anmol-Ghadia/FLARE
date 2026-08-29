// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare16_v3 at sqlite3.c:119195:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare16_v3 at sqlite3.c:119195:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare16_v3 at sqlite3.c:119195:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_stmt_scanstatus_reset at sqlite3.c:73956:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_decltype16 at sqlite3.c:73170:24 in sqlite3.h
// sqlite3_column_origin_name16 at sqlite3.c:73214:24 in sqlite3.h
// sqlite3_column_database_name16 at sqlite3.c:73186:24 in sqlite3.h
// sqlite3_stmt_scanstatus_reset at sqlite3.c:73956:17 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
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

#ifndef SQLITE_PREPARE_NO_VTAB
#define SQLITE_PREPARE_NO_VTAB 0x04
#endif
#ifndef SQLITE_PREPARE_PERSISTENT
#define SQLITE_PREPARE_PERSISTENT 0x01
#endif
#ifndef SQLITE_PREPARE_NORMALIZE
#define SQLITE_PREPARE_NORMALIZE 0x02
#endif
#ifndef SQLITE_PREPARE_DONT_LOG
#define SQLITE_PREPARE_DONT_LOG 0x10
#endif

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static unsigned int pick_flags(uint8_t b) {
    unsigned int flags = 0;
    if (b & 0x01) flags |= SQLITE_PREPARE_PERSISTENT;
    if (b & 0x02) flags |= SQLITE_PREPARE_NORMALIZE;
    if (b & 0x04) flags |= SQLITE_PREPARE_NO_VTAB;
    if (b & 0x08) flags |= SQLITE_PREPARE_DONT_LOG;
    return flags;
}

static void exercise_stmt(sqlite3_stmt *stmt) {
    int col_count, i;

    if (!stmt) return;

    col_count = sqlite3_column_count(stmt);

#ifdef SQLITE_ENABLE_STMT_SCANSTATUS
    sqlite3_stmt_scanstatus_reset(stmt);
#endif

    (void)sqlite3_step(stmt);

    for (i = -2; i < col_count + 2; ++i) {
        (void)sqlite3_column_decltype16(stmt, i);
        (void)sqlite3_column_origin_name16(stmt, i);
        (void)sqlite3_column_database_name16(stmt, i);
    }

#ifdef SQLITE_ENABLE_STMT_SCANSTATUS
    sqlite3_stmt_scanstatus_reset(stmt);
#endif

    sqlite3_reset(stmt);
}

int LLVMFuzzerTestOneInput_104(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    const void *tail = NULL;
    int rc;
    unsigned int flags = 0;
    int nByte;
    const void *sql16;
    size_t sql_bytes;

    write_dummy_file(Data, Size);

    rc = sqlite3_open("./dummy_file", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        return 0;
    }

    (void)sqlite3_exec(db,
        "PRAGMA writable_schema=ON;"
        "CREATE TABLE IF NOT EXISTS t1(a INTEGER, b TEXT, c BLOB);"
        "CREATE TABLE IF NOT EXISTS t2(x REAL, y NUMERIC);"
        "INSERT OR IGNORE INTO t1(rowid,a,b,c) VALUES(1,123,'abc',x'0102');"
        "INSERT OR IGNORE INTO t2(rowid,x,y) VALUES(1,3.14,'42');",
        NULL, NULL, NULL);

    if (Size == 0) {
        static const unsigned char fallback_sql16[] = {
            'S',0,'E',0,'L',0,'E',0,'C',0,'T',0,' ',0,
            'a',0,',',0,'b',0,',',0,'c',0,' ',0,
            'F',0,'R',0,'O',0,'M',0,' ',0,'t',0,'1',0,0,0
        };
        sql16 = fallback_sql16;
        nByte = (int)sizeof(fallback_sql16);
        flags = 0;
    } else {
        flags = pick_flags(Data[0]);
        sql16 = Data;
        sql_bytes = Size & ~(size_t)1;
        if (sql_bytes == 0) {
            static const unsigned char fallback_sql16[] = {
                'S',0,'E',0,'L',0,'E',0,'C',0,'T',0,' ',0,
                '*',0,' ',0,'F',0,'R',0,'O',0,'M',0,' ',0,'t',0,'1',0,0,0
            };
            sql16 = fallback_sql16;
            nByte = (int)sizeof(fallback_sql16);
        } else {
            if (sql_bytes > (size_t)0x7ffffffe) sql_bytes = (size_t)0x7ffffffe;
            nByte = (int)sql_bytes;
        }
    }

    rc = sqlite3_prepare16_v3(db, sql16, nByte, flags, &stmt, &tail);
    if (rc == SQLITE_OK && stmt != NULL) {
        exercise_stmt(stmt);
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    {
        static const unsigned char select1_sql16[] = {
            'S',0,'E',0,'L',0,'E',0,'C',0,'T',0,' ',0,
            'a',0,',',0,'b',0,',',0,'c',0,' ',0,
            'F',0,'R',0,'O',0,'M',0,' ',0,'t',0,'1',0,0,0
        };
        rc = sqlite3_prepare16_v3(db, select1_sql16, (int)sizeof(select1_sql16), flags, &stmt, &tail);
        if (rc == SQLITE_OK && stmt != NULL) {
            exercise_stmt(stmt);
            sqlite3_finalize(stmt);
            stmt = NULL;
        }
    }

    {
        static const unsigned char join_sql16[] = {
            'S',0,'E',0,'L',0,'E',0,'C',0,'T',0,' ',0,
            't',0,'1',0,'.',0,'a',0,',',0,'t',0,'2',0,'.',0,'y',0,' ',0,
            'F',0,'R',0,'O',0,'M',0,' ',0,'t',0,'1',0,' ',0,
            'J',0,'O',0,'I',0,'N',0,' ',0,'t',0,'2',0,' ',0,
            'O',0,'N',0,' ',0,'t',0,'1',0,'.',0,'r',0,'o',0,'w',0,'i',0,'d',0,'=',0,
            't',0,'2',0,'.',0,'r',0,'o',0,'w',0,'i',0,'d',0,0,0
        };
        rc = sqlite3_prepare16_v3(db, join_sql16, (int)sizeof(join_sql16), flags, &stmt, &tail);
        if (rc == SQLITE_OK && stmt != NULL) {
            exercise_stmt(stmt);
            sqlite3_finalize(stmt);
            stmt = NULL;
        }
    }

    sqlite3_close(db);
    return 0;
}