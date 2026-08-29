// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text16 at sqlite3.c:73409:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_value at sqlite3.c:73056:27 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_value_text16le at sqlite3.c:72133:24 in sqlite3.h
// sqlite3_value_text16be at sqlite3.c:72130:24 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_value at sqlite3.c:73056:27 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_value_text16le at sqlite3.c:72133:24 in sqlite3.h
// sqlite3_value_text16be at sqlite3.c:72130:24 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_value_text16le at sqlite3.c:72133:24 in sqlite3.h
// sqlite3_value_text16be at sqlite3.c:72130:24 in sqlite3.h
// sqlite3_value_bytes16 at sqlite3.c:72094:16 in sqlite3.h
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_value_text16le at sqlite3.c:72133:24 in sqlite3.h
// sqlite3_value_text16be at sqlite3.c:72130:24 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_value_text16le at sqlite3.c:72133:24 in sqlite3.h
// sqlite3_value_text16be at sqlite3.c:72130:24 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

static void udf_result_value(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  if (!ctx) return;

  if (argc <= 0 || argv == NULL) {
    sqlite3_result_null(ctx);
    return;
  }

  sqlite3_value *v = argv[0];

  if (argc > 1 && argv[1]) {
    int mode = sqlite3_value_int(argv[1]) & 3;
    if (mode == 0) {
      (void)sqlite3_value_text16(v);
    } else if (mode == 1) {
      (void)sqlite3_value_text16le(v);
    } else if (mode == 2) {
      (void)sqlite3_value_text16be(v);
    } else {
      (void)sqlite3_value_bytes16(v);
    }
  } else {
    (void)sqlite3_value_text16(v);
    (void)sqlite3_value_text16le(v);
    (void)sqlite3_value_text16be(v);
  }

  sqlite3_value *dup = sqlite3_value_dup(v);
  if (dup) {
    (void)sqlite3_value_text16(dup);
    (void)sqlite3_value_text16le(dup);
    (void)sqlite3_value_text16be(dup);
    sqlite3_result_value(ctx, dup);
    sqlite3_value_free(dup);
  } else {
    sqlite3_result_value(ctx, v);
  }
}

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *off) {
  uint32_t v = 0;
  size_t i;
  for (i = 0; i < 4 && *off < size; i++, (*off)++) {
    v = (v << 8) | data[*off];
  }
  return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;
  size_t off = 0;

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

  (void)sqlite3_create_function(db, "fuzz_result_value", 2, SQLITE_UTF8, NULL,
                                udf_result_value, NULL, NULL);

  (void)sqlite3_exec(db,
                     "CREATE TABLE t(a,b,c);"
                     "INSERT INTO t VALUES(NULL, 1, X'00010203');"
                     "INSERT INTO t VALUES('text', -5, X'FFEE');"
                     "INSERT INTO t VALUES(3.14159, 9223372036854775807, zeroblob(8));",
                     NULL, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  {
    const char *sql =
        "SELECT "
        "fuzz_result_value(?1, ?2), "
        "fuzz_result_value(a, ?2), "
        "fuzz_result_value(b, ?2), "
        "fuzz_result_value(c, ?2) "
        "FROM t;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      int bindType = (Size > 0) ? (Data[off++ % Size] % 8) : 0;
      switch (bindType) {
        case 0:
          sqlite3_bind_null(stmt, 1);
          break;
        case 1: {
          sqlite3_int64 v = (sqlite3_int64)read_u32(Data, Size, &off);
          sqlite3_bind_int64(stmt, 1, v);
          break;
        }
        case 2: {
          double d = 0.0;
          uint64_t bits = 0;
          size_t i;
          for (i = 0; i < 8 && off < Size; i++, off++) {
            bits = (bits << 8) | Data[off];
          }
          memcpy(&d, &bits, sizeof(d));
          sqlite3_bind_double(stmt, 1, d);
          break;
        }
        case 3: {
          int n = (int)((Size > off) ? (Size - off) : 0);
          sqlite3_bind_text(stmt, 1, (const char *)(Data + off), n, SQLITE_TRANSIENT);
          break;
        }
        case 4: {
          int n = (int)((Size > off) ? (Size - off) : 0);
          sqlite3_bind_blob(stmt, 1, Data + off, n, SQLITE_TRANSIENT);
          break;
        }
        case 5: {
          int n = (int)(Size > off ? ((Size - off) / 2) * 2 : 0);
          sqlite3_bind_text16(stmt, 1, Data + off, n, SQLITE_TRANSIENT);
          break;
        }
        case 6: {
          sqlite3_bind_zeroblob(stmt, 1, (int)(Size & 0xFFFF));
          break;
        }
        default:
          sqlite3_bind_text(stmt, 1, "./dummy_file", -1, SQLITE_STATIC);
          break;
      }

      sqlite3_bind_int(stmt, 2, (Size > 1) ? Data[Size - 1] : 0);

      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int cols = sqlite3_column_count(stmt);
        int i;
        for (i = 0; i < cols; i++) {
          sqlite3_value *uv = sqlite3_column_value(stmt, i);
          sqlite3_value *dup = sqlite3_value_dup(uv);
          if (dup) {
            (void)sqlite3_value_text16(dup);
            (void)sqlite3_value_text16le(dup);
            (void)sqlite3_value_text16be(dup);
            sqlite3_value_free(dup);
          }
        }
      }
    }
  }

  if (stmt) sqlite3_finalize(stmt);
  stmt = NULL;

  {
    const char *sql2 =
        "SELECT fuzz_result_value(readfile('./dummy_file'), ?1);";
    rc = sqlite3_prepare_v2(db, sql2, -1, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      sqlite3_bind_int(stmt, 1, (int)(Size & 3));
      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        sqlite3_value *uv = sqlite3_column_value(stmt, 0);
        sqlite3_value *dup = sqlite3_value_dup(uv);
        if (dup) {
          (void)sqlite3_value_text16(dup);
          (void)sqlite3_value_text16le(dup);
          (void)sqlite3_value_text16be(dup);
          sqlite3_value_free(dup);
        }
      }
    }
  }

  if (stmt) sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}