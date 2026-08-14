// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_subtype at sqlite3.c:72106:25 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_subtype at sqlite3.c:72106:25 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static void fuzz_sql_func(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  int i;
  (void)argc;
  for(i = 0; i < argc; i++){
    const void *b1 = sqlite3_value_blob(argv[i]);
    int n1 = sqlite3_value_bytes(argv[i]);
    const unsigned char *t1 = sqlite3_value_text(argv[i]);
    unsigned int st1 = sqlite3_value_subtype(argv[i]);
    int iv1 = sqlite3_value_int(argv[i]);

    const unsigned char *t2 = sqlite3_value_text(argv[i]);
    int n2 = sqlite3_value_bytes(argv[i]);
    const void *b2 = sqlite3_value_blob(argv[i]);
    unsigned int st2 = sqlite3_value_subtype(argv[i]);
    int iv2 = sqlite3_value_int(argv[i]);

    if((st1 ^ st2) & 1U){
      sqlite3_result_value(ctx, argv[i]);
    }else if(((n1 + n2 + iv1 + iv2) & 1) && t1){
      sqlite3_result_value(ctx, argv[i]);
    }else if(b1 || b2 || t2){
      sqlite3_result_value(ctx, argv[i]);
    }
  }

  if(argc > 0){
    sqlite3_result_value(ctx, argv[argc - 1]);
  }
}

static int consume_u8(const uint8_t **p, size_t *n){
  int v = 0;
  if(*n > 0){
    v = **p;
    (*p)++;
    (*n)--;
  }
  return v;
}

static int consume_u32(const uint8_t **p, size_t *n){
  int v = 0;
  int i;
  for(i = 0; i < 4 && *n > 0; i++){
    v = (v << 8) | **p;
    (*p)++;
    (*n)--;
  }
  return v;
}

int LLVMFuzzerTestOneInput_46(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;
  int i;

  FILE *fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size > 0) fwrite(Data, 1, Size, fp);
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
               "INSERT INTO t VALUES(NULL, 0, 1, 'text');"
               "INSERT INTO t VALUES(x'00ff', -123, 3.14159, 'another');",
               NULL, NULL, &errmsg);
  if(errmsg){
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  {
    const uint8_t *p = Data;
    size_t n = Size;
    int rounds = (consume_u8(&p, &n) % 8) + 1;

    for(i = 0; i < rounds; i++){
      int mode = consume_u8(&p, &n) % 6;
      int v1 = consume_u32(&p, &n);
      int v2 = consume_u32(&p, &n);
      int len = consume_u8(&p, &n) % 32;
      const char *tail = (const char *)p;
      size_t tail_len = n < (size_t)len ? n : (size_t)len;

      if(stmt){
        sqlite3_finalize(stmt);
        stmt = NULL;
      }

      switch(mode){
        case 0:
          rc = sqlite3_prepare_v2(
              db,
              "SELECT fuzzfunc(?1);",
              -1, &stmt, NULL);
          break;
        case 1:
          rc = sqlite3_prepare_v2(
              db,
              "SELECT fuzzfunc(?1, ?2, ?3, ?4);",
              -1, &stmt, NULL);
          break;
        case 2:
          rc = sqlite3_prepare_v2(
              db,
              "SELECT fuzzfunc(a,b,c,d) FROM t;",
              -1, &stmt, NULL);
          break;
        case 3:
          rc = sqlite3_prepare_v2(
              db,
              "SELECT fuzzfunc(CAST(?1 AS TEXT), CAST(?2 AS BLOB), CAST(?3 AS INTEGER));",
              -1, &stmt, NULL);
          break;
        case 4:
          rc = sqlite3_prepare_v2(
              db,
              "SELECT fuzzfunc(zeroblob(?1), ?2, NULL, x'414243');",
              -1, &stmt, NULL);
          break;
        default:
          rc = sqlite3_prepare_v2(
              db,
              "SELECT fuzzfunc(substr(?1,1,10), hex(?2), ?3, ?4) FROM t;",
              -1, &stmt, NULL);
          break;
      }

      if(rc != SQLITE_OK || stmt == NULL){
        continue;
      }

      sqlite3_bind_int(stmt, 1, v1);
      sqlite3_bind_int(stmt, 2, v2);
      sqlite3_bind_blob(stmt, 2, tail, (int)tail_len, SQLITE_TRANSIENT);
      sqlite3_bind_text(stmt, 1, tail, (int)tail_len, SQLITE_TRANSIENT);
      sqlite3_bind_int(stmt, 3, v1 ^ v2);
      sqlite3_bind_null(stmt, 4);

      if(n > tail_len){
        p += tail_len;
        n -= tail_len;
      }else{
        p += n;
        n = 0;
      }

      while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
      }

      sqlite3_reset(stmt);
      sqlite3_clear_bindings(stmt);

      sqlite3_bind_null(stmt, 1);
      sqlite3_bind_zeroblob(stmt, 2, v1 & 0xff);
      sqlite3_bind_text(stmt, 3, "12345", -1, SQLITE_STATIC);
      sqlite3_bind_blob(stmt, 4, Data, (int)(Size > 64 ? 64 : Size), SQLITE_TRANSIENT);

      while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
      }
    }
  }

  if(stmt){
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  sqlite3_close(db);
  return 0;
}