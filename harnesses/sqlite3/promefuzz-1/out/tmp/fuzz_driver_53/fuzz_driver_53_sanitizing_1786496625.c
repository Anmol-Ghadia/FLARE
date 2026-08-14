#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

static void exec_sql(sqlite3 *db, const char *sql) {
  if (!db || !sql) return;
  sqlite3_exec(db, sql, NULL, NULL, NULL);
}

static void step_and_cleanup(sqlite3_stmt *stmt, int do_interrupt) {
  if (!stmt) return;
  sqlite3 *db = sqlite3_db_handle(stmt);
  if (do_interrupt && db) {
    sqlite3_interrupt(db);
  }

  int limit = 0;
  while (limit++ < 8) {
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) continue;
    break;
  }
  sqlite3_reset(stmt);
  sqlite3_finalize(stmt);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt1 = NULL;
  sqlite3_stmt *stmt2 = NULL;
  sqlite3_stmt *iter = NULL;
  const char *tail8 = NULL;
  const void *tail16 = NULL;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  if (sqlite3_open("./dummy_file", &db) != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  exec_sql(db, "PRAGMA writable_schema=ON;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(a,b,c);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS u(x);");
  exec_sql(db, "INSERT INTO t VALUES(1,2,3);");
  exec_sql(db, "INSERT INTO u VALUES('seed');");

  {
    char *sql8 = (char *)sqlite3_malloc64(Size + 1);
    if (sql8) {
      if (Size) memcpy(sql8, Data, Size);
      sql8[Size] = '\0';

      {
        int n1 = (Size > 0) ? (int)Size : 0;
        sqlite3_prepare(db, sql8, n1, &stmt1, &tail8);
        if (stmt1) {
          (void)sqlite3_db_handle(stmt1);
        }

        sqlite3_prepare(db, sql8, -1, &stmt2, &tail8);
        if (stmt2) {
          (void)sqlite3_db_handle(stmt2);
        }

        iter = sqlite3_next_stmt(db, NULL);
        while (iter) {
          (void)sqlite3_db_handle(iter);
          iter = sqlite3_next_stmt(db, iter);
        }

        if ((Size & 1) == 0) sqlite3_interrupt(db);
        step_and_cleanup(stmt1, (Size & 2) != 0);
        stmt1 = NULL;
        step_and_cleanup(stmt2, (Size & 4) != 0);
        stmt2 = NULL;
      }

      sqlite3_free(sql8);
    }
  }

  {
    size_t utf16_bytes = Size;
    void *sql16 = sqlite3_malloc64(utf16_bytes + 2);
    if (sql16) {
      if (Size) memcpy(sql16, Data, Size);
      ((unsigned char *)sql16)[utf16_bytes] = 0;
      ((unsigned char *)sql16)[utf16_bytes + 1] = 0;

      sqlite3_prepare16(db, sql16, (int)utf16_bytes, &stmt1, &tail16);
      if (stmt1) {
        (void)sqlite3_db_handle(stmt1);
      }

      sqlite3_prepare16_v2(db, sql16, -1, &stmt2, &tail16);
      if (stmt2) {
        (void)sqlite3_db_handle(stmt2);
      }

      iter = sqlite3_next_stmt(db, NULL);
      while (iter) {
        (void)sqlite3_db_handle(iter);
        iter = sqlite3_next_stmt(db, iter);
      }

      if ((Size & 8) != 0) sqlite3_interrupt(db);
      step_and_cleanup(stmt1, (Size & 16) != 0);
      stmt1 = NULL;
      step_and_cleanup(stmt2, (Size & 32) != 0);
      stmt2 = NULL;

      sqlite3_free(sql16);
    }
  }

  iter = sqlite3_next_stmt(db, NULL);
  while (iter) {
    sqlite3_stmt *next = sqlite3_next_stmt(db, iter);
    sqlite3_finalize(iter);
    iter = next;
  }

  sqlite3_close(db);
  return 0;
}