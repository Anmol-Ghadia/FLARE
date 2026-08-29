// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_user_data at sqlite3.c:72704:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_get_auxdata at sqlite3.c:72813:18 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_get_auxdata at sqlite3.c:72813:18 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_set_auxdata at sqlite3.c:72841:17 in sqlite3.h
// sqlite3_result_error_nomem at sqlite3.c:72465:17 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_set_auxdata at sqlite3.c:72841:17 in sqlite3.h
// sqlite3_result_error_nomem at sqlite3.c:72465:17 in sqlite3.h
// sqlite3_get_auxdata at sqlite3.c:72813:18 in sqlite3.h
// sqlite3_get_auxdata at sqlite3.c:72813:18 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_value_double at sqlite3.c:72097:19 in sqlite3.h
// sqlite3_value_double at sqlite3.c:72097:19 in sqlite3.h
// sqlite3_result_double at sqlite3.c:72328:17 in sqlite3.h
// sqlite3_result_error_nomem at sqlite3.c:72465:17 in sqlite3.h
// sqlite3_result_error16 at sqlite3.c:72338:17 in sqlite3.h
// sqlite3_result_error16 at sqlite3.c:72338:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_result_double at sqlite3.c:72328:17 in sqlite3.h
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

static void fuzz_udf(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  void *ud = sqlite3_user_data(ctx);
  void *aux0 = sqlite3_get_auxdata(ctx, 0);
  void *aux1 = sqlite3_get_auxdata(ctx, 1);
  unsigned char selector = 0;

  if( ud ){
    selector = *(unsigned char *)ud;
  }

  if( argc > 0 && aux0 == NULL ){
    int n0 = sqlite3_value_bytes(argv[0]);
    const void *b0 = sqlite3_value_blob(argv[0]);
    void *copy0 = sqlite3_malloc(n0 > 0 ? n0 : 1);
    if( copy0 ){
      if( n0 > 0 && b0 ) memcpy(copy0, b0, (size_t)n0);
      sqlite3_set_auxdata(ctx, 0, copy0, aux_destructor);
    }else{
      sqlite3_result_error_nomem(ctx);
      return;
    }
  }

  if( argc > 1 && aux1 == NULL ){
    int n1 = sqlite3_value_bytes(argv[1]);
    const void *b1 = sqlite3_value_blob(argv[1]);
    void *copy1 = sqlite3_malloc(n1 > 0 ? n1 : 1);
    if( copy1 ){
      if( n1 > 0 && b1 ) memcpy(copy1, b1, (size_t)n1);
      sqlite3_set_auxdata(ctx, 1, copy1, aux_destructor);
    }else{
      sqlite3_result_error_nomem(ctx);
      return;
    }
  }

  aux0 = sqlite3_get_auxdata(ctx, 0);
  aux1 = sqlite3_get_auxdata(ctx, 1);

  switch(selector % 6){
    case 0:
      sqlite3_result_null(ctx);
      break;
    case 1: {
      double d = 0.0;
      if( argc > 0 ){
        d += sqlite3_value_double(argv[0]);
      }
      if( argc > 1 ){
        d += sqlite3_value_double(argv[1]);
      }
      if( aux0 ) d += 1.0;
      if( aux1 ) d += 2.0;
      sqlite3_result_double(ctx, d);
      break;
    }
    case 2:
      sqlite3_result_error_nomem(ctx);
      break;
    case 3: {
      static const unsigned char utf16_msg[] = {
        'E', 0, 'R', 0, 'R', 0, 0, 0
      };
      sqlite3_result_error16(ctx, utf16_msg, -1);
      break;
    }
    case 4: {
      static const unsigned char utf16_msg2[] = {
        'B', 0, 'A', 0, 'D', 0, '!', 0, 0, 0
      };
      sqlite3_result_error16(ctx, utf16_msg2, 8);
      break;
    }
    default:
      sqlite3_result_null(ctx);
      sqlite3_result_double(ctx, 3.141592653589793);
      break;
  }
}

int LLVMFuzzerTestOneInput_43(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;
  unsigned char user_byte = 0;
  const uint8_t *payload = Data;
  size_t payload_size = Size;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if( fp ){
    if( Size ) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  if( Size > 0 ){
    user_byte = Data[0];
    payload = Data + 1;
    payload_size = Size - 1;
  }

  rc = sqlite3_open(":memory:", &db);
  if( rc != SQLITE_OK || db == NULL ){
    if( db ) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db,
    "CREATE TABLE t(x);"
    "INSERT INTO t VALUES(1),(2),(3);",
    NULL, NULL, NULL);

  rc = sqlite3_create_function(
    db, "fuzzfunc", 2, SQLITE_UTF8, &user_byte, fuzz_udf, NULL, NULL
  );
  if( rc != SQLITE_OK ){
    sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db,
    "SELECT fuzzfunc('const1','const2');"
    "SELECT fuzzfunc('const1','const2');"
    "SELECT fuzzfunc(x,'const2') FROM t;"
    "SELECT fuzzfunc('other',x) FROM t;",
    NULL, NULL, &errmsg);
  if( errmsg ){
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  {
    const char *sqls[] = {
      "SELECT fuzzfunc(?1, ?2);",
      "SELECT fuzzfunc('A', 'B');",
      "SELECT fuzzfunc(x, x) FROM t;",
      "SELECT fuzzfunc(zeroblob(4), ?1);"
    };
    size_t i;
    for( i = 0; i < sizeof(sqls)/sizeof(sqls[0]); i++ ){
      rc = sqlite3_prepare_v2(db, sqls[i], -1, &stmt, NULL);
      if( rc != SQLITE_OK || stmt == NULL ){
        stmt = NULL;
        continue;
      }

      sqlite3_bind_blob(stmt, 1, payload, (int)payload_size, SQLITE_TRANSIENT);
      sqlite3_bind_blob(stmt, 2, Data, (int)Size, SQLITE_TRANSIENT);

      while( (rc = sqlite3_step(stmt)) == SQLITE_ROW ){
      }

      sqlite3_reset(stmt);
      sqlite3_clear_bindings(stmt);

      sqlite3_bind_text(stmt, 1, (const char *)payload, (int)payload_size, SQLITE_TRANSIENT);
      sqlite3_bind_double(stmt, 2, (double)user_byte);

      while( (rc = sqlite3_step(stmt)) == SQLITE_ROW ){
      }

      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  sqlite3_close(db);
  return 0;
}