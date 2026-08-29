// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_user_data at sqlite3.c:72704:18 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_subtype at sqlite3.c:72368:17 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_subtype at sqlite3.c:72368:17 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_aggregate_count at sqlite3.c:72894:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_subtype at sqlite3.c:72368:17 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_subtype at sqlite3.c:72368:17 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
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

static void fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  (void)argc;
  (void)argv;
  void *ud = sqlite3_user_data(ctx);
  sqlite3_int64 v = 0;
  if(ud){
    memcpy(&v, ud, sizeof(v) < sizeof(void*) ? sizeof(v) : sizeof(void*));
  }
  sqlite3_result_int64(ctx, v);
  sqlite3_result_subtype(ctx, (unsigned int)(v & 0xffffffffu));
}

static void fuzz_text(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  const unsigned char *txt = 0;
  int n = -1;
  if(argc > 0 && argv && argv[0]){
    txt = sqlite3_value_text(argv[0]);
    n = sqlite3_value_bytes(argv[0]);
  }
  if(txt){
    sqlite3_result_text(ctx, (const char *)txt, n, SQLITE_TRANSIENT);
  }else{
    sqlite3_result_text(ctx, "", 0, SQLITE_STATIC);
  }
  sqlite3_result_subtype(ctx, (unsigned int)(argc & 0xff));
}

typedef struct AggState {
  sqlite3_int64 sum;
  int calls;
  unsigned int subtype;
} AggState;

static void fuzz_agg_step(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  int alloc_sz = (argc > 0 && argv && argv[0]) ? (sqlite3_value_int(argv[0]) & 0x3f) : (int)sizeof(AggState);
  if(alloc_sz == 0) alloc_sz = (int)sizeof(AggState);
  AggState *st = (AggState *)sqlite3_aggregate_context(ctx, alloc_sz);
  if(!st) return;

  if(alloc_sz >= (int)sizeof(AggState)){
    st->calls++;
    if(argc > 1 && argv[1]){
      st->sum += sqlite3_value_int64(argv[1]);
    }else if(argc > 0 && argv[0]){
      st->sum += sqlite3_value_int64(argv[0]);
    }
    if(argc > 2 && argv[2]){
      st->subtype ^= (unsigned int)sqlite3_value_int(argv[2]);
    }
  }else{
    unsigned char *p = (unsigned char *)st;
    int i;
    for(i = 0; i < alloc_sz; i++){
      p[i] ^= (unsigned char)(i + argc);
    }
  }
}

static void fuzz_agg_final(sqlite3_context *ctx){
  int cnt = sqlite3_aggregate_count(ctx);
  AggState *st = (AggState *)sqlite3_aggregate_context(ctx, 0);
  if(st){
    sqlite3_result_subtype(ctx, st->subtype ^ (unsigned int)cnt);
    sqlite3_result_int64(ctx, st->sum + cnt + st->calls);
  }else{
    sqlite3_result_subtype(ctx, (unsigned int)cnt);
    sqlite3_result_int64(ctx, cnt);
  }
}

int LLVMFuzzerTestOneInput_113(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;
  sqlite3_int64 userdata_value = 0;
  size_t copy_n = Size < sizeof(userdata_value) ? Size : sizeof(userdata_value);

  FILE *fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  if(copy_n > 0){
    memcpy(&userdata_value, Data, copy_n);
  }

  rc = sqlite3_open(":memory:", &db);
  if(rc != SQLITE_OK || !db){
    if(db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzz_scalar", -1, SQLITE_UTF8, &userdata_value,
                          fuzz_scalar, NULL, NULL);
  sqlite3_create_function(db, "fuzz_text", -1, SQLITE_UTF8, NULL,
                          fuzz_text, NULL, NULL);
  sqlite3_create_function(db, "fuzz_agg", -1, SQLITE_UTF8, NULL,
                          NULL, fuzz_agg_step, fuzz_agg_final);

  sqlite3_exec(db,
    "CREATE TABLE t(a INTEGER, b TEXT, c BLOB);"
    "INSERT INTO t VALUES(1,'x',X'00');"
    "INSERT INTO t VALUES(-5,'hello',X'4142');"
    "INSERT INTO t VALUES(123456789,'world',NULL);",
    NULL, NULL, &errmsg);
  if(errmsg){
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  {
    const char *sqls[] = {
      "SELECT fuzz_scalar();",
      "SELECT fuzz_scalar(?1);",
      "SELECT fuzz_text(?1);",
      "SELECT fuzz_text(b) FROM t;",
      "SELECT fuzz_agg(a,b,c) FROM t;",
      "SELECT fuzz_agg(?1,?2,?3) FROM t;",
      "SELECT fuzz_agg(a) FROM t WHERE a > ?1;",
      "SELECT fuzz_agg(a) FROM t WHERE 0;",
      "SELECT fuzz_scalar(), fuzz_text(?1), fuzz_agg(a,b,c) FROM t;"
    };
    size_t i;
    for(i = 0; i < sizeof(sqls)/sizeof(sqls[0]); i++){
      rc = sqlite3_prepare_v2(db, sqls[i], -1, &stmt, NULL);
      if(rc != SQLITE_OK || !stmt){
        if(stmt){
          sqlite3_finalize(stmt);
          stmt = NULL;
        }
        continue;
      }

      sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
      sqlite3_bind_text(stmt, 2, (const char *)Data, (int)Size, SQLITE_STATIC);
      sqlite3_bind_int64(stmt, 3, userdata_value);

      while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
      }

      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  sqlite3_close(db);
  return 0;
}