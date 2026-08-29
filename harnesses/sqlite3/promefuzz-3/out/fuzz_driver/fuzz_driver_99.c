// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_open16 at sqlite3.c:155696:16 in sqlite3.h
// sqlite3_open16 at sqlite3.c:155696:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_busy_handler at sqlite3.c:153966:16 in sqlite3.h
// sqlite3_collation_needed at sqlite3.c:155803:16 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_busy_handler at sqlite3.c:153966:16 in sqlite3.h
// sqlite3_collation_needed at sqlite3.c:155803:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_busy_handler at sqlite3.c:153966:16 in sqlite3.h
// sqlite3_collation_needed at sqlite3.c:155803:16 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_busy_handler at sqlite3.c:153966:16 in sqlite3.h
// sqlite3_collation_needed at sqlite3.c:155803:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_prepare16_v2 at sqlite3.c:119183:16 in sqlite3.h
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

static int fuzz_authorizer(void *pUserData, int action,
                           const char *p1, const char *p2,
                           const char *dbName, const char *triggerName) {
  const unsigned char *b = (const unsigned char *)pUserData;
  unsigned v = 0;
  if (b) {
    v ^= b[0];
    v ^= (unsigned char)action;
    if (p1) v ^= (unsigned char)p1[0];
    if (p2) v ^= (unsigned char)p2[0];
    if (dbName) v ^= (unsigned char)dbName[0];
    if (triggerName) v ^= (unsigned char)triggerName[0];
  }
  switch (v % 3) {
    case 0: return SQLITE_OK;
    case 1: return SQLITE_IGNORE;
    default: return SQLITE_DENY;
  }
}

static int fuzz_busy_handler(void *pArg, int count) {
  const unsigned char *b = (const unsigned char *)pArg;
  if (!b) return 0;
  return ((b[count & 15] + count) & 1) ? 1 : 0;
}

static int fuzz_collation_cmp(void *pArg, int len1, const void *a, int len2, const void *b) {
  const unsigned char *u = (const unsigned char *)pArg;
  int bias = u ? (int)(u[0] & 1) : 0;
  int min = len1 < len2 ? len1 : len2;
  int rc = memcmp(a, b, (size_t)min);
  if (rc != 0) return rc;
  if (len1 != len2) return (len1 < len2) ? -1 : 1;
  return bias ? 0 : 0;
}

static void fuzz_collation_needed(void *pArg, sqlite3 *db, int eTextRep, const char *name) {
  if (!db || !name) return;
  sqlite3_create_collation(db, name, eTextRep, pArg, fuzz_collation_cmp);
}

static uint16_t read_u16(const uint8_t *Data, size_t Size, size_t *off) {
  uint16_t v = 0;
  if (*off < Size) v = Data[(*off)++];
  if (*off < Size) v |= ((uint16_t)Data[(*off)++]) << 8;
  return v;
}

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *off) {
  uint32_t v = 0;
  for (int i = 0; i < 4; i++) {
    if (*off < Size) v |= ((uint32_t)Data[(*off)++]) << (8 * i);
  }
  return v;
}

static int pick_limit_id(uint8_t v) {
  static const int ids[] = {
    SQLITE_LIMIT_LENGTH,
    SQLITE_LIMIT_SQL_LENGTH,
    SQLITE_LIMIT_COLUMN,
    SQLITE_LIMIT_EXPR_DEPTH,
    SQLITE_LIMIT_COMPOUND_SELECT,
    SQLITE_LIMIT_VDBE_OP,
    SQLITE_LIMIT_FUNCTION_ARG,
    SQLITE_LIMIT_ATTACHED,
    SQLITE_LIMIT_LIKE_PATTERN_LENGTH,
    SQLITE_LIMIT_VARIABLE_NUMBER,
    SQLITE_LIMIT_TRIGGER_DEPTH,
    SQLITE_LIMIT_WORKER_THREADS
  };
  return ids[v % (sizeof(ids) / sizeof(ids[0]))];
}

static void exec_some_sql(sqlite3 *db, const uint8_t *Data, size_t Size, size_t *off, int utf16) {
  if (!db) return;

  size_t remain = (*off < Size) ? (Size - *off) : 0;
  size_t sql_len = remain > 256 ? 256 : remain;

  if (utf16) {
    size_t bytes = (sql_len / 2) * 2;
    void *sql16 = calloc(1, bytes + 2);
    sqlite3_stmt *stmt = NULL;
    if (!sql16) return;
    if (bytes) memcpy(sql16, Data + *off, bytes);
    sqlite3_prepare16_v2(db, sql16, (int)bytes, &stmt, NULL);
    if (stmt) {
      for (int i = 0; i < 4; i++) {
        int rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW && rc != SQLITE_DONE) break;
      }
      sqlite3_finalize(stmt);
    }
    free(sql16);
    *off += bytes;
  } else {
    char *sql = (char *)calloc(1, sql_len + 1);
    sqlite3_stmt *stmt = NULL;
    if (!sql) return;
    if (sql_len) memcpy(sql, Data + *off, sql_len);
    sqlite3_prepare_v2(db, sql, (int)sql_len, &stmt, NULL);
    if (stmt) {
      for (int i = 0; i < 4; i++) {
        int rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW && rc != SQLITE_DONE) break;
      }
      sqlite3_finalize(stmt);
    }
    free(sql);
    *off += sql_len;
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;
  sqlite3 *db1 = NULL;
  sqlite3 *db2 = NULL;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  {
    uint16_t path16[] = {
      '.', 0, '/', 0, 'd', 0, 'u', 0, 'm', 0, 'm', 0, 'y', 0,
      '_', 0, 'f', 0, 'i', 0, 'l', 0, 'e', 0, 0, 0
    };
    if (Size > 0 && (Data[0] & 1)) {
      sqlite3_open16(NULL, &db1);
    } else {
      sqlite3_open16(path16, &db1);
    }
  }

  {
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    if (Size > 1) {
      if (Data[1] & 1) flags |= SQLITE_OPEN_URI;
      if (Data[1] & 2) flags |= SQLITE_OPEN_MEMORY;
      if (Data[1] & 4) flags |= SQLITE_OPEN_NOMUTEX;
      if (Data[1] & 8) flags |= SQLITE_OPEN_FULLMUTEX;
      if (Data[1] & 16) flags |= SQLITE_OPEN_SHAREDCACHE;
      if (Data[1] & 32) flags |= SQLITE_OPEN_PRIVATECACHE;
    }
    sqlite3_open_v2("./dummy_file", &db2, flags, NULL);
  }

  if (db1) {
    sqlite3_set_authorizer(db1, fuzz_authorizer, (void *)Data);
    sqlite3_busy_handler(db1, fuzz_busy_handler, (void *)Data);
    sqlite3_collation_needed(db1, (void *)Data, fuzz_collation_needed);

    for (int i = 0; i < 6; i++) {
      int id = pick_limit_id((off < Size) ? Data[off++] : (uint8_t)i);
      int newVal;
      if (off < Size && (Data[off] & 1)) {
        off++;
        newVal = -1;
      } else {
        uint32_t raw = read_u32(Data, Size, &off);
        newVal = (int)(raw % 100000);
      }
      sqlite3_limit(db1, id, newVal);
    }

    exec_some_sql(db1, Data, Size, &off, 0);
    exec_some_sql(db1, Data, Size, &off, 1);

    sqlite3_set_authorizer(db1, NULL, NULL);
    sqlite3_busy_handler(db1, NULL, NULL);
    sqlite3_collation_needed(db1, NULL, NULL);
  }

  if (db2) {
    sqlite3_set_authorizer(db2, fuzz_authorizer, (void *)Data);
    sqlite3_busy_handler(db2, fuzz_busy_handler, (void *)Data);
    sqlite3_collation_needed(db2, (void *)Data, fuzz_collation_needed);

    for (int i = 0; i < 6; i++) {
      int id = pick_limit_id((off < Size) ? Data[off++] : (uint8_t)(i + 7));
      int mode = (int)(read_u16(Data, Size, &off) % 3);
      int newVal = -1;
      if (mode == 1) {
        newVal = (int)(read_u32(Data, Size, &off) % 4096);
      } else if (mode == 2) {
        newVal = (int)(read_u32(Data, Size, &off) % 1000000);
      }
      sqlite3_limit(db2, id, newVal);
    }

    exec_some_sql(db2, Data, Size, &off, 0);
    exec_some_sql(db2, Data, Size, &off, 1);

    sqlite3_set_authorizer(db2, NULL, NULL);
    sqlite3_busy_handler(db2, NULL, NULL);
    sqlite3_collation_needed(db2, NULL, NULL);
  }

  if (db1) sqlite3_close(db1);
  if (db2) sqlite3_close(db2);
  return 0;
}