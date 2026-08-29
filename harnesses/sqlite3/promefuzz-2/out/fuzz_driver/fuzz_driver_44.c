// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_enable_load_extension at sqlite3.c:114763:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_enable_load_extension at sqlite3.c:114763:16 in sqlite3.h
// sqlite3_enable_load_extension at sqlite3.c:114763:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
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

static int fuzz_authorizer(void *pUserData, int action,
                           const char *p1, const char *p2,
                           const char *dbName, const char *triggerName) {
  const uint8_t *bytes = (const uint8_t *)pUserData;
  unsigned v = 0;

  if (bytes) {
    v ^= bytes[0];
    v ^= (unsigned)action;
    if (p1) v ^= (unsigned)(unsigned char)p1[0];
    if (p2) v ^= (unsigned)(unsigned char)p2[0];
    if (dbName) v ^= (unsigned)(unsigned char)dbName[0];
    if (triggerName) v ^= (unsigned)(unsigned char)triggerName[0];
  }

  switch (v % 4) {
    case 0: return SQLITE_OK;
    case 1: return SQLITE_IGNORE;
    case 2: return SQLITE_DENY;
    default: return 999;
  }
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  int rc;
  int onoff = 0;
  const char *tail = NULL;
  size_t sql_len;
  char *sql;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close_v2(db);
    return 0;
  }

  sqlite3_get_autocommit(db);
  sqlite3_errcode(db);
  sqlite3_extended_errcode(db);

  if (Size > 0) {
    onoff = Data[0] & 1;
  }
  sqlite3_enable_load_extension(db, onoff);
  sqlite3_enable_load_extension(db, !onoff);

  if (Size > 1) {
    if (Data[1] & 1) {
      sqlite3_set_authorizer(db, fuzz_authorizer, (void *)Data);
    } else {
      sqlite3_set_authorizer(db, NULL, NULL);
    }
  } else {
    sqlite3_set_authorizer(db, fuzz_authorizer, (void *)Data);
  }

  exec_sql(db, "PRAGMA foreign_keys=ON;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(a, b);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS u(x);");

  if (Size > 2) {
    switch (Data[2] % 4) {
      case 0:
        exec_sql(db, "BEGIN;");
        break;
      case 1:
        exec_sql(db, "BEGIN IMMEDIATE;");
        break;
      case 2:
        exec_sql(db, "SAVEPOINT s1;");
        break;
      default:
        break;
    }
  }

  sqlite3_get_autocommit(db);
  sqlite3_errcode(db);
  sqlite3_extended_errcode(db);

  sql_len = Size;
  sql = (char *)sqlite3_malloc64(sql_len + 1);
  if (sql) {
    if (sql_len) memcpy(sql, Data, sql_len);
    sql[sql_len] = '\0';

    rc = sqlite3_prepare_v2(db, sql, (int)sql_len, &stmt, &tail);
    sqlite3_errcode(db);
    sqlite3_extended_errcode(db);
    sqlite3_get_autocommit(db);

    if (rc == SQLITE_OK && stmt) {
      int step_count = 0;
      do {
        rc = sqlite3_step(stmt);
        sqlite3_errcode(db);
        sqlite3_extended_errcode(db);
        sqlite3_get_autocommit(db);
        step_count++;
      } while ((rc == SQLITE_ROW || rc == SQLITE_BUSY) && step_count < 8);

      sqlite3_finalize(stmt);
      stmt = NULL;
    }

    sqlite3_free(sql);
  }

  if (Size > 3) {
    switch (Data[3] % 5) {
      case 0:
        exec_sql(db, "COMMIT;");
        break;
      case 1:
        exec_sql(db, "ROLLBACK;");
        break;
      case 2:
        exec_sql(db, "RELEASE s1;");
        break;
      case 3:
        exec_sql(db, "ROLLBACK TO s1;");
        break;
      default:
        break;
    }
  }

  sqlite3_get_autocommit(db);
  sqlite3_errcode(db);
  sqlite3_extended_errcode(db);

  sqlite3_set_authorizer(db, NULL, NULL);
  sqlite3_enable_load_extension(db, 0);

  if (stmt) {
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  sqlite3_close_v2(db);
  return 0;
}