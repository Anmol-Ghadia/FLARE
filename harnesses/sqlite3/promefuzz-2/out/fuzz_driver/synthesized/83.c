// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text16 at sqlite3.c:72400:17 in sqlite3.h
// sqlite3_result_text16 at sqlite3.c:72400:17 in sqlite3.h
// sqlite3_result_text16le at sqlite3.c:72418:17 in sqlite3.h
// sqlite3_result_text16be at sqlite3.c:72409:17 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_result_error16 at sqlite3.c:72338:17 in sqlite3.h
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

static void udf_target(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  (void)argc;
  if( argv==0 || argc<1 ){
    sqlite3_result_null(ctx);
    return;
  }

  const void *blob = sqlite3_value_blob(argv[0]);
  int bytes = sqlite3_value_bytes(argv[0]);
  if( bytes < 0 ) bytes = 0;

  int mode = 0;
  if( argc > 1 ){
    mode = sqlite3_value_int(argv[1]);
  }

  switch( mode % 8 ){
    case 0: {
      sqlite3_result_text(ctx, (const char *)blob, bytes, SQLITE_TRANSIENT);
      break;
    }
    case 1: {
      sqlite3_result_text(ctx, (const char *)blob, -1, SQLITE_TRANSIENT);
      break;
    }
    case 2: {
      sqlite3_result_text16(ctx, blob, bytes, SQLITE_TRANSIENT);
      break;
    }
    case 3: {
      sqlite3_result_text16(ctx, blob, -1, SQLITE_TRANSIENT);
      break;
    }
    case 4: {
      sqlite3_result_text16le(ctx, blob, bytes, SQLITE_TRANSIENT);
      break;
    }
    case 5: {
      sqlite3_result_text16be(ctx, blob, bytes, SQLITE_TRANSIENT);
      break;
    }
    case 6: {
      sqlite3_result_blob(ctx, blob, bytes, SQLITE_TRANSIENT);
      break;
    }
    default: {
      sqlite3_result_error16(ctx, blob, (mode & 1) ? -1 : bytes);
      break;
    }
  }
}

int LLVMFuzzerTestOneInput_83(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;

  FILE *fp = fopen("./dummy_file", "wb");
  if( fp ){
    if( Size ) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if( rc != SQLITE_OK || db == NULL ){
    if( db ) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzz_result", 2, SQLITE_UTF8, NULL, udf_target, NULL, NULL);

  sqlite3_exec(db,
    "CREATE TABLE t(x BLOB, y INT);"
    "INSERT INTO t VALUES(readfile('./dummy_file'), 0);"
    "INSERT INTO t VALUES(readfile('./dummy_file'), 1);"
    "INSERT INTO t VALUES(readfile('./dummy_file'), 2);"
    "INSERT INTO t VALUES(readfile('./dummy_file'), 3);"
    "INSERT INTO t VALUES(readfile('./dummy_file'), 4);"
    "INSERT INTO t VALUES(readfile('./dummy_file'), 5);"
    "INSERT INTO t VALUES(readfile('./dummy_file'), 6);"
    "INSERT INTO t VALUES(readfile('./dummy_file'), 7);",
    NULL, NULL, &errmsg);
  if( errmsg ){
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  {
    const char *sqls[] = {
      "SELECT fuzz_result(x, y) FROM t;",
      "SELECT fuzz_result(CAST(x AS TEXT), y) FROM t;",
      "SELECT fuzz_result(substr(x,1,64), y) FROM t;",
      "SELECT fuzz_result(zeroblob(length(x)%256), y) FROM t;",
      "SELECT fuzz_result(x, abs(length(x))%16) FROM t;"
    };
    size_t i;
    for( i = 0; i < sizeof(sqls)/sizeof(sqls[0]); i++ ){
      rc = sqlite3_prepare_v2(db, sqls[i], -1, &stmt, NULL);
      if( rc == SQLITE_OK && stmt ){
        while( (rc = sqlite3_step(stmt)) == SQLITE_ROW ){
        }
      }
      if( stmt ){
        sqlite3_finalize(stmt);
        stmt = NULL;
      }
    }
  }

  sqlite3_close(db);
  return 0;
}