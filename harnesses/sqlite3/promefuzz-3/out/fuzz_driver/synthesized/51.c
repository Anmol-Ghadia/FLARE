// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare16 at sqlite3.c:119171:16 in sqlite3.h
// sqlite3_prepare16_v2 at sqlite3.c:119183:16 in sqlite3.h
// sqlite3_prepare16 at sqlite3.c:119171:16 in sqlite3.h
// sqlite3_prepare16_v2 at sqlite3.c:119183:16 in sqlite3.h
// sqlite3_prepare16 at sqlite3.c:119171:16 in sqlite3.h
// sqlite3_prepare16_v2 at sqlite3.c:119183:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open16 at sqlite3.c:155696:16 in sqlite3.h
// sqlite3_busy_handler at sqlite3.c:153966:16 in sqlite3.h
// sqlite3_collation_needed16 at sqlite3.c:155824:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_busy_handler at sqlite3.c:153966:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"

struct BusyCtx {
  const uint8_t *data;
  size_t size;
};

static int fuzz_busy_handler(void *ptr, int count) {
  struct BusyCtx *ctx = (struct BusyCtx *)ptr;
  if (!ctx || !ctx->data || ctx->size == 0) return 0;
  return (ctx->data[((size_t)count) % ctx->size] & 1) ? 1 : 0;
}

static void fuzz_collation_needed16(void *pArg, sqlite3 *db, int eTextRep, const void *zName) {
  (void)pArg;
  (void)db;
  (void)eTextRep;
  (void)zName;
}

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

static int write_dummy_file(const uint8_t *data, size_t size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return 0;
  if (size) fwrite(data, 1, size, fp);
  fclose(fp);
  return 1;
}

static void exec_stmt(sqlite3_stmt *stmt) {
  if (!stmt) return;
  for (int i = 0; i < 8; ++i) {
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) continue;
    if (rc == SQLITE_BUSY) continue;
    break;
  }
  sqlite3_reset(stmt);
  sqlite3_finalize(stmt);
}

int LLVMFuzzerTestOneInput_51(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;
  uint32_t mode = read_u32(&p, &remaining);
  struct BusyCtx busy_ctx = { Data, Size };

  write_dummy_file(p, remaining);

  sqlite3 *db = NULL;
  sqlite3 *db2 = NULL;

  if (mode & 1) {
    sqlite3_open("./dummy_file", &db);
  } else {
    static const uint16_t utf16_name[] = {
      '.', '/', 'd', 'u', 'm', 'm', 'y', '_', 'f', 'i', 'l', 'e', 0
    };
    sqlite3_open16(utf16_name, &db);
  }

  if (!db) return 0;

  sqlite3_busy_handler(db, fuzz_busy_handler, &busy_ctx);
  sqlite3_collation_needed16(db, &busy_ctx, fuzz_collation_needed16);

  sqlite3_open("./dummy_file", &db2);
  if (db2) {
    sqlite3_busy_handler(db2, fuzz_busy_handler, &busy_ctx);
  }

  if (db2 && (mode & 2)) {
    sqlite3_exec(db, "BEGIN EXCLUSIVE;", NULL, NULL, NULL);
    sqlite3_exec(db2, "BEGIN IMMEDIATE;", NULL, NULL, NULL);
    sqlite3_exec(db2, "CREATE TABLE IF NOT EXISTS t(x);", NULL, NULL, NULL);
    sqlite3_exec(db2, "INSERT INTO t VALUES(1);", NULL, NULL, NULL);
    sqlite3_exec(db2, "ROLLBACK;", NULL, NULL, NULL);
    sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
  }

  if (remaining > 0) {
    int sql_bytes = (int)(remaining & ~(size_t)1);
    const void *tail = NULL;
    sqlite3_stmt *stmt1 = NULL;
    sqlite3_stmt *stmt2 = NULL;

    sqlite3_prepare16(db, p, sql_bytes, &stmt1, &tail);
    exec_stmt(stmt1);

    sqlite3_prepare16_v2(db, p, sql_bytes, &stmt2, &tail);
    exec_stmt(stmt2);

    if ((mode & 4) && sql_bytes >= 2) {
      int partial = (sql_bytes / 2) & ~1;
      stmt1 = NULL;
      stmt2 = NULL;
      sqlite3_prepare16(db, p, partial, &stmt1, &tail);
      exec_stmt(stmt1);
      sqlite3_prepare16_v2(db, p, partial, &stmt2, &tail);
      exec_stmt(stmt2);
    }
  } else {
    static const uint16_t default_sql[] = {
      'C','R','E','A','T','E',' ','T','A','B','L','E',' ','I','F',' ','N','O','T',' ',
      'E','X','I','S','T','S',' ','t','(','x',')',';',
      'I','N','S','E','R','T',' ','I','N','T','O',' ','t',' ','V','A','L','U','E','S',
      '(','1',')',';','S','E','L','E','C','T',' ','*',' ','F','R','O','M',' ','t',';',0
    };
    const void *tail = NULL;
    sqlite3_stmt *stmt1 = NULL;
    sqlite3_stmt *stmt2 = NULL;
    int nbytes = (int)(sizeof(default_sql) - sizeof(uint16_t));

    sqlite3_prepare16(db, default_sql, nbytes, &stmt1, &tail);
    exec_stmt(stmt1);

    sqlite3_prepare16_v2(db, default_sql, nbytes, &stmt2, &tail);
    exec_stmt(stmt2);
  }

  if (db2) sqlite3_close(db2);
  sqlite3_close(db);
  return 0;
}