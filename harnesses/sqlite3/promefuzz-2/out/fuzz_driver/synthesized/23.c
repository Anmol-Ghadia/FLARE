// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_libversion at sqlite3.c:152357:24 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
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

static int fuzz_exec_callback(void *ctx, int argc, char **argv, char **colv) {
  (void)ctx;
  (void)argc;
  (void)argv;
  (void)colv;
  return 0;
}

int LLVMFuzzerTestOneInput_23(const uint8_t *Data, size_t Size) {
  sqlite3_blob *blob = NULL;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  const char *tail = NULL;
  const char *version;
  int rc;

  /* 1. sqlite3_blob_close */
  sqlite3_blob_close(blob);

  /* Prepare fuzz-derived SQL buffers */
  char *sql1 = (char *)malloc(Size + 1);
  char *sql2 = (char *)malloc(Size + 1);
  if (!sql1 || !sql2) {
    free(sql1);
    free(sql2);
    return 0;
  }
  memcpy(sql1, Data, Size);
  memcpy(sql2, Data, Size);
  sql1[Size] = '\0';
  sql2[Size] = '\0';

  /* Optional file creation as requested when file use is needed */
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  /* Open a database first so sqlite3_exec/sqlite3_close are used validly */
  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK && db == NULL) {
    free(sql1);
    free(sql2);
    return 0;
  }

  /* 2. sqlite3_exec */
  sqlite3_exec(db, sql1, fuzz_exec_callback, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  /* 3. sqlite3_close */
  rc = sqlite3_close(db);

  /* 4. sqlite3_close */
  if (rc == SQLITE_BUSY) {
    sqlite3_finalize(stmt);
    stmt = NULL;
    sqlite3_close(db);
  }

  /* 5. sqlite3_libversion */
  version = sqlite3_libversion();
  (void)version;

  /* 6. sqlite3_open */
  db = NULL;
  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK && db == NULL) {
    free(sql1);
    free(sql2);
    return 0;
  }

  /* Seed some state for later prepare/step */
  sqlite3_exec(db,
               "CREATE TABLE IF NOT EXISTS t(x TEXT);"
               "INSERT INTO t(x) VALUES('a');"
               "INSERT INTO t(x) VALUES('b');",
               NULL, NULL, NULL);

  /* 7. sqlite3_prepare_v2 */
  stmt = NULL;
  rc = sqlite3_prepare_v2(db, sql2, (int)Size, &stmt, &tail);
  if (rc != SQLITE_OK || stmt == NULL) {
    sqlite3_prepare_v2(db, "SELECT x FROM t;", -1, &stmt, &tail);
  }

  /* 8. sqlite3_step */
  if (stmt) {
    rc = sqlite3_step(stmt);

    /* 9. sqlite3_column_text */
    if (rc == SQLITE_ROW) {
      const unsigned char *txt = sqlite3_column_text(stmt, 0);
      (void)txt;
    }
  }

  /* 10. sqlite3_finalize */
  sqlite3_finalize(stmt);
  stmt = NULL;

  /* 11. sqlite3_close */
  sqlite3_close(db);

  free(sql1);
  free(sql2);
  return 0;
}