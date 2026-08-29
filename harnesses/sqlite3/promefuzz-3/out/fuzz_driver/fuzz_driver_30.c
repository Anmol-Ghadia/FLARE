// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_db_release_memory at sqlite3.c:153110:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_column_int at sqlite3.c:73041:16 in sqlite3.h
// sqlite3_column_int at sqlite3.c:73041:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_db_release_memory at sqlite3.c:153110:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt1 = NULL;
  sqlite3_stmt *stmt2 = NULL;
  const char *tail = NULL;
  int rc;

  write_dummy_file(Data, Size);

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(a INTEGER, b INTEGER);", NULL, NULL, NULL);
  sqlite3_exec(db, "INSERT INTO t(a,b) VALUES(1,2);", NULL, NULL, NULL);

  rc = sqlite3_prepare_v2(db, (const char *)Data, (int)Size, &stmt1, &tail);
  if (rc != SQLITE_OK || stmt1 == NULL) {
    sqlite3_db_release_memory(db);
    sqlite3_close(db);
    return 0;
  }

  (void)sqlite3_sql(stmt1);
  rc = sqlite3_step(stmt1);
  (void)rc;
  (void)sqlite3_sql(stmt1);
  (void)sqlite3_column_int(stmt1, 0);
  (void)sqlite3_column_int(stmt1, 1);
  (void)sqlite3_finalize(stmt1);
  stmt1 = NULL;

  (void)sqlite3_db_release_memory(db);

  if (tail && *tail) {
    rc = sqlite3_prepare_v2(db, tail, -1, &stmt2, NULL);
  } else {
    static const char fallback_sql[] = "SELECT 1, 2;";
    rc = sqlite3_prepare_v2(db, fallback_sql, -1, &stmt2, NULL);
  }

  if (rc == SQLITE_OK && stmt2 != NULL) {
    (void)sqlite3_sql(stmt2);
    rc = sqlite3_step(stmt2);
    (void)rc;
    (void)sqlite3_sql(stmt2);
    (void)sqlite3_finalize(stmt2);
    stmt2 = NULL;
  } else {
    (void)sqlite3_sql(NULL);
    (void)sqlite3_finalize(NULL);
  }

  sqlite3_close(db);
  return 0;
}