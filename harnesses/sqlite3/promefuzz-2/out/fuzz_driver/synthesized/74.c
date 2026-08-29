// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_numeric_type at sqlite3.c:74581:16 in sqlite3.h
// sqlite3_value_bytes16 at sqlite3.c:72094:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_value at sqlite3.c:73419:16 in sqlite3.h
// sqlite3_bind_value at sqlite3.c:73419:16 in sqlite3.h
// sqlite3_bind_value at sqlite3.c:73419:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_preupdate_hook at sqlite3.c:154525:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_preupdate_new at sqlite3.c:73822:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_preupdate_new at sqlite3.c:73822:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_preupdate_hook at sqlite3.c:154525:18 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_value at sqlite3.c:73056:27 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <sqlite3.h>

typedef struct {
    sqlite3 *db;
    int enabled;
} PreupdateCtx;

static void preupdate_cb(
    void *arg,
    sqlite3 *db,
    int op,
    const char *zDb,
    const char *zName,
    sqlite3_int64 iKey1,
    sqlite3_int64 iKey2
){
    PreupdateCtx *ctx = (PreupdateCtx *)arg;
    sqlite3_value *val = NULL;
    int idx;

    (void)zDb;
    (void)zName;
    (void)iKey1;
    (void)iKey2;

    if(ctx == NULL || !ctx->enabled || ctx->db != db){
        return;
    }

    for(idx = -1; idx < 4; idx++){
        int rc = sqlite3_preupdate_new(db, idx, &val);
        if(rc == SQLITE_OK && val != NULL){
            (void)sqlite3_value_nochange(val);
        }
    }

    if(op != SQLITE_DELETE){
        for(idx = 0; idx < 4; idx++){
            int rc = sqlite3_preupdate_new(db, idx, &val);
            if(rc == SQLITE_OK && val != NULL){
                (void)sqlite3_value_nochange(val);
            }
        }
    }
}

static void exec_sql(sqlite3 *db, const char *sql){
    char *err = NULL;
    sqlite3_exec(db, sql, NULL, NULL, &err);
    if(err){
        sqlite3_free(err);
    }
}

int LLVMFuzzerTestOneInput_74(const uint8_t *Data, size_t Size){
    sqlite3 *db = NULL;
    sqlite3_stmt *sel = NULL;
    sqlite3_stmt *ins = NULL;
    sqlite3_stmt *upd = NULL;
    sqlite3_stmt *del = NULL;
    sqlite3_value *val = NULL;
    PreupdateCtx ctx;
    int rc;
    int i;

    FILE *fp = fopen("./dummy_file", "wb");
    if(fp != NULL){
        if(Size > 0){
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    if(sqlite3_open(":memory:", &db) != SQLITE_OK){
        if(db != NULL){
            sqlite3_close(db);
        }
        return 0;
    }

    ctx.db = db;
    ctx.enabled = (Size > 0) ? (Data[0] & 1) : 0;

    sqlite3_preupdate_hook(db, ctx.enabled ? preupdate_cb : NULL, &ctx);

    exec_sql(db, "CREATE TABLE t(a,b,c);");
    exec_sql(db, "INSERT INTO t(a,b,c) VALUES(1,'123',x'4142');");
    exec_sql(db, "INSERT INTO t(a,b,c) VALUES(NULL,'notnum',zeroblob(4));");

    rc = sqlite3_prepare_v2(db, "SELECT a,b,c FROM t WHERE rowid=1;", -1, &sel, NULL);
    if(rc == SQLITE_OK && sel != NULL){
        if(sqlite3_step(sel) == SQLITE_ROW){
            for(i = 0; i < sqlite3_column_count(sel); i++){
                sqlite3_value *colval = sqlite3_column_value(sel, i);
                if(colval != NULL){
                    sqlite3_value *dup = sqlite3_value_dup(colval);
                    if(dup != NULL){
                        (void)sqlite3_value_type(dup);
                        (void)sqlite3_value_numeric_type(dup);
                        (void)sqlite3_value_bytes16(dup);
                        (void)sqlite3_value_nochange(dup);
                        if(i == 0){
                            val = dup;
                            dup = NULL;
                        }
                        sqlite3_value_free(dup);
                    }
                }
            }
        }
    }

    rc = sqlite3_prepare_v2(db, "INSERT INTO t(a,b,c) VALUES(?1,?2,?3);", -1, &ins, NULL);
    if(rc == SQLITE_OK && ins != NULL){
        if(val != NULL){
            (void)sqlite3_bind_value(ins, 1, val);
            (void)sqlite3_bind_value(ins, 2, val);
            (void)sqlite3_bind_value(ins, 3, val);
        }else{
            sqlite3_bind_null(ins, 1);
            sqlite3_bind_null(ins, 2);
            sqlite3_bind_null(ins, 3);
        }
        (void)sqlite3_step(ins);
    }

    rc = sqlite3_prepare_v2(db, "UPDATE t SET a=?1,b=?2,c=?3 WHERE rowid=1;", -1, &upd, NULL);
    if(rc == SQLITE_OK && upd != NULL){
        sqlite3_bind_int(upd, 1, (Size > 1) ? (int)Data[1] : 0);
        sqlite3_bind_text(upd, 2, (const char *)Data, (int)Size, SQLITE_TRANSIENT);
        sqlite3_bind_blob(upd, 3, Data, (int)Size, SQLITE_TRANSIENT);
        (void)sqlite3_step(upd);
    }

    rc = sqlite3_prepare_v2(db, "DELETE FROM t WHERE rowid=2;", -1, &del, NULL);
    if(rc == SQLITE_OK && del != NULL){
        (void)sqlite3_step(del);
    }

    if(del != NULL){
        sqlite3_finalize(del);
    }
    if(upd != NULL){
        sqlite3_finalize(upd);
    }
    if(ins != NULL){
        sqlite3_finalize(ins);
    }
    if(sel != NULL){
        sqlite3_finalize(sel);
    }

    sqlite3_value_free(val);
    sqlite3_preupdate_hook(db, NULL, NULL);
    sqlite3_close(db);
    return 0;
}