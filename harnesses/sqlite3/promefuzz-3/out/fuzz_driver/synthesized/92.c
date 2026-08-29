// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function_v2 at sqlite3.c:154242:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_user_data at sqlite3.c:72704:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function_v2 at sqlite3.c:154242:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_result_subtype at sqlite3.c:72368:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_get_auxdata at sqlite3.c:72813:18 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_result_subtype at sqlite3.c:72368:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_result_error_nomem at sqlite3.c:72465:17 in sqlite3.h
// sqlite3_set_auxdata at sqlite3.c:72841:17 in sqlite3.h
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_value_bytes16 at sqlite3.c:72094:16 in sqlite3.h
// sqlite3_result_error16 at sqlite3.c:72338:17 in sqlite3.h
// sqlite3_result_error16 at sqlite3.c:72338:17 in sqlite3.h
// sqlite3_result_error16 at sqlite3.c:72338:17 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_error_nomem at sqlite3.c:72465:17 in sqlite3.h
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_value_bytes16 at sqlite3.c:72094:16 in sqlite3.h
// sqlite3_result_error16 at sqlite3.c:72338:17 in sqlite3.h
// sqlite3_result_error16 at sqlite3.c:72338:17 in sqlite3.h
// sqlite3_result_subtype at sqlite3.c:72368:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_subtype at sqlite3.c:72368:17 in sqlite3.h
// sqlite3_result_error16 at sqlite3.c:72338:17 in sqlite3.h
// sqlite3_result_error_nomem at sqlite3.c:72465:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static void aux_destructor(void *p){
  sqlite3_free(p);
}

static void fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  void *ud = sqlite3_user_data(ctx);
  unsigned int st = ud ? *(unsigned char *)ud : 0;
  sqlite3_result_subtype(ctx, st);

  if(argc > 0 && argv && argv[0]){
    void *oldp = sqlite3_get_auxdata(ctx, 0);
    if(oldp){
      sqlite3_result_subtype(ctx, ((unsigned int)(uintptr_t)oldp) & 0xFFu);
    }

    {
      void *p = sqlite3_malloc(16);
      if(!p){
        sqlite3_result_error_nomem(ctx);
        return;
      }
      memcpy(p, "AUXDATA", 8);
      sqlite3_set_auxdata(ctx, 0, p, aux_destructor);
    }

    {
      const void *txt16 = sqlite3_value_text16(argv[0]);
      int bytes = sqlite3_value_bytes16(argv[0]);
      if(txt16){
        if(bytes < 0){
          sqlite3_result_error16(ctx, txt16, -1);
        }else{
          if((bytes & 1) != 0) bytes--;
          sqlite3_result_error16(ctx, txt16, bytes);
        }
        return;
      }
    }
  }

  {
    static const unsigned short msg[] = { 'E','R','R',0 };
    sqlite3_result_error16(ctx, msg, -1);
  }
}

static void fuzz_step(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  int req = 0;
  unsigned char *state;

  if(argc > 0 && argv && argv[0]){
    req = sqlite3_value_int(argv[0]) & 0x3FF;
  }

  state = (unsigned char *)sqlite3_aggregate_context(ctx, req);
  if(req > 0 && !state){
    sqlite3_result_error_nomem(ctx);
    return;
  }

  if(state){
    state[0] ^= (unsigned char)argc;
  }

  if(argc > 1 && argv && argv[1]){
    const void *txt16 = sqlite3_value_text16(argv[1]);
    int bytes = sqlite3_value_bytes16(argv[1]);
    if(txt16){
      if(bytes < 0){
        sqlite3_result_error16(ctx, txt16, -1);
      }else{
        if((bytes & 1) != 0) bytes--;
        sqlite3_result_error16(ctx, txt16, bytes);
      }
    }
  }

  sqlite3_result_subtype(ctx, (unsigned int)(req & 0xFF));
}

static void fuzz_final(sqlite3_context *ctx){
  unsigned char *state = (unsigned char *)sqlite3_aggregate_context(ctx, 0);
  if(state){
    sqlite3_result_subtype(ctx, state[0]);
    {
      static const unsigned short done[] = { 'D','O','N','E',0 };
      sqlite3_result_error16(ctx, done, -1);
    }
  }else{
    sqlite3_result_error_nomem(ctx);
  }
}

int LLVMFuzzerTestOneInput_92(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *sqlbuf = NULL;
  int rc;
  unsigned char *userdata = NULL;
  size_t sqlcap;
  int mode;

  if(sqlite3_initialize() != SQLITE_OK){
    return 0;
  }

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if(fp){
      if(Size) fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  rc = sqlite3_open(":memory:", &db);
  if(rc != SQLITE_OK || !db){
    if(db) sqlite3_close(db);
    return 0;
  }

  userdata = (unsigned char *)sqlite3_malloc(1);
  if(!userdata){
    sqlite3_close(db);
    return 0;
  }
  userdata[0] = (unsigned char)(Size ? Data[0] : 0);

  rc = sqlite3_create_function_v2(
      db, "fuzz_scalar", -1, SQLITE_UTF8, userdata,
      fuzz_scalar, NULL, NULL, aux_destructor);
  if(rc != SQLITE_OK){
    sqlite3_free(userdata);
    sqlite3_close(db);
    return 0;
  }

  rc = sqlite3_create_function_v2(
      db, "fuzz_agg", -1, SQLITE_UTF8, NULL,
      NULL, fuzz_step, fuzz_final, NULL);
  if(rc != SQLITE_OK){
    sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db,
               "CREATE TABLE t(a,b,c);"
               "INSERT INTO t VALUES(1,'x',X'0102');"
               "INSERT INTO t VALUES(2,'y',X'0304');"
               "INSERT INTO t VALUES(NULL,'z',NULL);",
               NULL, NULL, NULL);

  mode = (Size > 1) ? (Data[1] % 4) : 0;
  sqlcap = Size + 256;
  sqlbuf = (char *)malloc(sqlcap);
  if(!sqlbuf){
    sqlite3_close(db);
    return 0;
  }

  switch(mode){
    case 0:
      snprintf(sqlbuf, sqlcap,
               "SELECT fuzz_scalar(?1), fuzz_scalar(?1), fuzz_scalar(a), "
               "fuzz_agg(a,b) FROM t;");
      break;
    case 1:
      snprintf(sqlbuf, sqlcap,
               "SELECT fuzz_scalar(CAST(?1 AS TEXT)), "
               "fuzz_agg(length(?1), ?1) FROM t;");
      break;
    case 2:
      snprintf(sqlbuf, sqlcap,
               "WITH c(x) AS (SELECT ?1 UNION ALL SELECT a FROM t) "
               "SELECT fuzz_scalar(x), fuzz_agg(x, x) FROM c;");
      break;
    default:
      snprintf(sqlbuf, sqlcap,
               "SELECT fuzz_scalar(zeroblob(%u)), fuzz_agg(%u, ?1);",
               (unsigned)(Size & 0xFF), (unsigned)(Size & 0x3FF));
      break;
  }

  rc = sqlite3_prepare_v2(db, sqlbuf, -1, &stmt, NULL);
  if(rc == SQLITE_OK && stmt){
    sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }else{
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  rc = sqlite3_prepare_v2(db, "SELECT fuzz_scalar(?1), fuzz_scalar(?1);", -1, &stmt, NULL);
  if(rc == SQLITE_OK && stmt){
    sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }else{
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  free(sqlbuf);
  sqlite3_close(db);
  return 0;
}