// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_preupdate_depth at sqlite3.c:73800:16 in sqlite3.h
// sqlite3_preupdate_blobwrite at sqlite3.c:73811:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_preupdate_hook at sqlite3.c:154525:18 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_blob_open at sqlite3.c:82675:16 in sqlite3.h
// sqlite3_blob_bytes at sqlite3.c:83019:16 in sqlite3.h
// sqlite3_blob_write at sqlite3.c:83009:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_preupdate_hook at sqlite3.c:154525:18 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_value_double at sqlite3.c:72097:19 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_preupdate_count at sqlite3.c:73782:16 in sqlite3.h
// sqlite3_preupdate_depth at sqlite3.c:73800:16 in sqlite3.h
// sqlite3_preupdate_blobwrite at sqlite3.c:73811:16 in sqlite3.h
// sqlite3_preupdate_new at sqlite3.c:73822:16 in sqlite3.h
// sqlite3_preupdate_old at sqlite3.c:73719:16 in sqlite3.h
// sqlite3_preupdate_new at sqlite3.c:73822:16 in sqlite3.h
// sqlite3_preupdate_old at sqlite3.c:73719:16 in sqlite3.h
// sqlite3_preupdate_count at sqlite3.c:73782:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

typedef struct {
  const uint8_t *data;
  size_t size;
  size_t off;
} FuzzInput;

typedef struct {
  int max_cols;
  int extra_calls;
} HookCtx;

static uint8_t fi_u8(FuzzInput *in) {
  if (in->off >= in->size) return 0;
  return in->data[in->off++];
}

static uint32_t fi_u32(FuzzInput *in) {
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i) {
    v = (v << 8) | fi_u8(in);
  }
  return v;
}

static int fi_int_range(FuzzInput *in, int minv, int maxv) {
  if (maxv <= minv) return minv;
  return minv + (int)(fi_u32(in) % (uint32_t)(maxv - minv + 1));
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
}

static void touch_value(sqlite3_value *v) {
  if (!v) return;
  (void)sqlite3_value_type(v);
  (void)sqlite3_value_bytes(v);
  (void)sqlite3_value_int(v);
  (void)sqlite3_value_int64(v);
  (void)sqlite3_value_double(v);
  (void)sqlite3_value_text(v);
  (void)sqlite3_value_blob(v);
}

static void preupdate_cb(
  void *pCtx,
  sqlite3 *db,
  int op,
  char const *zDb,
  char const *zName,
  sqlite3_int64 iKey1,
  sqlite3_int64 iKey2
) {
  HookCtx *ctx = (HookCtx *)pCtx;
  int count = sqlite3_preupdate_count(db);
  int depth = sqlite3_preupdate_depth(db);
  int blobcol = sqlite3_preupdate_blobwrite(db);
  int limit = count;

  (void)zDb;
  (void)zName;
  (void)iKey1;
  (void)iKey2;
  (void)depth;
  (void)blobcol;

  if (ctx && ctx->max_cols >= 0 && ctx->max_cols < limit) {
    limit = ctx->max_cols;
  }

  for (int i = -2; i < limit + 2; ++i) {
    sqlite3_value *v = NULL;

    (void)sqlite3_preupdate_new(db, i, &v);
    if (op != SQLITE_DELETE) {
      touch_value(v);
    }

    v = NULL;
    (void)sqlite3_preupdate_old(db, i, &v);
    if (op == SQLITE_UPDATE || op == SQLITE_DELETE) {
      touch_value(v);
    }
  }

  if (ctx && ctx->extra_calls) {
    sqlite3_value *v = NULL;
    (void)sqlite3_preupdate_new(db, 0x7fffffff, &v);
    (void)sqlite3_preupdate_old(db, -0x7fffffff, &v);
    (void)sqlite3_preupdate_count(db);
    (void)sqlite3_preupdate_depth(db);
    (void)sqlite3_preupdate_blobwrite(db);
  }
}

int LLVMFuzzerTestOneInput_58(const uint8_t *Data, size_t Size) {
  FuzzInput in = { Data, Size, 0 };
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  HookCtx ctx;
  memset(&ctx, 0, sizeof(ctx));

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Data && Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  if (sqlite3_open_v2("./dummy_file",
                      &db,
                      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                      NULL) != SQLITE_OK || !db) {
    if (db) sqlite3_close(db);
    return 0;
  }

  exec_sql(db, "PRAGMA foreign_keys=ON;");
  exec_sql(db, "PRAGMA recursive_triggers=ON;");
  exec_sql(db, "PRAGMA journal_mode=OFF;");
  exec_sql(db, "DROP TABLE IF EXISTS t;");
  exec_sql(db, "DROP TABLE IF EXISTS log;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(a INTEGER PRIMARY KEY, b TEXT, c BLOB);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS log(x INTEGER);");
  exec_sql(db, "CREATE TRIGGER IF NOT EXISTS tr_ai AFTER INSERT ON t BEGIN INSERT INTO log VALUES (new.a); END;");
  exec_sql(db, "CREATE TRIGGER IF NOT EXISTS tr_au AFTER UPDATE ON t BEGIN INSERT INTO log VALUES (old.a + new.a); END;");
  exec_sql(db, "CREATE TRIGGER IF NOT EXISTS tr_ad AFTER DELETE ON t BEGIN INSERT INTO log VALUES (old.a); END;");

  ctx.max_cols = fi_int_range(&in, 0, 8);
  ctx.extra_calls = fi_u8(&in) & 1;

  (void)sqlite3_preupdate_hook(db, preupdate_cb, &ctx);

  exec_sql(db, "BEGIN;");
  exec_sql(db, "INSERT INTO t(a,b,c) VALUES(1,'x',x'010203');");
  exec_sql(db, "INSERT OR REPLACE INTO t(a,b,c) VALUES(2,'y',x'0405');");

  switch (fi_u8(&in) % 4) {
    case 0:
      exec_sql(db, "INSERT INTO t(a,b,c) VALUES(3,'z',x'06');");
      break;
    case 1:
      exec_sql(db, "UPDATE t SET b='updated', c=x'11223344' WHERE a=1;");
      break;
    case 2:
      exec_sql(db, "DELETE FROM t WHERE a=2;");
      break;
    default:
      exec_sql(db, "UPDATE t SET c=zeroblob(16) WHERE a=1;");
      if (sqlite3_prepare_v2(db, "SELECT c FROM t WHERE a=1;", -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          sqlite3_blob *blob = NULL;
          if (sqlite3_blob_open(db, "main", "t", "c", 1, 1, &blob) == SQLITE_OK && blob) {
            int n = sqlite3_blob_bytes(blob);
            if (n > 0) {
              unsigned char buf[32];
              int w = n < (int)sizeof(buf) ? n : (int)sizeof(buf);
              for (int i = 0; i < w; ++i) buf[i] = fi_u8(&in);
              (void)sqlite3_blob_write(blob, buf, w, 0);
            }
            sqlite3_blob_close(blob);
          }
        }
      }
      if (stmt) {
        sqlite3_finalize(stmt);
        stmt = NULL;
      }
      break;
  }

  if (fi_u8(&in) & 1) {
    exec_sql(db, "SAVEPOINT s1;");
    exec_sql(db, "UPDATE t SET b='nested' WHERE a=1;");
    exec_sql(db, "RELEASE s1;");
  }

  exec_sql(db, "COMMIT;");

  if (fi_u8(&in) & 1) {
    (void)sqlite3_preupdate_hook(db, NULL, NULL);
  }

  if (stmt) sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}