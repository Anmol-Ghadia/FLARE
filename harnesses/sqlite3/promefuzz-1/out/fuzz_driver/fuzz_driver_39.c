// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_db_readonly at sqlite3.c:156766:16 in sqlite3.h
// sqlite3_db_readonly at sqlite3.c:156766:16 in sqlite3.h
// sqlite3_db_readonly at sqlite3.c:156766:16 in sqlite3.h
// sqlite3_txn_state at sqlite3.c:153500:16 in sqlite3.h
// sqlite3_txn_state at sqlite3.c:153500:16 in sqlite3.h
// sqlite3_txn_state at sqlite3.c:153500:16 in sqlite3.h
// sqlite3_txn_state at sqlite3.c:153500:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_snapshot_recover at sqlite3.c:156875:16 in sqlite3.h
// sqlite3_snapshot_recover at sqlite3.c:156875:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_wal_checkpoint at sqlite3.c:154686:16 in sqlite3.h
// sqlite3_wal_checkpoint at sqlite3.c:154686:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_db_readonly at sqlite3.c:156766:16 in sqlite3.h
// sqlite3_txn_state at sqlite3.c:153500:16 in sqlite3.h
// sqlite3_wal_checkpoint at sqlite3.c:154686:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_snapshot_recover at sqlite3.c:156875:16 in sqlite3.h
// sqlite3_db_cacheflush at sqlite3.c:153134:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
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

static int exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  int rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
  return rc;
}

static void write_dummy_file(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3 *db2 = NULL;
  int rc;
  int pnLog = -2, pnCkpt = -2;
  size_t i;
  unsigned ctrl = 0;
  char namebuf[32];
  const char *zDbArg = "main";
  const char *zSchemaArg = "main";
  int mode;

  write_dummy_file(Data, Size);

  for (i = 0; i < Size && i < sizeof(ctrl); i++) {
    ctrl = (ctrl << 8) | Data[i];
  }

  rc = sqlite3_open_v2("./dummy_file", &db,
                       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_busy_timeout(db, (int)(ctrl & 0x3F));

  exec_sql(db, "PRAGMA journal_mode=WAL;");
  exec_sql(db, "PRAGMA synchronous=OFF;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(a BLOB, b INTEGER);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(x TEXT);");

  if (Size > 4) {
    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, "INSERT INTO t(a,b) VALUES(?,?);", -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      sqlite3_bind_blob(stmt, 1, Data, (int)Size, SQLITE_STATIC);
      sqlite3_bind_int(stmt, 2, (int)ctrl);
      sqlite3_step(stmt);
    }
    if (stmt) sqlite3_finalize(stmt);
  }

  if (ctrl & 1) exec_sql(db, "BEGIN;");
  if (ctrl & 2) exec_sql(db, "BEGIN IMMEDIATE;");
  if (ctrl & 4) exec_sql(db, "SAVEPOINT s1;");
  if (ctrl & 8) exec_sql(db, "INSERT INTO t2(x) VALUES('abc');");
  if (ctrl & 16) exec_sql(db, "SELECT * FROM t;");
  if (ctrl & 32) exec_sql(db, "ATTACH DATABASE './dummy_file' AS aux;");
  if (ctrl & 64) exec_sql(db, "PRAGMA wal_autocheckpoint=1;");
  if (ctrl & 128) exec_sql(db, "PRAGMA locking_mode=EXCLUSIVE;");

  switch ((ctrl >> 8) % 6) {
    case 0: zDbArg = "main"; break;
    case 1: zDbArg = "temp"; break;
    case 2: zDbArg = "aux"; break;
    case 3: zDbArg = ""; break;
    case 4: zDbArg = NULL; break;
    default:
      memset(namebuf, 0, sizeof(namebuf));
      for (i = 0; i < sizeof(namebuf) - 1 && 4 + i < Size; i++) {
        unsigned char c = Data[4 + i];
        namebuf[i] = (char)((c >= 32 && c <= 126) ? c : 'A');
      }
      zDbArg = namebuf;
      break;
  }

  switch ((ctrl >> 12) % 5) {
    case 0: zSchemaArg = "main"; break;
    case 1: zSchemaArg = "temp"; break;
    case 2: zSchemaArg = "aux"; break;
    case 3: zSchemaArg = NULL; break;
    default: zSchemaArg = zDbArg; break;
  }

  (void)sqlite3_db_readonly(db, zDbArg);
  (void)sqlite3_db_readonly(db, "main");
  (void)sqlite3_db_readonly(db, "no_such_db");

  (void)sqlite3_txn_state(db, zSchemaArg);
  (void)sqlite3_txn_state(db, NULL);
  (void)sqlite3_txn_state(db, "main");
  (void)sqlite3_txn_state(db, "no_such_schema");

  (void)sqlite3_db_cacheflush(db);

#ifdef SQLITE_ENABLE_SNAPSHOT
  (void)sqlite3_snapshot_recover(db, zDbArg);
  (void)sqlite3_snapshot_recover(db, "main");
#endif

  mode = (int)((ctrl >> 16) % 4);
  switch (mode) {
    case 0: mode = SQLITE_CHECKPOINT_PASSIVE; break;
    case 1: mode = SQLITE_CHECKPOINT_FULL; break;
    case 2: mode = SQLITE_CHECKPOINT_RESTART; break;
    default: mode = SQLITE_CHECKPOINT_TRUNCATE; break;
  }

  (void)sqlite3_wal_checkpoint_v2(db, zDbArg, mode, &pnLog, &pnCkpt);
  (void)sqlite3_wal_checkpoint_v2(db, "main", SQLITE_CHECKPOINT_PASSIVE, &pnLog, &pnCkpt);
  (void)sqlite3_wal_checkpoint_v2(db, NULL, SQLITE_CHECKPOINT_FULL, NULL, NULL);
  (void)sqlite3_wal_checkpoint(db, zDbArg);
  (void)sqlite3_wal_checkpoint(db, "main");

  if ((ctrl >> 20) & 1) {
    rc = sqlite3_open_v2("./dummy_file", &db2, SQLITE_OPEN_READONLY, NULL);
    if (rc == SQLITE_OK && db2) {
      (void)sqlite3_db_readonly(db2, "main");
      (void)sqlite3_txn_state(db2, NULL);
      (void)sqlite3_wal_checkpoint(db2, "main");
      (void)sqlite3_wal_checkpoint_v2(db2, "main", SQLITE_CHECKPOINT_PASSIVE, &pnLog, &pnCkpt);
#ifdef SQLITE_ENABLE_SNAPSHOT
      (void)sqlite3_snapshot_recover(db2, "main");
#endif
      (void)sqlite3_db_cacheflush(db2);
    }
  }

  exec_sql(db, "RELEASE s1;");
  exec_sql(db, "ROLLBACK TO s1;");
  exec_sql(db, "COMMIT;");
  exec_sql(db, "ROLLBACK;");
  exec_sql(db, "DETACH DATABASE aux;");

  if (db2) sqlite3_close(db2);
  sqlite3_close(db);
  return 0;
}