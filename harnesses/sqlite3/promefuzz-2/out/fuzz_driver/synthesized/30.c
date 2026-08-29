// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_column_int at sqlite3.c:73041:16 in sqlite3.h
// sqlite3_column_int at sqlite3.c:73041:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_db_release_memory at sqlite3.c:153110:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
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

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
  }
}

int LLVMFuzzerTestOneInput_30(const uint8_t *Data, size_t Size) {
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

  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(a INTEGER, b INTEGER);");
  exec_sql(db, "DELETE FROM t;");

  if (Size >= 8) {
    char sqlbuf[256];
    int v1 = (int)(unsigned char)Data[0];
    int v2 = (int)(unsigned char)Data[1];
    int v3 = (int)(unsigned char)Data[2];
    int v4 = (int)(unsigned char)Data[3];
    int v5 = (int)(unsigned char)Data[4];
    int v6 = (int)(unsigned char)Data[5];
    snprintf(sqlbuf, sizeof(sqlbuf),
             "INSERT INTO t(a,b) VALUES(%d,%d),(%d,%d),(%d,%d);",
             v1, v2, v3, v4, v5, v6);
    exec_sql(db, sqlbuf);
  } else {
    exec_sql(db, "INSERT INTO t(a,b) VALUES(1,2),(3,4),(5,6);");
  }

  {
    const char *sql1 = "SELECT a,b FROM t;";
    sqlite3_prepare_v2(db, sql1, -1, &stmt1, &tail);
  }

  /* Required order:
     sqlite3_sql -> sqlite3_step -> sqlite3_sql -> sqlite3_column_int ->
     sqlite3_column_int -> sqlite3_finalize -> sqlite3_db_release_memory ->
     sqlite3_sql -> sqlite3_step -> sqlite3_sql -> sqlite3_finalize
  */

  (void)sqlite3_sql(stmt1);
  rc = sqlite3_step(stmt1);
  (void)rc;
  (void)sqlite3_sql(stmt1);
  (void)sqlite3_column_int(stmt1, 0);
  (void)sqlite3_column_int(stmt1, 1);
  sqlite3_finalize(stmt1);
  stmt1 = NULL;

  sqlite3_db_release_memory(db);

  {
    const char *sql2;
    if (Size > 0 && (Data[0] & 1)) {
      sql2 = "SELECT COUNT(*), SUM(a) FROM t;";
    } else {
      sql2 = "SELECT b,a FROM t ORDER BY a;";
    }
    sqlite3_prepare_v2(db, sql2, -1, &stmt2, &tail);
  }

  (void)sqlite3_sql(stmt2);
  rc = sqlite3_step(stmt2);
  (void)rc;
  (void)sqlite3_sql(stmt2);
  sqlite3_finalize(stmt2);
  stmt2 = NULL;

  sqlite3_close(db);
  return 0;
}