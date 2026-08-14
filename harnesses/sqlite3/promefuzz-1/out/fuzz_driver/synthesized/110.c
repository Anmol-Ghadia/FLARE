// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_prepare at sqlite3.c:119055:16 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_stmt_scanstatus_reset at sqlite3.c:73956:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_prepare16 at sqlite3.c:119171:16 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_stmt_scanstatus_reset at sqlite3.c:73956:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_prepare16_v2 at sqlite3.c:119183:16 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_stmt_scanstatus_reset at sqlite3.c:73956:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_next_stmt at sqlite3.c:73597:26 in sqlite3.h
// sqlite3_db_handle at sqlite3.c:73563:21 in sqlite3.h
// sqlite3_stmt_scanstatus_reset at sqlite3.c:73956:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_stmt_scanstatus_reset at sqlite3.c:73956:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"

static int consume_u8(const uint8_t **data, size_t *size) {
  if (*size == 0) return 0;
  int v = **data;
  (*data)++;
  (*size)--;
  return v;
}

static int consume_i32(const uint8_t **data, size_t *size) {
  int v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; i++) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static void write_dummy_file(const uint8_t *data, size_t size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (size) fwrite(data, 1, size, fp);
  fclose(fp);
}

int LLVMFuzzerTestOneInput_110(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;

  write_dummy_file(Data, Size);

  sqlite3 *db = NULL;
  int open_mode = consume_u8(&p, &remaining) & 1;
  int rc;

  if (open_mode) {
    rc = sqlite3_open("./dummy_file", &db);
  } else {
    rc = sqlite3_open(":memory:", &db);
  }
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db,
               "CREATE TABLE IF NOT EXISTS t(a,b,c);"
               "INSERT INTO t VALUES(1,2,3);"
               "INSERT INTO t VALUES('x','y','z');",
               NULL, NULL, NULL);

  char *sql8 = (char *)malloc(remaining + 1);
  if (!sql8) {
    sqlite3_close(db);
    return 0;
  }
  memcpy(sql8, p, remaining);
  sql8[remaining] = '\0';

  /* Build a well-formed UTF-16 buffer with enough padding to tolerate
     SQLite's 2-byte scanning even when nbyte is odd. */
  size_t utf16_chars = remaining;
  size_t utf16_len = utf16_chars * 2;
  size_t sql16_alloc = utf16_len + 4; /* terminator + extra padding */
  unsigned char *sql16 = (unsigned char *)malloc(sql16_alloc);
  if (!sql16) {
    free(sql8);
    sqlite3_close(db);
    return 0;
  }
  for (size_t i = 0; i < utf16_chars; i++) {
    sql16[2 * i] = p[i];
    sql16[2 * i + 1] = 0;
  }
  sql16[utf16_len] = 0;
  sql16[utf16_len + 1] = 0;
  sql16[utf16_len + 2] = 0;
  sql16[utf16_len + 3] = 0;

  sqlite3_stmt *stmts[16];
  size_t stmt_count = 0;
  memset(stmts, 0, sizeof(stmts));

  for (int i = 0; i < 3 && stmt_count < 16; i++) {
    sqlite3_stmt *stmt = NULL;
    const char *tail8 = NULL;
    int mode = consume_u8(&p, &remaining) % 3;
    int nbyte = 0;

    if (mode == 0) {
      nbyte = -1;
    } else if (mode == 1) {
      nbyte = (int)(Size > 0 ? (consume_i32(&p, &remaining) % ((int)Size + 1)) : 0);
    } else {
      nbyte = (int)Size;
    }

    rc = sqlite3_prepare(db, sql8, nbyte, &stmt, &tail8);
    (void)rc;
    if (stmt && stmt_count < 16) {
      stmts[stmt_count++] = stmt;
      sqlite3_db_handle(stmt);
      sqlite3_stmt_scanstatus_reset(stmt);
      sqlite3_step(stmt);
      sqlite3_reset(stmt);
    }
  }

  for (int i = 0; i < 3 && stmt_count < 16; i++) {
    sqlite3_stmt *stmt = NULL;
    const void *tail16 = NULL;
    int mode = consume_u8(&p, &remaining) % 3;
    int nbyte = 0;

    if (mode == 0) {
      nbyte = -1;
    } else if (mode == 1) {
      nbyte = (int)(utf16_len > 0 ? (consume_i32(&p, &remaining) % ((int)utf16_len + 1)) : 0);
      if (nbyte > 0 && (nbyte & 1)) nbyte--; /* keep UTF-16 byte count even */
    } else {
      nbyte = (int)utf16_len;
    }

    rc = sqlite3_prepare16(db, sql16, nbyte, &stmt, &tail16);
    (void)rc;
    if (stmt && stmt_count < 16) {
      stmts[stmt_count++] = stmt;
      sqlite3_db_handle(stmt);
      sqlite3_stmt_scanstatus_reset(stmt);
      sqlite3_step(stmt);
      sqlite3_reset(stmt);
    }
  }

  for (int i = 0; i < 3 && stmt_count < 16; i++) {
    sqlite3_stmt *stmt = NULL;
    const void *tail16 = NULL;
    int mode = consume_u8(&p, &remaining) % 3;
    int nbyte = 0;

    if (mode == 0) {
      nbyte = -1;
    } else if (mode == 1) {
      nbyte = (int)(utf16_len > 0 ? (consume_i32(&p, &remaining) % ((int)utf16_len + 1)) : 0);
      if (nbyte > 0 && (nbyte & 1)) nbyte--; /* keep UTF-16 byte count even */
    } else {
      nbyte = (int)utf16_len;
    }

    rc = sqlite3_prepare16_v2(db, sql16, nbyte, &stmt, &tail16);
    (void)rc;
    if (stmt && stmt_count < 16) {
      stmts[stmt_count++] = stmt;
      sqlite3_db_handle(stmt);
      sqlite3_stmt_scanstatus_reset(stmt);
      sqlite3_step(stmt);
      sqlite3_reset(stmt);
    }
  }

  sqlite3_stmt *iter = NULL;
  int guard = 0;
  while ((iter = sqlite3_next_stmt(db, iter)) != NULL && guard++ < 64) {
    sqlite3 *owner = sqlite3_db_handle(iter);
    if (owner == db) {
      sqlite3_stmt_scanstatus_reset(iter);
      sqlite3_step(iter);
      sqlite3_reset(iter);
    }
  }

  for (size_t i = 0; i < stmt_count; i++) {
    if (stmts[i]) {
      sqlite3_stmt_scanstatus_reset(stmts[i]);
      sqlite3_finalize(stmts[i]);
    }
  }

  free(sql16);
  free(sql8);
  sqlite3_close(db);
  return 0;
}