// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_stmt_status at sqlite3.c:73618:16 in sqlite3.h
// sqlite3_stmt_readonly at sqlite3.c:73571:16 in sqlite3.h
// sqlite3_stmt_isexplain at sqlite3.c:73579:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_prepare16_v2 at sqlite3.c:119183:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_prepare16_v2 at sqlite3.c:119183:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_stmt_readonly at sqlite3.c:73571:16 in sqlite3.h
// sqlite3_stmt_isexplain at sqlite3.c:73579:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_stmt_status at sqlite3.c:73618:16 in sqlite3.h
// sqlite3_stmt_status at sqlite3.c:73618:16 in sqlite3.h
// sqlite3_stmt_status at sqlite3.c:73618:16 in sqlite3.h
// sqlite3_stmt_scanstatus at sqlite3.c:73897:16 in sqlite3.h
// sqlite3_stmt_scanstatus at sqlite3.c:73897:16 in sqlite3.h
// sqlite3_stmt_scanstatus at sqlite3.c:73897:16 in sqlite3.h
// sqlite3_stmt_scanstatus at sqlite3.c:73897:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_stmt_status at sqlite3.c:73618:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

#ifndef SQLITE_SCANSTAT_NLOOP
#define SQLITE_SCANSTAT_NLOOP 0
#endif
#ifndef SQLITE_SCANSTAT_NVISIT
#define SQLITE_SCANSTAT_NVISIT 1
#endif
#ifndef SQLITE_SCANSTAT_EST
#define SQLITE_SCANSTAT_EST 2
#endif
#ifndef SQLITE_SCANSTAT_NAME
#define SQLITE_SCANSTAT_NAME 3
#endif
#ifndef SQLITE_SCANSTAT_EXPLAIN
#define SQLITE_SCANSTAT_EXPLAIN 4
#endif
#ifndef SQLITE_SCANSTAT_SELECTID
#define SQLITE_SCANSTAT_SELECTID 5
#endif
#ifndef SQLITE_SCANSTAT_PARENTID
#define SQLITE_SCANSTAT_PARENTID 6
#endif
#ifndef SQLITE_SCANSTAT_NCYCLE
#define SQLITE_SCANSTAT_NCYCLE 7
#endif

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *off) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4; i++) {
        v <<= 8;
        if (*off < size) v |= data[(*off)++];
    }
    return v;
}

static int pick_int(const uint8_t *data, size_t size, size_t *off, int fallback) {
    if (*off >= size) return fallback;
    return (int)data[(*off)++];
}

static void exec_sql(sqlite3 *db, const char *sql) {
    sqlite3_stmt *s = NULL;
    const char *tail = NULL;
    if (!db || !sql) return;
    if (sqlite3_prepare_v2(db, sql, -1, &s, &tail) != SQLITE_OK || !s) {
        return;
    }
    while (sqlite3_step(s) == SQLITE_ROW) {
    }
    sqlite3_finalize(s);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    const void *tail16 = NULL;
    int rc;
    size_t off = 0;
    int use_file_db;
    int nByteMode;
    int status_ops[] = {
#ifdef SQLITE_STMTSTATUS_FULLSCAN_STEP
        SQLITE_STMTSTATUS_FULLSCAN_STEP,
#endif
#ifdef SQLITE_STMTSTATUS_SORT
        SQLITE_STMTSTATUS_SORT,
#endif
#ifdef SQLITE_STMTSTATUS_AUTOINDEX
        SQLITE_STMTSTATUS_AUTOINDEX,
#endif
#ifdef SQLITE_STMTSTATUS_VM_STEP
        SQLITE_STMTSTATUS_VM_STEP,
#endif
#ifdef SQLITE_STMTSTATUS_REPREPARE
        SQLITE_STMTSTATUS_REPREPARE,
#endif
#ifdef SQLITE_STMTSTATUS_RUN
        SQLITE_STMTSTATUS_RUN,
#endif
#ifdef SQLITE_STMTSTATUS_FILTER_MISS
        SQLITE_STMTSTATUS_FILTER_MISS,
#endif
#ifdef SQLITE_STMTSTATUS_FILTER_HIT
        SQLITE_STMTSTATUS_FILTER_HIT,
#endif
        -1
    };
    int scan_ops[] = {
        SQLITE_SCANSTAT_NLOOP,
        SQLITE_SCANSTAT_NVISIT,
        SQLITE_SCANSTAT_EST,
        SQLITE_SCANSTAT_NAME,
        SQLITE_SCANSTAT_EXPLAIN,
        SQLITE_SCANSTAT_SELECTID,
        SQLITE_SCANSTAT_PARENTID,
        SQLITE_SCANSTAT_NCYCLE
    };
    int i, j;

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp) {
            if (Size) fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    use_file_db = pick_int(Data, Size, &off, 0) & 1;
    rc = sqlite3_open(use_file_db ? "./dummy_file" : ":memory:", &db);
    if (rc != SQLITE_OK || db == NULL) {
        if (db) sqlite3_close(db);
        sqlite3_shutdown();
        return 0;
    }

    exec_sql(db, "CREATE TABLE IF NOT EXISTS t1(a INTEGER, b TEXT);");
    exec_sql(db, "INSERT OR IGNORE INTO t1(rowid,a,b) VALUES(1,1,'x');");

    sqlite3_stmt_status(NULL, 0, 0);
    sqlite3_stmt_readonly(NULL);
    sqlite3_stmt_isexplain(NULL);
    sqlite3_expired(NULL);

    {
        size_t sql_bytes = (off < Size) ? (Size - off) : 0;
        size_t utf16_bytes = sql_bytes & ~(size_t)1;
        int nByte;
        const unsigned char default_sql16[] = {
            'S',0,'E',0,'L',0,'E',0,'C',0,'T',0,' ',0,'*',0,' ',0,
            'F',0,'R',0,'O',0,'M',0,' ',0,'t',0,'1',0,';',0,0,0
        };

        if (utf16_bytes == 0) {
            sqlite3_prepare16_v2(db, default_sql16, (int)sizeof(default_sql16), &stmt, &tail16);
        } else {
            unsigned char *sql16 = (unsigned char *)sqlite3_malloc((int)utf16_bytes + 2);
            if (!sql16) {
                sqlite3_close(db);
                sqlite3_shutdown();
                return 0;
            }
            memcpy(sql16, Data + off, utf16_bytes);
            sql16[utf16_bytes] = 0;
            sql16[utf16_bytes + 1] = 0;

            nByteMode = pick_int(Data, Size, &off, 0) % 4;
            switch (nByteMode) {
                case 0:
                    nByte = (int)utf16_bytes;
                    break;
                case 1:
                    nByte = (int)utf16_bytes + 2;
                    break;
                case 2:
                    nByte = -1;
                    break;
                default:
                    nByte = (int)(read_u32(Data, Size, &off) % (utf16_bytes + 3));
                    break;
            }

            sqlite3_prepare16_v2(db, sql16, nByte, &stmt, &tail16);
            sqlite3_free(sql16);
        }
    }

    if (stmt) {
        for (i = 0; i < 3; i++) {
            rc = sqlite3_step(stmt);
            if (rc != SQLITE_ROW && rc != SQLITE_DONE) break;
            if (rc == SQLITE_DONE) break;
        }

        (void)sqlite3_stmt_readonly(stmt);
        (void)sqlite3_stmt_isexplain(stmt);
        (void)sqlite3_expired(stmt);

        for (i = 0; status_ops[i] != -1; i++) {
            int resetFlg = pick_int(Data, Size, &off, i & 1) & 1;
            (void)sqlite3_stmt_status(stmt, status_ops[i], resetFlg);
        }

        (void)sqlite3_stmt_status(stmt, -1, 0);
        (void)sqlite3_stmt_status(stmt, 0x7fffffff, 1);

        for (j = -1; j < 4; j++) {
            sqlite3_int64 i64_out = 0;
            double dbl_out = 0.0;
            const char *str_out = NULL;
            int int_out = 0;

            for (i = 0; i < (int)(sizeof(scan_ops) / sizeof(scan_ops[0])); i++) {
                switch (scan_ops[i]) {
                    case SQLITE_SCANSTAT_NLOOP:
                    case SQLITE_SCANSTAT_NVISIT:
#ifdef SQLITE_SCANSTAT_NCYCLE
                    case SQLITE_SCANSTAT_NCYCLE:
#endif
                        (void)sqlite3_stmt_scanstatus(stmt, j, scan_ops[i], &i64_out);
                        break;
                    case SQLITE_SCANSTAT_EST:
                        (void)sqlite3_stmt_scanstatus(stmt, j, scan_ops[i], &dbl_out);
                        break;
                    case SQLITE_SCANSTAT_NAME:
                    case SQLITE_SCANSTAT_EXPLAIN:
                        (void)sqlite3_stmt_scanstatus(stmt, j, scan_ops[i], &str_out);
                        break;
                    case SQLITE_SCANSTAT_SELECTID:
                    case SQLITE_SCANSTAT_PARENTID:
                        (void)sqlite3_stmt_scanstatus(stmt, j, scan_ops[i], &int_out);
                        break;
                    default:
                        break;
                }
            }
        }

        (void)sqlite3_reset(stmt);
        (void)sqlite3_step(stmt);

#ifdef SQLITE_STMTSTATUS_MEMUSED
        (void)sqlite3_stmt_status(stmt, SQLITE_STMTSTATUS_MEMUSED, 0);
#endif

        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    sqlite3_close(db);
    sqlite3_shutdown();
    return 0;
}