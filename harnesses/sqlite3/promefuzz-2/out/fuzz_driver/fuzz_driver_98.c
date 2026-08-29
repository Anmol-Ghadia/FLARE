// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_result_error_nomem at sqlite3.c:72465:17 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text16le at sqlite3.c:72418:17 in sqlite3.h
// sqlite3_result_zeroblob at sqlite3.c:72432:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_error_toobig at sqlite3.c:72457:17 in sqlite3.h
// sqlite3_result_error_nomem at sqlite3.c:72465:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_zeroblob at sqlite3.c:72432:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_text16le at sqlite3.c:72418:17 in sqlite3.h
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

static void fuzz_result_func(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  (void)argc;
  if( ctx==NULL ) return;

  if( argv==NULL || argv[0]==NULL ){
    sqlite3_result_error_nomem(ctx);
    return;
  }

  {
    const unsigned char *blob = sqlite3_value_blob(argv[0]);
    int bytes = sqlite3_value_bytes(argv[0]);
    int selector = 0;
    int n = 0;

    if( bytes > 0 && blob ){
      selector = blob[0] % 6;
    }else{
      selector = 0;
    }

    if( bytes >= 5 && blob ){
      n = (int)(
          ((unsigned int)blob[1]) |
          ((unsigned int)blob[2] << 8) |
          ((unsigned int)blob[3] << 16) |
          ((unsigned int)blob[4] << 24));
    }else{
      n = bytes;
    }

    switch( selector ){
      case 0: {
        const char *z = (const char *)blob;
        int len = bytes > 0 ? bytes - 1 : 0;
        if( bytes > 6 && blob[5] & 1 ) len = -1;
        sqlite3_result_text(ctx, z, len, SQLITE_TRANSIENT);
        break;
      }
      case 1: {
        const void *z16 = (const void *)blob;
        int len16 = bytes;
        if( bytes > 6 && blob[5] & 2 ) len16 = -1;
        sqlite3_result_text16le(ctx, z16, len16, SQLITE_TRANSIENT);
        break;
      }
      case 2: {
        sqlite3_result_zeroblob(ctx, n);
        break;
      }
      case 3: {
        sqlite3_result_int(ctx, n);
        break;
      }
      case 4: {
        sqlite3_result_error_toobig(ctx);
        break;
      }
      default: {
        sqlite3_result_error_nomem(ctx);
        break;
      }
    }

    if( bytes > 7 && blob ){
      switch( blob[6] % 4 ){
        case 0:
          sqlite3_result_int(ctx, n ^ bytes);
          break;
        case 1:
          sqlite3_result_zeroblob(ctx, bytes);
          break;
        case 2:
          sqlite3_result_text(ctx, (const char *)blob, bytes > 0 ? bytes : 0, SQLITE_TRANSIENT);
          break;
        default:
          sqlite3_result_text16le(ctx, blob, bytes, SQLITE_TRANSIENT);
          break;
      }
    }
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if( fp ){
    if( Size ) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if( rc != SQLITE_OK || db == NULL ){
    if( db ) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzz_result", 1, SQLITE_UTF8, NULL,
                          fuzz_result_func, NULL, NULL);

  sqlite3_exec(db,
      "CREATE TABLE t(x);"
      "INSERT INTO t(x) VALUES (zeroblob(0));"
      "INSERT INTO t(x) VALUES (readfile('./dummy_file'));",
      NULL, NULL, &errmsg);
  if( errmsg ){
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  rc = sqlite3_prepare_v2(db, "SELECT fuzz_result(?1);", -1, &stmt, NULL);
  if( rc == SQLITE_OK && stmt ){
    sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_reset(stmt);

    sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_reset(stmt);

    sqlite3_bind_zeroblob(stmt, 1, (int)(Size > 0x7fffffff ? 0x7fffffff : Size));
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  rc = sqlite3_prepare_v2(db,
      "SELECT fuzz_result(CAST(readfile('./dummy_file') AS BLOB));",
      -1, &stmt, NULL);
  if( rc == SQLITE_OK && stmt ){
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  sqlite3_close(db);
  return 0;
}