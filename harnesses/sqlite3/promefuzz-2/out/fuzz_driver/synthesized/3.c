// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
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
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static void fuzz_sql_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  if (!ctx || argc <= 0 || !argv || !argv[0]) {
    return;
  }

  sqlite3_value *v = argv[0];

  (void)sqlite3_value_int(v);
  (void)sqlite3_value_bytes(v);
  sqlite3_result_value(ctx, v);
  {
    const void *blob = sqlite3_value_blob(v);
    int alloc_sz = 0;
    void *mem = NULL;
    int err_len = -1;
    int blob_len = sqlite3_value_bytes(v);

    if (blob_len < 0) blob_len = 0;
    alloc_sz = blob_len;
    if (alloc_sz <= 0) alloc_sz = 1;

    mem = sqlite3_malloc(alloc_sz);

    if (blob && mem && blob_len > 0) {
      memcpy(mem, blob, (size_t)blob_len);
    } else if (mem && alloc_sz > 0) {
      memset(mem, 0, (size_t)alloc_sz);
    }

    if (blob && blob_len > 0) {
      err_len = blob_len;
    }

    sqlite3_result_error(ctx, (const char *)(blob ? blob : "fuzz"), err_len);
    sqlite3_result_blob(ctx, mem, blob_len, SQLITE_TRANSIENT);
    sqlite3_free(mem);
  }
}

int LLVMFuzzerTestOneInput_3(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  int rc;
  int mode;
  int n1, n2;

  if (!Data) {
    return 0;
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || !db) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzzfunc", 1, SQLITE_UTF8, NULL, fuzz_sql_func, NULL, NULL);

  mode = (Size > 0) ? (Data[0] % 6) : 0;
  n1 = (Size > 1) ? (int)(Data[1] % 256) : 0;
  n2 = (Size > 2) ? (int)(Data[2] % 256) : 0;

  switch (mode) {
    case 0:
      rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(?1);", -1, &stmt, NULL);
      if (rc == SQLITE_OK) {
        sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
      }
      break;
    case 1:
      rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(?1);", -1, &stmt, NULL);
      if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_STATIC);
      }
      break;
    case 2:
      rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(?1);", -1, &stmt, NULL);
      if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, (int)Size);
      }
      break;
    case 3:
      rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(CAST(?1 AS TEXT));", -1, &stmt, NULL);
      if (rc == SQLITE_OK) {
        sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
      }
      break;
    case 4:
      rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(randomblob(?1));", -1, &stmt, NULL);
      if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, n1);
      }
      break;
    default:
      rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(substr(?1,1,?2));", -1, &stmt, NULL);
      if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, n2);
      }
      break;
  }

  if (rc == SQLITE_OK && stmt) {
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    }
  }

  if (stmt) {
    sqlite3_finalize(stmt);
  }
  sqlite3_close(db);
  return 0;
}