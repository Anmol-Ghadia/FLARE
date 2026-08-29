// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_result_text64 at sqlite3.c:72383:17 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_result_text16be at sqlite3.c:72409:17 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_result_text16le at sqlite3.c:72418:17 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_result_text16 at sqlite3.c:72400:17 in sqlite3.h
// sqlite3_result_error_toobig at sqlite3.c:72457:17 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_result_blob64 at sqlite3.c:72314:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static void noop_destructor(void *p) {
  (void)p;
}

static void free_destructor(void *p) {
  free(p);
}

static uint32_t read_u32(const uint8_t **data, size_t *size) {
  uint32_t v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; i++) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static uint64_t read_u64(const uint8_t **data, size_t *size) {
  uint64_t v = 0;
  size_t n = *size < 8 ? *size : 8;
  for (size_t i = 0; i < n; i++) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static int read_i32(const uint8_t **data, size_t *size) {
  return (int)read_u32(data, size);
}

static unsigned char pick_encoding(uint8_t b) {
  switch (b % 5) {
    case 0: return SQLITE_UTF8;
    case 1: return SQLITE_UTF16;
    case 2: return SQLITE_UTF16BE;
    case 3: return SQLITE_UTF16LE;
    default: return 0xff;
  }
}

static int clamp_nonneg_int(int v, int maxv) {
  if (v < 0) return 0;
  if (v > maxv) return maxv;
  return v;
}

static int evenize_up_with_cap(int v, int cap_even) {
  if (v < 0) return -1;
  if (v & 1) v++;
  if (v > cap_even) v = cap_even;
  return v;
}

static void *dup_bytes_with_pad(const void *src, int n, int pad, int zero_pad) {
  size_t total;
  void *buf;
  if (n < 0 || pad < 0) return NULL;
  total = (size_t)n + (size_t)pad;
  buf = malloc(total ? total : 1);
  if (!buf) return NULL;
  if (n > 0 && src) memcpy(buf, src, (size_t)n);
  if (zero_pad && pad > 0) memset((unsigned char *)buf + n, 0, (size_t)pad);
  return buf;
}

static void udf_result_text64(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  const unsigned char *txt = sqlite3_value_text(argv[0]);
  int n = sqlite3_value_bytes(argv[0]);
  sqlite3_uint64 len = (sqlite3_uint64)sqlite3_value_int64(argv[1]);
  unsigned char enc = (unsigned char)(sqlite3_value_int(argv[2]) & 0xff);
  int dsel = sqlite3_value_int(argv[3]) & 3;

  char *buf = NULL;
  const char *p = (const char *)txt;
  void (*xDel)(void*) = SQLITE_TRANSIENT;

  if (n < 0) n = 0;
  if (dsel == 0) {
    xDel = SQLITE_TRANSIENT;
  } else if (dsel == 1) {
    xDel = SQLITE_STATIC;
  } else if (dsel == 2) {
    buf = (char *)dup_bytes_with_pad(p, n, 1, 1);
    if (buf) {
      p = buf;
      xDel = free_destructor;
    } else {
      xDel = SQLITE_TRANSIENT;
    }
  } else {
    xDel = noop_destructor;
  }

  sqlite3_result_text64(ctx, p, len, xDel, enc);
}

static void udf_result_text16be(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  const void *blob = sqlite3_value_blob(argv[0]);
  int blob_n = sqlite3_value_bytes(argv[0]);
  int n_in = sqlite3_value_int(argv[1]);
  int dsel = sqlite3_value_int(argv[2]) & 3;
  int safe_blob_n, safe_n, cap_even;
  void *buf = NULL;
  const void *p = blob;
  void (*xDel)(void*) = SQLITE_TRANSIENT;

  safe_blob_n = clamp_nonneg_int(blob_n, 4096);
  cap_even = safe_blob_n;
  if (cap_even & 1) cap_even--;
  safe_n = evenize_up_with_cap(n_in, cap_even);

  if (dsel == 0) {
    xDel = SQLITE_TRANSIENT;
  } else if (dsel == 1) {
    xDel = SQLITE_STATIC;
  } else if (dsel == 2) {
    int copy_n = (safe_n >= 0) ? safe_n : cap_even;
    buf = dup_bytes_with_pad(blob, copy_n, 2, 1);
    if (buf) {
      p = buf;
      xDel = free_destructor;
    } else {
      xDel = SQLITE_TRANSIENT;
    }
  } else {
    xDel = noop_destructor;
  }

  sqlite3_result_text16be(ctx, p, safe_n, xDel);
}

static void udf_result_text16le(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  const void *blob = sqlite3_value_blob(argv[0]);
  int blob_n = sqlite3_value_bytes(argv[0]);
  int n_in = sqlite3_value_int(argv[1]);
  int dsel = sqlite3_value_int(argv[2]) & 3;
  int safe_blob_n, safe_n, cap_even;
  void *buf = NULL;
  const void *p = blob;
  void (*xDel)(void*) = SQLITE_TRANSIENT;

  safe_blob_n = clamp_nonneg_int(blob_n, 4096);
  cap_even = safe_blob_n;
  if (cap_even & 1) cap_even--;
  safe_n = evenize_up_with_cap(n_in, cap_even);

  if (dsel == 0) {
    xDel = SQLITE_TRANSIENT;
  } else if (dsel == 1) {
    xDel = SQLITE_STATIC;
  } else if (dsel == 2) {
    int copy_n = (safe_n >= 0) ? safe_n : cap_even;
    buf = dup_bytes_with_pad(blob, copy_n, 2, 1);
    if (buf) {
      p = buf;
      xDel = free_destructor;
    } else {
      xDel = SQLITE_TRANSIENT;
    }
  } else {
    xDel = noop_destructor;
  }

  sqlite3_result_text16le(ctx, p, safe_n, xDel);
}

static void udf_result_text16(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  const void *blob = sqlite3_value_blob(argv[0]);
  int blob_n = sqlite3_value_bytes(argv[0]);
  int n_in = sqlite3_value_int(argv[1]);
  int dsel = sqlite3_value_int(argv[2]) & 3;
  int safe_blob_n, safe_n, cap_even;
  void *buf = NULL;
  const void *p = blob;
  void (*xDel)(void*) = SQLITE_TRANSIENT;

  safe_blob_n = clamp_nonneg_int(blob_n, 4096);
  cap_even = safe_blob_n;
  if (cap_even & 1) cap_even--;
  safe_n = evenize_up_with_cap(n_in, cap_even);

  if (dsel == 0) {
    xDel = SQLITE_TRANSIENT;
  } else if (dsel == 1) {
    xDel = SQLITE_STATIC;
  } else if (dsel == 2) {
    int copy_n = (safe_n >= 0) ? safe_n : cap_even;
    buf = dup_bytes_with_pad(blob, copy_n, 2, 1);
    if (buf) {
      p = buf;
      xDel = free_destructor;
    } else {
      xDel = SQLITE_TRANSIENT;
    }
  } else {
    xDel = noop_destructor;
  }

  sqlite3_result_text16(ctx, p, safe_n, xDel);
}

static void udf_result_error_toobig(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  (void)argv;
  sqlite3_result_error_toobig(ctx);
}

static void udf_result_blob64(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  const void *blob = sqlite3_value_blob(argv[0]);
  int blob_n = sqlite3_value_bytes(argv[0]);
  sqlite3_uint64 len = (sqlite3_uint64)sqlite3_value_int64(argv[1]);
  int dsel = sqlite3_value_int(argv[2]) & 3;

  void *buf = NULL;
  const void *p = blob;
  void (*xDel)(void*) = SQLITE_TRANSIENT;

  if (blob_n < 0) blob_n = 0;
  if (blob_n > 4096) blob_n = 4096;

  if (dsel == 0) {
    xDel = SQLITE_TRANSIENT;
  } else if (dsel == 1) {
    xDel = SQLITE_STATIC;
  } else if (dsel == 2) {
    buf = dup_bytes_with_pad(blob, blob_n, 0, 0);
    if (buf) {
      p = buf;
      xDel = free_destructor;
    } else {
      xDel = SQLITE_TRANSIENT;
    }
  } else {
    xDel = noop_destructor;
  }

  sqlite3_result_blob64(ctx, p, len, xDel);
}

int LLVMFuzzerTestOneInput_96(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "f_text64", 4, SQLITE_UTF8, NULL, udf_result_text64, NULL, NULL);
  sqlite3_create_function(db, "f_text16be", 3, SQLITE_UTF8, NULL, udf_result_text16be, NULL, NULL);
  sqlite3_create_function(db, "f_text16le", 3, SQLITE_UTF8, NULL, udf_result_text16le, NULL, NULL);
  sqlite3_create_function(db, "f_text16", 3, SQLITE_UTF8, NULL, udf_result_text16, NULL, NULL);
  sqlite3_create_function(db, "f_toobig", 0, SQLITE_UTF8, NULL, udf_result_error_toobig, NULL, NULL);
  sqlite3_create_function(db, "f_blob64", 3, SQLITE_UTF8, NULL, udf_result_blob64, NULL, NULL);

  sqlite3_exec(db, "CREATE TABLE t(x);", NULL, NULL, NULL);
  sqlite3_exec(db, "INSERT INTO t VALUES(readfile('./dummy_file'));", NULL, NULL, NULL);

  {
    const uint8_t *p = Data;
    size_t remaining = Size;
    int mode = remaining ? (*p++ % 6) : 0;
    if (remaining) remaining--;

    if (mode == 0) {
      const char *sql = "SELECT f_text64(?1, ?2, ?3, ?4);";
      rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
      if (rc == SQLITE_OK && stmt) {
        int text_len = (int)(remaining > 1024 ? 1024 : remaining);
        sqlite3_bind_text(stmt, 1, (const char *)p, text_len, SQLITE_TRANSIENT);

        {
          sqlite3_int64 lenv;
          if (remaining >= 8) {
            lenv = (sqlite3_int64)read_u64(&p, &remaining);
          } else {
            lenv = (sqlite3_int64)text_len;
          }
          sqlite3_bind_int64(stmt, 2, lenv);
        }

        {
          unsigned char enc = remaining ? pick_encoding(*p++) : SQLITE_UTF8;
          if (remaining) remaining--;
          sqlite3_bind_int(stmt, 3, (int)enc);
        }

        {
          int dsel = remaining ? (*p++ & 3) : 0;
          if (remaining) remaining--;
          sqlite3_bind_int(stmt, 4, dsel);
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        }
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    } else if (mode == 1) {
      const char *sql = "SELECT f_text16be(?1, ?2, ?3);";
      rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
      if (rc == SQLITE_OK && stmt) {
        int blob_len = (int)(remaining > 1024 ? 1024 : remaining);
        sqlite3_bind_blob(stmt, 1, p, blob_len, SQLITE_TRANSIENT);

        {
          int n = remaining >= 4 ? read_i32(&p, &remaining) : -1;
          sqlite3_bind_int(stmt, 2, n);
        }

        {
          int dsel = remaining ? (*p++ & 3) : 0;
          if (remaining) remaining--;
          sqlite3_bind_int(stmt, 3, dsel);
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        }
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    } else if (mode == 2) {
      const char *sql = "SELECT f_text16le(?1, ?2, ?3);";
      rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
      if (rc == SQLITE_OK && stmt) {
        int blob_len = (int)(remaining > 1024 ? 1024 : remaining);
        sqlite3_bind_blob(stmt, 1, p, blob_len, SQLITE_TRANSIENT);

        {
          int n = remaining >= 4 ? read_i32(&p, &remaining) : -1;
          sqlite3_bind_int(stmt, 2, n);
        }

        {
          int dsel = remaining ? (*p++ & 3) : 0;
          if (remaining) remaining--;
          sqlite3_bind_int(stmt, 3, dsel);
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        }
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    } else if (mode == 3) {
      const char *sql = "SELECT f_text16(?1, ?2, ?3);";
      rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
      if (rc == SQLITE_OK && stmt) {
        int blob_len = (int)(remaining > 1024 ? 1024 : remaining);
        sqlite3_bind_blob(stmt, 1, p, blob_len, SQLITE_TRANSIENT);

        {
          int n = remaining >= 4 ? read_i32(&p, &remaining) : -1;
          sqlite3_bind_int(stmt, 2, n);
        }

        {
          int dsel = remaining ? (*p++ & 3) : 0;
          if (remaining) remaining--;
          sqlite3_bind_int(stmt, 3, dsel);
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        }
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    } else if (mode == 4) {
      const char *sql = "SELECT f_toobig();";
      rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
      if (rc == SQLITE_OK && stmt) {
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        }
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    } else {
      const char *sql = "SELECT f_blob64(?1, ?2, ?3);";
      rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
      if (rc == SQLITE_OK && stmt) {
        int blob_len = (int)(remaining > 1024 ? 1024 : remaining);
        sqlite3_bind_blob(stmt, 1, p, blob_len, SQLITE_TRANSIENT);

        {
          sqlite3_int64 lenv;
          if (remaining >= 8) {
            lenv = (sqlite3_int64)read_u64(&p, &remaining);
          } else {
            lenv = (sqlite3_int64)blob_len;
          }
          sqlite3_bind_int64(stmt, 2, lenv);
        }

        {
          int dsel = remaining ? (*p++ & 3) : 0;
          if (remaining) remaining--;
          sqlite3_bind_int(stmt, 3, dsel);
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        }
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  {
    const char *sqls[] = {
      "SELECT f_text64(x, 0, 1, 0) FROM t;",
      "SELECT f_text64(x, 2147483648, 2, 2) FROM t;",
      "SELECT f_text16be(x, 0, 2) FROM t;",
      "SELECT f_text16le(x, 8, 2) FROM t;",
      "SELECT f_text16(x, 2, 2) FROM t;",
      "SELECT f_blob64(x, 2147483648, 2) FROM t;",
      "SELECT f_toobig() FROM t;"
    };
    size_t count = sizeof(sqls) / sizeof(sqls[0]);
    size_t idx = Size ? (Data[0] % count) : 0;
    rc = sqlite3_prepare_v2(db, sqls[idx], -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  sqlite3_exec(db, "DROP TABLE IF EXISTS t;", NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
  sqlite3_close(db);
  return 0;
}