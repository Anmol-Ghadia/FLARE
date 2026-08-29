// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_wal_checkpoint at sqlite3.c:154686:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_snapshot_recover at sqlite3.c:156875:16 in sqlite3.h
// sqlite3_snapshot_open at sqlite3.c:156819:16 in sqlite3.h
// sqlite3_snapshot_open at sqlite3.c:156819:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_snapshot_open at sqlite3.c:156819:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_snapshot_recover at sqlite3.c:156875:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_snapshot_free at sqlite3.c:156906:17 in sqlite3.h
// sqlite3_snapshot_free at sqlite3.c:156906:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_snapshot_get at sqlite3.c:156783:16 in sqlite3.h
// sqlite3_snapshot_get at sqlite3.c:156783:16 in sqlite3.h
// sqlite3_snapshot_cmp at sqlite3.c:52001:16 in sqlite3.h
// sqlite3_snapshot_cmp at sqlite3.c:52001:16 in sqlite3.h
// sqlite3_snapshot_cmp at sqlite3.c:52001:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
}

static void write_dummy_file(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

static int pick_mode(uint8_t b) {
  switch (b & 3) {
    case 0: return SQLITE_CHECKPOINT_PASSIVE;
    case 1: return SQLITE_CHECKPOINT_FULL;
    case 2: return SQLITE_CHECKPOINT_RESTART;
    default: return SQLITE_CHECKPOINT_TRUNCATE;
  }
}

int LLVMFuzzerTestOneInput_108(const uint8_t *Data, size_t Size) {
  sqlite3 *db1 = NULL, *db2 = NULL;
  sqlite3_snapshot *snap1 = NULL, *snap2 = NULL;
  sqlite3_stmt *stmt = NULL;
  int pnLog = -1, pnCkpt = -1;
  int mode = SQLITE_CHECKPOINT_PASSIVE;
  const char *schema = "main";
  const char *zDbArg = "main";

  write_dummy_file(Data, Size);

  (void)sqlite3_open("./dummy_file", &db1);
  (void)sqlite3_open("./dummy_file", &db2);
  if (!db1 || !db2) goto cleanup;

  sqlite3_busy_timeout(db1, 1);
  sqlite3_busy_timeout(db2, 1);

  exec_sql(db1, "PRAGMA journal_mode=WAL;");
  exec_sql(db2, "PRAGMA journal_mode=WAL;");
  exec_sql(db1, "PRAGMA application_id;");
  exec_sql(db2, "PRAGMA application_id;");
  exec_sql(db1, "CREATE TABLE IF NOT EXISTS t(x BLOB, y INTEGER);");

  exec_sql(db1, "BEGIN;");
  exec_sql(db1, "INSERT INTO t(x,y) VALUES(zeroblob(1),1);");
  exec_sql(db1, "COMMIT;");

  exec_sql(db1, "BEGIN;");
  if (Size > 0) {
    sqlite3_prepare_v2(db1, "INSERT INTO t(x,y) VALUES(?1,?2);", -1, &stmt, NULL);
    if (stmt) {
      sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
      sqlite3_bind_int(stmt, 2, (int)(Data[0]));
      sqlite3_step(stmt);
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }
  exec_sql(db1, "COMMIT;");

  exec_sql(db1, "BEGIN;");
  (void)sqlite3_snapshot_get(db1, schema, &snap1);
  exec_sql(db1, "COMMIT;");

  exec_sql(db1, "BEGIN;");
  exec_sql(db1, "INSERT INTO t(x,y) VALUES('second',2);");
  exec_sql(db1, "COMMIT;");

  exec_sql(db1, "BEGIN;");
  (void)sqlite3_snapshot_get(db1, schema, &snap2);
  exec_sql(db1, "COMMIT;");

  if (snap1 && snap2) {
    (void)sqlite3_snapshot_cmp(snap1, snap2);
    (void)sqlite3_snapshot_cmp(snap2, snap1);
    (void)sqlite3_snapshot_cmp(snap1, snap1);
  }

  mode = pick_mode(Size ? Data[0] : 0);
  if (Size > 1) {
    switch (Data[1] % 4) {
      case 0: zDbArg = "main"; break;
      case 1: zDbArg = ""; break;
      case 2: zDbArg = NULL; break;
      default: zDbArg = "nosuch"; break;
    }
  }

  (void)sqlite3_wal_checkpoint(db1, "main");
  (void)sqlite3_wal_checkpoint_v2(db1, zDbArg, mode, &pnLog, &pnCkpt);
  (void)sqlite3_wal_checkpoint_v2(db1, "main", SQLITE_CHECKPOINT_PASSIVE, NULL, NULL);

  (void)sqlite3_snapshot_recover(db2, "main");

  exec_sql(db2, "BEGIN;");
  if (snap1) (void)sqlite3_snapshot_open(db2, "main", snap1);
  if (snap2) (void)sqlite3_snapshot_open(db2, "main", snap2);
  exec_sql(db2, "COMMIT;");

  exec_sql(db2, "BEGIN;");
  sqlite3_prepare_v2(db2, "SELECT * FROM t;", -1, &stmt, NULL);
  if (stmt) {
    sqlite3_step(stmt);
    if (snap1) (void)sqlite3_snapshot_open(db2, "main", snap1);
    sqlite3_finalize(stmt);
    stmt = NULL;
  }
  exec_sql(db2, "COMMIT;");

  exec_sql(db2, "BEGIN;");
  exec_sql(db2, "SELECT count(*) FROM t;");
  (void)sqlite3_snapshot_recover(db2, "main");
  exec_sql(db2, "COMMIT;");

cleanup:
  if (stmt) sqlite3_finalize(stmt);
  if (snap1) sqlite3_snapshot_free(snap1);
  if (snap2) sqlite3_snapshot_free(snap2);
  if (db1) sqlite3_close(db1);
  if (db2) sqlite3_close(db2);
  return 0;
}