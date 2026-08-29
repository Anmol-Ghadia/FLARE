// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_wal_autocheckpoint at sqlite3.c:154574:16 in sqlite3.h
// sqlite3_wal_autocheckpoint at sqlite3.c:154574:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_unlock_notify at sqlite3.c:157112:16 in sqlite3.h
// sqlite3_unlock_notify at sqlite3.c:157112:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_wal_autocheckpoint at sqlite3.c:154574:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_unlock_notify at sqlite3.c:157112:16 in sqlite3.h
// sqlite3_unlock_notify at sqlite3.c:157112:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_enable_shared_cache at sqlite3.c:53207:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
// sqlite3_close_v2 at sqlite3.c:153536:16 in sqlite3.h
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

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; ++i) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int read_int(const uint8_t **p, size_t *n) {
  return (int)read_u32(p, n);
}

static const char *pick_sql(const uint8_t b) {
  switch (b % 12) {
    case 0: return "PRAGMA journal_mode=WAL;";
    case 1: return "PRAGMA journal_mode=DELETE;";
    case 2: return "PRAGMA synchronous=OFF;";
    case 3: return "CREATE TABLE IF NOT EXISTS t(a INTEGER, b TEXT);";
    case 4: return "INSERT INTO t(a,b) VALUES(1,'x');";
    case 5: return "INSERT INTO t(a,b) VALUES(random(), hex(randomblob(8)));";
    case 6: return "UPDATE t SET b=hex(randomblob(16)) WHERE a IN (SELECT a FROM t LIMIT 1);";
    case 7: return "DELETE FROM t WHERE rowid IN (SELECT rowid FROM t LIMIT 1);";
    case 8: return "SELECT * FROM t;";
    case 9: return "BEGIN IMMEDIATE;";
    case 10: return "COMMIT;";
    default: return "ROLLBACK;";
  }
}

static void unlock_notify_cb(void **apArg, int nArg) {
  (void)apArg;
  (void)nArg;
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
}

static void step_stmt_once(sqlite3 *db, const char *sql) {
  sqlite3_stmt *stmt = NULL;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK && stmt) {
    (void)sqlite3_step(stmt);
  }
  if (stmt) sqlite3_finalize(stmt);
}

int LLVMFuzzerTestOneInput_101(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db1 = NULL;
  sqlite3 *db2 = NULL;
  sqlite3_stmt *held_stmt = NULL;
  const uint8_t *p = Data;
  size_t n = Size;

  sqlite3_enable_shared_cache(1);

  if (sqlite3_open_v2("./dummy_file",
                      &db1,
                      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
                      NULL) != SQLITE_OK) {
    if (db1) sqlite3_close_v2(db1);
    return 0;
  }

  if (sqlite3_open_v2("./dummy_file",
                      &db2,
                      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI | SQLITE_OPEN_SHAREDCACHE,
                      NULL) != SQLITE_OK) {
    sqlite3_close_v2(db1);
    if (db2) sqlite3_close_v2(db2);
    return 0;
  }

  exec_sql(db1, "PRAGMA journal_mode=WAL;");
  exec_sql(db2, "PRAGMA journal_mode=WAL;");
  exec_sql(db1, "CREATE TABLE IF NOT EXISTS t(a INTEGER, b TEXT);");
  exec_sql(db1, "INSERT INTO t(a,b) VALUES(0,'init');");

  sqlite3_busy_timeout(db1, read_int(&p, &n));
  sqlite3_busy_timeout(db2, read_int(&p, &n));

  sqlite3_wal_autocheckpoint(db1, read_int(&p, &n));
  sqlite3_wal_autocheckpoint(db2, read_int(&p, &n));

  (void)sqlite3_get_autocommit(db1);
  (void)sqlite3_get_autocommit(db2);

  if (n > 0 && (p[0] & 1)) {
    if (sqlite3_prepare_v2(db1, "SELECT * FROM t;", -1, &held_stmt, NULL) == SQLITE_OK && held_stmt) {
      (void)sqlite3_step(held_stmt);
    }
    p++;
    n--;
  }

  if (n > 0 && (p[0] & 1)) {
    exec_sql(db1, "BEGIN IMMEDIATE;");
    exec_sql(db1, "INSERT INTO t(a,b) VALUES(123,'txn');");
    (void)sqlite3_db_cacheflush(db1);
    (void)sqlite3_get_autocommit(db1);
    p++;
    n--;
  }

  if (n > 0 && (p[0] & 1)) {
    exec_sql(db2, "BEGIN IMMEDIATE;");
    p++;
    n--;
  } else {
    step_stmt_once(db2, "INSERT INTO t(a,b) VALUES(456,'other');");
  }

#ifdef SQLITE_ENABLE_UNLOCK_NOTIFY
  {
    sqlite3_stmt *s2 = NULL;
    int rc = sqlite3_prepare_v2(db2, "INSERT INTO t(a,b) VALUES(789,'lock');", -1, &s2, NULL);
    if (rc == SQLITE_OK && s2) {
      rc = sqlite3_step(s2);
      if (rc == SQLITE_LOCKED || rc == SQLITE_LOCKED_SHAREDCACHE) {
        (void)sqlite3_unlock_notify(db2, unlock_notify_cb, db1);
        (void)sqlite3_unlock_notify(db2, NULL, NULL);
      }
    }
    if (s2) sqlite3_finalize(s2);
  }
#endif

  while (n > 0) {
    uint8_t op = *p++;
    n--;

    switch (op % 8) {
      case 0:
        sqlite3_busy_timeout((op & 1) ? db1 : db2, read_int(&p, &n));
        break;
      case 1:
        sqlite3_wal_autocheckpoint((op & 1) ? db1 : db2, read_int(&p, &n));
        break;
      case 2:
        (void)sqlite3_db_cacheflush((op & 1) ? db1 : db2);
        break;
      case 3:
        (void)sqlite3_get_autocommit((op & 1) ? db1 : db2);
        break;
      case 4:
        exec_sql((op & 1) ? db1 : db2, pick_sql(op));
        break;
      case 5:
        step_stmt_once((op & 1) ? db1 : db2, pick_sql(op));
        break;
      case 6:
#ifdef SQLITE_ENABLE_UNLOCK_NOTIFY
        (void)sqlite3_unlock_notify((op & 1) ? db1 : db2, unlock_notify_cb, (void *)(uintptr_t)op);
        if (op & 2) {
          (void)sqlite3_unlock_notify((op & 1) ? db1 : db2, NULL, NULL);
        }
#endif
        break;
      case 7:
        exec_sql((op & 1) ? db1 : db2, (op & 2) ? "COMMIT;" : "ROLLBACK;");
        break;
    }
  }

  if (held_stmt) sqlite3_finalize(held_stmt);

  (void)sqlite3_db_cacheflush(db1);
  (void)sqlite3_db_cacheflush(db2);
  (void)sqlite3_get_autocommit(db1);
  (void)sqlite3_get_autocommit(db2);

  exec_sql(db1, "ROLLBACK;");
  exec_sql(db2, "ROLLBACK;");

  sqlite3_close_v2(db2);
  sqlite3_close_v2(db1);
  return 0;
}