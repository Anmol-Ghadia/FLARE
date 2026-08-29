// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
// sqlite3_set_last_insert_rowid at sqlite3.c:153320:17 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
// sqlite3_blob_open at sqlite3.c:82675:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_blob_bytes at sqlite3.c:83019:16 in sqlite3.h
// sqlite3_blob_write at sqlite3.c:83009:16 in sqlite3.h
// sqlite3_blob_read at sqlite3.c:83002:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_blob_open at sqlite3.c:82675:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_blob_open at sqlite3.c:82675:16 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
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
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; i++) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static sqlite3_int64 read_i64(const uint8_t **p, size_t *n) {
  sqlite3_int64 v = 0;
  size_t take = *n < 8 ? *n : 8;
  for (size_t i = 0; i < take; i++) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static size_t copy_fuzz_string(const uint8_t **p, size_t *n, char *out, size_t out_sz) {
  if (out_sz == 0) return 0;
  if (*n == 0) {
    out[0] = '\0';
    return 0;
  }
  size_t len = (*p)[0] % out_sz;
  *p += 1;
  *n -= 1;
  if (len > *n) len = *n;
  memcpy(out, *p, len);
  out[len] = '\0';
  *p += len;
  *n -= len;
  return len;
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  (void)sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  if (sqlite3_open("./dummy_file", &db) != SQLITE_OK) {
    if (db) {
      (void)sqlite3_errmsg(db);
      (void)sqlite3_errcode(db);
      (void)sqlite3_extended_errcode(db);
      sqlite3_close(db);
    } else {
      (void)sqlite3_errmsg(NULL);
      (void)sqlite3_errcode(NULL);
      (void)sqlite3_extended_errcode(NULL);
    }
    return 0;
  }

  exec_sql(db, "PRAGMA foreign_keys=ON;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t1(id INTEGER PRIMARY KEY, b BLOB, txt TEXT, u UNIQUE);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(id INTEGER PRIMARY KEY, c BLOB);");
  exec_sql(db, "DELETE FROM t1;");
  exec_sql(db, "DELETE FROM t2;");

  {
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(db, "INSERT INTO t1(b, txt, u) VALUES(?, ?, ?);", -1, &stmt, NULL) == SQLITE_OK && stmt) {
      int blob_len = (int)(n > 64 ? 64 : n);
      sqlite3_bind_blob(stmt, 1, p, blob_len, SQLITE_STATIC);
      sqlite3_bind_text(stmt, 2, (const char *)p, blob_len, SQLITE_STATIC);
      sqlite3_bind_int(stmt, 3, (int)read_u32(&p, &n));
      (void)sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
  }

  {
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(db, "INSERT INTO t2(c) VALUES(zeroblob(?));", -1, &stmt, NULL) == SQLITE_OK && stmt) {
      sqlite3_bind_int(stmt, 1, (int)(read_u32(&p, &n) % 128));
      (void)sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
  }

  (void)sqlite3_last_insert_rowid(db);
  sqlite3_set_last_insert_rowid(db, read_i64(&p, &n));
  (void)sqlite3_last_insert_rowid(db);

  for (int i = 0; i < 4; i++) {
    char zDb[16], zTable[32], zColumn[32];
    sqlite3_blob *blob = NULL;
    sqlite3_int64 rowid;
    int flags;
    int rc;

    copy_fuzz_string(&p, &n, zDb, sizeof(zDb) - 1);
    copy_fuzz_string(&p, &n, zTable, sizeof(zTable) - 1);
    copy_fuzz_string(&p, &n, zColumn, sizeof(zColumn) - 1);
    rowid = read_i64(&p, &n);
    flags = (int)(read_u32(&p, &n) & 1);

    if (zDb[0] == '\0') strcpy(zDb, (i & 1) ? "main" : "temp");
    if (zTable[0] == '\0') strcpy(zTable, (i % 3 == 0) ? "t1" : "t2");
    if (zColumn[0] == '\0') strcpy(zColumn, (i % 2 == 0) ? "b" : "c");

    rc = sqlite3_blob_open(db, zDb, zTable, zColumn, rowid, flags, &blob);
    if (rc != SQLITE_OK) {
      (void)sqlite3_errmsg(db);
      (void)sqlite3_errcode(db);
      (void)sqlite3_extended_errcode(db);
    } else if (blob) {
      int bytes = sqlite3_blob_bytes(blob);
      if (bytes > 0 && n > 0) {
        int amt = bytes < (int)n ? bytes : (int)n;
        if (flags) {
          (void)sqlite3_blob_write(blob, p, amt, 0);
        } else {
          unsigned char tmp[64];
          int read_amt = amt < (int)sizeof(tmp) ? amt : (int)sizeof(tmp);
          (void)sqlite3_blob_read(blob, tmp, read_amt, 0);
        }
      }
    }
    if (blob) sqlite3_blob_close(blob);
  }

  {
    sqlite3_blob *blob = NULL;
    int rc = sqlite3_blob_open(db, "main", "t1", "b", 1, 0, &blob);
    if (rc != SQLITE_OK) {
      (void)sqlite3_errmsg(db);
      (void)sqlite3_errcode(db);
      (void)sqlite3_extended_errcode(db);
    }
    if (blob) sqlite3_blob_close(blob);
  }

  {
    sqlite3_blob *blob = NULL;
    int rc = sqlite3_blob_open(db, "main", "t2", "c", sqlite3_last_insert_rowid(db), 1, &blob);
    if (rc != SQLITE_OK) {
      (void)sqlite3_errmsg(db);
      (void)sqlite3_errcode(db);
      (void)sqlite3_extended_errcode(db);
    }
    if (blob) sqlite3_blob_close(blob);
  }

  (void)sqlite3_errmsg(db);
  (void)sqlite3_errcode(db);
  (void)sqlite3_extended_errcode(db);
  (void)sqlite3_last_insert_rowid(db);

  sqlite3_close(db);
  return 0;
}