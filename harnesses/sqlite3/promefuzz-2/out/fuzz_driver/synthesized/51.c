// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_subtype at sqlite3.c:72106:25 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_subtype at sqlite3.c:72106:25 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_subtype at sqlite3.c:72106:25 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_subtype at sqlite3.c:72106:25 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
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

static void fuzz_sql_function(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int i;
  if (!ctx) return;

  for (i = 0; i < argc; ++i) {
    sqlite3_value *v = argv ? argv[i] : NULL;
    (void)sqlite3_value_int(v);
    (void)sqlite3_value_bytes(v);
    (void)sqlite3_value_subtype(v);

    sqlite3_value *dup1 = sqlite3_value_dup(v);
    if (dup1) {
      (void)sqlite3_value_int(dup1);
      (void)sqlite3_value_bytes(dup1);
      (void)sqlite3_value_subtype(dup1);
      sqlite3_result_value(ctx, dup1);

      sqlite3_value *dup2 = sqlite3_value_dup(dup1);
      if (dup2) {
        (void)sqlite3_value_int(dup2);
        (void)sqlite3_value_bytes(dup2);
        (void)sqlite3_value_subtype(dup2);
        sqlite3_result_value(ctx, dup2);
      }
      sqlite3_value_free(dup2);
    }
    sqlite3_value_free(dup1);

    sqlite3_result_value(ctx, v);
  }

  if (argc == 0) {
    sqlite3_result_value(ctx, NULL);
    (void)sqlite3_value_dup(NULL);
    sqlite3_value_free(NULL);
    (void)sqlite3_value_int(NULL);
    (void)sqlite3_value_bytes(NULL);
    (void)sqlite3_value_subtype(NULL);
  }
}

int LLVMFuzzerTestOneInput_51(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;
  int i;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzzfunc", -1, SQLITE_UTF8, NULL, fuzz_sql_function, NULL, NULL);

  sqlite3_exec(db, "CREATE TABLE t(a,b,c,d,e);", NULL, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  {
    const char *sql =
        "WITH RECURSIVE c(x) AS (SELECT 1 UNION ALL SELECT x+1 FROM c WHERE x<3) "
        "SELECT fuzzfunc("
        "?,"
        "?1,"
        "CAST(?2 AS INTEGER),"
        "CAST(?3 AS TEXT),"
        "CAST(?4 AS BLOB),"
        "zeroblob(abs(CAST(?5 AS INTEGER)) % 32),"
        "NULL,"
        "x"
        ") FROM c;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  }

  if (rc == SQLITE_OK && stmt) {
    size_t quarter = Size / 4;
    size_t half = Size / 2;
    sqlite3_int64 i64v = 0;
    double dv = 0.0;

    for (i = 0; i < (int)(Size < sizeof(i64v) ? Size : sizeof(i64v)); ++i) {
      i64v = (i64v << 8) | Data[i];
    }
    if (Size >= sizeof(uint32_t)) {
      uint32_t tmp = 0;
      memcpy(&tmp, Data, sizeof(tmp));
      dv = (double)tmp;
    }

    sqlite3_bind_null(stmt, 1);
    sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, i64v);
    sqlite3_bind_text(stmt, 3, (const char *)Data, (int)quarter, SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 4, Data + (quarter < Size ? quarter : 0),
                      (int)((half > quarter) ? (half - quarter) : 0), SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 5, dv);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    }

    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);

    sqlite3_bind_text(stmt, 1, (const char *)Data, (int)Size, SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 2, Data, (int)(Size > 16 ? 16 : Size), SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3,
                      (const char *)(Data + (half < Size ? half : 0)),
                      (int)(Size - (half < Size ? half : Size)),
                      SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, (int)Size);
    sqlite3_bind_int(stmt, 5, (int)(Size ? Data[Size - 1] : 0));

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    }
  }

  if (stmt) {
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  sqlite3_close(db);
  return 0;
}