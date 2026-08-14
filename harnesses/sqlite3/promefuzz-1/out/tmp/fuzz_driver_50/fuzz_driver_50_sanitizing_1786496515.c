#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static void bind_by_tag(sqlite3_stmt *stmt, int idx, const uint8_t *data, size_t size, uint8_t tag){
  switch(tag % 6){
    case 0:
      sqlite3_bind_null(stmt, idx);
      break;
    case 1: {
      sqlite3_int64 v = 0;
      size_t n = size < sizeof(v) ? size : sizeof(v);
      memcpy(&v, data, n);
      sqlite3_bind_int64(stmt, idx, v);
      break;
    }
    case 2: {
      double d = 0.0;
      size_t n = size < sizeof(d) ? size : sizeof(d);
      memcpy(&d, data, n);
      sqlite3_bind_double(stmt, idx, d);
      break;
    }
    case 3:
      sqlite3_bind_text(stmt, idx, (const char *)data, (int)size, SQLITE_TRANSIENT);
      break;
    case 4:
      sqlite3_bind_blob(stmt, idx, data, (int)size, SQLITE_TRANSIENT);
      break;
    default: {
      int n = 0;
      if(size >= 4){
        memcpy(&n, data, 4);
      }else if(size > 0){
        n = data[0];
      }
      if(n < 0) n = -n;
      n %= 4096;
      sqlite3_bind_zeroblob(stmt, idx, n);
      break;
    }
  }
}

static void exercise_value(sqlite3_stmt *stmt, int col){
  sqlite3_value *v = sqlite3_column_value(stmt, col);
  if(v == NULL) return;

  (void)sqlite3_value_bytes(v);
  (void)sqlite3_value_bytes16(v);
  (void)sqlite3_value_type(v);
  (void)sqlite3_value_frombind(v);
  (void)sqlite3_value_nochange(v);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  sqlite3_stmt *src = NULL;
  int rc;
  size_t off = 0;
  int i;

  (void)remove("./dummy_file");
  {
    FILE *fp = fopen("./dummy_file", "wb");
    if(fp != NULL){
      if(Size > 0) fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  rc = sqlite3_open(":memory:", &db);
  if(rc != SQLITE_OK || db == NULL){
    if(db) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db,
               "CREATE TABLE t(a,b,c,d,e);"
               "INSERT INTO t VALUES(NULL,123,4.5,'txt',x'010203');",
               NULL, NULL, NULL);

  rc = sqlite3_prepare_v2(db, "SELECT ?1, ?2, ?3, ?4, ?5", -1, &stmt, NULL);
  if(rc == SQLITE_OK && stmt != NULL){
    for(i = 1; i <= 5; i++){
      uint8_t tag = 0;
      size_t remain = off < Size ? (Size - off) : 0;
      size_t chunk = 0;

      if(remain > 0){
        tag = Data[off++];
        remain--;
        if(remain > 0){
          chunk = (size_t)(tag % (remain + 1));
          if(chunk > remain) chunk = remain;
        }
      }

      bind_by_tag(stmt, i, Data + off, chunk, tag);
      off += chunk;
    }

    if(sqlite3_step(stmt) == SQLITE_ROW){
      int cols = sqlite3_column_count(stmt);
      for(i = 0; i < cols; i++){
        exercise_value(stmt, i);
      }
    }
    sqlite3_reset(stmt);

    rc = sqlite3_prepare_v2(db, "SELECT ?1", -1, &src, NULL);
    if(rc == SQLITE_OK && src != NULL){
      bind_by_tag(src, 1, Data, Size, Size ? Data[0] : 0);
      if(sqlite3_step(src) == SQLITE_ROW){
        sqlite3_value *v = sqlite3_column_value(src, 0);
        if(v != NULL){
          (void)sqlite3_bind_value(stmt, 1, v);
        }
        exercise_value(src, 0);
      }
      sqlite3_finalize(src);
      src = NULL;
    }

    rc = sqlite3_prepare_v2(db,
                            "SELECT 42, 3.14159, 'hello', x'001122', zeroblob(8), NULL",
                            -1, &src, NULL);
    if(rc == SQLITE_OK && src != NULL){
      if(sqlite3_step(src) == SQLITE_ROW){
        int cols = sqlite3_column_count(src);
        for(i = 0; i < cols; i++){
          sqlite3_value *v = sqlite3_column_value(src, i);
          if(v != NULL){
            if(i < 5){
              (void)sqlite3_bind_value(stmt, i + 1, v);
            }
            exercise_value(src, i);
          }
        }
      }
      sqlite3_finalize(src);
      src = NULL;
    }

    if(sqlite3_step(stmt) == SQLITE_ROW){
      int cols = sqlite3_column_count(stmt);
      for(i = 0; i < cols; i++){
        exercise_value(stmt, i);
      }
    }
  }

  if(src) sqlite3_finalize(src);
  if(stmt) sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}