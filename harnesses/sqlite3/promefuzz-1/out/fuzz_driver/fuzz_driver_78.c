// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_commit_hook at sqlite3.c:154449:18 in sqlite3.h
// sqlite3_commit_hook at sqlite3.c:154449:18 in sqlite3.h
// sqlite3_wal_hook at sqlite3.c:154595:18 in sqlite3.h
// sqlite3_wal_hook at sqlite3.c:154595:18 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_unlock_notify at sqlite3.c:157112:16 in sqlite3.h
// sqlite3_unlock_notify at sqlite3.c:157112:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_unlock_notify at sqlite3.c:157112:16 in sqlite3.h
// sqlite3_unlock_notify at sqlite3.c:157112:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_commit_hook at sqlite3.c:154449:18 in sqlite3.h
// sqlite3_wal_hook at sqlite3.c:154595:18 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_enable_shared_cache at sqlite3.c:53207:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
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

static int g_commit_mode = 0;
static int g_progress_mode = 0;
static int g_wal_mode = 0;
static int g_unlock_count = 0;

static int fuzz_commit_hook(void *pArg) {
  (void)pArg;
  if (g_commit_mode == 0) return 0;
  if (g_commit_mode == 1) return 1;
  return (g_commit_mode & 1);
}

static int fuzz_progress_handler(void *pArg) {
  (void)pArg;
  if (g_progress_mode == 0) return 0;
  if (g_progress_mode == 1) return 1;
  return (g_progress_mode & 1);
}

static int fuzz_wal_hook(void *pArg, sqlite3 *db, const char *dbname, int nPages) {
  (void)pArg;
  (void)db;
  (void)dbname;
  (void)nPages;
  if (g_wal_mode == 0) return SQLITE_OK;
  if (g_wal_mode == 1) return SQLITE_BUSY;
  if (g_wal_mode == 2) return SQLITE_ERROR;
  return SQLITE_OK;
}

static void fuzz_unlock_notify(void **apArg, int nArg) {
  int i;
  for (i = 0; i < nArg; i++) {
    if (apArg && apArg[i]) {
      g_unlock_count++;
    }
  }
}

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
  uint32_t v = 0;
  size_t i;
  for (i = 0; i < 4 && *Off < Size; i++, (*Off)++) {
    v = (v << 8) | Data[*Off];
  }
  return v;
}

static int exec_sql(sqlite3 *db, const char *sql) {
  char *err = NULL;
  int rc = sqlite3_exec(db, sql, NULL, NULL, &err);
  if (err) sqlite3_free(err);
  return rc;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db1 = NULL, *db2 = NULL;
  sqlite3_stmt *stmt = NULL;
  FILE *fp;
  size_t off = 0;
  int rc;
  int flags1, flags2;
  int nOps;
  void *oldp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3_enable_shared_cache(1);

  flags1 = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI;
  flags2 = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI | SQLITE_OPEN_SHAREDCACHE;

  rc = sqlite3_open_v2("./dummy_file", &db1, flags1, NULL);
  if (rc != SQLITE_OK || db1 == NULL) {
    if (db1) sqlite3_close(db1);
    return 0;
  }

  rc = sqlite3_open_v2("file:./dummy_file?cache=shared", &db2, flags2, NULL);
  if (rc != SQLITE_OK || db2 == NULL) {
    sqlite3_close(db1);
    if (db2) sqlite3_close(db2);
    return 0;
  }

  exec_sql(db1, "PRAGMA journal_mode=WAL;");
  exec_sql(db2, "PRAGMA journal_mode=WAL;");
  exec_sql(db1, "CREATE TABLE IF NOT EXISTS t(a INTEGER, b TEXT);");
  exec_sql(db1, "CREATE TABLE IF NOT EXISTS t2(x INTEGER);");

  g_commit_mode = (int)(read_u32(Data, Size, &off) % 3);
  g_progress_mode = (int)(read_u32(Data, Size, &off) % 3);
  g_wal_mode = (int)(read_u32(Data, Size, &off) % 4);
  nOps = (int)(read_u32(Data, Size, &off) % 32);

  oldp = sqlite3_commit_hook(db1, fuzz_commit_hook, (void*)db1);
  (void)oldp;
  oldp = sqlite3_commit_hook(db1, fuzz_commit_hook, (void*)db2);
  (void)oldp;
  oldp = sqlite3_wal_hook(db1, fuzz_wal_hook, (void*)db1);
  (void)oldp;
  oldp = sqlite3_wal_hook(db1, fuzz_wal_hook, (void*)db2);
  (void)oldp;

  if (nOps > 0) {
    sqlite3_progress_handler(db1, nOps, fuzz_progress_handler, (void*)db1);
  } else {
    sqlite3_progress_handler(db1, 0, fuzz_progress_handler, (void*)db1);
  }

  sqlite3_progress_handler(db1, -1, fuzz_progress_handler, (void*)db2);
  sqlite3_progress_handler(db1, 1 + (int)(Size % 8), fuzz_progress_handler, (void*)db1);

  rc = sqlite3_prepare_v2(db1,
                          "INSERT INTO t(a,b) VALUES(?1,?2);"
                          "SELECT * FROM t WHERE a>=?3 ORDER BY a,b;",
                          -1, &stmt, NULL);
  if (rc == SQLITE_OK && stmt) {
    sqlite3_bind_int(stmt, 1, (int)(Size & 0x7fffffff));
    sqlite3_bind_blob(stmt, 2, Data, (int)Size, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, (int)((Size > 0) ? Data[0] : 0));

    (void)sqlite3_db_handle(stmt);

    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);

    sqlite3_bind_int(stmt, 1, (int)(read_u32(Data, Size, &off) & 0x7fffffff));
    sqlite3_bind_text(stmt, 2, (const char *)Data, (int)Size, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, (int)((Size > 1) ? Data[1] : 0));

    if ((Size & 1) != 0) {
      sqlite3_interrupt(db1);
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  exec_sql(db1, "BEGIN IMMEDIATE;");
  exec_sql(db1, "INSERT INTO t2(x) VALUES(1);");

  exec_sql(db2, "BEGIN;");
  rc = exec_sql(db2, "SELECT * FROM t2;");
  (void)rc;

  rc = exec_sql(db2, "DROP TABLE t2;");
  if (rc == SQLITE_LOCKED || rc == SQLITE_BUSY) {
#ifdef SQLITE_ENABLE_UNLOCK_NOTIFY
    (void)sqlite3_unlock_notify(db2, fuzz_unlock_notify, db2);
    exec_sql(db1, "COMMIT;");
#else
    (void)g_unlock_count;
#endif
  } else {
#ifdef SQLITE_ENABLE_UNLOCK_NOTIFY
    (void)sqlite3_unlock_notify(db2, NULL, NULL);
#endif
    exec_sql(db1, "COMMIT;");
  }

#ifdef SQLITE_ENABLE_UNLOCK_NOTIFY
  rc = sqlite3_prepare_v2(db2, "BEGIN IMMEDIATE; INSERT INTO t(a,b) VALUES(99,'x');", -1, &stmt, NULL);
  if (rc == SQLITE_OK && stmt) {
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    }
    if (rc == SQLITE_LOCKED) {
      (void)sqlite3_unlock_notify(db2, fuzz_unlock_notify, db1);
    } else {
      (void)sqlite3_unlock_notify(db2, NULL, NULL);
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }
#endif

  sqlite3_commit_hook(db1, NULL, NULL);
  sqlite3_wal_hook(db1, NULL, NULL);
  sqlite3_progress_handler(db1, 0, NULL, NULL);

  exec_sql(db2, "ROLLBACK;");
  exec_sql(db1, "ROLLBACK;");

  if (stmt) sqlite3_finalize(stmt);
  sqlite3_close(db2);
  sqlite3_close(db1);
  return 0;
}