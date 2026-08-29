// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_db_readonly at sqlite3.c:156766:16 in sqlite3.h
// sqlite3_db_readonly at sqlite3.c:156766:16 in sqlite3.h
// sqlite3_db_readonly at sqlite3.c:156766:16 in sqlite3.h
// sqlite3_db_readonly at sqlite3.c:156766:16 in sqlite3.h
// sqlite3_snapshot_get at sqlite3.c:156783:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_snapshot_open at sqlite3.c:156819:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_snapshot_open at sqlite3.c:156819:16 in sqlite3.h
// sqlite3_snapshot_recover at sqlite3.c:156875:16 in sqlite3.h
// sqlite3_snapshot_recover at sqlite3.c:156875:16 in sqlite3.h
// sqlite3_snapshot_recover at sqlite3.c:156875:16 in sqlite3.h
// sqlite3_wal_checkpoint at sqlite3.c:154686:16 in sqlite3.h
// sqlite3_wal_checkpoint at sqlite3.c:154686:16 in sqlite3.h
// sqlite3_wal_checkpoint at sqlite3.c:154686:16 in sqlite3.h
// sqlite3_wal_checkpoint at sqlite3.c:154686:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_snapshot_free at sqlite3.c:156906:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
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
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t i, take = *n < 4 ? *n : 4;
  for (i = 0; i < take; i++) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int read_byte(const uint8_t **p, size_t *n) {
  if (*n == 0) return 0;
  int v = **p;
  (*p)++;
  (*n)--;
  return v;
}

static size_t read_bytes(const uint8_t **p, size_t *n, uint8_t *out, size_t max_out) {
  size_t len = 0;
  if (max_out == 0) return 0;
  if (*n == 0) {
    out[0] = 0;
    return 0;
  }
  len = read_byte(p, n) % max_out;
  if (len > *n) len = *n;
  memcpy(out, *p, len);
  *p += len;
  *n -= len;
  return len;
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
}

static void prepare_db(sqlite3 *db, const uint8_t *data, size_t size) {
  exec_sql(db, "PRAGMA journal_mode=WAL;");
  exec_sql(db, "PRAGMA synchronous=OFF;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(a BLOB, b INTEGER);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(x TEXT);");
  exec_sql(db, "ATTACH DATABASE './dummy_file_attached' AS aux;");
  exec_sql(db, "PRAGMA aux.journal_mode=WAL;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS aux.u(y BLOB);");

  sqlite3_stmt *stmt = NULL;
  if (sqlite3_prepare_v2(db, "INSERT INTO t(a,b) VALUES(?,?);", -1, &stmt, NULL) == SQLITE_OK) {
    sqlite3_bind_blob(stmt, 1, data, (int)(size > 4096 ? 4096 : size), SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, (int)size);
    sqlite3_step(stmt);
  }
  sqlite3_finalize(stmt);

  stmt = NULL;
  if (sqlite3_prepare_v2(db, "INSERT INTO aux.u(y) VALUES(?);", -1, &stmt, NULL) == SQLITE_OK) {
    sqlite3_bind_blob(stmt, 1, data, (int)(size > 2048 ? 2048 : size), SQLITE_STATIC);
    sqlite3_step(stmt);
  }
  sqlite3_finalize(stmt);

  exec_sql(db, "BEGIN;");
  exec_sql(db, "SELECT count(*) FROM t;");
}

int LLVMFuzzerTestOneInput_52(const uint8_t *Data, size_t Size) {
  FILE *fp;
  sqlite3 *db = NULL;
  sqlite3 *db2 = NULL;
  sqlite3_snapshot *snap = NULL;
  sqlite3_stmt *active_stmt = NULL;
  const uint8_t *p = Data;
  size_t n = Size;
  int rc;
  int pnLog = -2, pnCkpt = -2;
  int mode;
  char namebuf[32];
  size_t namelen;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Data && Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }
  fp = fopen("./dummy_file_attached", "wb");
  if (fp) fclose(fp);

  if (sqlite3_open("./dummy_file", &db) != SQLITE_OK) {
    if (db) sqlite3_close(db);
    return 0;
  }

  prepare_db(db, Data, Size);

  if (sqlite3_open("./dummy_file", &db2) == SQLITE_OK) {
    exec_sql(db2, "PRAGMA journal_mode=WAL;");
    exec_sql(db2, "BEGIN;");
    exec_sql(db2, "INSERT INTO t2(x) VALUES('x');");
    exec_sql(db2, "COMMIT;");
  }

  namelen = read_bytes(&p, &n, (uint8_t *)namebuf, sizeof(namebuf) - 1);
  namebuf[namelen] = '\0';

  (void)sqlite3_db_readonly(db, "main");
  (void)sqlite3_db_readonly(db, "temp");
  (void)sqlite3_db_readonly(db, "aux");
  (void)sqlite3_db_readonly(db, namelen ? namebuf : "no_such_db");

  rc = sqlite3_snapshot_get(db, "main", &snap);
  if (rc == SQLITE_OK && snap) {
    if ((read_byte(&p, &n) & 1) != 0) {
      if (sqlite3_prepare_v2(db, "SELECT * FROM t;", -1, &active_stmt, NULL) == SQLITE_OK) {
        sqlite3_step(active_stmt);
      }
    }
    (void)sqlite3_snapshot_open(db, "main", snap);
    if (active_stmt) {
      sqlite3_finalize(active_stmt);
      active_stmt = NULL;
    }
    (void)sqlite3_snapshot_open(db, "main", snap);
  }

  (void)sqlite3_snapshot_recover(db, "main");
  exec_sql(db, "COMMIT;");
  (void)sqlite3_snapshot_recover(db, "main");
  exec_sql(db, "BEGIN;");
  (void)sqlite3_snapshot_recover(db, "aux");

  mode = (int)(read_u32(&p, &n) % 4);
  switch (mode) {
    case 0: mode = SQLITE_CHECKPOINT_PASSIVE; break;
    case 1: mode = SQLITE_CHECKPOINT_FULL; break;
    case 2: mode = SQLITE_CHECKPOINT_RESTART; break;
    default: mode = SQLITE_CHECKPOINT_TRUNCATE; break;
  }

  (void)sqlite3_wal_checkpoint(db, "main");
  (void)sqlite3_wal_checkpoint(db, "aux");
  (void)sqlite3_wal_checkpoint(db, namelen ? namebuf : "");
  (void)sqlite3_wal_checkpoint(db, NULL);

  (void)sqlite3_wal_checkpoint_v2(db, "main", mode, &pnLog, &pnCkpt);
  (void)sqlite3_wal_checkpoint_v2(db, "aux", mode, &pnLog, &pnCkpt);
  (void)sqlite3_wal_checkpoint_v2(db, "", mode, &pnLog, &pnCkpt);
  (void)sqlite3_wal_checkpoint_v2(db, NULL, mode, NULL, NULL);
  (void)sqlite3_wal_checkpoint_v2(db, namelen ? namebuf : "bad", mode, &pnLog, &pnCkpt);

  if (db2) {
    (void)sqlite3_wal_checkpoint_v2(db2, "main", SQLITE_CHECKPOINT_PASSIVE, &pnLog, &pnCkpt);
    sqlite3_close(db2);
  }

  if (snap) {
    sqlite3_snapshot_free(snap);
    snap = NULL;
  }

  if (active_stmt) sqlite3_finalize(active_stmt);
  exec_sql(db, "ROLLBACK;");
  sqlite3_close(db);
  return 0;
}