// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_bind_text16 at sqlite3.c:73409:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_bind_text16 at sqlite3.c:73409:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_value_text16le at sqlite3.c:72133:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_value at sqlite3.c:73056:27 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_text16be at sqlite3.c:72130:24 in sqlite3.h
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_value_text16le at sqlite3.c:72133:24 in sqlite3.h
// sqlite3_value_text16be at sqlite3.c:72130:24 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static void udf_echo(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int i;
  for (i = 0; i < argc; i++) {
    (void)sqlite3_value_int(argv[i]);
    (void)sqlite3_value_text16(argv[i]);
    (void)sqlite3_value_text16le(argv[i]);
    (void)sqlite3_value_text16be(argv[i]);
  }
  if (argc > 0) {
    sqlite3_result_value(ctx, argv[0]);
  }
}

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *off) {
  uint32_t v = 0;
  size_t i;
  for (i = 0; i < 4; i++) {
    v <<= 8;
    if (*off < size) {
      v |= data[*off];
      (*off)++;
    }
  }
  return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  sqlite3_value *dupv = NULL;
  char *errmsg = NULL;
  int rc;
  size_t off = 0;
  uint32_t mode;
  uint32_t n1;
  uint32_t n2;
  const char *sql;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) {
      sqlite3_close(db);
    }
    return 0;
  }

  (void)sqlite3_create_function(db, "fuzz_value_ops", -1, SQLITE_UTF8, NULL,
                                udf_echo, NULL, NULL);

  (void)sqlite3_exec(db,
                     "CREATE TABLE t(a,b,c);"
                     "INSERT INTO t VALUES(NULL, 0, '');"
                     "INSERT INTO t VALUES(1, -1, 'text');"
                     "INSERT INTO t VALUES(1234567890123, 3.14159, x'00010203');"
                     "INSERT INTO t VALUES('9223372036854775807', '-2147483648', 'utf8');",
                     NULL, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  mode = read_u32(Data, Size, &off);
  n1 = read_u32(Data, Size, &off);
  n2 = read_u32(Data, Size, &off);

  switch (mode % 6U) {
    case 0:
      sql = "SELECT fuzz_value_ops(?1), fuzz_value_ops(?2), fuzz_value_ops(?3)";
      break;
    case 1:
      sql = "SELECT fuzz_value_ops(a), fuzz_value_ops(b), fuzz_value_ops(c) FROM t";
      break;
    case 2:
      sql = "SELECT fuzz_value_ops(CAST(?1 AS TEXT)), "
            "fuzz_value_ops(CAST(?2 AS BLOB)), "
            "fuzz_value_ops(CAST(?3 AS INTEGER))";
      break;
    case 3:
      sql = "SELECT fuzz_value_ops(substr(?1,1,10)), fuzz_value_ops(hex(?2)), fuzz_value_ops(length(?3))";
      break;
    case 4:
      sql = "SELECT fuzz_value_ops(a||b||c) FROM t WHERE rowid = (?1 % 4) + 1";
      break;
    default:
      sql = "SELECT fuzz_value_ops(?1), fuzz_value_ops(NULL), fuzz_value_ops(x'FFFE')";
      break;
  }

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc == SQLITE_OK && stmt) {
    int bind_mode = (int)(mode % 5U);
    int idx = 1;
    const void *tail_ptr = (off < Size) ? (const void *)(Data + off) : (const void *)Data;
    int tail_len = (off < Size) ? (int)(Size - off) : 0;
    int full_len = (int)((Size > (size_t)0x7fffffff) ? 0x7fffffff : Size);

    switch (bind_mode) {
      case 0:
        (void)sqlite3_bind_blob(stmt, idx++, Data, full_len, SQLITE_STATIC);
        (void)sqlite3_bind_text(stmt, idx++, (const char *)Data, full_len, SQLITE_STATIC);
        (void)sqlite3_bind_int(stmt, idx++, (int)n1);
        break;
      case 1:
        (void)sqlite3_bind_text16(stmt, idx++, Data, full_len, SQLITE_STATIC);
        (void)sqlite3_bind_int64(stmt, idx++, (sqlite3_int64)(((uint64_t)n1 << 32) | n2));
        (void)sqlite3_bind_null(stmt, idx++);
        break;
      case 2:
        (void)sqlite3_bind_double(stmt, idx++, (double)n1 / ((double)((n2 % 97U) + 1U)));
        (void)sqlite3_bind_blob(stmt, idx++, tail_ptr, tail_len, SQLITE_STATIC);
        (void)sqlite3_bind_text(stmt, idx++, "", 0, SQLITE_STATIC);
        break;
      case 3:
        (void)sqlite3_bind_zeroblob(stmt, idx++, (int)(n1 % 1024U));
        (void)sqlite3_bind_text16(stmt, idx++, tail_ptr, tail_len, SQLITE_STATIC);
        (void)sqlite3_bind_int(stmt, idx++, (int)n2);
        break;
      default:
        (void)sqlite3_bind_null(stmt, idx++);
        (void)sqlite3_bind_blob(stmt, idx++, Data, full_len > 1024 ? 1024 : full_len, SQLITE_STATIC);
        (void)sqlite3_bind_text(stmt, idx++, (const char *)tail_ptr, tail_len, SQLITE_STATIC);
        break;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      int cols = sqlite3_column_count(stmt);
      int i;
      for (i = 0; i < cols; i++) {
        sqlite3_value *colv = sqlite3_column_value(stmt, i);
        if (colv) {
          dupv = sqlite3_value_dup(colv);
          if (dupv) {
            (void)sqlite3_value_int(dupv);
            (void)sqlite3_value_text16(dupv);
            (void)sqlite3_value_text16le(dupv);
            (void)sqlite3_value_text16be(dupv);
            sqlite3_value_free(dupv);
            dupv = NULL;
          }
        }
      }
    }
  }

  if (stmt) {
    sqlite3_finalize(stmt);
  }
  sqlite3_close(db);
  return 0;
}