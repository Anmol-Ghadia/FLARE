// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare at sqlite3.c:119055:16 in sqlite3.h
// sqlite3_prepare_v3 at sqlite3.c:119085:16 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_stmt_readonly at sqlite3.c:73571:16 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_stmt_scanstatus_reset at sqlite3.c:73956:17 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_stmt_readonly at sqlite3.c:73571:16 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_stmt_scanstatus_reset at sqlite3.c:73956:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_stmt_readonly at sqlite3.c:73571:16 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_stmt_scanstatus_reset at sqlite3.c:73956:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v3 at sqlite3.c:119085:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **data, size_t *size) {
  uint32_t v = 0;
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

static unsigned int choose_prep_flags(uint32_t x) {
  unsigned int flags = 0;
#ifdef SQLITE_PREPARE_PERSISTENT
  if (x & 1u) flags |= SQLITE_PREPARE_PERSISTENT;
#endif
#ifdef SQLITE_PREPARE_NORMALIZE
  if (x & 2u) flags |= SQLITE_PREPARE_NORMALIZE;
#endif
#ifdef SQLITE_PREPARE_NO_VTAB
  if (x & 4u) flags |= SQLITE_PREPARE_NO_VTAB;
#endif
#ifdef SQLITE_PREPARE_DONT_LOG
  if (x & 8u) flags |= SQLITE_PREPARE_DONT_LOG;
#endif
  return flags;
}

static void exec_sql(sqlite3 *db, const char *sql) {
  sqlite3_stmt *stmt = NULL;
  const char *tail = NULL;
  if (sqlite3_prepare_v3(db, sql, -1, 0, &stmt, &tail) == SQLITE_OK && stmt) {
    while (sqlite3_step(stmt) == SQLITE_ROW) {
    }
    sqlite3_finalize(stmt);
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  sqlite3 *db = NULL;
  if (sqlite3_open("./dummy_file", &db) != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  exec_sql(db, "PRAGMA journal_mode=OFF;");
  exec_sql(db, "PRAGMA synchronous=OFF;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t1(a,b,c);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(x,y);");
  exec_sql(db, "INSERT INTO t1 VALUES(1,'one',x'00');");
  exec_sql(db, "INSERT INTO t2 VALUES(2,'two');");

  const uint8_t *p = Data;
  size_t remaining = Size;

  size_t sql_len = remaining > 0 ? (size_t)(*p % (remaining + 1)) : 0;
  if (remaining > 0) {
    p++;
    remaining--;
    if (sql_len > remaining) sql_len = remaining;
  }

  char *sql = (char *)malloc(sql_len + 1);
  if (!sql) {
    sqlite3_close(db);
    return 0;
  }
  if (sql_len > 0) {
    memcpy(sql, p, sql_len);
    p += sql_len;
    remaining -= sql_len;
  }
  sql[sql_len] = '\0';

  int nbyte_mode = read_int(&p, &remaining) % 4;
  int nbyte;
  switch (nbyte_mode) {
    case 0:
      nbyte = -1;
      break;
    case 1:
      nbyte = 0;
      break;
    case 2:
      nbyte = (int)sql_len;
      break;
    default:
      nbyte = (int)sql_len + 1;
      break;
  }

  sqlite3_stmt *stmt1 = NULL;
  sqlite3_stmt *stmt2 = NULL;
  const char *tail1 = NULL;
  const char *tail2 = NULL;

  (void)sqlite3_prepare(db, sql, nbyte, &stmt1, &tail1);

  unsigned int prep_flags = choose_prep_flags(read_u32(&p, &remaining));
  int nbyte2_mode = read_int(&p, &remaining) % 4;
  int nbyte2;
  switch (nbyte2_mode) {
    case 0:
      nbyte2 = -1;
      break;
    case 1:
      nbyte2 = 0;
      break;
    case 2:
      nbyte2 = (int)sql_len;
      break;
    default:
      nbyte2 = (int)sql_len + 1;
      break;
  }

  (void)sqlite3_prepare_v3(db, sql, nbyte2, prep_flags, &stmt2, &tail2);

  sqlite3_stmt *iter = NULL;
  int loop_guard = 0;
  while ((iter = sqlite3_next_stmt(db, iter)) != NULL && loop_guard++ < 128) {
    (void)sqlite3_stmt_readonly(iter);
    (void)sqlite3_column_name16(iter, -1);
    (void)sqlite3_column_name16(iter, 0);
    (void)sqlite3_column_name16(iter, 1);
    (void)sqlite3_column_name16(iter, 1024);

    int step_count = 0;
    int rc;
    while (step_count++ < 8) {
      rc = sqlite3_step(iter);
      if (rc != SQLITE_ROW) break;
      (void)sqlite3_column_name16(iter, 0);
      (void)sqlite3_column_name16(iter, 1);
    }

#ifdef SQLITE_ENABLE_STMT_SCANSTATUS
    sqlite3_stmt_scanstatus_reset(iter);
#endif
    (void)sqlite3_reset(iter);
  }

  if (stmt1) {
    (void)sqlite3_stmt_readonly(stmt1);
    (void)sqlite3_column_name16(stmt1, 0);
#ifdef SQLITE_ENABLE_STMT_SCANSTATUS
    sqlite3_stmt_scanstatus_reset(stmt1);
#endif
    sqlite3_finalize(stmt1);
  }

  if (stmt2 && stmt2 != stmt1) {
    (void)sqlite3_stmt_readonly(stmt2);
    (void)sqlite3_column_name16(stmt2, 0);
#ifdef SQLITE_ENABLE_STMT_SCANSTATUS
    sqlite3_stmt_scanstatus_reset(stmt2);
#endif
    sqlite3_finalize(stmt2);
  }

  free(sql);
  sqlite3_close(db);
  return 0;
}