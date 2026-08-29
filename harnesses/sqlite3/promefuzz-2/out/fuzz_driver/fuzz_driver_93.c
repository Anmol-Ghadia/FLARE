// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_snapshot_get at sqlite3.c:156783:16 in sqlite3.h
// sqlite3_snapshot_cmp at sqlite3.c:52001:16 in sqlite3.h
// sqlite3_snapshot_cmp at sqlite3.c:52001:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_snapshot_open at sqlite3.c:156819:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_snapshot_open at sqlite3.c:156819:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_snapshot_recover at sqlite3.c:156875:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_wal_checkpoint at sqlite3.c:154686:16 in sqlite3.h
// sqlite3_wal_checkpoint at sqlite3.c:154686:16 in sqlite3.h
// sqlite3_wal_checkpoint at sqlite3.c:154686:16 in sqlite3.h
// sqlite3_snapshot_free at sqlite3.c:156906:17 in sqlite3.h
// sqlite3_snapshot_free at sqlite3.c:156906:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_get_autocommit at sqlite3.c:155857:16 in sqlite3.h
// sqlite3_snapshot_get at sqlite3.c:156783:16 in sqlite3.h
// sqlite3_snapshot_cmp at sqlite3.c:52001:16 in sqlite3.h
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

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
  uint32_t v = 0;
  size_t i;
  for (i = 0; i < 4 && *Off < Size; ++i, ++(*Off)) {
    v = (v << 8) | Data[*Off];
  }
  return v;
}

static int exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
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
  size_t off = 0;
  uint32_t selector;
  sqlite3 *db1 = NULL;
  sqlite3 *db2 = NULL;
  sqlite3_snapshot *snap1 = NULL;
  sqlite3_snapshot *snap2 = NULL;
  sqlite3_stmt *stmt = NULL;
  int rc;

  write_dummy_file(Data, Size);

  selector = read_u32(Data, Size, &off);

  rc = sqlite3_open("./dummy_file", &db1);
  if (rc != SQLITE_OK || db1 == NULL) {
    if (db1) sqlite3_close(db1);
    return 0;
  }

  rc = sqlite3_open("./dummy_file", &db2);
  if (rc != SQLITE_OK || db2 == NULL) {
    sqlite3_close(db1);
    if (db2) sqlite3_close(db2);
    return 0;
  }

  sqlite3_busy_timeout(db1, 1);
  sqlite3_busy_timeout(db2, 1);

  exec_sql(db1, "PRAGMA journal_mode=WAL;");
  exec_sql(db2, "PRAGMA journal_mode=WAL;");
  exec_sql(db1, "PRAGMA application_id;");
  exec_sql(db2, "PRAGMA application_id;");
  exec_sql(db1, "CREATE TABLE IF NOT EXISTS t(a BLOB, b INTEGER);");

  if (Size > off) {
    sqlite3_stmt *ins = NULL;
    rc = sqlite3_prepare_v2(db1, "INSERT INTO t(a,b) VALUES(?,?);", -1, &ins, NULL);
    if (rc == SQLITE_OK && ins) {
      sqlite3_bind_blob(ins, 1, Data + off, (int)(Size - off), SQLITE_STATIC);
      sqlite3_bind_int(ins, 2, (int)selector);
      sqlite3_step(ins);
    }
    if (ins) sqlite3_finalize(ins);
  } else {
    exec_sql(db1, "INSERT INTO t(a,b) VALUES(x'00',1);");
  }

  exec_sql(db1, "BEGIN;");
  (void)sqlite3_get_autocommit(db1);

#ifdef SQLITE_ENABLE_SNAPSHOT
  rc = sqlite3_snapshot_get(db1, "main", &snap1);
  if (rc == SQLITE_OK && snap1) {
    (void)sqlite3_snapshot_cmp(snap1, snap1);
  }

  if ((selector & 1U) == 0) {
    exec_sql(db1, "COMMIT;");
    exec_sql(db1, "BEGIN;");
  }

  exec_sql(db2, "INSERT INTO t(a,b) VALUES(x'01',2);");

  rc = sqlite3_snapshot_get(db1, "main", &snap2);
  if (rc == SQLITE_OK && snap2 && snap1) {
    (void)sqlite3_snapshot_cmp(snap1, snap2);
    (void)sqlite3_snapshot_cmp(snap2, snap1);
  }

  if ((selector & 2U) != 0) {
    rc = sqlite3_prepare_v2(db1, "SELECT * FROM t;", -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      sqlite3_step(stmt);
      (void)sqlite3_snapshot_open(db1, "main", snap1 ? snap1 : snap2);
      sqlite3_reset(stmt);
    }
  } else {
    (void)sqlite3_snapshot_open(db1, "main", snap1 ? snap1 : snap2);
  }

  if (stmt) {
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  exec_sql(db1, "ROLLBACK;");
  (void)sqlite3_snapshot_recover(db1, "main");
#endif

  (void)sqlite3_get_autocommit(db1);
  (void)sqlite3_get_autocommit(db2);

  (void)sqlite3_wal_checkpoint(db1, "main");
  (void)sqlite3_wal_checkpoint(db2, "main");
  (void)sqlite3_wal_checkpoint(db1, NULL);

#ifdef SQLITE_ENABLE_SNAPSHOT
  if (snap1) sqlite3_snapshot_free(snap1);
  if (snap2) sqlite3_snapshot_free(snap2);
#endif

  if (stmt) sqlite3_finalize(stmt);
  sqlite3_close(db2);
  sqlite3_close(db1);
  return 0;
}