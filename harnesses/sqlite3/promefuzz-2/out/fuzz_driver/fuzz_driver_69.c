// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_vtab_nochange at sqlite3.c:72738:16 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_result_zeroblob64 at sqlite3.c:72436:16 in sqlite3.h
// sqlite3_result_zeroblob64 at sqlite3.c:72436:16 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_result_zeroblob64 at sqlite3.c:72436:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t rd32(const uint8_t *p){
  return ((uint32_t)p[0]) | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint64_t rd64(const uint8_t *p){
  return ((uint64_t)p[0]) |
         ((uint64_t)p[1] << 8) |
         ((uint64_t)p[2] << 16) |
         ((uint64_t)p[3] << 24) |
         ((uint64_t)p[4] << 32) |
         ((uint64_t)p[5] << 40) |
         ((uint64_t)p[6] << 48) |
         ((uint64_t)p[7] << 56);
}

static void fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  int mode = 0;
  sqlite3_int64 i64v = 0;
  sqlite3_uint64 zb = 0;

  if(argc > 0 && argv && argv[0]){
    mode = sqlite3_value_int(argv[0]) & 7;
    i64v = sqlite3_value_int64(argv[0]);
  }
  if(argc > 1 && argv && argv[1]){
    zb = (sqlite3_uint64)sqlite3_value_int64(argv[1]);
  }

  (void)sqlite3_vtab_nochange(ctx);

  switch(mode){
    case 0:
      sqlite3_result_int64(ctx, i64v);
      break;
    case 1:
      if(argc > 0 && argv && argv[0]){
        sqlite3_result_value(ctx, argv[0]);
      }else{
        sqlite3_result_int64(ctx, 0);
      }
      break;
    case 2:
      if(argc > 0 && argv && argv[0]){
        const void *b = sqlite3_value_blob(argv[0]);
        int n = sqlite3_value_bytes(argv[0]);
        if(n < 0) n = 0;
        sqlite3_result_blob(ctx, b ? b : "", n, SQLITE_TRANSIENT);
      }else{
        static const unsigned char empty_blob[1] = {0};
        sqlite3_result_blob(ctx, empty_blob, 0, SQLITE_STATIC);
      }
      break;
    case 3:
      if(argc > 1 && argv && argv[1]){
        const void *b = sqlite3_value_blob(argv[1]);
        int n = sqlite3_value_bytes(argv[1]);
        if(n < 0) n = 0;
        sqlite3_result_blob(ctx, b ? b : "", n, SQLITE_TRANSIENT);
      }else{
        static const unsigned char one[1] = {0x41};
        sqlite3_result_blob(ctx, one, 1, SQLITE_STATIC);
      }
      break;
    case 4:
      sqlite3_result_zeroblob64(ctx, zb & 0xffff);
      break;
    case 5:
      sqlite3_result_zeroblob64(ctx, zb);
      break;
    case 6:
      sqlite3_result_int64(ctx, ~i64v);
      break;
    default:
      if(argc > 0 && argv && argv[0]){
        sqlite3_result_value(ctx, argv[0]);
      }else{
        sqlite3_result_zeroblob64(ctx, 0);
      }
      break;
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  FILE *fp;
  int rc;
  size_t pos = 0;

  fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Data && Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if(rc != SQLITE_OK || db == NULL){
    if(db) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db, "CREATE TABLE t(a,b);", NULL, NULL, NULL);

  if(Size >= 8){
    sqlite3_limit(db, SQLITE_LIMIT_LENGTH, (int)(rd32(Data) & 0x1fffff));
    sqlite3_limit(db, SQLITE_LIMIT_SQL_LENGTH, (int)(rd32(Data + 4) & 0x1fffff));
    pos = 8;
  }

  {
    char fname[256];
    int nArgChoices[] = {-1, 0, 1, 2, 3};
    int encChoices[] = {
      SQLITE_UTF8,
      SQLITE_UTF16LE,
      SQLITE_UTF16BE,
      SQLITE_UTF16,
      SQLITE_UTF8 | SQLITE_DETERMINISTIC,
      SQLITE_UTF8 | SQLITE_DIRECTONLY,
      SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_DIRECTONLY
    };
    int nArg = nArgChoices[(Size > pos) ? (Data[pos++] % (sizeof(nArgChoices)/sizeof(nArgChoices[0]))) : 0];
    int enc = encChoices[(Size > pos) ? (Data[pos++] % (sizeof(encChoices)/sizeof(encChoices[0]))) : 0];
    size_t remain = (Size > pos) ? (Size - pos) : 0;
    size_t flen = remain > 0 ? (Data[pos++] % 32) : 4;
    if(flen == 0) flen = 1;
    if(flen > 250) flen = 250;
    for(size_t i = 0; i < flen; i++){
      unsigned char c = (pos < Size) ? Data[pos++] : (unsigned char)('a' + (i % 26));
      fname[i] = (char)('a' + (c % 26));
    }
    fname[flen] = '\0';

    sqlite3_create_function(db, fname, nArg, enc, NULL, fuzz_scalar, NULL, NULL);
    sqlite3_create_function(db, "fuzzfunc", -1, SQLITE_UTF8, NULL, fuzz_scalar, NULL, NULL);
    sqlite3_create_function(db, "fuzzfunc", 2, SQLITE_UTF8 | SQLITE_DETERMINISTIC, NULL, fuzz_scalar, NULL, NULL);
    sqlite3_create_function(db, "fuzzfunc", 1, SQLITE_UTF16LE, NULL, fuzz_scalar, NULL, NULL);
    sqlite3_create_function(db, "fuzzfunc", 0, SQLITE_UTF8, NULL, NULL, NULL, NULL);
    sqlite3_create_function(db, "fuzzfunc", 2, SQLITE_UTF8, NULL, fuzz_scalar, NULL, NULL);
  }

  sqlite3_exec(db, "CREATE TEMP TABLE IF NOT EXISTS x(v);", NULL, NULL, NULL);
  sqlite3_exec(db, "INSERT INTO x(v) VALUES(1),(2),(3);", NULL, NULL, NULL);

  {
    static const char *sqls[] = {
      "SELECT fuzzfunc(?1, ?2);",
      "SELECT fuzzfunc(?1);",
      "SELECT fuzzfunc();",
      "SELECT fuzzfunc(a,b) FROM t;",
      "SELECT fuzzfunc(v, randomblob(8)) FROM x;",
      "SELECT fuzzfunc(CAST(?1 AS BLOB), CAST(?2 AS INTEGER));",
      "SELECT fuzzfunc(readfile('./dummy_file'));",
      "SELECT fuzzfunc(zeroblob(?1));"
    };
    int idx = (Size > pos) ? (Data[pos++] % (sizeof(sqls)/sizeof(sqls[0]))) : 0;
    const char *sql = sqls[idx];

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc == SQLITE_OK && stmt){
      if(Size - pos >= 8){
        sqlite3_bind_int64(stmt, 1, (sqlite3_int64)rd64(Data + pos));
        pos += 8;
      }else{
        sqlite3_bind_int64(stmt, 1, (sqlite3_int64)Size);
      }

      if(Size > pos){
        int blobn = (int)((Size - pos) > 1024 ? 1024 : (Size - pos));
        sqlite3_bind_blob(stmt, 2, Data + pos, blobn, SQLITE_STATIC);
      }else{
        static const unsigned char d[1] = {0};
        sqlite3_bind_blob(stmt, 2, d, 0, SQLITE_STATIC);
      }

      while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
      }
    }
    if(stmt) sqlite3_finalize(stmt);
    stmt = NULL;
  }

  {
    size_t sqln = (Size > pos) ? (Size - pos) : 0;
    if(sqln > 0){
      char *sql = (char *)malloc(sqln + 1);
      if(sql){
        for(size_t i = 0; i < sqln; i++){
          unsigned char c = Data[pos + i];
          sql[i] = (char)((c >= 32 && c <= 126) ? c : ' ');
        }
        sql[sqln] = '\0';
        sqlite3_exec(db, sql, NULL, NULL, &errmsg);
        if(errmsg){
          sqlite3_free(errmsg);
          errmsg = NULL;
        }
        free(sql);
      }
    }
  }

  sqlite3_close(db);
  return 0;
}