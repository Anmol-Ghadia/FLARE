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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
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