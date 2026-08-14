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

static int g_busy_count = 0;
static int g_progress_count = 0;
static int g_commit_count = 0;
static int g_profile_count = 0;
static int g_seen_db_handle = 0;

static int fuzz_busy_handler(void *pArg, int count) {
  const unsigned char *cfg = (const unsigned char *)pArg;
  g_busy_count++;
  if (!cfg) return 0;
  return (cfg[count & 7] & 1) ? 1 : 0;
}

static void fuzz_profile(void *pArg, const char *sql, sqlite3_uint64 ns) {
  const unsigned char *cfg = (const unsigned char *)pArg;
  (void)sql;
  (void)ns;
  g_profile_count++;
  if (cfg && (cfg[0] & 0x80)) {
    volatile unsigned char sink = cfg[g_profile_count & 7];
    (void)sink;
  }
}

static int fuzz_commit_hook(void *pArg) {
  const unsigned char *cfg = (const unsigned char *)pArg;
  g_commit_count++;
  if (!cfg) return 0;
  return (cfg[g_commit_count & 7] & 1) ? 1 : 0;
}

static int fuzz_progress_handler(void *pArg) {
  const unsigned char *cfg = (const unsigned char *)pArg;
  g_progress_count++;
  if (!cfg) return 0;
  return (cfg[g_progress_count & 15] & 1) ? 1 : 0;
}

static void fuzz_db_handle_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3 *db = sqlite3_context_db_handle(ctx);
  (void)argc;
  (void)argv;
  if (db) g_seen_db_handle = 1;
  sqlite3_result_int(ctx, db != NULL);
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *err = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &err);
  if (err) sqlite3_free(err);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db1 = NULL;
  sqlite3 *db2 = NULL;
  sqlite3_stmt *stmt = NULL;
  int rc;
  int i;
  int nOps;
  char sqlbuf[256];
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

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

  sqlite3_create_function(db1, "fuzz_db_handle", 0, SQLITE_UTF8, NULL,
                          fuzz_db_handle_func, NULL, NULL);

  sqlite3_busy_handler(db1, fuzz_busy_handler, (void *)Data);
  sqlite3_busy_handler(db1, NULL, NULL);
  sqlite3_busy_handler(db1, fuzz_busy_handler, (void *)Data);

  sqlite3_profile(db1, fuzz_profile, (void *)Data);
  sqlite3_profile(db1, NULL, NULL);
  sqlite3_profile(db1, fuzz_profile, (void *)Data);

  sqlite3_commit_hook(db1, fuzz_commit_hook, (void *)Data);
  sqlite3_commit_hook(db1, NULL, NULL);
  sqlite3_commit_hook(db1, fuzz_commit_hook, (void *)Data);

  nOps = (Size > 0) ? ((int)(Data[0] % 32) - 8) : 0;
  sqlite3_progress_handler(db1, nOps, fuzz_progress_handler, (void *)Data);
  sqlite3_progress_handler(db1, 0, NULL, NULL);
  sqlite3_progress_handler(db1, (Size > 1) ? ((int)(Data[1] % 16) + 1) : 4,
                           fuzz_progress_handler, (void *)Data);

  exec_sql(db1, "PRAGMA journal_mode=DELETE;");
  exec_sql(db1, "PRAGMA synchronous=OFF;");
  exec_sql(db1, "CREATE TABLE IF NOT EXISTS t(a INTEGER, b TEXT);");
  exec_sql(db1, "DELETE FROM t;");

  if (Size > 2) {
    for (i = 0; i < (int)(Data[2] % 8); i++) {
      sqlite3_snprintf(sizeof(sqlbuf), sqlbuf,
                       "INSERT INTO t(a,b) VALUES(%d,'x%02x');",
                       (Size > (size_t)(3 + i) ? Data[3 + i] : i),
                       (unsigned)(Size > (size_t)(3 + i) ? Data[3 + i] : i));
      exec_sql(db1, sqlbuf);
    }
  }

  exec_sql(db1, "SELECT fuzz_db_handle();");

  exec_sql(db2, "PRAGMA journal_mode=DELETE;");
  exec_sql(db2, "BEGIN EXCLUSIVE;");

  sqlite3_prepare_v2(db1, "BEGIN; INSERT INTO t(a,b) VALUES(123,'busy'); COMMIT;", -1, &stmt, NULL);
  if (stmt) {
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  exec_sql(db2, "ROLLBACK;");

  if (Size > 3 && (Data[3] & 1)) {
    sqlite3_interrupt(db1);
  }

  sqlite3_prepare_v2(
      db1,
      "WITH RECURSIVE c(x) AS (SELECT 1 UNION ALL SELECT x+1 FROM c WHERE x<1000) "
      "SELECT sum(x), fuzz_db_handle() FROM c;",
      -1, &stmt, NULL);
  if (stmt) {
    if (Size > 4 && (Data[4] & 1)) {
      sqlite3_interrupt(db1);
    }
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  exec_sql(db1, "BEGIN;");
  exec_sql(db1, "INSERT INTO t(a,b) VALUES(1,'commit');");
  exec_sql(db1, "COMMIT;");

  if (Size > 5) {
    sqlite3_snprintf(sizeof(sqlbuf), sqlbuf,
                     "INSERT INTO t(a,b) VALUES(%u, ?1);",
                     (unsigned)Data[5]);
    sqlite3_prepare_v2(db1, sqlbuf, -1, &stmt, NULL);
    if (stmt) {
      const void *blob = Size > 6 ? (const void *)(Data + 6) : (const void *)"";
      int blob_sz = Size > 6 ? (int)(Size - 6) : 0;
      sqlite3_bind_text(stmt, 1, (const char *)blob, blob_sz, SQLITE_TRANSIENT);
      if (Size > 6 && (Data[6] & 1)) {
        sqlite3_interrupt(db1);
      }
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  sqlite3_busy_handler(db1, NULL, NULL);
  sqlite3_profile(db1, NULL, NULL);
  sqlite3_commit_hook(db1, NULL, NULL);
  sqlite3_progress_handler(db1, 0, NULL, NULL);

  sqlite3_close(db2);
  sqlite3_close(db1);
  return 0;
}