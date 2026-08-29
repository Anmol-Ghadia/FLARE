// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare16 at sqlite3.c:119171:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare16_v3 at sqlite3.c:119195:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare16 at sqlite3.c:119171:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_complete at sqlite3.c:151963:16 in sqlite3.h
// sqlite3_complete16 at sqlite3.c:152128:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
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

static void write_dummy_file(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size > 0) {
    fwrite(Data, 1, Size, fp);
  }
  fclose(fp);
}

static unsigned int read_u32(const uint8_t *p, size_t n) {
  unsigned int v = 0;
  size_t i;
  for (i = 0; i < n && i < 4; i++) {
    v = (v << 8) | p[i];
  }
  return v;
}

int LLVMFuzzerTestOneInput_102(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt1 = NULL;
  sqlite3_stmt *stmt2 = NULL;
  sqlite3_stmt *stmt3 = NULL;
  const char *tail8 = NULL;
  const void *tail16a = NULL;
  const void *tail16b = NULL;
  char *sql8 = NULL;
  void *sql16 = NULL;
  int rc;
  unsigned int prepFlags = 0;
  int nByte8 = 0;
  int nByte16 = 0;
  size_t sql8_len;
  size_t sql16_len;
  size_t i;

  rc = sqlite3_initialize();
  if (rc != SQLITE_OK) {
    return 0;
  }

  write_dummy_file(Data, Size);

  sql8_len = Size;
  sql8 = (char *)malloc(sql8_len + 1);
  if (!sql8) {
    return 0;
  }
  if (sql8_len > 0) {
    memcpy(sql8, Data, sql8_len);
  }
  sql8[sql8_len] = '\0';

  sql16_len = (Size & ~(size_t)1);
  sql16 = malloc(sql16_len + 2);
  if (!sql16) {
    free(sql8);
    return 0;
  }
  if (sql16_len > 0) {
    memcpy(sql16, Data, sql16_len);
  }
  ((unsigned char *)sql16)[sql16_len] = 0;
  ((unsigned char *)sql16)[sql16_len + 1] = 0;

  (void)sqlite3_complete(sql8);
  (void)sqlite3_complete16(sql16);

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    free(sql16);
    free(sql8);
    return 0;
  }

  if (Size >= 4) {
    prepFlags = read_u32(Data, 4);
  }
#ifdef SQLITE_PREPARE_PERSISTENT
  prepFlags &= (unsigned int)(
      SQLITE_PREPARE_PERSISTENT
#ifdef SQLITE_PREPARE_NORMALIZE
      | SQLITE_PREPARE_NORMALIZE
#endif
#ifdef SQLITE_PREPARE_NO_VTAB
      | SQLITE_PREPARE_NO_VTAB
#endif
#ifdef SQLITE_PREPARE_DONT_LOG
      | SQLITE_PREPARE_DONT_LOG
#endif
      );
#else
  prepFlags = 0;
#endif

  nByte8 = (int)sql8_len;
  nByte16 = (int)sql16_len;

  (void)sqlite3_prepare_v2(db, sql8, nByte8, &stmt1, &tail8);
  if (stmt1) {
    sqlite3_step(stmt1);
    sqlite3_reset(stmt1);
    sqlite3_finalize(stmt1);
    stmt1 = NULL;
  }

  (void)sqlite3_prepare16(db, sql16, nByte16, &stmt2, &tail16a);
  if (stmt2) {
    sqlite3_step(stmt2);
    sqlite3_reset(stmt2);
    sqlite3_finalize(stmt2);
    stmt2 = NULL;
  }

  (void)sqlite3_prepare16_v3(db, sql16, nByte16, prepFlags, &stmt3, &tail16b);
  if (stmt3) {
    sqlite3_step(stmt3);
    sqlite3_reset(stmt3);
    sqlite3_finalize(stmt3);
    stmt3 = NULL;
  }

  if (tail8 && tail8 >= sql8 && tail8 <= sql8 + sql8_len) {
    int rem8 = (int)((sql8 + sql8_len) - tail8);
    sqlite3_stmt *stmt_tail8 = NULL;
    (void)sqlite3_prepare_v2(db, tail8, rem8, &stmt_tail8, &tail8);
    if (stmt_tail8) {
      sqlite3_step(stmt_tail8);
      sqlite3_finalize(stmt_tail8);
    }
  }

  if (tail16a && (const unsigned char *)tail16a >= (const unsigned char *)sql16 &&
      (const unsigned char *)tail16a <= ((const unsigned char *)sql16 + sql16_len)) {
    int rem16 = (int)(((const unsigned char *)sql16 + sql16_len) - (const unsigned char *)tail16a);
    sqlite3_stmt *stmt_tail16 = NULL;
    (void)sqlite3_prepare16(db, tail16a, rem16, &stmt_tail16, NULL);
    if (stmt_tail16) {
      sqlite3_step(stmt_tail16);
      sqlite3_finalize(stmt_tail16);
    }
  }

  for (i = 0; i < 3 && i < sql8_len; i++) {
    size_t off = (sql8_len == 0) ? 0 : ((size_t)sql8[i] % (sql8_len + 1));
    if (off < sql8_len) {
      sqlite3_stmt *s = NULL;
      const char *t = NULL;
      (void)sqlite3_prepare_v2(db, sql8 + off, (int)(sql8_len - off), &s, &t);
      if (s) {
        sqlite3_step(s);
        sqlite3_finalize(s);
      }
    }
  }

  sqlite3_close(db);
  free(sql16);
  free(sql8);
  return 0;
}