// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_pointer at sqlite3.c:72110:18 in sqlite3.h
// sqlite3_value_pointer at sqlite3.c:72110:18 in sqlite3.h
// sqlite3_value_pointer at sqlite3.c:72110:18 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

static void fuzz_sql_func(sqlite3_context *ctx, int argc, sqlite3_value **argv){
    int i;
    for(i = 0; i < argc; i++){
        sqlite3_value *dup = sqlite3_value_dup(argv[i]);
        if(dup){
            const void *blob1 = sqlite3_value_blob(dup);
            (void)blob1;
            (void)sqlite3_value_int(dup);
            (void)sqlite3_value_pointer(dup, "fuzzptr");
            (void)sqlite3_value_pointer(dup, "");
            (void)sqlite3_value_pointer(dup, "other");
            sqlite3_result_value(ctx, dup);
            sqlite3_value_free(dup);
        }else{
            sqlite3_result_value(ctx, argv[i]);
        }
    }

    if(argc > 0){
        sqlite3_result_value(ctx, argv[0]);
    }else{
        sqlite3_result_null(ctx);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *errmsg = NULL;
    int rc;
    size_t pos = 0;
    int mode, tsel;
    int ival = 0;
    sqlite3_int64 i64val = 0;
    double dval = 0.0;
    const void *blobptr = NULL;
    int bloblen = 0;
    char *textbuf = NULL;
    char *sql = NULL;
    int sql_len;

    FILE *fp = fopen("./dummy_file", "wb");
    if(fp){
        if(Size) fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    rc = sqlite3_open(":memory:", &db);
    if(rc != SQLITE_OK || db == NULL){
        if(db) sqlite3_close(db);
        return 0;
    }

    sqlite3_create_function(db, "fuzzfunc", -1, SQLITE_UTF8, NULL, fuzz_sql_func, NULL, NULL);

    sqlite3_exec(db,
        "CREATE TABLE t(a,b,c,d);"
        "INSERT INTO t VALUES(1,'text',x'010203',NULL);"
        "INSERT INTO t VALUES(-5,'12345',x'',99);",
        NULL, NULL, &errmsg);
    if(errmsg){
        sqlite3_free(errmsg);
        errmsg = NULL;
    }

    if(Size > 0){
        mode = Data[pos++] % 6;
    }else{
        mode = 0;
    }

    switch(mode){
        case 0:
            sql = "SELECT fuzzfunc(?1)";
            break;
        case 1:
            sql = "SELECT fuzzfunc(?1, ?2)";
            break;
        case 2:
            sql = "SELECT fuzzfunc(a) FROM t";
            break;
        case 3:
            sql = "SELECT fuzzfunc(a,b,c,d) FROM t";
            break;
        case 4:
            sql = "SELECT fuzzfunc(?1, a, ?2, c) FROM t";
            break;
        default:
            sql = "SELECT fuzzfunc(CAST(?1 AS TEXT), CAST(?2 AS BLOB), CAST(?3 AS INTEGER))";
            break;
    }

    sql_len = (int)strlen(sql);
    rc = sqlite3_prepare_v2(db, sql, sql_len, &stmt, NULL);
    if(rc != SQLITE_OK || stmt == NULL){
        sqlite3_close(db);
        return 0;
    }

    if(pos < Size) tsel = Data[pos++] % 7; else tsel = 0;
    if(Size - pos >= sizeof(int)){
        memcpy(&ival, Data + pos, sizeof(int));
        pos += sizeof(int);
    }else if(pos < Size){
        ival = (int)Data[pos++];
    }

    if(Size - pos >= sizeof(sqlite3_int64)){
        memcpy(&i64val, Data + pos, sizeof(sqlite3_int64));
        pos += sizeof(sqlite3_int64);
    }else{
        i64val = (sqlite3_int64)ival;
    }

    if(Size - pos >= sizeof(double)){
        memcpy(&dval, Data + pos, sizeof(double));
        pos += sizeof(double);
    }else{
        dval = (double)ival;
    }

    blobptr = (pos < Size) ? (const void *)(Data + pos) : (const void *)"";
    bloblen = (int)(Size - pos);

    textbuf = (char *)malloc(bloblen + 1);
    if(textbuf){
        if(bloblen > 0) memcpy(textbuf, blobptr, bloblen);
        textbuf[bloblen] = '\0';
    }

    switch(tsel){
        case 0:
            sqlite3_bind_null(stmt, 1);
            break;
        case 1:
            sqlite3_bind_int(stmt, 1, ival);
            break;
        case 2:
            sqlite3_bind_int64(stmt, 1, i64val);
            break;
        case 3:
            sqlite3_bind_double(stmt, 1, dval);
            break;
        case 4:
            sqlite3_bind_text(stmt, 1, textbuf ? textbuf : "", -1, SQLITE_TRANSIENT);
            break;
        case 5:
            sqlite3_bind_blob(stmt, 1, blobptr, bloblen, SQLITE_TRANSIENT);
            break;
        default:
            sqlite3_bind_zeroblob(stmt, 1, bloblen);
            break;
    }

    if(pos < Size) tsel = Data[pos++] % 6; else tsel = 0;
    switch(tsel){
        case 0:
            sqlite3_bind_null(stmt, 2);
            break;
        case 1:
            sqlite3_bind_int(stmt, 2, ~ival);
            break;
        case 2:
            sqlite3_bind_int64(stmt, 2, ~i64val);
            break;
        case 3:
            sqlite3_bind_double(stmt, 2, -dval);
            break;
        case 4:
            sqlite3_bind_text(stmt, 2, textbuf ? textbuf : "", bloblen > 0 ? bloblen / 2 : 0, SQLITE_TRANSIENT);
            break;
        default:
            sqlite3_bind_blob(stmt, 2, blobptr, bloblen > 0 ? bloblen / 2 : 0, SQLITE_TRANSIENT);
            break;
    }

    if(pos < Size) tsel = Data[pos++] % 4; else tsel = 0;
    switch(tsel){
        case 0:
            sqlite3_bind_null(stmt, 3);
            break;
        case 1:
            sqlite3_bind_int(stmt, 3, ival ^ 0x55555555);
            break;
        case 2:
            sqlite3_bind_text(stmt, 3, textbuf ? textbuf : "", -1, SQLITE_TRANSIENT);
            break;
        default:
            sqlite3_bind_blob(stmt, 3, blobptr, bloblen, SQLITE_TRANSIENT);
            break;
    }

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
    }

    sqlite3_finalize(stmt);
    free(textbuf);
    sqlite3_close(db);
    return 0;
}