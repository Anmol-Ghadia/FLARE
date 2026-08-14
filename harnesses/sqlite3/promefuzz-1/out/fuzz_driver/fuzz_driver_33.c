// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_blob_open at sqlite3.c:82675:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
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

static int fuzz_callback(void *ctx, int argc, char **argv, char **colv) {
  const uint8_t *data = (const uint8_t *)ctx;
  int ret = 0;
  if (data && argc >= 0) {
    ret = data[0] & 1;
  }
  (void)argv;
  (void)colv;
  return ret;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  sqlite3_blob *blob = NULL;
  char *errmsg = NULL;
  char *m1 = NULL;
  char *m2 = NULL;
  char *sql = NULL;
  int rc;
  sqlite3_int64 rowid = 1;
  int flags = 0;

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

  if (Size > 0) {
    sql = (char *)malloc(Size + 1);
    if (sql) {
      memcpy(sql, Data, Size);
      sql[Size] = '\0';
    }
  } else {
    sql = (char *)malloc(1);
    if (sql) sql[0] = '\0';
  }

  sqlite3_prepare_v2(db, "SELECT 1;", -1, &stmt, NULL);

  /* Required order: sqlite3_finalize -> sqlite3_exec -> sqlite3_mprintf
     -> sqlite3_mprintf -> sqlite3_free -> sqlite3_free -> sqlite3_blob_open */
  sqlite3_finalize(stmt);
  stmt = NULL;

  if (sql) {
    sqlite3_exec(db, sql, fuzz_callback, (void *)Data, &errmsg);
  } else {
    sqlite3_exec(db, "", fuzz_callback, (void *)Data, &errmsg);
  }

  m1 = sqlite3_mprintf("%q", sql ? sql : "");
  m2 = sqlite3_mprintf("%.*s", (int)(Size > 1024 ? 1024 : Size), (const char *)Data);

  sqlite3_free(m1);
  m1 = NULL;

  sqlite3_free(m2);
  m2 = NULL;

  sqlite3_exec(db,
               "CREATE TABLE IF NOT EXISTS t(b BLOB);"
               "DELETE FROM t;"
               "INSERT INTO t(b) VALUES(zeroblob(16));",
               NULL, NULL, NULL);

  if (Size >= 8) {
    rowid = (sqlite3_int64)(
        ((sqlite3_uint64)Data[0] << 56) |
        ((sqlite3_uint64)Data[1] << 48) |
        ((sqlite3_uint64)Data[2] << 40) |
        ((sqlite3_uint64)Data[3] << 32) |
        ((sqlite3_uint64)Data[4] << 24) |
        ((sqlite3_uint64)Data[5] << 16) |
        ((sqlite3_uint64)Data[6] << 8)  |
        ((sqlite3_uint64)Data[7]));
    if (rowid == 0) rowid = 1;
  }

  if (Size > 8) {
    flags = Data[8] & 1;
  }

  sqlite3_blob_open(db, "main", "t", "b", rowid, flags, &blob);

  if (blob) {
    sqlite3_blob_close(blob);
    blob = NULL;
  }

  sqlite3_free(errmsg);
  free(sql);
  sqlite3_close(db);
  return 0;
}