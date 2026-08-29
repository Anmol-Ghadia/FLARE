// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_value at sqlite3.c:73056:27 in sqlite3.h
// sqlite3_bind_value at sqlite3.c:73419:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_column_value at sqlite3.c:73056:27 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_numeric_type at sqlite3.c:74581:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_numeric_type at sqlite3.c:74581:16 in sqlite3.h
// sqlite3_value_numeric_type at sqlite3.c:74581:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t rd32(const uint8_t *p) {
  return ((uint32_t)p[0]) |
         ((uint32_t)p[1] << 8) |
         ((uint32_t)p[2] << 16) |
         ((uint32_t)p[3] << 24);
}

static int exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
  return rc;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *ins = NULL;
  sqlite3_stmt *sel = NULL;
  sqlite3_stmt *bind_from_value = NULL;
  int rc;

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

  exec_sql(db, "PRAGMA temp_store=MEMORY;");
  exec_sql(db, "CREATE TABLE t(a,b,c,d,e);");

  rc = sqlite3_prepare_v2(
      db,
      "INSERT INTO t(a,b,c,d,e) VALUES(?1, ?2, ?3, ?4, ?5);",
      -1, &ins, NULL);
  if (rc != SQLITE_OK || ins == NULL) {
    sqlite3_close(db);
    return 0;
  }

  {
    size_t off = 0;
    int i;
    for (i = 1; i <= 5; i++) {
      uint8_t tag = 0;
      uint32_t len = 0;
      const void *ptr = NULL;
      int ilen = 0;

      if (off < Size) tag = Data[off++];
      if (off + 4 <= Size) {
        len = rd32(Data + off);
        off += 4;
      } else if (off < Size) {
        len = (uint32_t)(Size - off);
      }

      if (off < Size) {
        size_t remain = Size - off;
        if (len > remain) len = (uint32_t)remain;
        ptr = Data + off;
        ilen = (int)len;
        off += len;
      }

      switch (tag % 7) {
        case 0:
          sqlite3_bind_null(ins, i);
          break;
        case 1:
          sqlite3_bind_int64(ins, i, (sqlite3_int64)(int64_t)len);
          break;
        case 2: {
          double d = 0.0;
          if (ilen >= (int)sizeof(double)) {
            memcpy(&d, ptr, sizeof(double));
          } else {
            d = (double)len;
          }
          sqlite3_bind_double(ins, i, d);
          break;
        }
        case 3:
          sqlite3_bind_text(ins, i, (const char *)ptr, ilen, SQLITE_TRANSIENT);
          break;
        case 4:
          sqlite3_bind_blob(ins, i, ptr, ilen, SQLITE_TRANSIENT);
          break;
        case 5:
          sqlite3_bind_zeroblob(ins, i, ilen);
          break;
        case 6:
        default:
          sqlite3_bind_text(ins, i, "12345", -1, SQLITE_STATIC);
          break;
      }
    }
    sqlite3_step(ins);
  }

  sqlite3_finalize(ins);
  ins = NULL;

  rc = sqlite3_prepare_v2(db, "SELECT a,b,c,d,e FROM t;", -1, &sel, NULL);
  if (rc != SQLITE_OK || sel == NULL) {
    sqlite3_close(db);
    return 0;
  }

  rc = sqlite3_prepare_v2(
      db,
      "SELECT ?1, ?2, ?3, ?4, ?5;",
      -1, &bind_from_value, NULL);
  if (rc != SQLITE_OK || bind_from_value == NULL) {
    sqlite3_finalize(sel);
    sqlite3_close(db);
    return 0;
  }

  while ((rc = sqlite3_step(sel)) == SQLITE_ROW) {
    int col;
    for (col = 0; col < 5; col++) {
      sqlite3_value *v = sqlite3_column_value(sel, col);
      if (v) {
        (void)sqlite3_bind_value(bind_from_value, col + 1, v);
      }
    }

    sqlite3_step(bind_from_value);
    sqlite3_reset(bind_from_value);
    sqlite3_clear_bindings(bind_from_value);

    for (col = 0; col < 5; col++) {
      sqlite3_value *v2 = sqlite3_column_value(bind_from_value, col);
      if (v2) {
        (void)sqlite3_value_type(v2);
        (void)sqlite3_value_numeric_type(v2);
        (void)sqlite3_value_blob(v2);
        (void)sqlite3_value_bytes(v2);

        if (Size & 1) {
          (void)sqlite3_value_bytes(v2);
          (void)sqlite3_value_blob(v2);
          (void)sqlite3_value_type(v2);
          (void)sqlite3_value_numeric_type(v2);
        } else {
          (void)sqlite3_value_numeric_type(v2);
          (void)sqlite3_value_type(v2);
          (void)sqlite3_value_bytes(v2);
          (void)sqlite3_value_blob(v2);
        }
      }
    }
  }

  sqlite3_finalize(bind_from_value);
  sqlite3_finalize(sel);
  sqlite3_close(db);
  return 0;
}