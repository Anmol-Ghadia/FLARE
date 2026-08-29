// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_context_db_handle at sqlite3.c:72719:21 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_result_error_code at sqlite3.c:72445:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_result_error_toobig at sqlite3.c:72457:17 in sqlite3.h
// sqlite3_result_error_nomem at sqlite3.c:72465:17 in sqlite3.h
// sqlite3_result_error_code at sqlite3.c:72445:17 in sqlite3.h
// sqlite3_result_error_code at sqlite3.c:72445:17 in sqlite3.h
// sqlite3_result_error_code at sqlite3.c:72445:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_context_db_handle at sqlite3.c:72719:21 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_result_error_code at sqlite3.c:72445:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_context_db_handle at sqlite3.c:72719:21 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_result_error_code at sqlite3.c:72445:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_result_error_toobig at sqlite3.c:72457:17 in sqlite3.h
// sqlite3_result_error_nomem at sqlite3.c:72465:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static void fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  (void)argv;

  sqlite3 *db = sqlite3_context_db_handle(ctx);
  if (db) {
    int err = sqlite3_errcode(db);
    sqlite3_result_error_code(ctx, err);
  }

  sqlite3_result_null(ctx);
  sqlite3_result_error_toobig(ctx);
  sqlite3_result_error_nomem(ctx);
  sqlite3_result_error_code(ctx, 0);
  sqlite3_result_error_code(ctx, SQLITE_TOOBIG);
  sqlite3_result_error_code(ctx, SQLITE_NOMEM);
  sqlite3_result_null(ctx);
}

static void fuzz_agg_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int n = 0;
  if (argc > 0 && argv && argv[0]) {
    n = sqlite3_value_int(argv[0]) & 0x3ff;
  }

  void *p1 = sqlite3_aggregate_context(ctx, n);
  if (p1 && n > 0) {
    memset(p1, 0xA5, (size_t)n);
  }

  (void)sqlite3_aggregate_context(ctx, 0);
  (void)sqlite3_aggregate_context(ctx, n ? n / 2 : 1);

  sqlite3 *db = sqlite3_context_db_handle(ctx);
  if (db && (argc > 1) && argv[1]) {
    int code = sqlite3_value_int(argv[1]);
    sqlite3_result_error_code(ctx, code);
  } else {
    sqlite3_result_null(ctx);
  }
}

static void fuzz_agg_final(sqlite3_context *ctx) {
  void *p0 = sqlite3_aggregate_context(ctx, 0);
  if (p0) {
    ((unsigned char *)p0)[0] ^= 0;
  }

  sqlite3 *db = sqlite3_context_db_handle(ctx);
  if (db) {
    int code = sqlite3_extended_errcode(db);
    sqlite3_result_error_code(ctx, code);
  }

  sqlite3_result_null(ctx);
  sqlite3_result_error_toobig(ctx);
  sqlite3_result_error_nomem(ctx);
}

int LLVMFuzzerTestOneInput_47(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzz_scalar", -1, SQLITE_UTF8, NULL,
                          fuzz_scalar, NULL, NULL);
  sqlite3_create_function(db, "fuzz_agg", -1, SQLITE_UTF8, NULL,
                          NULL, fuzz_agg_step, fuzz_agg_final);

  sqlite3_exec(db,
               "CREATE TABLE IF NOT EXISTS t(x, y);"
               "DELETE FROM t;",
               NULL, NULL, NULL);

  {
    sqlite3_stmt *ins = NULL;
    if (sqlite3_prepare_v2(db, "INSERT INTO t(x,y) VALUES(?,?);", -1, &ins, NULL) == SQLITE_OK) {
      size_t off = 0;
      int rows = 0;
      while (off < Size && rows < 8) {
        int v1 = Data[off++];
        int v2 = (off < Size) ? Data[off++] : 0;
        sqlite3_bind_int(ins, 1, v1);
        sqlite3_bind_int(ins, 2, v2);
        sqlite3_step(ins);
        sqlite3_reset(ins);
        sqlite3_clear_bindings(ins);
        rows++;
      }
    }
    if (ins) sqlite3_finalize(ins);
  }

  if (Size > 0) {
    switch (Data[0] % 4) {
      case 0:
        sqlite3_prepare_v2(db, "SELECT fuzz_scalar(x,y) FROM t;", -1, &stmt, NULL);
        break;
      case 1:
        sqlite3_prepare_v2(db, "SELECT fuzz_agg(x,y) FROM t;", -1, &stmt, NULL);
        break;
      case 2:
        sqlite3_prepare_v2(db, "SELECT fuzz_scalar(), fuzz_agg();", -1, &stmt, NULL);
        break;
      default: {
        char *sql = (char *)malloc(Size + 1);
        if (sql) {
          memcpy(sql, Data, Size);
          sql[Size] = '\0';
          sqlite3_exec(db, sql, NULL, NULL, &errmsg);
          free(sql);
        }
        if (errmsg) {
          sqlite3_free(errmsg);
          errmsg = NULL;
        }
        sqlite3_prepare_v2(db,
                           "SELECT fuzz_scalar(x), fuzz_agg(y) FROM t WHERE x>=0;",
                           -1, &stmt, NULL);
        break;
      }
    }
  } else {
    sqlite3_prepare_v2(db, "SELECT fuzz_scalar(), fuzz_agg();", -1, &stmt, NULL);
  }

  if (stmt) {
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  sqlite3_exec(db, "SELECT fuzz_scalar(NULL), fuzz_agg(NULL);", NULL, NULL, NULL);

  sqlite3_close(db);
  return 0;
}