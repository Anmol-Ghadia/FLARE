// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_db_release_memory at sqlite3.c:153110:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_sql at sqlite3.c:73648:24 in sqlite3.h
// sqlite3_column_int at sqlite3.c:73041:16 in sqlite3.h
// sqlite3_column_int at sqlite3.c:73041:16 in sqlite3.h
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

static int prepare_stmt(sqlite3 *db, const char *sql, sqlite3_stmt **stmt) {
  if (!db || !sql || !stmt) return SQLITE_MISUSE;
  *stmt = NULL;
  return sqlite3_prepare_v2(db, sql, -1, stmt, NULL);
}

int LLVMFuzzerTestOneInput_30(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt1 = NULL;
  sqlite3_stmt *stmt2 = NULL;
  int rc;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Data && Size) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db,
               "CREATE TABLE IF NOT EXISTS t(a INTEGER, b INTEGER);"
               "DELETE FROM t;"
               "INSERT INTO t(a,b) VALUES(1,2),(3,4),(5,6);",
               NULL, NULL, NULL);

  {
    const char *sql1 = "SELECT a,b FROM t;";
    const char *sql2 = "SELECT length(sql) FROM sqlite_schema;";

    if (Size > 0) {
      switch (Data[0] % 4) {
        case 0:
          sql1 = "SELECT a,b FROM t;";
          break;
        case 1:
          sql1 = "SELECT b,a FROM t WHERE a>=0;";
          break;
        case 2:
          sql1 = "SELECT 123,456;";
          break;
        default:
          sql1 = "SELECT a FROM t UNION ALL SELECT b FROM t;";
          break;
      }
    }

    if (Size > 1) {
      switch (Data[1] % 4) {
        case 0:
          sql2 = "SELECT length(sql) FROM sqlite_schema;";
          break;
        case 1:
          sql2 = "SELECT count(*) FROM sqlite_schema;";
          break;
        case 2:
          sql2 = "SELECT 7;";
          break;
        default:
          sql2 = "SELECT name FROM sqlite_schema;";
          break;
      }
    }

    if (prepare_stmt(db, sql1, &stmt1) == SQLITE_OK) {
      const char *s1a = sqlite3_sql(stmt1);
      (void)s1a;

      rc = sqlite3_step(stmt1);

      const char *s1b = sqlite3_sql(stmt1);
      (void)s1b;

      if (rc == SQLITE_ROW) {
        int col0 = sqlite3_column_int(stmt1, 0);
        int col1 = sqlite3_column_int(stmt1, 1);
        (void)col0;
        (void)col1;
      }

      sqlite3_finalize(stmt1);
      stmt1 = NULL;
    } else {
      sqlite3_finalize(stmt1);
      stmt1 = NULL;
    }

    sqlite3_db_release_memory(db);

    if (prepare_stmt(db, sql2, &stmt2) == SQLITE_OK) {
      const char *s2a = sqlite3_sql(stmt2);
      (void)s2a;

      rc = sqlite3_step(stmt2);

      const char *s2b = sqlite3_sql(stmt2);
      (void)s2b;

      sqlite3_finalize(stmt2);
      stmt2 = NULL;
    } else {
      sqlite3_finalize(stmt2);
      stmt2 = NULL;
    }
  }

  sqlite3_close(db);
  return 0;
}