// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_blob_write at sqlite3.c:83009:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_blob_read at sqlite3.c:83002:16 in sqlite3.h
// sqlite3_blob_read at sqlite3.c:83002:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_blob_open at sqlite3.c:82675:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"

static int get_u32(const uint8_t *data, size_t size, size_t *off, uint32_t *out) {
  if (*off + 4 > size) return 0;
  *out = ((uint32_t)data[*off]) |
         ((uint32_t)data[*off + 1] << 8) |
         ((uint32_t)data[*off + 2] << 16) |
         ((uint32_t)data[*off + 3] << 24);
  *off += 4;
  return 1;
}

static int get_i32(const uint8_t *data, size_t size, size_t *off, int *out) {
  uint32_t v = 0;
  if (!get_u32(data, size, off, &v)) return 0;
  *out = (int)v;
  return 1;
}

static size_t take_bytes(const uint8_t *data, size_t size, size_t *off, const uint8_t **ptr, size_t max_take) {
  uint32_t len = 0;
  if (!get_u32(data, size, off, &len)) {
    *ptr = NULL;
    return 0;
  }
  size_t remain = size - *off;
  size_t n = (size_t)len;
  if (n > remain) n = remain;
  if (n > max_take) n = max_take;
  *ptr = data + *off;
  *off += n;
  return n;
}

static char *make_cstring(const uint8_t *data, size_t n) {
  char *s = (char *)malloc(n + 1);
  if (!s) return NULL;
  if (n && data) memcpy(s, data, n);
  s[n] = '\0';
  return s;
}

static void fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int sum = argc;
  for (int i = 0; i < argc; ++i) {
    sum += sqlite3_value_bytes(argv[i]);
  }
  sqlite3_result_int(ctx, sum);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;
  sqlite3 *db = NULL;
  sqlite3_blob *blob = NULL;
  sqlite3_stmt *stmt = NULL;
  const char *tail = NULL;
  FILE *fp = NULL;
  int rc;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db, "PRAGMA journal_mode=OFF;", NULL, NULL, NULL);
  sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);
  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(id INTEGER PRIMARY KEY, b BLOB);", NULL, NULL, NULL);

  {
    const uint8_t *blob_src = NULL;
    size_t blob_len = take_bytes(Data, Size, &off, &blob_src, 4096);
    sqlite3_stmt *ins = NULL;
    if (sqlite3_prepare_v2(db, "INSERT INTO t(b) VALUES(zeroblob(?1));", -1, &ins, NULL) == SQLITE_OK && ins) {
      int zlen = blob_len > 0 ? (int)blob_len : 1;
      sqlite3_bind_int(ins, 1, zlen);
      sqlite3_step(ins);
    }
    sqlite3_finalize(ins);

    if (sqlite3_blob_open(db, "main", "t", "b", 1, 1, &blob) != SQLITE_OK) {
      blob = NULL;
    }

    if (blob) {
      int n = 0, iOffset = 0;
      static const uint8_t zero = 0;
      const uint8_t *write_buf = blob_len ? blob_src : &zero;
      size_t write_buf_len = blob_len ? blob_len : 1;

      if (!get_i32(Data, Size, &off, &n)) n = (int)write_buf_len;
      if (!get_i32(Data, Size, &off, &iOffset)) iOffset = 0;

      if (n < 0) {
        n = -(n % 32);
      } else {
        n = n % ((int)write_buf_len + 8);
      }

      (void)sqlite3_blob_write(blob, write_buf, n, iOffset);
      (void)sqlite3_errmsg(db);

      {
        int rn = 0, rOffset = 0;
        if (!get_i32(Data, Size, &off, &rn)) rn = n;
        if (!get_i32(Data, Size, &off, &rOffset)) rOffset = iOffset;
        if (rn < 0) {
          rn = -(rn % 32);
        } else {
          rn = rn % ((int)write_buf_len + 8);
        }

        if (rn < 0) rn = 0;
        void *read_buf = malloc((size_t)rn ? (size_t)rn : 1);
        if (read_buf) {
          (void)sqlite3_blob_read(blob, read_buf, rn, rOffset);
          free(read_buf);
        } else {
          (void)sqlite3_blob_read(blob, NULL, 0, rOffset);
        }
      }
      (void)sqlite3_errmsg(db);
    } else {
      (void)sqlite3_errmsg(db);
      (void)sqlite3_errmsg(db);
    }
  }

  {
    const uint8_t *fname_data = NULL;
    size_t fname_len = take_bytes(Data, Size, &off, &fname_data, 255);
    char *fname = NULL;
    int nArg = -1;
    uint32_t enc_sel = 0;

    if (fname_len > 0 && fname_data) {
      fname = make_cstring(fname_data, fname_len);
    } else {
      static const uint8_t def_name[] = "f";
      fname = make_cstring(def_name, sizeof(def_name) - 1);
    }

    if (!get_i32(Data, Size, &off, &nArg)) nArg = -1;
    if (nArg < -1) nArg = -1;
    if (nArg > 127) nArg = 127;
    if (!get_u32(Data, Size, &off, &enc_sel)) enc_sel = 0;

    if (fname) {
      int enc = SQLITE_UTF8;
      switch (enc_sel % 4) {
        case 0: enc = SQLITE_UTF8; break;
        case 1: enc = SQLITE_UTF16LE; break;
        case 2: enc = SQLITE_UTF16BE; break;
        default: enc = SQLITE_UTF16; break;
      }
      if (enc_sel & 4) enc |= SQLITE_DETERMINISTIC;
#ifdef SQLITE_DIRECTONLY
      if (enc_sel & 8) enc |= SQLITE_DIRECTONLY;
#endif
      (void)sqlite3_create_function(db, fname, nArg, enc, NULL, fuzz_scalar, NULL, NULL);
      free(fname);
    }
  }

  {
    const uint8_t *sql_data = NULL;
    size_t sql_len = take_bytes(Data, Size, &off, &sql_data, 4096);
    char *sql = NULL;
    int nByte = -1;

    if (sql_len > 0 && sql_data) {
      sql = make_cstring(sql_data, sql_len);
    } else {
      static const uint8_t def_sql[] = "SELECT 1;";
      sql = make_cstring(def_sql, sizeof(def_sql) - 1);
    }

    if (!get_i32(Data, Size, &off, &nByte)) nByte = -1;

    if (sql) {
      if (nByte >= 0) {
        int maxn = (int)strlen(sql) + 1;
        if (nByte > maxn + 16) nByte = maxn + 16;
      }
      (void)sqlite3_prepare_v2(db, sql, nByte, &stmt, &tail);
      sqlite3_finalize(stmt);
      free(sql);
    }
  }

  if (blob) sqlite3_blob_close(blob);
  sqlite3_close(db);
  return 0;
}