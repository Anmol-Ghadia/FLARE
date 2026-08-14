// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_preupdate_old at sqlite3.c:73719:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_preupdate_new at sqlite3.c:73822:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_preupdate_hook at sqlite3.c:154525:18 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_blob_open at sqlite3.c:82675:16 in sqlite3.h
// sqlite3_blob_bytes at sqlite3.c:83019:16 in sqlite3.h
// sqlite3_blob_write at sqlite3.c:83009:16 in sqlite3.h
// sqlite3_blob_write at sqlite3.c:83009:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_preupdate_hook at sqlite3.c:154525:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_preupdate_count at sqlite3.c:73782:16 in sqlite3.h
// sqlite3_preupdate_depth at sqlite3.c:73800:16 in sqlite3.h
// sqlite3_preupdate_blobwrite at sqlite3.c:73811:16 in sqlite3.h
// sqlite3_preupdate_old at sqlite3.c:73719:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_preupdate_new at sqlite3.c:73822:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_preupdate_old at sqlite3.c:73719:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_preupdate_new at sqlite3.c:73822:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_preupdate_old at sqlite3.c:73719:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_preupdate_new at sqlite3.c:73822:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
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
} FuzzCursor;

typedef struct {
  sqlite3 *db;
  int mode;
} HookCtx;

static uint8_t take_u8(FuzzCursor *c) {
  if (c->off >= c->size) return 0;
  return c->data[c->off++];
}

static uint32_t take_u32(FuzzCursor *c) {
  uint32_t v = 0;
  for (int i = 0; i < 4; i++) {
    v = (v << 8) | take_u8(c);
  }
  return v;
}

static int bounded_index(FuzzCursor *c, int limit) {
  if (limit <= 0) return 0;
  return (int)(take_u32(c) % (uint32_t)limit);
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *err = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &err);
  sqlite3_free(err);
}

static void preupdate_cb(
    void *arg,
    sqlite3 *db,
    int op,
    const char *zDb,
    const char *zName,
    sqlite3_int64 iKey1,
    sqlite3_int64 iKey2) {
  (void)zDb;
  (void)zName;
  (void)iKey1;
  (void)iKey2;

  HookCtx *ctx = (HookCtx *)arg;
  int count = sqlite3_preupdate_count(db);
  int depth = sqlite3_preupdate_depth(db);
  int blobcol = sqlite3_preupdate_blobwrite(db);
  (void)depth;
  (void)blobcol;

  sqlite3_value *val = NULL;

  (void)sqlite3_preupdate_old(db, -1, &val);
  if (val) (void)sqlite3_value_nochange(val);
  val = NULL;

  (void)sqlite3_preupdate_new(db, -1, &val);
  if (val) (void)sqlite3_value_nochange(val);
  val = NULL;

  (void)sqlite3_preupdate_old(db, count, &val);
  if (val) (void)sqlite3_value_nochange(val);
  val = NULL;

  (void)sqlite3_preupdate_new(db, count, &val);
  if (val) (void)sqlite3_value_nochange(val);
  val = NULL;

  for (int i = 0; i < count; i++) {
    val = NULL;
    if (op == SQLITE_UPDATE || op == SQLITE_DELETE) {
      (void)sqlite3_preupdate_old(db, i, &val);
      if (val) {
        (void)sqlite3_value_nochange(val);
        (void)sqlite3_value_type(val);
        (void)sqlite3_value_bytes(val);
      }
    }
    val = NULL;
    if (op == SQLITE_UPDATE || op == SQLITE_INSERT) {
      (void)sqlite3_preupdate_new(db, i, &val);
      if (val) {
        (void)sqlite3_value_nochange(val);
        (void)sqlite3_value_type(val);
        (void)sqlite3_value_bytes(val);
      }
    }
  }

  if (ctx && ctx->mode) {
    val = NULL;
    if (op == SQLITE_UPDATE && count > 0) {
      (void)sqlite3_preupdate_old(db, 0, &val);
      if (val) {
        (void)sqlite3_value_nochange(val);
        (void)sqlite3_value_int64(val);
      }
      val = NULL;
      (void)sqlite3_preupdate_new(db, 0, &val);
      if (val) {
        (void)sqlite3_value_nochange(val);
        (void)sqlite3_value_text(val);
      }
    }
  }
}

int LLVMFuzzerTestOneInput_59(const uint8_t *Data, size_t Size) {
  FuzzCursor c = { Data, Size, 0 };
  sqlite3 *db = NULL;
  HookCtx ctx;
  sqlite3_stmt *stmt = NULL;
  int rc;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  if (sqlite3_open(":memory:", &db) != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  ctx.db = db;
  ctx.mode = take_u8(&c) & 1;

  sqlite3_preupdate_hook(db, preupdate_cb, &ctx);

  exec_sql(db, "PRAGMA foreign_keys=ON;");
  exec_sql(db, "PRAGMA recursive_triggers=ON;");
  exec_sql(db, "CREATE TABLE t1(a INTEGER PRIMARY KEY, b TEXT, c BLOB);");
  exec_sql(db, "CREATE TABLE log(x TEXT);");
  exec_sql(db,
           "CREATE TRIGGER tr1 AFTER UPDATE ON t1 "
           "BEGIN INSERT INTO log VALUES('u'); END;");
  exec_sql(db,
           "INSERT INTO t1(a,b,c) VALUES(1,'init',x'010203');"
           "INSERT INTO t1(a,b,c) VALUES(2,'more',x'0405');");

  switch (take_u8(&c) % 5) {
    case 0: {
      const char *sql = "INSERT INTO t1(a,b,c) VALUES(?1,?2,?3);";
      rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
      if (rc == SQLITE_OK && stmt) {
        sqlite3_bind_int(stmt, 1, (int)take_u32(&c));
        if (c.off < c.size) {
          sqlite3_bind_text(stmt, 2, (const char *)(c.data + c.off),
                            (int)(c.size - c.off), SQLITE_TRANSIENT);
          sqlite3_bind_blob(stmt, 3, c.data + c.off, (int)(c.size - c.off),
                            SQLITE_TRANSIENT);
        } else {
          sqlite3_bind_null(stmt, 2);
          sqlite3_bind_null(stmt, 3);
        }
        (void)sqlite3_step(stmt);
      }
      break;
    }
    case 1: {
      const char *sql = "UPDATE t1 SET b=?1, c=?2 WHERE a=?3;";
      rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
      if (rc == SQLITE_OK && stmt) {
        if (c.off < c.size) {
          sqlite3_bind_text(stmt, 1, (const char *)(c.data + c.off),
                            (int)(c.size - c.off), SQLITE_TRANSIENT);
          sqlite3_bind_blob(stmt, 2, c.data + c.off, (int)(c.size - c.off),
                            SQLITE_TRANSIENT);
        } else {
          sqlite3_bind_text(stmt, 1, "", 0, SQLITE_STATIC);
          sqlite3_bind_blob(stmt, 2, "", 0, SQLITE_STATIC);
        }
        sqlite3_bind_int(stmt, 3, bounded_index(&c, 4));
        (void)sqlite3_step(stmt);
      }
      break;
    }
    case 2: {
      const char *sql = "DELETE FROM t1 WHERE a=?1;";
      rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
      if (rc == SQLITE_OK && stmt) {
        sqlite3_bind_int(stmt, 1, bounded_index(&c, 4));
        (void)sqlite3_step(stmt);
      }
      break;
    }
    case 3: {
      exec_sql(db, "BEGIN;");
      rc = sqlite3_prepare_v2(db, "UPDATE t1 SET c=zeroblob(?1) WHERE a=1;",
                              -1, &stmt, NULL);
      if (rc == SQLITE_OK && stmt) {
        sqlite3_bind_int(stmt, 1, (int)(take_u8(&c) % 64));
        (void)sqlite3_step(stmt);
      }
      sqlite3_finalize(stmt);
      stmt = NULL;

      sqlite3_blob *blob = NULL;
      rc = sqlite3_blob_open(db, "main", "t1", "c", 1, 1, &blob);
      if (rc == SQLITE_OK && blob) {
        int n = sqlite3_blob_bytes(blob);
        if (n > 0) {
          int wr = (int)((c.size - c.off) > (size_t)n ? n : (int)(c.size - c.off));
          if (wr > 0) {
            (void)sqlite3_blob_write(blob, c.data + c.off, wr, 0);
          } else {
            uint8_t zero = 0;
            (void)sqlite3_blob_write(blob, &zero, 1, 0);
          }
        }
        sqlite3_blob_close(blob);
      }
      exec_sql(db, "COMMIT;");
      break;
    }
    default: {
      exec_sql(db,
               "UPDATE t1 SET b='x'||b, c=zeroblob(8) WHERE a=1;"
               "DELETE FROM t1 WHERE a=2;"
               "INSERT INTO t1(a,b,c) VALUES(3,'z',x'99');");
      break;
    }
  }

  sqlite3_finalize(stmt);
  sqlite3_preupdate_hook(db, NULL, NULL);
  sqlite3_close(db);
  return 0;
}