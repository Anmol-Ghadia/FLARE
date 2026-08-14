// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_aggregate_count at sqlite3.c:72894:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_set_auxdata at sqlite3.c:72841:17 in sqlite3.h
// sqlite3_user_data at sqlite3.c:72704:18 in sqlite3.h
// sqlite3_aggregate_count at sqlite3.c:72894:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_user_data at sqlite3.c:72704:18 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_zeroblob at sqlite3.c:72432:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_set_auxdata at sqlite3.c:72841:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_set_auxdata at sqlite3.c:72841:17 in sqlite3.h
// sqlite3_user_data at sqlite3.c:72704:18 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_zeroblob at sqlite3.c:72432:17 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_set_auxdata at sqlite3.c:72841:17 in sqlite3.h
// sqlite3_user_data at sqlite3.c:72704:18 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_zeroblob at sqlite3.c:72432:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static void aux_destructor(void *p){
  sqlite3_free(p);
}

static int read_u32(const uint8_t *Data, size_t Size, size_t *off){
  uint32_t v = 0;
  size_t i;
  for(i = 0; i < 4; i++){
    v <<= 8;
    if(*off < Size){
      v |= Data[*off];
      (*off)++;
    }
  }
  return (int)v;
}

static void fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  int mode = 0;
  int a0 = 0;
  int a1 = 0;

  if(argc > 0 && argv && argv[0]) a0 = sqlite3_value_int(argv[0]);
  if(argc > 1 && argv && argv[1]) a1 = sqlite3_value_int(argv[1]);
  mode = (a0 ^ a1) & 3;

  {
    char *p = (char *)sqlite3_malloc((size_t)((a0 & 31) + 1));
    if(p){
      memset(p, 0x41, (size_t)((a0 & 31) + 1));
      sqlite3_set_auxdata(ctx, 0, p, aux_destructor);
    }
  }
  {
    char *p = (char *)sqlite3_malloc((size_t)((a1 & 15) + 1));
    if(p){
      memset(p, 0x42, (size_t)((a1 & 15) + 1));
      sqlite3_set_auxdata(ctx, 1, p, aux_destructor);
    }
  }

  {
    void *ud = sqlite3_user_data(ctx);
    if(ud){
      volatile unsigned char sink = *((unsigned char *)ud);
      (void)sink;
    }
  }

  if(mode & 1){
    sqlite3_result_int(ctx, a0 ^ a1);
  }else{
    int n = a0;
    if(n < 0) n = -n;
    n &= 0xfff;
    sqlite3_result_zeroblob(ctx, n);
  }
}

static void fuzz_scalar2(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  int n = 0;

  if(argc > 0 && argv && argv[0]){
    n = sqlite3_value_int(argv[0]);
  }

  {
    char *p = (char *)sqlite3_malloc((size_t)((n & 31) + 1));
    if(p){
      memset(p, 0x43, (size_t)((n & 31) + 1));
      sqlite3_set_auxdata(ctx, 0, p, aux_destructor);
    }
  }

  (void)sqlite3_user_data(ctx);

  if(n & 1){
    sqlite3_result_int(ctx, n);
  }else{
    int z = n;
    if(z < 0) z = -z;
    z &= 0xfff;
    sqlite3_result_zeroblob(ctx, z);
  }
}

typedef struct AggState {
  int calls;
  int sum;
  int last_count;
} AggState;

static void fuzz_step(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  AggState *st;
  int count = sqlite3_aggregate_count(ctx);
  int v0 = 0;
  int v1 = 0;

  if(argc > 0 && argv && argv[0]) v0 = sqlite3_value_int(argv[0]);
  if(argc > 1 && argv && argv[1]) v1 = sqlite3_value_int(argv[1]);

  st = (AggState *)sqlite3_aggregate_context(ctx, (int)sizeof(*st));
  if(st){
    st->calls++;
    st->last_count = count;
    st->sum += v0 ^ v1;
  }

  {
    char *p = (char *)sqlite3_malloc((size_t)((v0 & 7) + 1));
    if(p){
      memset(p, 0x44, (size_t)((v0 & 7) + 1));
      sqlite3_set_auxdata(ctx, 0, p, aux_destructor);
    }
  }

  (void)sqlite3_user_data(ctx);
}

static void fuzz_final(sqlite3_context *ctx){
  AggState *st;
  int count = sqlite3_aggregate_count(ctx);

  st = (AggState *)sqlite3_aggregate_context(ctx, 0);
  (void)sqlite3_user_data(ctx);

  if(st){
    if((st->calls ^ st->sum ^ st->last_count ^ count) & 1){
      sqlite3_result_int(ctx, st->sum ^ st->calls ^ st->last_count ^ count);
    }else{
      int n = st->sum ^ count;
      if(n < 0) n = -n;
      n &= 0xfff;
      sqlite3_result_zeroblob(ctx, n);
    }
  }else{
    sqlite3_result_int(ctx, count);
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;
  size_t off = 0;
  unsigned char user_byte = 0;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size > 0){
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if(rc != SQLITE_OK || db == NULL){
    if(db) sqlite3_close(db);
    return 0;
  }

  if(Size > 0){
    user_byte = Data[0];
    off = 1;
  }

  sqlite3_create_function(db, "fuzz_scalar", 2, SQLITE_UTF8, &user_byte,
                          fuzz_scalar, NULL, NULL);
  sqlite3_create_function(db, "fuzz_scalar2", 1, SQLITE_UTF8, &user_byte,
                          fuzz_scalar2, NULL, NULL);
  sqlite3_create_function(db, "fuzz_agg", 2, SQLITE_UTF8, &user_byte,
                          NULL, fuzz_step, fuzz_final);

  sqlite3_exec(db,
    "CREATE TABLE t(a,b);"
    "INSERT INTO t VALUES(1,2),(3,4),(5,6),(-7,8),(0,0);",
    NULL, NULL, NULL);

  {
    int v1 = read_u32(Data, Size, &off);
    int v2 = read_u32(Data, Size, &off);
    int v3 = read_u32(Data, Size, &off);

    const char *sqls[] = {
      "SELECT fuzz_scalar(?1, ?2);",
      "SELECT fuzz_scalar2(?1);",
      "SELECT fuzz_agg(a, b) FROM t;",
      "SELECT fuzz_agg(?1, ?2);",
      "SELECT fuzz_scalar(a, b) FROM t;",
      "SELECT fuzz_agg(a, ?1) FROM t WHERE a > ?2;"
    };
    size_t i;
    for(i = 0; i < sizeof(sqls)/sizeof(sqls[0]); i++){
      stmt = NULL;
      rc = sqlite3_prepare_v2(db, sqls[i], -1, &stmt, NULL);
      if(rc == SQLITE_OK && stmt){
        sqlite3_bind_int(stmt, 1, v1);
        sqlite3_bind_int(stmt, 2, v2);
        sqlite3_bind_int(stmt, 3, v3);

        while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
        }
      }
      if(stmt){
        sqlite3_finalize(stmt);
        stmt = NULL;
      }
    }
  }

  if(Size > 0){
    char *sql = (char *)malloc(Size + 1);
    if(sql){
      memcpy(sql, Data, Size);
      sql[Size] = '\0';
      sqlite3_exec(db, sql, NULL, NULL, &errmsg);
      if(errmsg){
        sqlite3_free(errmsg);
        errmsg = NULL;
      }
      free(sql);
    }
  }

  sqlite3_close(db);
  return 0;
}