// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_int64 at sqlite3.c:73046:25 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_int64 at sqlite3.c:73046:25 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_column_int64 at sqlite3.c:73046:25 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_result_pointer at sqlite3.c:72356:17 in sqlite3.h
// sqlite3_result_blob at sqlite3.c:72304:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
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

static void udf_target(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int mode = 0;
  sqlite3_int64 i64v = 0;
  const void *blob = NULL;
  int nblob = 0;
  const unsigned char *text = NULL;
  int ntext = 0;
  static const char ptype[] = "fuzz_ptr_type";

  if (argc > 0) {
    mode = sqlite3_value_int(argv[0]);
  }
  if (argc > 1) {
    blob = sqlite3_value_blob(argv[1]);
    nblob = sqlite3_value_bytes(argv[1]);
  }
  if (argc > 2) {
    text = sqlite3_value_text(argv[2]);
    ntext = sqlite3_value_bytes(argv[2]);
  }
  if (argc > 3) {
    i64v = sqlite3_value_int64(argv[3]);
  }

  switch ((unsigned)mode % 6U) {
    case 0:
      sqlite3_result_int64(ctx, i64v);
      break;
    case 1:
      sqlite3_result_blob(ctx, blob, nblob >= 0 ? nblob : 0, SQLITE_TRANSIENT);
      break;
    case 2:
      sqlite3_result_text(ctx, (const char *)text, ntext >= 0 ? ntext : 0, SQLITE_TRANSIENT);
      break;
    case 3:
      if (argc > 1) {
        sqlite3_result_value(ctx, argv[1]);
      } else {
        sqlite3_result_int64(ctx, 0);
      }
      break;
    case 4:
      sqlite3_result_pointer(ctx, (void *)blob, ptype, NULL);
      break;
    default:
      if (blob && nblob >= 0) {
        sqlite3_result_blob(ctx, blob, nblob, SQLITE_TRANSIENT);
      } else if (text && ntext >= 0) {
        sqlite3_result_text(ctx, (const char *)text, ntext, SQLITE_TRANSIENT);
      } else {
        sqlite3_result_int64(ctx, i64v);
      }
      break;
  }
}

static int exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
  }
  return rc;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  int rc;
  size_t file_sz;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    file_sz = Size > 4096 ? 4096 : Size;
    if (file_sz > 0) {
      fwrite(Data, 1, file_sz, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzzfunc", 4, SQLITE_UTF8, NULL, udf_target, NULL, NULL);

  exec_sql(db, "CREATE TABLE t(a,b,c,d);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS files(x);");

  rc = sqlite3_prepare_v2(
      db,
      "INSERT INTO t(a,b,c,d) VALUES(?1,?2,?3,?4);",
      -1,
      &stmt,
      NULL);
  if (rc == SQLITE_OK && stmt) {
    sqlite3_bind_int(stmt, 1, (Size > 0) ? Data[0] : 0);
    sqlite3_bind_blob(stmt, 2, Data, (int)Size, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, (const char *)Data, (int)Size, SQLITE_STATIC);

    if (Size >= 8) {
      sqlite3_int64 v = 0;
      memcpy(&v, Data, 8);
      sqlite3_bind_int64(stmt, 4, v);
    } else {
      sqlite3_bind_int64(stmt, 4, (sqlite3_int64)Size);
    }

    sqlite3_step(stmt);
  }
  if (stmt) {
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  rc = sqlite3_prepare_v2(
      db,
      "SELECT fuzzfunc(?1, ?2, ?3, ?4);",
      -1,
      &stmt,
      NULL);
  if (rc == SQLITE_OK && stmt) {
    sqlite3_bind_int(stmt, 1, (Size > 0) ? Data[0] : 0);
    sqlite3_bind_blob(stmt, 2, Data, (int)Size, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, (const char *)Data, (int)Size, SQLITE_STATIC);

    if (Size >= 16) {
      sqlite3_int64 v = 0;
      memcpy(&v, Data + 8, 8);
      sqlite3_bind_int64(stmt, 4, v);
    } else {
      sqlite3_bind_int64(stmt, 4, (sqlite3_int64)(Size * 3));
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      (void)sqlite3_column_type(stmt, 0);
      (void)sqlite3_column_blob(stmt, 0);
      (void)sqlite3_column_bytes(stmt, 0);
      (void)sqlite3_column_text(stmt, 0);
      (void)sqlite3_column_int64(stmt, 0);
    }
  }
  if (stmt) {
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  rc = sqlite3_prepare_v2(
      db,
      "SELECT fuzzfunc(a,b,c,d) FROM t;",
      -1,
      &stmt,
      NULL);
  if (rc == SQLITE_OK && stmt) {
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      (void)sqlite3_column_type(stmt, 0);
      (void)sqlite3_column_blob(stmt, 0);
      (void)sqlite3_column_bytes(stmt, 0);
      (void)sqlite3_column_text(stmt, 0);
      (void)sqlite3_column_int64(stmt, 0);
    }
  }
  if (stmt) {
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  rc = sqlite3_prepare_v2(
      db,
      "SELECT fuzzfunc(length(readfile('./dummy_file')), readfile('./dummy_file'), "
      "hex(readfile('./dummy_file')), ?1);",
      -1,
      &stmt,
      NULL);
  if (rc == SQLITE_OK && stmt) {
    sqlite3_bind_int64(stmt, 1, (sqlite3_int64)Size);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      (void)sqlite3_column_type(stmt, 0);
      (void)sqlite3_column_blob(stmt, 0);
      (void)sqlite3_column_bytes(stmt, 0);
      (void)sqlite3_column_text(stmt, 0);
      (void)sqlite3_column_int64(stmt, 0);
    }
  }
  if (stmt) {
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  sqlite3_close(db);
  return 0;
}