// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare16 at sqlite3.c:119171:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare16_v2 at sqlite3.c:119183:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare16_v3 at sqlite3.c:119195:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_decltype16 at sqlite3.c:73170:24 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_decltype16 at sqlite3.c:73170:24 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_decltype16 at sqlite3.c:73170:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_decltype16 at sqlite3.c:73170:24 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_decltype16 at sqlite3.c:73170:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
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

static unsigned int read_u32(const uint8_t **data, size_t *size) {
  unsigned int v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; ++i) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static int read_int(const uint8_t **data, size_t *size) {
  return (int)read_u32(data, size);
}

static unsigned int choose_prep_flags(unsigned int raw) {
  unsigned int flags = 0;
#ifdef SQLITE_PREPARE_PERSISTENT
  if (raw & 1u) flags |= SQLITE_PREPARE_PERSISTENT;
#endif
#ifdef SQLITE_PREPARE_NORMALIZE
  if (raw & 2u) flags |= SQLITE_PREPARE_NORMALIZE;
#endif
#ifdef SQLITE_PREPARE_NO_VTAB
  if (raw & 4u) flags |= SQLITE_PREPARE_NO_VTAB;
#endif
#ifdef SQLITE_PREPARE_DONT_LOG
  if (raw & 8u) flags |= SQLITE_PREPARE_DONT_LOG;
#endif
  return flags;
}

static void exercise_stmt(sqlite3_stmt *stmt, int idx_seed) {
  if (!stmt) return;

  int col_count = sqlite3_column_count(stmt);

  (void)sqlite3_column_name16(stmt, -1);
  (void)sqlite3_column_decltype16(stmt, -1);
  (void)sqlite3_column_text16(stmt, -1);
  (void)sqlite3_column_name16(stmt, col_count);
  (void)sqlite3_column_decltype16(stmt, col_count);
  (void)sqlite3_column_text16(stmt, col_count);

  for (int i = 0; i < col_count; ++i) {
    (void)sqlite3_column_name16(stmt, i);
    (void)sqlite3_column_decltype16(stmt, i);
  }

  int step_rc;
  int steps = 0;
  do {
    step_rc = sqlite3_step(stmt);
    if (step_rc == SQLITE_ROW) {
      for (int i = 0; i < col_count; ++i) {
        (void)sqlite3_column_text16(stmt, i);
        (void)sqlite3_column_name16(stmt, i);
        (void)sqlite3_column_decltype16(stmt, i);
      }
      if (col_count > 0) {
        int idx = idx_seed % (col_count + 2) - 1;
        (void)sqlite3_column_text16(stmt, idx);
        (void)sqlite3_column_name16(stmt, idx);
        (void)sqlite3_column_decltype16(stmt, idx);
      }
    }
    steps++;
  } while (step_rc == SQLITE_ROW && steps < 8);

  sqlite3_reset(stmt);
  sqlite3_clear_bindings(stmt);
}

int LLVMFuzzerTestOneInput_45(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  if (sqlite3_open("./dummy_file", &db) != SQLITE_OK || !db) {
    if (db) sqlite3_close(db);
    return 0;
  }

  (void)sqlite3_exec(db,
                     "CREATE TABLE IF NOT EXISTS t("
                     "a INTEGER, b TEXT, c REAL, d BLOB);"
                     "INSERT INTO t(a,b,c,d) VALUES(1,'x',3.14,X'00');"
                     "INSERT INTO t(a,b,c,d) VALUES(2,'y',2.71,X'0102');",
                     NULL, NULL, NULL);

  const uint8_t *p = Data;
  size_t remaining = Size;

  size_t sql_bytes = remaining;
  if (sql_bytes > 4094) sql_bytes = 4094;

  unsigned char sql16[4096];
  memset(sql16, 0, sizeof(sql16));
  if (sql_bytes > 0) {
    memcpy(sql16, p, sql_bytes);
  }

  if ((sql_bytes & 1u) != 0) {
    sql16[sql_bytes] = 0;
    sql_bytes++;
  }
  sql16[sql_bytes] = 0;
  sql16[sql_bytes + 1] = 0;

  int nByte_variants[4];
  nByte_variants[0] = (int)sql_bytes;
  nByte_variants[1] = (int)(sql_bytes + 2);
  nByte_variants[2] = -1;
  nByte_variants[3] = (int)(sql_bytes > 2 ? sql_bytes - 2 : sql_bytes);

  int seed1 = read_int(&p, &remaining);
  int seed2 = read_int(&p, &remaining);
  unsigned int prep_flags = choose_prep_flags(read_u32(&p, &remaining));

  sqlite3_stmt *stmt = NULL;
  const void *tail = NULL;

  if (sqlite3_prepare16(db, sql16, nByte_variants[seed1 & 3], &stmt, &tail) == SQLITE_OK && stmt) {
    exercise_stmt(stmt, seed1);
  }
  if (stmt) {
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  tail = NULL;
  if (sqlite3_prepare16_v2(db, sql16, nByte_variants[(seed1 >> 2) & 3], &stmt, &tail) == SQLITE_OK && stmt) {
    exercise_stmt(stmt, seed2);
  }
  if (stmt) {
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  tail = NULL;
  if (sqlite3_prepare16_v3(db, sql16, nByte_variants[(seed1 >> 4) & 3], prep_flags, &stmt, &tail) == SQLITE_OK && stmt) {
    exercise_stmt(stmt, seed1 ^ seed2);
  }
  if (stmt) {
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  sqlite3_close(db);
  return 0;
}