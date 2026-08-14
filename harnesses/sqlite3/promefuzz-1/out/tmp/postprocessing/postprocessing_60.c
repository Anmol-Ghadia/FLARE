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

static int g_busy_calls = 0;
static int g_progress_calls = 0;
static int g_commit_calls = 0;

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; ++i) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int consume_int(const uint8_t **p, size_t *n, int defv) {
  if (*n == 0) return defv;
  return (int)read_u32(p, n);
}

static int consume_bool(const uint8_t **p, size_t *n) {
  if (*n == 0) return 0;
  int v = (**p) & 1;
  (*p)++;
  (*n)--;
  return v;
}

static size_t consume_bytes(const uint8_t **p, size_t *n, char *out, size_t outsz) {
  if (outsz == 0) return 0;
  size_t want = (*n == 0) ? 0 : ((size_t)(**p) % outsz);
  if (*n) {
    (*p)++;
    (*n)--;
  }
  if (want > *n) want = *n;
  memcpy(out, *p, want);
  out[want] = '\0';
  *p += want;
  *n -= want;
  return want;
}

static int busy_cb(void *pArg, int count) {
  const uint8_t *b = (const uint8_t *)pArg;
  g_busy_calls++;
  if (b == NULL) return (count & 1) == 0;
  return ((b[0] + count + g_busy_calls) & 1);
}

static int progress_cb(void *pArg) {
  const uint8_t *b = (const uint8_t *)pArg;
  g_progress_calls++;
  if (b == NULL) return 0;
  return ((b[0] + g_progress_calls) % 7) == 0;
}

static int commit_cb(void *pArg) {
  const uint8_t *b = (const uint8_t *)pArg;
  g_commit_calls++;
  if (b == NULL) return 0;
  return ((b[0] + g_commit_calls) % 5) == 0;
}

static void udf_dbhandle(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  (void)argv;
  sqlite3 *db = sqlite3_context_db_handle(ctx);
  if (db) {
    const void *msg16 = sqlite3_errmsg16(db);
    if (msg16) {
      const unsigned char *u = (const unsigned char *)msg16;
      int val = (int)u[0];
      sqlite3_result_int(ctx, val);
      return;
    }
  }
  sqlite3_result_null(ctx);
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
  (void)sqlite3_errmsg16(db);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;
  sqlite3 *db1 = NULL;
  sqlite3 *db2 = NULL;
  sqlite3_stmt *stmt = NULL;
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  int flags_pool[] = {
    SQLITE_OPEN_READONLY,
    SQLITE_OPEN_READWRITE,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_PRIVATECACHE,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_SHAREDCACHE
  };
  int idx1 = consume_int(&p, &n, 2);
  int idx2 = consume_int(&p, &n, 3);
  idx1 = idx1 < 0 ? -idx1 : idx1;
  idx2 = idx2 < 0 ? -idx2 : idx2;
  idx1 %= (int)(sizeof(flags_pool) / sizeof(flags_pool[0]));
  idx2 %= (int)(sizeof(flags_pool) / sizeof(flags_pool[0]));

  char vfs_name[32];
  consume_bytes(&p, &n, vfs_name, sizeof(vfs_name) - 1);
  const char *zVfs = consume_bool(&p, &n) ? vfs_name : NULL;

  sqlite3_open_v2("./dummy_file", &db1, flags_pool[idx1], zVfs);
  sqlite3_open_v2("./dummy_file", &db2, flags_pool[idx2], zVfs);

  if (db1) {
    sqlite3_busy_handler(db1, busy_cb, (void *)Data);
    sqlite3_progress_handler(db1, (consume_int(&p, &n, 1) % 32) - 8, progress_cb, (void *)Data);
    sqlite3_commit_hook(db1, commit_cb, (void *)Data);
    sqlite3_create_function(db1, "fuzz_dbh", 0, SQLITE_UTF8, NULL, udf_dbhandle, NULL, NULL);

    sqlite3_busy_handler(db1, consume_bool(&p, &n) ? NULL : busy_cb, (void *)p);
    sqlite3_progress_handler(db1, consume_int(&p, &n, 10) % 64, consume_bool(&p, &n) ? progress_cb : NULL, (void *)p);
    sqlite3_commit_hook(db1, consume_bool(&p, &n) ? commit_cb : NULL, (void *)p);

    exec_sql(db1, "PRAGMA journal_mode=WAL;");
    exec_sql(db1, "PRAGMA busy_timeout=1;");
    exec_sql(db1, "CREATE TABLE IF NOT EXISTS t(a,b);");
    exec_sql(db1, "INSERT INTO t(a,b) VALUES(1,2);");
    exec_sql(db1, "SELECT fuzz_dbh();");
    exec_sql(db1, "BEGIN;");
    exec_sql(db1, "INSERT INTO t(a,b) VALUES(3,4);");
    exec_sql(db1, "COMMIT;");
    exec_sql(db1, "BEGIN IMMEDIATE;");
  }

  if (db2) {
    sqlite3_busy_handler(db2, consume_bool(&p, &n) ? busy_cb : NULL, (void *)Data);
    sqlite3_progress_handler(db2, (consume_int(&p, &n, 5) % 16) + 1, progress_cb, (void *)Data);
    sqlite3_commit_hook(db2, consume_bool(&p, &n) ? commit_cb : NULL, (void *)Data);

    exec_sql(db2, "CREATE TABLE IF NOT EXISTS t(a,b);");
    exec_sql(db2, "BEGIN EXCLUSIVE;");
    exec_sql(db2, "INSERT INTO t(a,b) VALUES(5,6);");
  }

  if (db1) {
    char sqlbuf[256];
    size_t slen = consume_bytes(&p, &n, sqlbuf, sizeof(sqlbuf) - 1);
    if (slen == 0) {
      strcpy(sqlbuf, "SELECT * FROM t WHERE a=1;");
    }
    if (sqlite3_prepare_v2(db1, sqlbuf, -1, &stmt, NULL) == SQLITE_OK) {
      while (sqlite3_step(stmt) == SQLITE_ROW) {
      }
    } else {
      (void)sqlite3_errmsg16(db1);
    }
  }

  if (stmt) sqlite3_finalize(stmt);
  if (db2) {
    exec_sql(db2, "ROLLBACK;");
    sqlite3_close(db2);
  }
  if (db1) {
    exec_sql(db1, "ROLLBACK;");
    sqlite3_close(db1);
  }
  return 0;
}