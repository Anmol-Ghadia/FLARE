// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_value_text16le at sqlite3.c:72133:24 in sqlite3.h
// sqlite3_bind_value at sqlite3.c:73419:16 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_reset at sqlite3.c:72032:16 in sqlite3.h
// sqlite3_clear_bindings at sqlite3.c:72053:16 in sqlite3.h
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_value_text16le at sqlite3.c:72133:24 in sqlite3.h
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_value_text16le at sqlite3.c:72133:24 in sqlite3.h
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_value_text16le at sqlite3.c:72133:24 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_value_free at sqlite3.c:72261:17 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
// sqlite3_bind_int64 at sqlite3.c:73350:16 in sqlite3.h
// sqlite3_bind_double at sqlite3.c:73337:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_zeroblob at sqlite3.c:73450:16 in sqlite3.h
// sqlite3_bind_text at sqlite3.c:73387:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_column_value at sqlite3.c:73056:27 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
// sqlite3_value_dup at sqlite3.c:72238:27 in sqlite3.h
// sqlite3_value_text16 at sqlite3.c:72127:24 in sqlite3.h
// sqlite3_value_text16le at sqlite3.c:72133:24 in sqlite3.h
// sqlite3_bind_value at sqlite3.c:73419:16 in sqlite3.h
// sqlite3_bind_null at sqlite3.c:73360:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static uint32_t rd32(const uint8_t *p){
  return ((uint32_t)p[0]) |
         ((uint32_t)p[1] << 8) |
         ((uint32_t)p[2] << 16) |
         ((uint32_t)p[3] << 24);
}

static int exec_sql(sqlite3 *db, const char *sql){
  char *errmsg = 0;
  int rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
  if(errmsg) sqlite3_free(errmsg);
  return rc;
}

static sqlite3_value *make_value_from_sql(sqlite3 *db, const uint8_t *data, size_t size, uint8_t mode){
  sqlite3_stmt *stmt = 0;
  sqlite3_value *out = 0;
  const char *sql = "SELECT ?1";

  if(sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK || !stmt){
    return 0;
  }

  switch(mode % 7){
    case 0:
      sqlite3_bind_null(stmt, 1);
      break;
    case 1: {
      sqlite3_int64 v = 0;
      if(size >= 8){
        v = (sqlite3_int64)(
          ((uint64_t)data[0]) |
          ((uint64_t)data[1] << 8) |
          ((uint64_t)data[2] << 16) |
          ((uint64_t)data[3] << 24) |
          ((uint64_t)data[4] << 32) |
          ((uint64_t)data[5] << 40) |
          ((uint64_t)data[6] << 48) |
          ((uint64_t)data[7] << 56));
      }else if(size > 0){
        v = (sqlite3_int64)data[0];
      }
      sqlite3_bind_int64(stmt, 1, v);
      break;
    }
    case 2: {
      double d = 0.0;
      if(size >= sizeof(double)){
        memcpy(&d, data, sizeof(double));
      }else if(size > 0){
        d = (double)data[0];
      }
      sqlite3_bind_double(stmt, 1, d);
      break;
    }
    case 3:
      sqlite3_bind_text(stmt, 1, (const char *)data, (int)size, SQLITE_TRANSIENT);
      break;
    case 4:
      sqlite3_bind_blob(stmt, 1, data, (int)size, SQLITE_TRANSIENT);
      break;
    case 5: {
      int n = size ? (int)(data[0] % 64) : 0;
      sqlite3_bind_zeroblob(stmt, 1, n);
      break;
    }
    default:
      sqlite3_bind_text(stmt, 1, "", 0, SQLITE_STATIC);
      break;
  }

  if(sqlite3_step(stmt) == SQLITE_ROW){
    sqlite3_value *col = sqlite3_column_value(stmt, 0);
    out = sqlite3_value_dup(col);
  }

  sqlite3_finalize(stmt);
  return out;
}

static void exercise_value(sqlite3_stmt *bind_stmt, sqlite3_value *v, int idx){
  sqlite3_value *dup1 = sqlite3_value_dup(v);
  sqlite3_value *dup2 = sqlite3_value_dup(dup1);

  if(dup1){
    (void)sqlite3_value_text16(dup1);
    (void)sqlite3_value_text16le(dup1);
    (void)sqlite3_bind_value(bind_stmt, idx, dup1);
  }else{
    (void)sqlite3_bind_null(bind_stmt, idx);
  }

  if(dup2){
    (void)sqlite3_value_text16(dup2);
    (void)sqlite3_value_text16le(dup2);
    (void)sqlite3_bind_value(bind_stmt, idx, dup2);
  }

  sqlite3_value_free(dup2);
  sqlite3_value_free(dup1);

  {
    sqlite3_value *null_dup = sqlite3_value_dup(NULL);
    sqlite3_value_free(null_dup);
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db = 0;
  sqlite3_stmt *bind_stmt = 0;
  sqlite3_value *v1 = 0, *v2 = 0, *v3 = 0;
  int rc;

  FILE *fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if(rc != SQLITE_OK || !db){
    if(db) sqlite3_close(db);
    return 0;
  }

  exec_sql(db, "PRAGMA foreign_keys=OFF;");
  exec_sql(db, "CREATE TABLE t(a,b,c);");
  exec_sql(db, "INSERT INTO t VALUES(1,'x',X'00');");

  if(sqlite3_prepare_v2(db, "SELECT ?1, ?2, ?3", -1, &bind_stmt, 0) != SQLITE_OK || !bind_stmt){
    sqlite3_close(db);
    return 0;
  }

  v1 = make_value_from_sql(db, Data, Size, Size ? Data[0] : 0);
  v2 = make_value_from_sql(db, Data + (Size > 1 ? 1 : 0), Size > 1 ? Size - 1 : 0, Size > 1 ? Data[1] : 1);
  if(Size >= 4){
    uint32_t off = rd32(Data) % (uint32_t)(Size + 1);
    v3 = make_value_from_sql(db, Data + (off < Size ? off : Size), off < Size ? Size - off : 0, Size > 2 ? Data[2] : 2);
  }else{
    v3 = make_value_from_sql(db, Data, Size, 2);
  }

  exercise_value(bind_stmt, v1, 1);
  exercise_value(bind_stmt, v2, 2);
  exercise_value(bind_stmt, v3, 3);

  (void)sqlite3_step(bind_stmt);
  sqlite3_reset(bind_stmt);
  sqlite3_clear_bindings(bind_stmt);

  if(v1){
    (void)sqlite3_value_text16(v1);
    (void)sqlite3_value_text16le(v1);
  }
  if(v2){
    (void)sqlite3_value_text16(v2);
    (void)sqlite3_value_text16le(v2);
  }
  if(v3){
    (void)sqlite3_value_text16(v3);
    (void)sqlite3_value_text16le(v3);
  }

  sqlite3_value_free(v1);
  sqlite3_value_free(v2);
  sqlite3_value_free(v3);

  sqlite3_finalize(bind_stmt);
  sqlite3_close(db);
  return 0;
}