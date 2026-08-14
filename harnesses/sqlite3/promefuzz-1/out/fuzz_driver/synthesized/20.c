// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
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
#include <stdio.h>
#include <sqlite3.h>

static void udf(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  const unsigned char *txt = sqlite3_value_text(argv[0]);

  sqlite3_uint64 alloc_size = 1;
  if (txt) {
    alloc_size = (sqlite3_uint64)strlen((const char *)txt) + 1;
  }

  void *mem = sqlite3_malloc64(alloc_size);
  if (mem && txt) {
    memcpy(mem, txt, (size_t)alloc_size);
  } else if (mem) {
    ((char *)mem)[0] = '\0';
  }

  sqlite3_result_text(ctx, (const char *)mem, -1, SQLITE_TRANSIENT);
  sqlite3_free(mem);
}

int LLVMFuzzerTestOneInput_20(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzzfunc", 1, SQLITE_UTF8, NULL, udf, NULL, NULL);

  sqlite3_exec(db, "CREATE TABLE t(x);", NULL, NULL, NULL);

  sqlite3_stmt *stmt = NULL;
  if (sqlite3_prepare_v2(db, "SELECT fuzzfunc(?1);", -1, &stmt, NULL) == SQLITE_OK) {
    sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
    sqlite3_step(stmt);
  }
  if (stmt) sqlite3_finalize(stmt);

  if (sqlite3_prepare_v2(db, "SELECT fuzzfunc(CAST(?1 AS TEXT));", -1, &stmt, NULL) == SQLITE_OK) {
    sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_STATIC);
    sqlite3_step(stmt);
  }
  if (stmt) sqlite3_finalize(stmt);

  if (sqlite3_prepare_v2(db, "SELECT fuzzfunc(?1);", -1, &stmt, NULL) == SQLITE_OK) {
    if (Size >= 8) {
      sqlite3_int64 v = 0;
      memcpy(&v, Data, 8);
      sqlite3_bind_int64(stmt, 1, v);
    } else {
      sqlite3_bind_null(stmt, 1);
    }
    sqlite3_step(stmt);
  }
  if (stmt) sqlite3_finalize(stmt);

  sqlite3_close(db);
  return 0;
}