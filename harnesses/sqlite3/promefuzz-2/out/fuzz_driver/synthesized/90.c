// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_wal_hook at sqlite3.c:154595:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_wal_hook at sqlite3.c:154595:18 in sqlite3.h
// sqlite3_prepare at sqlite3.c:119055:16 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_wal_hook at sqlite3.c:154595:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

typedef struct {
  int progress_mode;
  int progress_limit;
  int progress_count;
  int auth_mode;
  int wal_mode;
} FuzzCtx;

static int fuzz_progress(void *pArg) {
  FuzzCtx *ctx = (FuzzCtx *)pArg;
  if (!ctx) return 0;
  ctx->progress_count++;
  if (ctx->progress_mode && ctx->progress_count > ctx->progress_limit) {
    return 1;
  }
  return 0;
}

static int fuzz_authorizer(
    void *pUserData,
    int action,
    const char *p3,
    const char *p4,
    const char *p5,
    const char *p6) {
  FuzzCtx *ctx = (FuzzCtx *)pUserData;
  (void)action;
  (void)p3;
  (void)p4;
  (void)p5;
  (void)p6;
  if (!ctx) return SQLITE_OK;

  switch (ctx->auth_mode % 4) {
    case 0:
      return SQLITE_OK;
    case 1:
      return SQLITE_IGNORE;
    case 2:
      return SQLITE_DENY;
    default:
      return 999;
  }
}

static int fuzz_wal_hook(void *pArg, sqlite3 *db, const char *zDb, int nPages) {
  FuzzCtx *ctx = (FuzzCtx *)pArg;
  (void)db;
  (void)zDb;
  (void)nPages;
  if (!ctx) return SQLITE_OK;

  switch (ctx->wal_mode % 3) {
    case 0:
      return SQLITE_OK;
    case 1:
      return SQLITE_BUSY;
    default:
      return SQLITE_ERROR;
  }
}

int LLVMFuzzerTestOneInput_90(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  sqlite3_stmt *iter = NULL;
  const char *tail = NULL;
  char *sql = NULL;
  int rc;
  FuzzCtx ctx;
  size_t sql_len;
  int nByteMode;
  int nByte;

  if (Data == NULL) return 0;

  memset(&ctx, 0, sizeof(ctx));

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
      if (Size > 0) fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  if (Size < 5) {
    sqlite3_next_stmt(db, NULL);
    sqlite3_progress_handler(db, 0, NULL, NULL);
    sqlite3_set_authorizer(db, NULL, NULL);
    sqlite3_wal_hook(db, NULL, NULL);
    sqlite3_close(db);
    return 0;
  }

  ctx.progress_mode = Data[0] & 1;
  ctx.progress_limit = (int)(Data[1] % 16);
  ctx.auth_mode = (int)(Data[2] % 4);
  ctx.wal_mode = (int)(Data[3] % 3);
  nByteMode = (int)(Data[4] % 4);

  sql_len = Size - 5;
  sql = (char *)malloc(sql_len + 1);
  if (!sql) {
    sqlite3_close(db);
    return 0;
  }
  if (sql_len > 0) memcpy(sql, Data + 5, sql_len);
  sql[sql_len] = '\0';

  sqlite3_exec(db, "PRAGMA journal_mode=WAL;", NULL, NULL, NULL);
  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(a,b);", NULL, NULL, NULL);
  sqlite3_exec(db, "INSERT INTO t(a,b) VALUES(1,'x'),(2,'y'),(3,'z');", NULL, NULL, NULL);

  sqlite3_progress_handler(db, (Data[0] % 32), fuzz_progress, &ctx);
  sqlite3_set_authorizer(db, fuzz_authorizer, &ctx);
  sqlite3_wal_hook(db, fuzz_wal_hook, &ctx);

  switch (nByteMode) {
    case 0:
      nByte = -1;
      break;
    case 1:
      nByte = 0;
      break;
    case 2:
      nByte = (int)sql_len;
      break;
    default:
      nByte = (int)sql_len + 1;
      break;
  }

  rc = sqlite3_prepare(db, sql, nByte, &stmt, &tail);

  if (stmt) {
    sqlite3 *db2 = sqlite3_db_handle(stmt);
    if (db2) {
      sqlite3_stmt *s = NULL;
      do {
        s = sqlite3_next_stmt(db2, s);
        if (s) {
          sqlite3_db_handle(s);
        }
      } while (s != NULL);
    }

    for (;;) {
      rc = sqlite3_step(stmt);
      if (rc != SQLITE_ROW) break;
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  iter = NULL;
  do {
    iter = sqlite3_next_stmt(db, iter);
    if (iter) {
      sqlite3_db_handle(iter);
    }
  } while (iter != NULL);

  sqlite3_progress_handler(db, 0, NULL, NULL);
  sqlite3_set_authorizer(db, NULL, NULL);
  sqlite3_wal_hook(db, NULL, NULL);

  free(sql);
  sqlite3_close(db);
  return 0;
}