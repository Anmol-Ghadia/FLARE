#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

static sqlite3_int64 read_i64(const uint8_t *p){
  sqlite3_uint64 v = 0;
  size_t i;
  for(i = 0; i < 8; i++){
    v = (v << 8) | p[i];
  }
  return (sqlite3_int64)v;
}

static sqlite3_uint64 read_u64(const uint8_t *p){
  sqlite3_uint64 v = 0;
  size_t i;
  for(i = 0; i < 8; i++){
    v = (v << 8) | p[i];
  }
  return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_blob *blob = NULL;
  void *mem = NULL;
  unsigned char rndbuf[256];
  int rc;
  sqlite3_int64 rowid = 1;
  sqlite3_uint64 realloc_sz = 0;
  int rnd_n = 0;
  size_t pos = 0;
  const char *errmsg;
  int blob_sz;

  FILE *fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size > 0){
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if(rc != SQLITE_OK || db == NULL){
    if(db){
      sqlite3_close(db);
    }
    return 0;
  }

  sqlite3_exec(db,
               "CREATE TABLE IF NOT EXISTS t(id INTEGER PRIMARY KEY, b BLOB);",
               NULL, NULL, NULL);
  sqlite3_exec(db,
               "DELETE FROM t;",
               NULL, NULL, NULL);

  {
    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, "INSERT INTO t(id,b) VALUES(?,?);", -1, &stmt, NULL);
    if(rc == SQLITE_OK && stmt){
      sqlite3_bind_int64(stmt, 1, 1);
      sqlite3_bind_blob(stmt, 2, Data, (int)Size, SQLITE_STATIC);
      sqlite3_step(stmt);
      sqlite3_reset(stmt);

      sqlite3_bind_int64(stmt, 1, 2);
      if(Size > 1){
        sqlite3_bind_blob(stmt, 2, Data + 1, (int)(Size - 1), SQLITE_STATIC);
      }else{
        sqlite3_bind_blob(stmt, 2, "", 0, SQLITE_STATIC);
      }
      sqlite3_step(stmt);
    }
    if(stmt){
      sqlite3_finalize(stmt);
    }
  }

  rc = sqlite3_blob_open(db, "main", "t", "b", 1, 0, &blob);
  if(rc != SQLITE_OK){
    blob = NULL;
  }

  if(Size >= 8){
    rowid = read_i64(Data);
    pos = 8;
  }
  if(Size >= pos + 8){
    realloc_sz = read_u64(Data + pos);
    pos += 8;
  }
  if(Size > pos){
    rnd_n = (int)(Data[pos] % (sizeof(rndbuf) + 1));
  }

  errmsg = sqlite3_errmsg(db);
  (void)errmsg;

  if(blob){
    sqlite3_blob_reopen(blob, rowid);
    blob_sz = sqlite3_blob_bytes(blob);
  }else{
    blob_sz = 0;
  }

  if(blob_sz < 0) blob_sz = 0;
  if(realloc_sz > (sqlite3_uint64)(blob_sz + 1024)){
    realloc_sz = (sqlite3_uint64)(blob_sz + 1024);
  }

  mem = sqlite3_realloc64(NULL, realloc_sz);

  if(mem && rnd_n > 0){
    memset(mem, 0, (size_t)((realloc_sz < (sqlite3_uint64)rnd_n) ? realloc_sz : (sqlite3_uint64)rnd_n));
  }

  sqlite3_randomness(rnd_n, rnd_n > 0 ? rndbuf : NULL);

  if(mem){
    sqlite3_free(mem);
  }
  if(blob){
    sqlite3_blob_close(blob);
  }
  sqlite3_close(db);
  return 0;
}