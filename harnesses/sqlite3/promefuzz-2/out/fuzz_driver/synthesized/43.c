// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_value at sqlite3.c:73056:27 in sqlite3.h
// sqlite3_bind_value at sqlite3.c:73419:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_value at sqlite3.c:73056:27 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_numeric_type at sqlite3.c:74581:16 in sqlite3.h
// sqlite3_value_frombind at sqlite3.c:72232:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_value at sqlite3.c:73056:27 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_bind_value at sqlite3.c:73419:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_numeric_type at sqlite3.c:74581:16 in sqlite3.h
// sqlite3_value_frombind at sqlite3.c:72232:16 in sqlite3.h
// sqlite3_value_nochange at sqlite3.c:72227:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static int consume_u8(const uint8_t **p, size_t *n){
  if(*n == 0) return 0;
  int v = **p;
  (*p)++;
  (*n)--;
  return v;
}

static int consume_int(const uint8_t **p, size_t *n){
  int v = 0;
  int i;
  for(i = 0; i < 4; i++){
    v = (v << 8) | consume_u8(p, n);
  }
  return v;
}

static sqlite3_int64 consume_i64(const uint8_t **p, size_t *n){
  sqlite3_int64 v = 0;
  int i;
  for(i = 0; i < 8; i++){
    v = (v << 8) | (sqlite3_int64)consume_u8(p, n);
  }
  return v;
}

static double consume_double(const uint8_t **p, size_t *n){
  union {
    uint64_t u;
    double d;
  } x;
  x.u = 0;
  for(int i = 0; i < 8; i++){
    x.u = (x.u << 8) | (uint64_t)consume_u8(p, n);
  }
  return x.d;
}

int LLVMFuzzerTestOneInput_43(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt_select = NULL;
  sqlite3_stmt *stmt_bind = NULL;
  sqlite3_stmt *stmt_insert = NULL;
  sqlite3_value *colv = NULL;
  int rc;

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if(fp){
      fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  rc = sqlite3_open(":memory:", &db);
  if(rc != SQLITE_OK || db == NULL){
    if(db) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db,
               "CREATE TABLE t(a,b,c);"
               "INSERT INTO t VALUES(NULL, 123, '456');"
               "INSERT INTO t VALUES('789', x'010203', 3.14);"
               "INSERT INTO t VALUES('notnum', zeroblob(4), -7);",
               NULL, NULL, NULL);

  sqlite3_prepare_v2(db, "INSERT INTO t(a,b,c) VALUES(?1,?2,?3);", -1, &stmt_insert, NULL);
  if(stmt_insert){
    const uint8_t *p = Data;
    size_t n = Size;
    int mode1 = consume_u8(&p, &n) % 5;
    int mode2 = consume_u8(&p, &n) % 5;
    int mode3 = consume_u8(&p, &n) % 5;

    switch(mode1){
      case 0: sqlite3_bind_null(stmt_insert, 1); break;
      case 1: sqlite3_bind_int64(stmt_insert, 1, consume_i64(&p, &n)); break;
      case 2: sqlite3_bind_double(stmt_insert, 1, consume_double(&p, &n)); break;
      case 3: {
        int len = (int)(n > 32 ? 32 : n);
        sqlite3_bind_text(stmt_insert, 1, (const char *)p, len, SQLITE_TRANSIENT);
        p += len;
        n -= len;
        break;
      }
      default: {
        int len = (int)(n > 32 ? 32 : n);
        sqlite3_bind_blob(stmt_insert, 1, p, len, SQLITE_TRANSIENT);
        p += len;
        n -= len;
        break;
      }
    }

    switch(mode2){
      case 0: sqlite3_bind_null(stmt_insert, 2); break;
      case 1: sqlite3_bind_int64(stmt_insert, 2, consume_i64(&p, &n)); break;
      case 2: sqlite3_bind_double(stmt_insert, 2, consume_double(&p, &n)); break;
      case 3: {
        int len = (int)(n > 32 ? 32 : n);
        sqlite3_bind_text(stmt_insert, 2, (const char *)p, len, SQLITE_TRANSIENT);
        p += len;
        n -= len;
        break;
      }
      default: {
        int len = consume_u8(&p, &n);
        sqlite3_bind_zeroblob(stmt_insert, 2, len);
        break;
      }
    }

    switch(mode3){
      case 0: sqlite3_bind_null(stmt_insert, 3); break;
      case 1: sqlite3_bind_int64(stmt_insert, 3, consume_i64(&p, &n)); break;
      case 2: sqlite3_bind_double(stmt_insert, 3, consume_double(&p, &n)); break;
      case 3: {
        int len = (int)(n > 32 ? 32 : n);
        sqlite3_bind_text(stmt_insert, 3, (const char *)p, len, SQLITE_TRANSIENT);
        p += len;
        n -= len;
        break;
      }
      default: {
        int len = (int)(n > 32 ? 32 : n);
        sqlite3_bind_blob(stmt_insert, 3, p, len, SQLITE_TRANSIENT);
        p += len;
        n -= len;
        break;
      }
    }

    sqlite3_step(stmt_insert);
    sqlite3_finalize(stmt_insert);
    stmt_insert = NULL;
  }

  rc = sqlite3_prepare_v2(
      db,
      "SELECT a,b,c FROM t ORDER BY rowid LIMIT 1 OFFSET ?1;",
      -1,
      &stmt_select,
      NULL);
  if(rc != SQLITE_OK || stmt_select == NULL){
    sqlite3_close(db);
    return 0;
  }

  sqlite3_bind_int(stmt_select, 1, (Size > 0) ? (Data[0] % 8) : 0);

  rc = sqlite3_prepare_v2(
      db,
      "SELECT ?1, ?2, ?3;",
      -1,
      &stmt_bind,
      NULL);
  if(rc != SQLITE_OK || stmt_bind == NULL){
    sqlite3_finalize(stmt_select);
    sqlite3_close(db);
    return 0;
  }

  if(sqlite3_step(stmt_select) == SQLITE_ROW){
    int col = (Size > 1) ? (Data[1] % 3) : 0;
    int param = (Size > 2) ? ((Data[2] % 3) + 1) : 1;

    colv = sqlite3_column_value(stmt_select, col);
    if(colv){
      sqlite3_bind_value(stmt_bind, param, colv);
    }
  }

  sqlite3_step(stmt_bind);
  sqlite3_reset(stmt_bind);

  if(sqlite3_step(stmt_bind) == SQLITE_ROW){
    for(int i = 0; i < 3; i++){
      sqlite3_value *v = sqlite3_column_value(stmt_bind, i);
      if(v){
        (void)sqlite3_value_type(v);
        (void)sqlite3_value_numeric_type(v);
        (void)sqlite3_value_frombind(v);
        (void)sqlite3_value_nochange(v);
      }
    }
  }

  sqlite3_reset(stmt_bind);

  if(Size > 3){
    int loops = 1 + (Data[3] % 4);
    for(int k = 0; k < loops; k++){
      int src_col = (Size > (size_t)(4 + k)) ? (Data[4 + k] % 3) : (k % 3);
      int dst_param = (k % 3) + 1;

      sqlite3_reset(stmt_select);
      sqlite3_clear_bindings(stmt_select);
      sqlite3_bind_int(stmt_select, 1, k % 4);

      if(sqlite3_step(stmt_select) == SQLITE_ROW){
        sqlite3_value *v = sqlite3_column_value(stmt_select, src_col);
        if(v){
          sqlite3_clear_bindings(stmt_bind);
          sqlite3_bind_value(stmt_bind, dst_param, v);
          (void)sqlite3_value_type(v);
          (void)sqlite3_value_numeric_type(v);
          (void)sqlite3_value_frombind(v);
          (void)sqlite3_value_nochange(v);
          sqlite3_step(stmt_bind);
          sqlite3_reset(stmt_bind);
        }
      }
    }
  }

  sqlite3_finalize(stmt_bind);
  sqlite3_finalize(stmt_select);
  sqlite3_close(db);
  return 0;
}