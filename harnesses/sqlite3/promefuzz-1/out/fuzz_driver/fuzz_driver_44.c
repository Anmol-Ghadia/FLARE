// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_column_type at sqlite3.c:73072:16 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_text at sqlite3.c:73051:33 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare16 at sqlite3.c:119171:16 in sqlite3.h
// sqlite3_prepare16 at sqlite3.c:119171:16 in sqlite3.h
// sqlite3_prepare16 at sqlite3.c:119171:16 in sqlite3.h
// sqlite3_transfer_bindings at sqlite3.c:73539:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_transfer_bindings at sqlite3.c:73539:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_parameter_count at sqlite3.c:73479:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
// sqlite3_stmt_busy at sqlite3.c:73586:16 in sqlite3.h
// sqlite3_expired at sqlite3.c:71934:16 in sqlite3.h
// sqlite3_data_count at sqlite3.c:72912:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Off) {
  uint32_t v = 0;
  size_t i;
  for (i = 0; i < 4 && *Off < Size; ++i, ++(*Off)) {
    v = (v << 8) | Data[*Off];
  }
  return v;
}

static int clamp_int(uint32_t v, int maxv) {
  if (maxv <= 0) return 0;
  return (int)(v % (uint32_t)maxv);
}

static void write_dummy_file(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

static void bind_from_bytes(sqlite3_stmt *stmt, const uint8_t *Data, size_t Size, size_t *Off) {
  int nparam, i;
  if (!stmt) return;
  nparam = sqlite3_bind_parameter_count(stmt);
  for (i = 1; i <= nparam; ++i) {
    uint32_t kind = read_u32(Data, Size, Off) % 6U;
    switch (kind) {
      case 0:
        sqlite3_bind_null(stmt, i);
        break;
      case 1: {
        sqlite3_int64 x = (sqlite3_int64)(int32_t)read_u32(Data, Size, Off);
        sqlite3_bind_int64(stmt, i, x);
        break;
      }
      case 2: {
        double d = (double)(int32_t)read_u32(Data, Size, Off) / 17.0;
        sqlite3_bind_double(stmt, i, d);
        break;
      }
      case 3: {
        int len = clamp_int(read_u32(Data, Size, Off), 32);
        if (*Off + (size_t)len > Size) len = (int)(Size - *Off);
        sqlite3_bind_text(stmt, i, (const char *)(Data + *Off), len, SQLITE_TRANSIENT);
        *Off += (size_t)len;
        break;
      }
      case 4: {
        int len = clamp_int(read_u32(Data, Size, Off), 32);
        if (*Off + (size_t)len > Size) len = (int)(Size - *Off);
        sqlite3_bind_blob(stmt, i, Data + *Off, len, SQLITE_TRANSIENT);
        *Off += (size_t)len;
        break;
      }
      default:
        sqlite3_bind_zeroblob(stmt, i, clamp_int(read_u32(Data, Size, Off), 64));
        break;
    }
  }
}

static void exercise_stmt(sqlite3_stmt *stmt, const uint8_t *Data, size_t Size, size_t *Off) {
  int steps, i;
  if (!stmt) {
    sqlite3_data_count(NULL);
    sqlite3_stmt_busy(NULL);
    sqlite3_expired(NULL);
    sqlite3_reset(NULL);
    return;
  }

  sqlite3_expired(stmt);
  sqlite3_stmt_busy(stmt);
  sqlite3_data_count(stmt);

  steps = 1 + clamp_int(read_u32(Data, Size, Off), 4);
  for (i = 0; i < steps; ++i) {
    int rc = sqlite3_step(stmt);
    sqlite3_data_count(stmt);
    sqlite3_stmt_busy(stmt);
    sqlite3_expired(stmt);

    if (rc == SQLITE_ROW) {
      int cols = sqlite3_data_count(stmt);
      int c;
      for (c = 0; c < cols; ++c) {
        (void)sqlite3_column_type(stmt, c);
        (void)sqlite3_column_bytes(stmt, c);
        (void)sqlite3_column_text(stmt, c);
      }
      if ((read_u32(Data, Size, Off) & 1U) != 0) {
        sqlite3_reset(stmt);
        sqlite3_stmt_busy(stmt);
      }
    } else if (rc == SQLITE_DONE) {
      if ((read_u32(Data, Size, Off) & 1U) != 0) {
        sqlite3_reset(stmt);
      }
      break;
    } else {
      sqlite3_reset(stmt);
      break;
    }
  }

  sqlite3_data_count(stmt);
  sqlite3_stmt_busy(stmt);
  sqlite3_expired(stmt);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt1 = NULL;
  sqlite3_stmt *stmt2 = NULL;
  sqlite3_stmt *stmt3 = NULL;
  const void *tail1 = NULL;
  const void *tail2 = NULL;
  const void *tail3 = NULL;
  size_t off = 0;
  int rc;
  static const char *sql_pool[] = {
    "SELECT 1;",
    "SELECT ?, ?1, ?2;",
    "SELECT * FROM t;",
    "INSERT INTO t(a,b) VALUES(?1,?2);",
    "UPDATE t SET a=?1 WHERE rowid=?2;",
    "DELETE FROM t WHERE a=?1;",
    "SELECT a,b FROM t WHERE a>?1;",
    "PRAGMA schema_version;",
    "PRAGMA incremental_vacuum;",
    "CREATE TABLE IF NOT EXISTS t(a,b);",
    "SELECT length(readfile('./dummy_file'));"
  };
  const size_t sql_pool_count = sizeof(sql_pool) / sizeof(sql_pool[0]);

  write_dummy_file(Data, Size);

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || !db) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db,
               "CREATE TABLE IF NOT EXISTS t(a,b);"
               "INSERT INTO t(a,b) VALUES(1,'x');"
               "INSERT INTO t(a,b) VALUES(2,'y');",
               NULL, NULL, NULL);

  {
    const char *sql1 = sql_pool[clamp_int(read_u32(Data, Size, &off), (int)sql_pool_count)];
    const char *sql2 = sql_pool[clamp_int(read_u32(Data, Size, &off), (int)sql_pool_count)];

    size_t len1 = strlen(sql1);
    size_t len2 = strlen(sql2);
    size_t bytes1 = (len1 + 1) * 2;
    size_t bytes2 = (len2 + 1) * 2;
    unsigned char *utf16_1 = (unsigned char *)calloc(bytes1, 1);
    unsigned char *utf16_2 = (unsigned char *)calloc(bytes2, 1);

    if (utf16_1 && utf16_2) {
      size_t i;
      for (i = 0; i < len1; ++i) utf16_1[i * 2] = (unsigned char)sql1[i];
      for (i = 0; i < len2; ++i) utf16_2[i * 2] = (unsigned char)sql2[i];

      sqlite3_prepare16(db, utf16_1, (int)bytes1, &stmt1, &tail1);
      sqlite3_prepare16(db, utf16_2, (int)bytes2, &stmt2, &tail2);

      if ((read_u32(Data, Size, &off) & 1U) != 0) {
        int cut = clamp_int(read_u32(Data, Size, &off), (int)bytes1);
        sqlite3_prepare16(db, utf16_1, cut, &stmt3, &tail3);
      }
    }

    free(utf16_1);
    free(utf16_2);
  }

  bind_from_bytes(stmt1, Data, Size, &off);
  bind_from_bytes(stmt2, Data, Size, &off);
  bind_from_bytes(stmt3, Data, Size, &off);

  exercise_stmt(stmt1, Data, Size, &off);
  exercise_stmt(stmt2, Data, Size, &off);
  exercise_stmt(stmt3, Data, Size, &off);

  if (stmt1 && stmt2) {
    sqlite3_transfer_bindings(stmt1, stmt2);
    sqlite3_expired(stmt1);
    sqlite3_expired(stmt2);
    sqlite3_stmt_busy(stmt1);
    sqlite3_stmt_busy(stmt2);
    sqlite3_data_count(stmt1);
    sqlite3_data_count(stmt2);
  }

  if (stmt2 && stmt1) {
    sqlite3_transfer_bindings(stmt2, stmt1);
  }

  if (stmt1) sqlite3_reset(stmt1);
  if (stmt2) sqlite3_reset(stmt2);
  if (stmt3) sqlite3_reset(stmt3);

  if (stmt1) sqlite3_finalize(stmt1);
  if (stmt2) sqlite3_finalize(stmt2);
  if (stmt3) sqlite3_finalize(stmt3);
  sqlite3_close(db);
  return 0;
}