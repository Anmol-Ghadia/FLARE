// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_prepare at sqlite3.c:119055:16 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_prepare at sqlite3.c:119055:16 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_prepare16 at sqlite3.c:119171:16 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_prepare16_v2 at sqlite3.c:119183:16 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
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

  for (int i = 0; i < 8; ++i) {
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) break;
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
  FILE *fp = fopen("./dummy_file", "wb");

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

      sqlite3_prepare(db, sql8, Size ? (int)Size : 0, &stmt1, &tail8);
      if (stmt1) (void)sqlite3_db_handle(stmt1);

      sqlite3_prepare(db, sql8, -1, &stmt2, &tail8);
      if (stmt2) (void)sqlite3_db_handle(stmt2);

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

      sqlite3_free(sql8);
    }
  }

  {
    size_t even_bytes = Size & ~(size_t)1;
    unsigned char *sql16 = (unsigned char *)sqlite3_malloc64(even_bytes + 4);
    if (sql16) {
      if (even_bytes) memcpy(sql16, Data, even_bytes);
      sql16[even_bytes] = 0;
      sql16[even_bytes + 1] = 0;
      sql16[even_bytes + 2] = 0;
      sql16[even_bytes + 3] = 0;

      sqlite3_prepare16(db, sql16, (int)even_bytes, &stmt1, &tail16);
      if (stmt1) (void)sqlite3_db_handle(stmt1);

      sqlite3_prepare16_v2(db, sql16, (int)(even_bytes + 2), &stmt2, &tail16);
      if (stmt2) (void)sqlite3_db_handle(stmt2);

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