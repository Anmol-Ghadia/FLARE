// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_value at sqlite3.c:73056:27 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_value_frombind at sqlite3.c:72232:16 in sqlite3.h
// sqlite3_value_numeric_type at sqlite3.c:74581:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_numeric_type at sqlite3.c:74581:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_frombind at sqlite3.c:72232:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_value at sqlite3.c:73056:27 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
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

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
  }
}

static void touch_value_apis(sqlite3_value *v) {
  if (!v) return;

  (void)sqlite3_value_type(v);
  (void)sqlite3_value_int(v);
  (void)sqlite3_value_nochange(v);
  (void)sqlite3_value_frombind(v);
  (void)sqlite3_value_numeric_type(v);

  {
    const void *p = sqlite3_value_blob(v);
    (void)p;
  }

  /* Revisit in different orders to exercise stateful conversions. */
  (void)sqlite3_value_numeric_type(v);
  (void)sqlite3_value_type(v);
  {
    const void *p = sqlite3_value_blob(v);
    (void)p;
  }
  (void)sqlite3_value_int(v);
  (void)sqlite3_value_frombind(v);
  (void)sqlite3_value_nochange(v);
  (void)sqlite3_value_type(v);
}

static void run_stmt_and_fuzz_values(sqlite3 *db, const uint8_t *Data, size_t Size) {
  sqlite3_stmt *stmt = NULL;
  const char *sql =
      "SELECT "
      "?1, "                  /* bound blob/text/integer */
      "?2, "                  /* bound integer */
      "?3, "                  /* bound double */
      "x'414243', "           /* literal blob */
      "'12345', "             /* numeric-looking text */
      "'not_a_number', "      /* non-numeric text */
      "NULL, "                /* null */
      "zeroblob(?4), "        /* blob-like value */
      "CAST(?5 AS TEXT), "    /* bound text */
      "length(?1)";           /* expression from bound parameter */

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK || !stmt) {
    if (stmt) sqlite3_finalize(stmt);
    return;
  }

  sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);

  {
    sqlite3_int64 i64v = 0;
    size_t n = Size < sizeof(i64v) ? Size : sizeof(i64v);
    memcpy(&i64v, Data, n);
    sqlite3_bind_int64(stmt, 2, i64v);
  }

  {
    double dv = 0.0;
    size_t n = Size < sizeof(dv) ? Size : sizeof(dv);
    memcpy(&dv, Data, n);
    sqlite3_bind_double(stmt, 3, dv);
  }

  sqlite3_bind_int(stmt, 4, (int)(Size % 64));

  if (Size > 0) {
    sqlite3_bind_text(stmt, 5, (const char *)Data, (int)Size, SQLITE_STATIC);
  } else {
    sqlite3_bind_text(stmt, 5, "", 0, SQLITE_STATIC);
  }

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    int cols = sqlite3_column_count(stmt);
    int i;
    for (i = 0; i < cols; i++) {
      sqlite3_value *v = sqlite3_column_value(stmt, i);
      sqlite3_value *dup = NULL;

      if (v) {
        dup = sqlite3_value_dup(v);
        if (dup) {
          touch_value_apis(dup);
          sqlite3_value_free(dup);
        }
      }
    }
  }

  sqlite3_finalize(stmt);
}

static void run_update_returning(sqlite3 *db, const uint8_t *Data, size_t Size) {
  sqlite3_stmt *stmt = NULL;
  const char *sql =
      "UPDATE t SET a=?1, b=?2 WHERE rowid=1 "
      "RETURNING a, b, c, ?3, '456', x'00ff', NULL";

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK || !stmt) {
    if (stmt) sqlite3_finalize(stmt);
    return;
  }

  sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, (int)(Size * 3U));
  if (Size > 0) {
    sqlite3_bind_text(stmt, 3, (const char *)Data, (int)Size, SQLITE_STATIC);
  } else {
    sqlite3_bind_null(stmt, 3);
  }

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    int cols = sqlite3_column_count(stmt);
    int i;
    for (i = 0; i < cols; i++) {
      sqlite3_value *v = sqlite3_column_value(stmt, i);
      sqlite3_value *dup = NULL;

      if (v) {
        dup = sqlite3_value_dup(v);
        if (dup) {
          touch_value_apis(dup);
          sqlite3_value_free(dup);
        }
      }
    }
  }

  sqlite3_finalize(stmt);
}

int LLVMFuzzerTestOneInput_90(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Data && Size) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  if (sqlite3_open(":memory:", &db) != SQLITE_OK || !db) {
    if (db) sqlite3_close(db);
    return 0;
  }

  exec_sql(db, "CREATE TABLE t(a BLOB, b INTEGER, c TEXT);");
  exec_sql(db, "INSERT INTO t(a,b,c) VALUES(x'01', 42, 'hello');");

  run_stmt_and_fuzz_values(db, Data, Size);
  run_update_returning(db, Data, Size);

  sqlite3_close(db);
  return 0;
}