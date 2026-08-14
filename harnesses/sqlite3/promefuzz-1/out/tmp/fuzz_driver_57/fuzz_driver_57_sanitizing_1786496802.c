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
  (void)argc;
  (void)argv;

  sqlite3_result_int(ctx, 12345);
  sqlite3_result_zeroblob(ctx, 0);

  {
    void *ud = sqlite3_user_data(ctx);
    if(ud){
      volatile unsigned char sink = *((unsigned char *)ud);
      (void)sink;
    }
  }

  {
    char *p = (char *)sqlite3_malloc(16);
    if(p){
      memset(p, 0x41, 16);
      sqlite3_set_auxdata(ctx, 0, p, aux_destructor);
    }
  }

  {
    char *p = (char *)sqlite3_malloc(8);
    if(p){
      memset(p, 0x42, 8);
      sqlite3_set_auxdata(ctx, 1, p, aux_destructor);
    }
  }
}

static void fuzz_scalar2(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  int n = 0;
  if(argc > 0 && argv && argv[0]){
    n = sqlite3_value_int(argv[0]);
  }

  sqlite3_result_zeroblob(ctx, n);
  sqlite3_result_int(ctx, n);

  {
    char *p = (char *)sqlite3_malloc((n & 31) + 1);
    if(p){
      memset(p, 0x43, (size_t)((n & 31) + 1));
      sqlite3_set_auxdata(ctx, 0, p, aux_destructor);
    }
  }

  (void)sqlite3_user_data(ctx);
}

typedef struct AggState {
  int calls;
  int sum;
  int last_count;
} AggState;

static void fuzz_step(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  AggState *st;
  int count = sqlite3_aggregate_count(ctx);
  st = (AggState *)sqlite3_aggregate_context(ctx, (int)sizeof(*st));
  if(st){
    st->calls++;
    st->last_count = count;
    if(argc > 0 && argv && argv[0]){
      st->sum += sqlite3_value_int(argv[0]);
    }
  }

  {
    int n = 0;
    if(argc > 1 && argv && argv[1]){
      n = sqlite3_value_int(argv[1]);
    }
    sqlite3_result_zeroblob(ctx, n);
    sqlite3_result_int(ctx, count);
  }

  {
    char *p = (char *)sqlite3_malloc(4);
    if(p){
      memcpy(p, "agg", 4);
      sqlite3_set_auxdata(ctx, 0, p, aux_destructor);
    }
  }

  (void)sqlite3_user_data(ctx);
}

static void fuzz_final(sqlite3_context *ctx){
  AggState *st;
  int count = sqlite3_aggregate_count(ctx);

  st = (AggState *)sqlite3_aggregate_context(ctx, 0);
  if(st){
    sqlite3_result_int(ctx, st->sum ^ st->calls ^ st->last_count ^ count);
  }else{
    sqlite3_result_int(ctx, count);
  }

  sqlite3_result_zeroblob(ctx, count & 0xff);
  (void)sqlite3_user_data(ctx);
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
    int v4 = read_u32(Data, Size, &off);

    const char *sqls[] = {
      "SELECT fuzz_scalar(?1, ?2);",
      "SELECT fuzz_scalar2(?1);",
      "SELECT fuzz_agg(a, b) FROM t;",
      "SELECT fuzz_agg(?1, ?2);",
      "SELECT fuzz_scalar2(length(readfile('./dummy_file')));",
      "SELECT fuzz_scalar(a, b) FROM t;",
      "SELECT fuzz_agg(a, ?1) FROM t WHERE a > ?2;",
      "SELECT fuzz_scalar2(?1), fuzz_agg(a,b) FROM t;"
    };
    size_t i;
    for(i = 0; i < sizeof(sqls)/sizeof(sqls[0]); i++){
      stmt = NULL;
      rc = sqlite3_prepare_v2(db, sqls[i], -1, &stmt, NULL);
      if(rc == SQLITE_OK && stmt){
        sqlite3_bind_int(stmt, 1, v1);
        sqlite3_bind_int(stmt, 2, v2);
        sqlite3_bind_int(stmt, 3, v3);
        sqlite3_bind_int(stmt, 4, v4);

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