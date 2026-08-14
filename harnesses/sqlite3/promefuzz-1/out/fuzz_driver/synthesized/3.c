// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_result_error at sqlite3.c:72332:17 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static void fuzz_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  if (!ctx || !argv || !argv[0]) return;

  sqlite3_value *v = argv[0];

  (void)sqlite3_value_int(v);
  (void)sqlite3_value_bytes(v);
  sqlite3_result_value(ctx, v);
  {
    const void *blob = sqlite3_value_blob(v);
    int alloc_sz = sqlite3_value_bytes(v);
    if (alloc_sz < 0) alloc_sz = 0;
    void *mem = sqlite3_malloc(alloc_sz > 0 ? alloc_sz : 1);
    sqlite3_result_error(ctx, (const char *)blob, alloc_sz > 0 ? alloc_sz : -1);
    sqlite3_result_blob(ctx, blob, alloc_sz > 0 ? alloc_sz : 0, SQLITE_TRANSIENT);
    sqlite3_free(mem);
  }
}

int LLVMFuzzerTestOneInput_3(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *sql = NULL;
  int rc;

  if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
    if (db) sqlite3_close(db);
    return 0;
  }

  rc = sqlite3_create_function(db, "fuzzfunc", 1, SQLITE_UTF8, NULL, fuzz_func, NULL, NULL);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return 0;
  }

  rc = sqlite3_exec(db, "CREATE TABLE t(x);", NULL, NULL, NULL);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return 0;
  }

  rc = sqlite3_exec(db, "INSERT INTO t VALUES(?1);", NULL, NULL, NULL);
  (void)rc;

  sql = sqlite3_mprintf("SELECT fuzzfunc(?1);");
  if (!sql) {
    sqlite3_close(db);
    return 0;
  }

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_free(sql);
  if (rc != SQLITE_OK || !stmt) {
    if (stmt) sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
  }

  if (Size == 0) {
    sqlite3_bind_null(stmt, 1);
  } else {
    switch (Data[0] % 5) {
      case 0:
        sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
        break;
      case 1:
        sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_TRANSIENT);
        break;
      case 2: {
        int val = 0;
        size_t n = Size < sizeof(val) ? Size : sizeof(val);
        memcpy(&val, Data, n);
        sqlite3_bind_int(stmt, 1, val);
        break;
      }
      case 3: {
        sqlite3_int64 val = 0;
        size_t n = Size < sizeof(val) ? Size : sizeof(val);
        memcpy(&val, Data, n);
        sqlite3_bind_int64(stmt, 1, val);
        break;
      }
      default:
        sqlite3_bind_null(stmt, 1);
        break;
    }
  }

  (void)sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}