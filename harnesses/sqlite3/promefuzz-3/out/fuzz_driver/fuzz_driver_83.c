// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_bind_pointer at sqlite3.c:73369:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_text16le at sqlite3.c:72133:24 in sqlite3.h
// sqlite3_value_pointer at sqlite3.c:72110:18 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_text16le at sqlite3.c:72133:24 in sqlite3.h
// sqlite3_value_pointer at sqlite3.c:72110:18 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

static void fuzz_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int i;
  for (i = 0; i < argc; i++) {
    sqlite3_value *v = argv[i];
    sqlite3_value *dup = sqlite3_value_dup(v);

    if (dup) {
      (void)sqlite3_value_blob(dup);
      (void)sqlite3_value_bytes(dup);
      (void)sqlite3_value_text16le(dup);
      (void)sqlite3_value_pointer(dup, "fuzzptr");
      sqlite3_result_value(ctx, dup);
      sqlite3_value_free(dup);
    } else {
      (void)sqlite3_value_blob(v);
      (void)sqlite3_value_bytes(v);
      (void)sqlite3_value_text16le(v);
      (void)sqlite3_value_pointer(v, "fuzzptr");
      sqlite3_result_value(ctx, v);
    }
  }

  if (argc == 0) {
    sqlite3_result_null(ctx);
  }
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t *off) {
  uint32_t v = 0;
  size_t i;
  for (i = 0; i < 4 && *off < n; i++, (*off)++) {
    v = (v << 8) | p[*off];
  }
  return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  size_t off = 0;
  int rc;
  int mode;
  int nparam, i;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzzfunc", -1, SQLITE_UTF8, NULL, fuzz_func, NULL, NULL);

  sqlite3_exec(db,
               "CREATE TABLE t(a,b,c,d);"
               "INSERT INTO t VALUES(NULL, 123, 45.67, x'414243');"
               "INSERT INTO t VALUES('text', -1, 0.0, x'00ff10');"
               "INSERT INTO t VALUES('', 999999999, -3.14, x'');",
               NULL, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  mode = (Size > 0) ? (Data[off++] % 6) : 0;
  nparam = (Size > off) ? (int)(Data[off++] % 6) : 0;

  switch (mode) {
    case 0:
      rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(?1, ?2, ?3, ?4, ?5, ?6);", -1, &stmt, NULL);
      break;
    case 1:
      rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(a,b,c,d) FROM t;", -1, &stmt, NULL);
      break;
    case 2:
      rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(?1, a, ?2, d) FROM t;", -1, &stmt, NULL);
      break;
    case 3:
      rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(CAST(?1 AS TEXT), CAST(?2 AS BLOB), CAST(?3 AS REAL));", -1, &stmt, NULL);
      break;
    case 4:
      rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(zeroblob(?1), ?2, ?3);", -1, &stmt, NULL);
      break;
    default:
      rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(readfile('./dummy_file'));", -1, &stmt, NULL);
      if (rc != SQLITE_OK) {
        rc = sqlite3_prepare_v2(db, "SELECT fuzzfunc(?1);", -1, &stmt, NULL);
      }
      break;
  }

  if (rc == SQLITE_OK && stmt) {
    int bind_count = sqlite3_bind_parameter_count(stmt);
    for (i = 1; i <= bind_count; i++) {
      int t = (Size > off) ? (Data[off++] % 7) : 0;
      switch (t) {
        case 0:
          sqlite3_bind_null(stmt, i);
          break;
        case 1: {
          sqlite3_int64 v = (sqlite3_int64)rd32(Data, Size, &off);
          sqlite3_bind_int64(stmt, i, v);
          break;
        }
        case 2: {
          double d = 0.0;
          uint64_t bits = 0;
          size_t j;
          for (j = 0; j < 8 && off < Size; j++, off++) {
            bits = (bits << 8) | Data[off];
          }
          memcpy(&d, &bits, sizeof(d) < sizeof(bits) ? sizeof(d) : sizeof(bits));
          sqlite3_bind_double(stmt, i, d);
          break;
        }
        case 3: {
          int len = (Size > off) ? (int)(Data[off++] % 32) : 0;
          if ((size_t)len > Size - off) len = (int)(Size - off);
          sqlite3_bind_text(stmt, i, (const char *)(Data + off), len, SQLITE_TRANSIENT);
          off += len;
          break;
        }
        case 4: {
          int len = (Size > off) ? (int)(Data[off++] % 32) : 0;
          if ((size_t)len > Size - off) len = (int)(Size - off);
          sqlite3_bind_blob(stmt, i, Data + off, len, SQLITE_TRANSIENT);
          off += len;
          break;
        }
        case 5: {
          int n = (Size > off) ? (int)(Data[off++] % 64) : 0;
          sqlite3_bind_zeroblob(stmt, i, n);
          break;
        }
        default: {
          static int dummy;
          sqlite3_bind_pointer(stmt, i, &dummy, "fuzzptr", NULL);
          break;
        }
      }
    }

    for (i = 0; i < 3; i++) {
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      }
      sqlite3_reset(stmt);
      sqlite3_clear_bindings(stmt);

      if (i == 0 && nparam > 0) {
        int j;
        for (j = 1; j <= sqlite3_bind_parameter_count(stmt) && j <= nparam; j++) {
          int len = (Size > off) ? (int)(Data[off++] % 16) : 0;
          if ((size_t)len > Size - off) len = (int)(Size - off);
          sqlite3_bind_text(stmt, j, (const char *)(Data + off), len, SQLITE_TRANSIENT);
          off += len;
        }
      } else if (i == 1) {
        int j;
        for (j = 1; j <= sqlite3_bind_parameter_count(stmt); j++) {
          sqlite3_bind_null(stmt, j);
        }
      }
    }
  }

  if (stmt) sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}