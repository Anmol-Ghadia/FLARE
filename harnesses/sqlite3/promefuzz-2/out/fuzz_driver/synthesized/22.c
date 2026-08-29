// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_int at sqlite3.c:73041:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_int at sqlite3.c:73041:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

static int consume_u32(const uint8_t **data, size_t *size) {
  int v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; ++i) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static char *make_cstring(const uint8_t *data, size_t size) {
  char *s = (char *)malloc(size + 1);
  if (!s) return NULL;
  memcpy(s, data, size);
  s[size] = '\0';
  return s;
}

int LLVMFuzzerTestOneInput_22(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  const char *tail = NULL;
  char *fmt1 = NULL;
  char *fmt2 = NULL;
  char *sql_input = NULL;
  FILE *fp = NULL;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
    fp = NULL;
  }

  if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db,
               "CREATE TABLE t(a INTEGER, b BLOB);"
               "INSERT INTO t VALUES(1, x'00');"
               "INSERT INTO t VALUES(123, x'414243');"
               "INSERT INTO t VALUES(-7, NULL);",
               NULL, NULL, NULL);

  sql_input = make_cstring(Data, Size);
  if (!sql_input) {
    sqlite3_close(db);
    return 0;
  }

  fmt1 = sqlite3_mprintf("%s", sql_input);
  fmt2 = sqlite3_mprintf("%s", fmt1 ? fmt1 : sql_input);

  sqlite3_prepare_v2(db, fmt2 ? fmt2 : sql_input, -1, &stmt, &tail);

  sqlite3_free(fmt1);

  (void)sqlite3_errmsg(db);

  if (stmt) {
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
      int col = consume_u32(&Data, &Size);
      if (col < 0) col = -col;
      col %= 2;

      (void)sqlite3_column_bytes(stmt, col);
      (void)sqlite3_column_int(stmt, col);
      (void)sqlite3_column_blob(stmt, col);
    } else {
      if (sqlite3_prepare_v2(db, "SELECT a,b FROM t;", -1, &stmt, &tail) == SQLITE_OK && stmt) {
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
          int col = consume_u32(&Data, &Size);
          if (col < 0) col = -col;
          col %= 2;

          (void)sqlite3_column_bytes(stmt, col);
          (void)sqlite3_column_int(stmt, col);
          (void)sqlite3_column_blob(stmt, col);
        }
      }
    }
  }

  sqlite3_finalize(stmt);
  sqlite3_free(fmt2);
  free(sql_input);
  sqlite3_close(db);
  return 0;
}