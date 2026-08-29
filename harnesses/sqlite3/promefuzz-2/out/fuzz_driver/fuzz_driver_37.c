// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static void fuzz_agg(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  int n = 0;
  void *p;
  int i;

  if( argc > 0 && argv[0] ){
    n = sqlite3_value_bytes(argv[0]);
  }

  if( argc > 1 && argv[1] ){
    n ^= sqlite3_value_bytes(argv[1]);
  }

  n = (n & 0x3ff) - ((n & 0x400) ? 1024 : 0);

  p = sqlite3_aggregate_context(ctx, n);
  if( p && argc > 0 && argv[0] ){
    const unsigned char *txt = sqlite3_value_text(argv[0]);
    int len = sqlite3_value_bytes(argv[0]);
    if( txt && len > 0 ){
      int copy = len < 32 ? len : 32;
      memcpy(p, txt, (size_t)copy);
    }
  }

  for( i = 0; i < argc; i++ ){
    if( argv[i] ){
      (void)sqlite3_value_bytes(argv[i]);
      (void)sqlite3_value_text(argv[i]);
    }
  }

  (void)sqlite3_aggregate_context(ctx, 0);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  int rc;
  int step_rc;
  const char *sql =
    "WITH RECURSIVE c(x) AS ("
    "  SELECT 1 "
    "  UNION ALL "
    "  SELECT x+1 FROM c WHERE x<4"
    ") "
    "SELECT fuzz_agg(?1, ?2) FROM c;";

  rc = sqlite3_open(":memory:", &db);
  if( rc != SQLITE_OK || db == NULL ){
    if( db ) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(
    db, "fuzz_agg", 2, SQLITE_UTF8, NULL, NULL, fuzz_agg, NULL
  );

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if( rc == SQLITE_OK && stmt ){
    size_t split = Size / 2;
    sqlite3_bind_blob(stmt, 1, Data, (int)split, SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 2, Data + split, (int)(Size - split), SQLITE_TRANSIENT);

    while( (step_rc = sqlite3_step(stmt)) == SQLITE_ROW ){
    }

    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);

    sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 2, Data, (int)Size, SQLITE_TRANSIENT);

    while( (step_rc = sqlite3_step(stmt)) == SQLITE_ROW ){
    }
  }

  if( stmt ) sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}