// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_realloc64 at sqlite3.c:15954:18 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_bind_blob at sqlite3.c:73315:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_blob_open at sqlite3.c:82675:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_blob_reopen at sqlite3.c:83034:16 in sqlite3.h
// sqlite3_blob_bytes at sqlite3.c:83019:16 in sqlite3.h
// sqlite3_blob_bytes at sqlite3.c:83019:16 in sqlite3.h
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

static sqlite3_int64 read_i64(const uint8_t *p, size_t n){
  sqlite3_int64 v = 0;
  size_t i;
  size_t lim = n < 8 ? n : 8;
  for(i = 0; i < lim; i++){
    v = (v << 8) | p[i];
  }
  return v;
}

static sqlite3_uint64 read_u64(const uint8_t *p, size_t n){
  sqlite3_uint64 v = 0;
  size_t i;
  size_t lim = n < 8 ? n : 8;
  for(i = 0; i < lim; i++){
    v = (v << 8) | p[i];
  }
  return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_blob *blob = NULL;
  sqlite3_stmt *stmt = NULL;
  void *mem = NULL;
  FILE *fp;
  int rc;
  const char *errmsg;
  sqlite3_int64 rowid = 1;
  sqlite3_uint64 realloc_sz = 0;
  int rnd_n = 0;
  unsigned char rndbuf[256];

  fp = fopen("./dummy_file", "wb");
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

  sqlite3_exec(db, "PRAGMA journal_mode=OFF;", NULL, NULL, NULL);
  sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);
  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(id INTEGER PRIMARY KEY, b BLOB);", NULL, NULL, NULL);
  sqlite3_exec(db, "DELETE FROM t;", NULL, NULL, NULL);

  rc = sqlite3_prepare_v2(db, "INSERT INTO t(id,b) VALUES(1,?1),(2,?2);", -1, &stmt, NULL);
  if(rc == SQLITE_OK && stmt){
    size_t half = Size / 2;
    sqlite3_bind_blob(stmt, 1, Data, (int)half, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 2, Data + half, (int)(Size - half), SQLITE_STATIC);
    sqlite3_step(stmt);
  }
  if(stmt){
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  rc = sqlite3_blob_open(db, "main", "t", "b", 1, 0, &blob);
  if(rc != SQLITE_OK){
    blob = NULL;
  }

  errmsg = sqlite3_errmsg(db);
  (void)errmsg;

  if(Size >= 8){
    rowid = read_i64(Data, 8);
  }
  rc = sqlite3_blob_reopen(blob, rowid);
  (void)rc;

  if(blob){
    (void)sqlite3_blob_bytes(blob);
  }else{
    (void)sqlite3_blob_bytes(NULL);
  }

  if(Size >= 16){
    realloc_sz = read_u64(Data + 8, 8);
  }else if(Size >= 8){
    realloc_sz = read_u64(Data, Size);
  }else{
    realloc_sz = Size;
  }
  realloc_sz &= 0xFFFFULL;

  mem = sqlite3_malloc64(1);
  if(mem){
    memset(mem, 0, 1);
  }
  mem = sqlite3_realloc64(mem, realloc_sz);
  if(mem && realloc_sz > 0){
    ((unsigned char *)mem)[0] ^= (unsigned char)(Size & 0xFF);
  }

  rnd_n = (int)(Size % (sizeof(rndbuf) + 1));
  sqlite3_randomness(rnd_n, rnd_n > 0 ? rndbuf : NULL);

  if(Size & 1){
    sqlite3_randomness(0, NULL);
  }
  if(Size & 2){
    sqlite3_randomness((int)(sizeof(rndbuf) / 2), rndbuf);
  }

  sqlite3_free(mem);
  if(blob){
    sqlite3_blob_close(blob);
  }
  sqlite3_close(db);
  return 0;
}