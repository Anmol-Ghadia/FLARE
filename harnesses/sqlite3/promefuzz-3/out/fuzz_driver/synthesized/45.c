// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_txn_state at sqlite3.c:153500:16 in sqlite3.h
// sqlite3_txn_state at sqlite3.c:153500:16 in sqlite3.h
// sqlite3_txn_state at sqlite3.c:153500:16 in sqlite3.h
// sqlite3_txn_state at sqlite3.c:153500:16 in sqlite3.h
// sqlite3_snapshot_get at sqlite3.c:156783:16 in sqlite3.h
// sqlite3_snapshot_recover at sqlite3.c:156875:16 in sqlite3.h
// sqlite3_wal_checkpoint at sqlite3.c:154686:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_snapshot_open at sqlite3.c:156819:16 in sqlite3.h
// sqlite3_snapshot_open at sqlite3.c:156819:16 in sqlite3.h
// sqlite3_snapshot_get at sqlite3.c:156783:16 in sqlite3.h
// sqlite3_snapshot_open at sqlite3.c:156819:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_snapshot_free at sqlite3.c:156906:17 in sqlite3.h
// sqlite3_snapshot_free at sqlite3.c:156906:17 in sqlite3.h
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

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; i++) {
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

static size_t read_buf(const uint8_t **p, size_t *n, char *out, size_t out_sz) {
  if (out_sz == 0) return 0;
  size_t want = (*n == 0) ? 0 : (size_t)(read_byte(p, n) % out_sz);
  if (want > out_sz - 1) want = out_sz - 1;
  size_t take = want < *n ? want : *n;
  if (take > 0) memcpy(out, *p, take);
  out[take] = '\0';
  *p += take;
  *n -= take;
  return take;
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
}

static void write_dummy_file(const uint8_t *data, size_t size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (size) fwrite(data, 1, size, fp);
  fclose(fp);
}

int LLVMFuzzerTestOneInput_45(const uint8_t *Data, size_t Size) {
#ifndef SQLITE_ENABLE_SNAPSHOT
  (void)Data;
  (void)Size;
  return 0;
#else
  const uint8_t *p = Data;
  size_t n = Size;
  sqlite3 *db1 = NULL;
  sqlite3 *db2 = NULL;
  sqlite3_snapshot *snap1 = NULL;
  sqlite3_snapshot *snap2 = NULL;
  sqlite3_stmt *stmt = NULL;
  int pnLog = -1, pnCkpt = -1;
  char schema_buf[32];
  char attach_name[32];
  const char *schema = "main";
  const char *checkpoint_schema = "main";
  int rc;

  write_dummy_file(Data, Size);
  remove("./dummy_db");
  remove("./dummy_db-wal");
  remove("./dummy_db-shm");
  remove("./dummy_attached");
  remove("./dummy_attached-wal");
  remove("./dummy_attached-shm");

  if (sqlite3_open("./dummy_db", &db1) != SQLITE_OK) {
    if (db1) sqlite3_close(db1);
    return 0;
  }
  if (sqlite3_open("./dummy_db", &db2) != SQLITE_OK) {
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
  exec_sql(db1, "CREATE TABLE IF NOT EXISTS t(a BLOB, b TEXT);");
  exec_sql(db1, "BEGIN;");
  exec_sql(db1, "INSERT INTO t(a,b) VALUES(randomblob(8),'seed');");
  exec_sql(db1, "COMMIT;");

  if (read_byte(&p, &n) & 1) {
    exec_sql(db1, "ATTACH DATABASE './dummy_attached' AS aux;");
    exec_sql(db1, "PRAGMA aux.journal_mode=WAL;");
    exec_sql(db1, "CREATE TABLE IF NOT EXISTS aux.t2(x);");
    exec_sql(db1, "BEGIN;");
    exec_sql(db1, "INSERT INTO aux.t2 VALUES(1);");
    exec_sql(db1, "COMMIT;");
  }

  if (read_byte(&p, &n) & 1) {
    read_buf(&p, &n, schema_buf, sizeof(schema_buf));
    if (schema_buf[0]) schema = schema_buf;
  } else {
    static const char *schemas[] = {"main", "aux", "", "nope"};
    schema = schemas[read_byte(&p, &n) % 4];
  }

  if (read_byte(&p, &n) & 1) {
    read_buf(&p, &n, attach_name, sizeof(attach_name));
    checkpoint_schema = attach_name;
  } else {
    static const char *cp_schemas[] = {"main", "aux", "", "nope"};
    checkpoint_schema = cp_schemas[read_byte(&p, &n) % 4];
  }

  switch (read_byte(&p, &n) % 4) {
    case 0:
      exec_sql(db1, "BEGIN;");
      break;
    case 1:
      exec_sql(db1, "BEGIN DEFERRED;");
      break;
    case 2:
      exec_sql(db1, "BEGIN IMMEDIATE;");
      break;
    default:
      break;
  }

  if (read_byte(&p, &n) & 1) {
    sqlite3_prepare_v2(db1, "SELECT * FROM t;", -1, &stmt, NULL);
    if (stmt && (read_byte(&p, &n) & 1)) {
      sqlite3_step(stmt);
    }
  }

  if (read_byte(&p, &n) & 1) {
    sqlite3_exec(db1, "SELECT * FROM t;", NULL, NULL, NULL);
  }

  if (read_byte(&p, &n) & 1) {
    exec_sql(db1, "SAVEPOINT s1;");
  }

  if (read_byte(&p, &n) & 1) {
    exec_sql(db1, "INSERT INTO t(a,b) VALUES(randomblob(16),'x');");
  }

  (void)sqlite3_txn_state(db1, NULL);
  (void)sqlite3_txn_state(db1, "main");
  (void)sqlite3_txn_state(db1, schema);
  (void)sqlite3_txn_state(db1, "nope");

  rc = sqlite3_snapshot_get(db1, schema, &snap1);
  if (rc != SQLITE_OK) {
    snap1 = NULL;
  }

  if (read_byte(&p, &n) & 1) {
    exec_sql(db2, "BEGIN;");
    exec_sql(db2, "INSERT INTO t(a,b) VALUES(randomblob(32),'writer');");
    exec_sql(db2, "COMMIT;");
  }

  if (read_byte(&p, &n) & 1) {
    (void)sqlite3_snapshot_recover(db1, schema);
  }

  (void)sqlite3_wal_checkpoint(db1, checkpoint_schema);

  {
    static const int modes[] = {
      SQLITE_CHECKPOINT_PASSIVE,
      SQLITE_CHECKPOINT_FULL,
      SQLITE_CHECKPOINT_RESTART,
      SQLITE_CHECKPOINT_TRUNCATE
    };
    const char *zDbArg;
    switch (read_byte(&p, &n) % 4) {
      case 0: zDbArg = checkpoint_schema; break;
      case 1: zDbArg = NULL; break;
      case 2: zDbArg = ""; break;
      default: zDbArg = "nope"; break;
    }
    (void)sqlite3_wal_checkpoint_v2(
        db1, zDbArg, modes[read_byte(&p, &n) % 4], &pnLog, &pnCkpt);
  }

  if (snap1) {
    if (read_byte(&p, &n) & 1) {
      exec_sql(db1, "ROLLBACK;");
      exec_sql(db1, "BEGIN;");
    }
    (void)sqlite3_snapshot_open(db1, schema, snap1);
  }

  if (read_byte(&p, &n) & 1) {
    sqlite3_snapshot bogus;
    size_t cpy = n < sizeof(bogus.hidden) ? n : sizeof(bogus.hidden);
    memset(&bogus, 0, sizeof(bogus));
    if (cpy) memcpy(bogus.hidden, p, cpy);
    (void)sqlite3_snapshot_open(db1, schema, &bogus);
  }

  if (read_byte(&p, &n) & 1) {
    exec_sql(db1, "COMMIT;");
    exec_sql(db1, "BEGIN;");
    rc = sqlite3_snapshot_get(db1, "main", &snap2);
    if (rc != SQLITE_OK) snap2 = NULL;
    if (snap2) {
      (void)sqlite3_snapshot_open(db1, "main", snap2);
    }
  }

  if (stmt) {
    if (read_byte(&p, &n) & 1) sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
  }

  if (snap1) sqlite3_snapshot_free(snap1);
  if (snap2) sqlite3_snapshot_free(snap2);

  exec_sql(db1, "ROLLBACK;");
  exec_sql(db2, "ROLLBACK;");
  exec_sql(db1, "DETACH DATABASE aux;");

  sqlite3_close(db2);
  sqlite3_close(db1);

  remove("./dummy_db");
  remove("./dummy_db-wal");
  remove("./dummy_db-shm");
  remove("./dummy_attached");
  remove("./dummy_attached-wal");
  remove("./dummy_attached-shm");
  return 0;
#endif
}