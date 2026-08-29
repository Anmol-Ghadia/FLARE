// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_commit_hook at sqlite3.c:154449:18 in sqlite3.h
// sqlite3_rollback_hook at sqlite3.c:154499:18 in sqlite3.h
// sqlite3_trace at sqlite3.c:154368:18 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_commit_hook at sqlite3.c:154449:18 in sqlite3.h
// sqlite3_rollback_hook at sqlite3.c:154499:18 in sqlite3.h
// sqlite3_trace at sqlite3.c:154368:18 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_commit_hook at sqlite3.c:154449:18 in sqlite3.h
// sqlite3_rollback_hook at sqlite3.c:154499:18 in sqlite3.h
// sqlite3_trace at sqlite3.c:154368:18 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_commit_hook at sqlite3.c:154449:18 in sqlite3.h
// sqlite3_rollback_hook at sqlite3.c:154499:18 in sqlite3.h
// sqlite3_trace at sqlite3.c:154368:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

static int g_authorizer_calls = 0;
static int g_progress_calls = 0;
static int g_commit_calls = 0;
static int g_rollback_calls = 0;
static int g_trace_calls = 0;

struct FuzzCtx {
  const uint8_t *data;
  size_t size;
};

static uint8_t ctx_byte(const struct FuzzCtx *ctx, size_t idx) {
  if (!ctx || !ctx->data || ctx->size == 0) return 0;
  return ctx->data[idx % ctx->size];
}

static int fuzz_authorizer(void *pUserData, int action,
                           const char *p1, const char *p2,
                           const char *dbName, const char *triggerName) {
  const struct FuzzCtx *ctx = (const struct FuzzCtx *)pUserData;
  size_t idx = (size_t)(g_authorizer_calls++);
  int mode = (int)(ctx_byte(ctx, idx) % 4);
  (void)action;
  (void)p1;
  (void)p2;
  (void)dbName;
  (void)triggerName;
  switch (mode) {
    case 0: return SQLITE_OK;
    case 1: return SQLITE_IGNORE;
    case 2: return SQLITE_DENY;
    default: return SQLITE_OK;
  }
}

static int fuzz_progress(void *pArg) {
  const struct FuzzCtx *ctx = (const struct FuzzCtx *)pArg;
  size_t idx = (size_t)(g_progress_calls++);
  return (ctx_byte(ctx, idx) & 1) ? 1 : 0;
}

static int fuzz_commit(void *pArg) {
  const struct FuzzCtx *ctx = (const struct FuzzCtx *)pArg;
  size_t idx = (size_t)(g_commit_calls++);
  return (ctx_byte(ctx, idx) % 3) == 0 ? 1 : 0;
}

static void fuzz_rollback(void *pArg) {
  const struct FuzzCtx *ctx = (const struct FuzzCtx *)pArg;
  (void)ctx;
  g_rollback_calls++;
}

static void fuzz_trace(void *pArg, const char *sql) {
  const struct FuzzCtx *ctx = (const struct FuzzCtx *)pArg;
  (void)ctx;
  (void)sql;
  g_trace_calls++;
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *sql = NULL;
  int rc;
  int nOps = 0;
  size_t sql_len = 0;
  FILE *fp;
  struct FuzzCtx ctx;

  ctx.data = Data;
  ctx.size = Size;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open_v2(":memory:", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_busy_timeout(db, 1);
  sqlite3_limit(db, SQLITE_LIMIT_SQL_LENGTH, 4096);
  sqlite3_limit(db, SQLITE_LIMIT_EXPR_DEPTH, 20);
  sqlite3_limit(db, SQLITE_LIMIT_COMPOUND_SELECT, 5);

  exec_sql(db, "PRAGMA journal_mode=MEMORY;");
  exec_sql(db, "PRAGMA synchronous=OFF;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(a,b,c);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS u(x,y);");
  exec_sql(db, "INSERT INTO t VALUES(1,2,3);");
  exec_sql(db, "INSERT INTO u VALUES('x','y');");
  exec_sql(db, "CREATE TRIGGER IF NOT EXISTS tr AFTER INSERT ON t BEGIN INSERT INTO u VALUES('trig','fire'); END;");

  sqlite3_set_authorizer(db, fuzz_authorizer, &ctx);
  nOps = (Size > 1) ? (int)(Data[1] % 16) : 0;
  sqlite3_progress_handler(db, nOps, fuzz_progress, &ctx);
  sqlite3_commit_hook(db, fuzz_commit, &ctx);
  sqlite3_rollback_hook(db, fuzz_rollback, &ctx);
  sqlite3_trace(db, fuzz_trace, &ctx);

  if (Size > 2 && (Data[2] & 1)) {
    sqlite3_set_authorizer(db, NULL, NULL);
  }
  if (Size > 2 && (Data[2] & 2)) {
    sqlite3_progress_handler(db, 0, NULL, NULL);
  }
  if (Size > 2 && (Data[2] & 4)) {
    sqlite3_commit_hook(db, NULL, NULL);
  }
  if (Size > 2 && (Data[2] & 8)) {
    sqlite3_rollback_hook(db, NULL, NULL);
  }
  if (Size > 2 && (Data[2] & 16)) {
    sqlite3_trace(db, NULL, NULL);
  }

  if (Size > 3 && (Data[3] & 1)) {
    sqlite3_set_authorizer(db, fuzz_authorizer, &ctx);
  }
  if (Size > 3 && (Data[3] & 2)) {
    sqlite3_progress_handler(db, ((Size > 4) ? ((int)(Data[4] % 32) + 1) : 1),
                             fuzz_progress, &ctx);
  }
  if (Size > 3 && (Data[3] & 4)) {
    sqlite3_commit_hook(db, fuzz_commit, &ctx);
  }
  if (Size > 3 && (Data[3] & 8)) {
    sqlite3_rollback_hook(db, fuzz_rollback, &ctx);
  }
  if (Size > 3 && (Data[3] & 16)) {
    sqlite3_trace(db, fuzz_trace, &ctx);
  }

  sql_len = Size > 5 ? Size - 5 : 0;
  if (sql_len > 0) {
    size_t max_len = sql_len > 2048 ? 2048 : sql_len;
    sql = (char *)sqlite3_malloc((int)max_len + 1);
    if (sql) {
      memcpy(sql, Data + 5, max_len);
      sql[max_len] = '\0';

      rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
      if (rc == SQLITE_OK && stmt) {
        if (Size > 0 && (Data[0] & 0x10)) {
          sqlite3_interrupt(db);
        }
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
          if (Size > 0 && (Data[0] & 0x20)) {
            sqlite3_interrupt(db);
          }
        }
      }
      if (stmt) {
        sqlite3_finalize(stmt);
        stmt = NULL;
      }

      exec_sql(db, sql);
    }
  } else {
    rc = sqlite3_prepare_v2(db, "SELECT * FROM t, u;", -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      if (Size > 0 && (Data[0] & 0x10)) {
        sqlite3_interrupt(db);
      }
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (Size > 0 && (Data[0] & 0x20)) {
          sqlite3_interrupt(db);
        }
      }
    }
    if (stmt) {
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  sqlite3_interrupt(db);

  sqlite3_set_authorizer(db, NULL, NULL);
  sqlite3_progress_handler(db, 0, NULL, NULL);
  sqlite3_commit_hook(db, NULL, NULL);
  sqlite3_rollback_hook(db, NULL, NULL);
  sqlite3_trace(db, NULL, NULL);

  if (sql) sqlite3_free(sql);
  sqlite3_close(db);
  return 0;
}