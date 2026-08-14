// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_libversion at sqlite3.c:152357:24 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
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

static int fuzz_callback(void *unused, int argc, char **argv, char **colnames) {
  (void)unused;
  (void)argc;
  (void)argv;
  (void)colnames;
  return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3_blob *blob = NULL;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  char *sql = NULL;
  const char *tail = NULL;
  const unsigned char *txt = NULL;
  const char *ver = NULL;
  int rc;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  sql = (char *)malloc(Size + 1);
  if (!sql) {
    sqlite3_blob_close(blob);
    return 0;
  }
  if (Size > 0) {
    memcpy(sql, Data, Size);
  }
  sql[Size] = '\0';

  rc = sqlite3_blob_close(blob);
  (void)rc;

  rc = sqlite3_open(":memory:", &db);
  if (db) {
    rc = sqlite3_exec(db, sql, fuzz_callback, NULL, &errmsg);
    (void)rc;
    if (errmsg) {
      sqlite3_free(errmsg);
      errmsg = NULL;
    }
  }

  rc = sqlite3_close(NULL);
  (void)rc;

  rc = sqlite3_close(NULL);
  (void)rc;

  ver = sqlite3_libversion();
  (void)ver;

  if (db) {
    rc = sqlite3_close(db);
    if (rc != SQLITE_OK) {
      db = NULL;
    } else {
      db = NULL;
    }
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (db) {
    rc = sqlite3_prepare_v2(db, sql, (int)Size, &stmt, &tail);
    (void)tail;
    if (rc == SQLITE_OK && stmt) {
      rc = sqlite3_step(stmt);
      if (rc == SQLITE_ROW) {
        txt = sqlite3_column_text(stmt, 0);
        (void)txt;
      }
      rc = sqlite3_finalize(stmt);
      stmt = NULL;
      (void)rc;
    } else {
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
    rc = sqlite3_close(db);
    (void)rc;
    db = NULL;
  }

  free(sql);
  return 0;
}