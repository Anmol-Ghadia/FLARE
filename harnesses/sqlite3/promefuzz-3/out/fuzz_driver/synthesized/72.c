// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare16 at sqlite3.c:119171:16 in sqlite3.h
// sqlite3_prepare16_v2 at sqlite3.c:119183:16 in sqlite3.h
// sqlite3_prepare16_v3 at sqlite3.c:119195:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_count at sqlite3.c:72903:16 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_decltype16 at sqlite3.c:73170:24 in sqlite3.h
// sqlite3_column_origin_name at sqlite3.c:73210:24 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_name16 at sqlite3.c:73147:24 in sqlite3.h
// sqlite3_column_decltype16 at sqlite3.c:73170:24 in sqlite3.h
// sqlite3_column_origin_name at sqlite3.c:73210:24 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

#ifndef SQLITE_PREPARE_PERSISTENT
#define SQLITE_PREPARE_PERSISTENT 0x01
#endif
#ifndef SQLITE_PREPARE_NORMALIZE
#define SQLITE_PREPARE_NORMALIZE 0x02
#endif
#ifndef SQLITE_PREPARE_NO_VTAB
#define SQLITE_PREPARE_NO_VTAB 0x04
#endif
#ifndef SQLITE_PREPARE_DONT_LOG
#define SQLITE_PREPARE_DONT_LOG 0x10
#endif

static unsigned int read_u32(const uint8_t *p, size_t n){
  unsigned int v = 0;
  size_t i;
  for(i = 0; i < n && i < 4; i++){
    v = (v << 8) | p[i];
  }
  return v;
}

static void write_dummy_file(const uint8_t *data, size_t size){
  FILE *fp = fopen("./dummy_file", "wb");
  if(!fp) return;
  if(size) fwrite(data, 1, size, fp);
  fclose(fp);
}

static void exec_sql(sqlite3 *db, const char *sql){
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if(errmsg) sqlite3_free(errmsg);
}

static void exercise_stmt(sqlite3_stmt *stmt, int idx_seed){
  int col_count, i;
  if(!stmt) return;

  sqlite3_step(stmt);
  col_count = sqlite3_column_count(stmt);

  for(i = -2; i < col_count + 2; i++){
    int idx = i;
    if(idx_seed & 1) idx = col_count - i;
    (void)sqlite3_column_name16(stmt, idx);
    (void)sqlite3_column_decltype16(stmt, idx);
    (void)sqlite3_column_origin_name(stmt, idx);
  }

  sqlite3_reset(stmt);
  sqlite3_step(stmt);

  for(i = 0; i < col_count; i++){
    (void)sqlite3_column_name16(stmt, i);
    (void)sqlite3_column_decltype16(stmt, i);
    (void)sqlite3_column_origin_name(stmt, i);
  }
}

static void *make_utf16_buffer(const uint8_t *data, size_t size, int *out_nbyte){
  size_t even_size;
  unsigned char *buf;

  if(size == 0){
    static const unsigned char fallback_sql[] = {
      'S',0,'E',0,'L',0,'E',0,'C',0,'T',0,' ',0,
      '*',0,' ',0,'F',0,'R',0,'O',0,'M',0,' ',0,
      't',0,'1',0,';',0,0,0
    };
    buf = (unsigned char *)malloc(sizeof(fallback_sql));
    if(!buf) return NULL;
    memcpy(buf, fallback_sql, sizeof(fallback_sql));
    *out_nbyte = (int)(sizeof(fallback_sql) - 2);
    return buf;
  }

  even_size = size & ~(size_t)1;
  buf = (unsigned char *)malloc(even_size + 2);
  if(!buf) return NULL;

  if(even_size){
    memcpy(buf, data, even_size);
  }
  buf[even_size] = 0;
  buf[even_size + 1] = 0;
  *out_nbyte = (int)even_size;
  return buf;
}

int LLVMFuzzerTestOneInput_72(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt1 = NULL, *stmt2 = NULL, *stmt3 = NULL;
  const void *tail1 = NULL, *tail2 = NULL, *tail3 = NULL;
  int rc;
  unsigned int flags = 0;
  int sql_nbyte = 0;
  void *sql16 = NULL;
  char *db_err = NULL;

  write_dummy_file(Data, Size);

  if(sqlite3_open("./dummy_file", &db) != SQLITE_OK){
    if(db) sqlite3_close(db);
    return 0;
  }

  exec_sql(db, "PRAGMA writable_schema=ON;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t1(a INTEGER, b TEXT, c REAL);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(x BLOB, y NUMERIC);");
  exec_sql(db, "DELETE FROM t1;");
  exec_sql(db, "DELETE FROM t2;");
  exec_sql(db, "INSERT INTO t1(a,b,c) VALUES (1,'one',1.5),(2,'two',2.5),(3,NULL,3.5);");
  exec_sql(db, "INSERT INTO t2(x,y) VALUES (X'010203',42),(X'',-1);");
  sqlite3_exec(db,
               "ATTACH DATABASE './dummy_file' AS aux;"
               "DETACH DATABASE aux;",
               NULL, NULL, &db_err);
  if(db_err) sqlite3_free(db_err);

  sql16 = make_utf16_buffer(Data, Size, &sql_nbyte);
  if(!sql16){
    sqlite3_close(db);
    return 0;
  }

  flags = read_u32(Data, Size) & (SQLITE_PREPARE_PERSISTENT |
                                  SQLITE_PREPARE_NORMALIZE |
                                  SQLITE_PREPARE_NO_VTAB |
                                  SQLITE_PREPARE_DONT_LOG);

  rc = sqlite3_prepare16(db, sql16, sql_nbyte, &stmt1, &tail1);
  (void)rc;
  (void)tail1;
  exercise_stmt(stmt1, (int)(flags ^ 0x11));

  rc = sqlite3_prepare16_v2(db, sql16, sql_nbyte, &stmt2, &tail2);
  (void)rc;
  (void)tail2;
  exercise_stmt(stmt2, (int)(flags ^ 0x22));

  rc = sqlite3_prepare16_v3(db, sql16, sql_nbyte, flags, &stmt3, &tail3);
  (void)rc;
  (void)tail3;
  exercise_stmt(stmt3, (int)(flags ^ 0x33));

  if(stmt1) sqlite3_finalize(stmt1);
  if(stmt2) sqlite3_finalize(stmt2);
  if(stmt3) sqlite3_finalize(stmt3);
  free(sql16);
  sqlite3_close(db);
  return 0;
}