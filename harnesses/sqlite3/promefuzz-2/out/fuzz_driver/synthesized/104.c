// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_pointer at sqlite3.c:73369:16 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_bind_pointer at sqlite3.c:73369:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_bind_text16 at sqlite3.c:73409:16 in sqlite3.h
// sqlite3_bind_int at sqlite3.c:73347:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_int at sqlite3.c:73041:16 in sqlite3.h
// sqlite3_column_bytes at sqlite3.c:73026:16 in sqlite3.h
// sqlite3_column_blob at sqlite3.c:73016:24 in sqlite3.h
// sqlite3_column_text16 at sqlite3.c:73066:24 in sqlite3.h
// sqlite3_value_bytes16 at sqlite3.c:72094:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_value_text16le at sqlite3.c:72133:24 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_int at sqlite3.c:72100:16 in sqlite3.h
// sqlite3_value_pointer at sqlite3.c:72110:18 in sqlite3.h
// sqlite3_value_pointer at sqlite3.c:72110:18 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_value_blob at sqlite3.c:72078:24 in sqlite3.h
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_value_text16le at sqlite3.c:72133:24 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_bytes16 at sqlite3.c:72094:16 in sqlite3.h
// sqlite3_result_value at sqlite3.c:72428:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static void pointer_destructor(void *p){
  free(p);
}

static void fuzz_udf(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  int i;
  for(i = 0; i < argc; i++){
    sqlite3_value *v = argv[i];
    const void *b1 = sqlite3_value_blob(v);
    int n1 = sqlite3_value_bytes(v);
    const void *t16 = sqlite3_value_text16(v);
    int n16 = sqlite3_value_bytes16(v);
    const void *t16le = sqlite3_value_text16le(v);
    int iv = sqlite3_value_int(v);
    void *p1 = sqlite3_value_pointer(v, "fuzz.ptr");
    void *p2 = sqlite3_value_pointer(v, "other.ptr");

    if(b1 && n1 > 0){
      volatile unsigned char c = ((const unsigned char *)b1)[0];
      (void)c;
    }
    if(t16 && n16 >= 2){
      volatile unsigned char c = ((const unsigned char *)t16)[0];
      (void)c;
    }
    if(t16le && n16 >= 2){
      volatile unsigned char c = ((const unsigned char *)t16le)[0];
      (void)c;
    }
    if(p1 || p2){
      volatile uintptr_t up = (uintptr_t)(p1 ? p1 : p2);
      (void)up;
    }
    if(iv == 0x12345678){
      sqlite3_result_value(ctx, v);
      return;
    }

    (void)sqlite3_value_blob(v);
    (void)sqlite3_value_text16(v);
    (void)sqlite3_value_text16le(v);
    (void)sqlite3_value_bytes(v);
    (void)sqlite3_value_bytes16(v);
  }

  if(argc > 0){
    sqlite3_result_value(ctx, argv[argc - 1]);
  }else{
    sqlite3_result_null(ctx);
  }
}

static uint32_t read_u32(const uint8_t **data, size_t *size){
  uint32_t v = 0;
  size_t n = *size < 4 ? *size : 4;
  size_t i;
  for(i = 0; i < n; i++){
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static int exec_sql(sqlite3 *db, const char *sql){
  char *errmsg = NULL;
  int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if(errmsg) sqlite3_free(errmsg);
  return rc;
}

int LLVMFuzzerTestOneInput_104(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  const uint8_t *p = Data;
  size_t n = Size;
  int rc;
  uint32_t mode;
  int intval;
  double dval;
  char *sql = NULL;
  int sql_from_sqlite = 0;
  size_t sql_len;
  void *ptr_mem = NULL;

  FILE *fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if(rc != SQLITE_OK || db == NULL){
    if(db) sqlite3_close(db);
    return 0;
  }

  sqlite3_create_function(db, "fuzzfunc", -1, SQLITE_UTF8, NULL, fuzz_udf, NULL, NULL);

  exec_sql(db, "CREATE TABLE t(a,b,c,d);");
  exec_sql(db, "INSERT INTO t VALUES(1,'text',x'001122',NULL);");
  exec_sql(db, "INSERT INTO t VALUES(-1,'',x'',123456789);");

  mode = read_u32(&p, &n);

  if(n == 0){
    sql = sqlite3_mprintf("SELECT fuzzfunc(a,b,c,d) FROM t;");
    sql_from_sqlite = 1;
  }else{
    sql_len = n > 512 ? 512 : n;
    sql = (char *)malloc(sql_len + 1);
    if(!sql){
      sqlite3_close(db);
      return 0;
    }
    memcpy(sql, p, sql_len);
    sql[sql_len] = '\0';
    {
      size_t i;
      for(i = 0; i < sql_len; i++){
        if((unsigned char)sql[i] == 0) sql[i] = ' ';
      }
    }
    p += sql_len;
    n -= sql_len;
  }

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if(rc != SQLITE_OK || stmt == NULL){
    if(sql){
      if(sql_from_sqlite) sqlite3_free(sql);
      else free(sql);
    }
    sqlite3_close(db);
    return 0;
  }

  intval = (int)read_u32(&p, &n);
  dval = (double)(int32_t)read_u32(&p, &n) / 3.0;

  switch(mode % 8){
    case 0:
      sqlite3_bind_int(stmt, 1, intval);
      break;
    case 1:
      sqlite3_bind_text(stmt, 1, (const char *)p, (int)n, SQLITE_TRANSIENT);
      break;
    case 2:
      sqlite3_bind_blob(stmt, 1, p, (int)n, SQLITE_TRANSIENT);
      break;
    case 3:
      sqlite3_bind_double(stmt, 1, dval);
      break;
    case 4:
      sqlite3_bind_null(stmt, 1);
      break;
    case 5:
      ptr_mem = malloc(n ? n : 1);
      if(ptr_mem){
        if(n) memcpy(ptr_mem, p, n);
        sqlite3_bind_pointer(stmt, 1, ptr_mem, "fuzz.ptr", pointer_destructor);
        ptr_mem = NULL;
      }
      break;
    case 6:
      ptr_mem = malloc(n ? n : 1);
      if(ptr_mem){
        if(n) memcpy(ptr_mem, p, n);
        sqlite3_bind_pointer(stmt, 1, ptr_mem, "other.ptr", pointer_destructor);
        ptr_mem = NULL;
      }
      break;
    default:
      sqlite3_bind_text16(stmt, 1, p, (int)(n & ~(size_t)1), SQLITE_TRANSIENT);
      break;
  }

  sqlite3_bind_int(stmt, 2, intval ^ 0x55aa55aa);
  sqlite3_bind_double(stmt, 3, dval);
  sqlite3_bind_blob(stmt, 4, Data, (int)(Size > 64 ? 64 : Size), SQLITE_TRANSIENT);

  for(;;){
    rc = sqlite3_step(stmt);
    if(rc == SQLITE_ROW){
      int cols = sqlite3_column_count(stmt);
      int i;
      for(i = 0; i < cols; i++){
        (void)sqlite3_column_int(stmt, i);
        (void)sqlite3_column_bytes(stmt, i);
        (void)sqlite3_column_blob(stmt, i);
        (void)sqlite3_column_text16(stmt, i);
      }
      continue;
    }
    break;
  }

  sqlite3_finalize(stmt);
  if(sql){
    if(sql_from_sqlite) sqlite3_free(sql);
    else free(sql);
  }
  sqlite3_close(db);
  return 0;
}