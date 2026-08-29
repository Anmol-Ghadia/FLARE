// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_changes64 at sqlite3.c:153335:26 in sqlite3.h
// sqlite3_total_changes at sqlite3.c:153360:16 in sqlite3.h
// sqlite3_total_changes64 at sqlite3.c:153351:26 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_changes64 at sqlite3.c:153335:26 in sqlite3.h
// sqlite3_total_changes at sqlite3.c:153360:16 in sqlite3.h
// sqlite3_total_changes64 at sqlite3.c:153351:26 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_changes64 at sqlite3.c:153335:26 in sqlite3.h
// sqlite3_total_changes at sqlite3.c:153360:16 in sqlite3.h
// sqlite3_total_changes64 at sqlite3.c:153351:26 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_changes64 at sqlite3.c:153335:26 in sqlite3.h
// sqlite3_total_changes at sqlite3.c:153360:16 in sqlite3.h
// sqlite3_total_changes64 at sqlite3.c:153351:26 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_changes64 at sqlite3.c:153335:26 in sqlite3.h
// sqlite3_total_changes at sqlite3.c:153360:16 in sqlite3.h
// sqlite3_total_changes64 at sqlite3.c:153351:26 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_changes64 at sqlite3.c:153335:26 in sqlite3.h
// sqlite3_total_changes at sqlite3.c:153360:16 in sqlite3.h
// sqlite3_total_changes64 at sqlite3.c:153351:26 in sqlite3.h
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
  const uint8_t *data = (const uint8_t *)pUserData;
  unsigned v = 0;

  if (data) {
    v ^= (unsigned)(data[0]);
    if (p1) v ^= (unsigned)(unsigned char)p1[0];
    if (p2) v ^= (unsigned)(unsigned char)p2[0];
    if (dbName) v ^= (unsigned)(unsigned char)dbName[0];
    if (triggerName) v ^= (unsigned)(unsigned char)triggerName[0];
    v ^= (unsigned)action;
  }

  switch (v % 3U) {
    case 0: return SQLITE_OK;
    case 1: return SQLITE_IGNORE;
    default: return SQLITE_DENY;
  }
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
  (void)sqlite3_changes(db);
  (void)sqlite3_changes64(db);
  (void)sqlite3_total_changes(db);
  (void)sqlite3_total_changes64(db);
}

static void run_prepared(sqlite3 *db, const char *sql, const uint8_t *data, size_t size) {
  sqlite3_stmt *stmt = NULL;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    return;
  }

  sqlite3 *owner = sqlite3_db_handle(stmt);
  if (owner) {
    (void)sqlite3_changes(owner);
    (void)sqlite3_changes64(owner);
    (void)sqlite3_total_changes(owner);
    (void)sqlite3_total_changes64(owner);
  }

  int bind_count = sqlite3_bind_parameter_count(stmt);
  for (int i = 1; i <= bind_count; ++i) {
    if (size == 0) {
      sqlite3_bind_null(stmt, i);
      continue;
    }
    uint8_t b = data[(size_t)(i - 1) % size];
    switch (b % 4) {
      case 0:
        sqlite3_bind_int(stmt, i, (int)b);
        break;
      case 1:
        sqlite3_bind_int64(stmt, i, (sqlite3_int64)((size_t)b * 1234567u));
        break;
      case 2:
        sqlite3_bind_text(stmt, i, (const char *)data, (int)size, SQLITE_TRANSIENT);
        break;
      default:
        sqlite3_bind_blob(stmt, i, data, (int)size, SQLITE_TRANSIENT);
        break;
    }
  }

  for (int i = 0; i < 8; ++i) {
    int rc = sqlite3_step(stmt);
    (void)sqlite3_changes(db);
    (void)sqlite3_changes64(db);
    (void)sqlite3_total_changes(db);
    (void)sqlite3_total_changes64(db);
    if (rc != SQLITE_ROW && rc != SQLITE_DONE) break;
    if (rc == SQLITE_DONE) break;
  }

  sqlite3_reset(stmt);
  sqlite3_clear_bindings(stmt);

  owner = sqlite3_db_handle(stmt);
  if (owner) {
    (void)sqlite3_changes(owner);
    (void)sqlite3_changes64(owner);
    (void)sqlite3_total_changes(owner);
    (void)sqlite3_total_changes64(owner);
  }

  sqlite3_finalize(stmt);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Data && Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  if (sqlite3_open("./dummy_file", &db) != SQLITE_OK) {
    if (db) sqlite3_close(db);
    return 0;
  }

  (void)sqlite3_changes(db);
  (void)sqlite3_changes64(db);
  (void)sqlite3_total_changes(db);
  (void)sqlite3_total_changes64(db);
  (void)sqlite3_db_handle(NULL);

  if (Size > 0) {
    if (Data[0] & 1) {
      sqlite3_set_authorizer(db, fuzz_authorizer, (void *)Data);
    } else {
      sqlite3_set_authorizer(db, NULL, NULL);
    }
  } else {
    sqlite3_set_authorizer(db, fuzz_authorizer, (void *)Data);
  }

  exec_sql(db, "PRAGMA foreign_keys=ON;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t1(a INTEGER PRIMARY KEY, b TEXT UNIQUE);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(x INTEGER REFERENCES t1(a) ON DELETE CASCADE, y TEXT);");
  exec_sql(db, "CREATE VIEW IF NOT EXISTS v1 AS SELECT a,b FROM t1;");
  exec_sql(db,
           "CREATE TRIGGER IF NOT EXISTS trg_ai AFTER INSERT ON t1 "
           "BEGIN INSERT INTO t2(x,y) VALUES (new.a, new.b); END;");
  exec_sql(db,
           "CREATE TRIGGER IF NOT EXISTS trg_v1 INSTEAD OF INSERT ON v1 "
           "BEGIN INSERT INTO t1(a,b) VALUES (new.a,new.b); END;");

  run_prepared(db, "INSERT OR REPLACE INTO t1(a,b) VALUES(?1,?2);", Data, Size);
  run_prepared(db, "UPDATE t1 SET b=?1 WHERE a=?2;", Data, Size);
  run_prepared(db, "DELETE FROM t1 WHERE a=?1;", Data, Size);
  run_prepared(db, "INSERT INTO v1(a,b) VALUES(?1,?2);", Data, Size);
  run_prepared(db, "SELECT count(*), changes(), total_changes() FROM t1;", Data, Size);
  run_prepared(db, "SELECT a,b FROM t1 WHERE b LIKE ?1;", Data, Size);

  if (Size > 1 && (Data[1] & 1)) {
    sqlite3_set_authorizer(db, NULL, NULL);
    run_prepared(db, "INSERT INTO t1(a,b) VALUES(?1,?2);", Data, Size);
    run_prepared(db, "DELETE FROM t1;", Data, Size);
  }

  (void)sqlite3_changes(db);
  (void)sqlite3_changes64(db);
  (void)sqlite3_total_changes(db);
  (void)sqlite3_total_changes64(db);

  sqlite3_close(db);
  return 0;
}