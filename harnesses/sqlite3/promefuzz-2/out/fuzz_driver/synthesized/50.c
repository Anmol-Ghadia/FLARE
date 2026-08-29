// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_preupdate_hook at sqlite3.c:154525:18 in sqlite3.h
// sqlite3_preupdate_depth at sqlite3.c:73800:16 in sqlite3.h
// sqlite3_preupdate_count at sqlite3.c:73782:16 in sqlite3.h
// sqlite3_preupdate_blobwrite at sqlite3.c:73811:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_blob_open at sqlite3.c:82675:16 in sqlite3.h
// sqlite3_blob_bytes at sqlite3.c:83019:16 in sqlite3.h
// sqlite3_blob_write at sqlite3.c:83009:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_preupdate_depth at sqlite3.c:73800:16 in sqlite3.h
// sqlite3_preupdate_count at sqlite3.c:73782:16 in sqlite3.h
// sqlite3_preupdate_blobwrite at sqlite3.c:73811:16 in sqlite3.h
// sqlite3_preupdate_old at sqlite3.c:73719:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_value_double at sqlite3.c:72097:19 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_preupdate_new at sqlite3.c:73822:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_value_double at sqlite3.c:72097:19 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_preupdate_old at sqlite3.c:73719:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_preupdate_new at sqlite3.c:73822:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static int g_initialized = 0;

static void preupdate_cb(
    void *ctx,
    sqlite3 *db,
    int op,
    const char *zDb,
    const char *zName,
    sqlite3_int64 iKey1,
    sqlite3_int64 iKey2
){
    (void)ctx;
    (void)zDb;
    (void)zName;
    (void)iKey1;
    (void)iKey2;

    int depth = sqlite3_preupdate_depth(db);
    int count = sqlite3_preupdate_count(db);
    int blobcol = sqlite3_preupdate_blobwrite(db);
    (void)depth;
    (void)blobcol;

    if( count < 0 ) count = 0;

    for( int i = -1; i <= count + 1; ++i ){
        sqlite3_value *v = NULL;
        int rc_old = sqlite3_preupdate_old(db, i, &v);
        if( rc_old == SQLITE_OK && v ){
            (void)sqlite3_value_nochange(v);
            (void)sqlite3_value_type(v);
            (void)sqlite3_value_bytes(v);
            (void)sqlite3_value_int64(v);
            (void)sqlite3_value_double(v);
            (void)sqlite3_value_text(v);
            (void)sqlite3_value_blob(v);
        }

        v = NULL;
        int rc_new = sqlite3_preupdate_new(db, i, &v);
        if( rc_new == SQLITE_OK && v ){
            (void)sqlite3_value_nochange(v);
            (void)sqlite3_value_type(v);
            (void)sqlite3_value_bytes(v);
            (void)sqlite3_value_int64(v);
            (void)sqlite3_value_double(v);
            (void)sqlite3_value_text(v);
            (void)sqlite3_value_blob(v);
        }
    }

    if( op == SQLITE_UPDATE || op == SQLITE_INSERT || op == SQLITE_DELETE ){
        sqlite3_value *v0 = NULL;
        if( sqlite3_preupdate_old(db, 0, &v0) == SQLITE_OK && v0 ){
            (void)sqlite3_value_nochange(v0);
        }
        v0 = NULL;
        if( sqlite3_preupdate_new(db, 0, &v0) == SQLITE_OK && v0 ){
            (void)sqlite3_value_nochange(v0);
        }
    }
}

static void exec_sql(sqlite3 *db, const char *sql){
    char *errmsg = NULL;
    sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if( errmsg ) sqlite3_free(errmsg);
}

static uint32_t consume_u32(const uint8_t **p, size_t *n){
    uint32_t v = 0;
    size_t take = *n < 4 ? *n : 4;
    for(size_t i = 0; i < take; ++i){
        v = (v << 8) | (*p)[i];
    }
    *p += take;
    *n -= take;
    return v;
}

static int consume_int(const uint8_t **p, size_t *n, int mod){
    if( mod <= 0 ) return 0;
    return (int)(consume_u32(p, n) % (uint32_t)mod);
}

int LLVMFuzzerTestOneInput_50(const uint8_t *Data, size_t Size){
    if( !g_initialized ){
        sqlite3_initialize();
        g_initialized = 1;
    }

    FILE *fp = fopen("./dummy_file", "wb");
    if( fp ){
        if( Size ) fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    sqlite3 *db = NULL;
    if( sqlite3_open(":memory:", &db) != SQLITE_OK ){
        if( db ) sqlite3_close(db);
        return 0;
    }

    sqlite3_preupdate_hook(db, preupdate_cb, NULL);

    (void)sqlite3_preupdate_depth(db);
    (void)sqlite3_preupdate_count(db);
    (void)sqlite3_preupdate_blobwrite(db);

    exec_sql(db, "PRAGMA foreign_keys=ON;");
    exec_sql(db, "PRAGMA recursive_triggers=ON;");
    exec_sql(db, "CREATE TABLE t1(a INTEGER PRIMARY KEY, b TEXT, c BLOB, d INTEGER);");
    exec_sql(db, "CREATE TABLE t2(x INTEGER PRIMARY KEY, y TEXT);");
    exec_sql(db, "CREATE TRIGGER tr1 AFTER UPDATE ON t1 BEGIN INSERT OR REPLACE INTO t2(x,y) VALUES (new.a, new.b); END;");
    exec_sql(db, "CREATE TRIGGER tr2 AFTER INSERT ON t1 BEGIN UPDATE t2 SET y=new.b WHERE x=new.a; END;");
    exec_sql(db, "INSERT INTO t1(a,b,c,d) VALUES(1,'init',x'00',0);");
    exec_sql(db, "INSERT INTO t2(x,y) VALUES(1,'seed');");

    const uint8_t *p = Data;
    size_t n = Size;

    int steps = 1 + consume_int(&p, &n, 32);
    for( int s = 0; s < steps; ++s ){
        int action = consume_int(&p, &n, 8);

        sqlite3_stmt *stmt = NULL;
        int rc;

        switch( action ){
            case 0: {
                rc = sqlite3_prepare_v2(db,
                    "INSERT OR REPLACE INTO t1(a,b,c,d) VALUES(?1,?2,?3,?4);",
                    -1, &stmt, NULL);
                if( rc == SQLITE_OK ){
                    int a = consume_int(&p, &n, 16);
                    int len = consume_int(&p, &n, 32);
                    if( (size_t)len > n ) len = (int)n;
                    sqlite3_bind_int(stmt, 1, a);
                    sqlite3_bind_text(stmt, 2, (const char *)p, len, SQLITE_TRANSIENT);
                    sqlite3_bind_blob(stmt, 3, p, len, SQLITE_TRANSIENT);
                    sqlite3_bind_int(stmt, 4, consume_int(&p, &n, 1000));
                    p += len;
                    n -= len;
                    sqlite3_step(stmt);
                }
                sqlite3_finalize(stmt);
                break;
            }
            case 1: {
                rc = sqlite3_prepare_v2(db,
                    "UPDATE t1 SET b=?1, c=?2, d=?3 WHERE a=?4;",
                    -1, &stmt, NULL);
                if( rc == SQLITE_OK ){
                    int len = consume_int(&p, &n, 32);
                    if( (size_t)len > n ) len = (int)n;
                    sqlite3_bind_text(stmt, 1, (const char *)p, len, SQLITE_TRANSIENT);
                    sqlite3_bind_blob(stmt, 2, p, len, SQLITE_TRANSIENT);
                    sqlite3_bind_int(stmt, 3, consume_int(&p, &n, 1000));
                    sqlite3_bind_int(stmt, 4, consume_int(&p, &n, 16));
                    p += len;
                    n -= len;
                    sqlite3_step(stmt);
                }
                sqlite3_finalize(stmt);
                break;
            }
            case 2: {
                rc = sqlite3_prepare_v2(db,
                    "DELETE FROM t1 WHERE a=?1;",
                    -1, &stmt, NULL);
                if( rc == SQLITE_OK ){
                    sqlite3_bind_int(stmt, 1, consume_int(&p, &n, 16));
                    sqlite3_step(stmt);
                }
                sqlite3_finalize(stmt);
                break;
            }
            case 3: {
                rc = sqlite3_prepare_v2(db,
                    "INSERT OR REPLACE INTO t2(x,y) VALUES(?1,?2);",
                    -1, &stmt, NULL);
                if( rc == SQLITE_OK ){
                    int len = consume_int(&p, &n, 32);
                    if( (size_t)len > n ) len = (int)n;
                    sqlite3_bind_int(stmt, 1, consume_int(&p, &n, 16));
                    sqlite3_bind_text(stmt, 2, (const char *)p, len, SQLITE_TRANSIENT);
                    p += len;
                    n -= len;
                    sqlite3_step(stmt);
                }
                sqlite3_finalize(stmt);
                break;
            }
            case 4:
                exec_sql(db, "BEGIN;");
                break;
            case 5:
                exec_sql(db, "COMMIT;");
                break;
            case 6:
                exec_sql(db, "ROLLBACK;");
                break;
            case 7: {
                rc = sqlite3_prepare_v2(db,
                    "UPDATE t1 SET c=zeroblob(?1) WHERE a=?2;",
                    -1, &stmt, NULL);
                if( rc == SQLITE_OK ){
                    sqlite3_bind_int(stmt, 1, consume_int(&p, &n, 64));
                    sqlite3_bind_int(stmt, 2, consume_int(&p, &n, 16));
                    sqlite3_step(stmt);
                }
                sqlite3_finalize(stmt);

                sqlite3_blob *blob = NULL;
                int rowid = consume_int(&p, &n, 16);
                if( sqlite3_blob_open(db, "main", "t1", "c", rowid, 1, &blob) == SQLITE_OK ){
                    int blen = sqlite3_blob_bytes(blob);
                    if( blen > 0 ){
                        int off = consume_int(&p, &n, blen);
                        int maxw = blen - off;
                        if( maxw < 0 ) maxw = 0;
                        int wlen = consume_int(&p, &n, maxw + 1);
                        if( wlen > 0 && (size_t)wlen <= n ){
                            sqlite3_blob_write(blob, p, wlen, off);
                            p += wlen;
                            n -= wlen;
                        }
                    }
                    sqlite3_blob_close(blob);
                }
                break;
            }
        }
    }

    sqlite3_close(db);
    return 0;
}