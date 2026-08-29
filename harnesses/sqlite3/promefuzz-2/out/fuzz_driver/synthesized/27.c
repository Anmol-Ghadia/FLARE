// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_int64 at sqlite3.c:73046:25 in sqlite3.h
// sqlite3_column_int64 at sqlite3.c:73046:25 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

static void write_dummy_file(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size > 0) {
    fwrite(Data, 1, Size, fp);
  }
  fclose(fp);
}

static int prepare_stmt(sqlite3 *db, sqlite3_stmt **stmt) {
  static const char *kSql = "SELECT CAST(?1 AS INTEGER)";
  return sqlite3_prepare_v2(db, kSql, -1, stmt, NULL);
}

int LLVMFuzzerTestOneInput_27(const uint8_t *Data, size_t Size) {
  write_dummy_file(Data, Size);

  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *text = NULL;
  int rc;

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  rc = prepare_stmt(db, &stmt);
  if (rc != SQLITE_OK || stmt == NULL) {
    (void)sqlite3_errmsg(db);
    sqlite3_close(db);
    return 0;
  }

  text = (char *)sqlite3_malloc64(Size + 1);
  if (!text) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
  }
  if (Size > 0) {
    memcpy(text, Data, Size);
  }
  text[Size] = '\0';

  /* Required call sequence:
     sqlite3_bind_text -> sqlite3_step -> sqlite3_reset ->
     sqlite3_bind_text -> sqlite3_step -> sqlite3_errmsg -> sqlite3_reset ->
     sqlite3_bind_text -> sqlite3_step -> sqlite3_errmsg -> sqlite3_reset ->
     sqlite3_errmsg -> sqlite3_bind_text -> sqlite3_step -> sqlite3_column_int64
  */

  (void)sqlite3_bind_text(stmt, 1, text, -1, SQLITE_TRANSIENT);
  (void)sqlite3_step(stmt);
  (void)sqlite3_reset(stmt);

  (void)sqlite3_bind_text(stmt, 1, text, -1, SQLITE_TRANSIENT);
  (void)sqlite3_step(stmt);
  (void)sqlite3_errmsg(db);
  (void)sqlite3_reset(stmt);

  (void)sqlite3_bind_text(stmt, 1, text, -1, SQLITE_TRANSIENT);
  (void)sqlite3_step(stmt);
  (void)sqlite3_errmsg(db);
  (void)sqlite3_reset(stmt);

  (void)sqlite3_errmsg(db);
  (void)sqlite3_bind_text(stmt, 1, text, -1, SQLITE_TRANSIENT);
  rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW) {
    (void)sqlite3_column_int64(stmt, 0);
  } else {
    (void)sqlite3_column_int64(stmt, 0);
  }

  sqlite3_free(text);
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}