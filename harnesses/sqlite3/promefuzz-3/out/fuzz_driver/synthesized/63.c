// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_db_filename at sqlite3.c:156750:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_int64 at sqlite3.c:156693:26 in sqlite3.h
// sqlite3_uri_key at sqlite3.c:156670:24 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static uint32_t rd32(const uint8_t *p){
  return ((uint32_t)p[0]) |
         ((uint32_t)p[1] << 8) |
         ((uint32_t)p[2] << 16) |
         ((uint32_t)p[3] << 24);
}

static sqlite3_int64 rd64(const uint8_t *p){
  uint64_t v = 0;
  for(int i = 0; i < 8; i++){
    v |= ((uint64_t)p[i]) << (8 * i);
  }
  return (sqlite3_int64)v;
}

static size_t bounded_copy(char *dst, size_t dstsz, const uint8_t *src, size_t n){
  size_t m;
  if(dstsz == 0) return 0;
  m = n < (dstsz - 1) ? n : (dstsz - 1);
  if(m > 0) memcpy(dst, src, m);
  dst[m] = '\0';
  return m;
}

int LLVMFuzzerTestOneInput_63(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *created = NULL;
  FILE *fp = NULL;
  int rc;

  if(Size == 0){
    sqlite3_uri_parameter(NULL, "x");
    sqlite3_uri_int64(NULL, "x", 0);
    sqlite3_uri_key(NULL, 0);
    return 0;
  }

  fp = fopen("./dummy_file", "wb");
  if(fp){
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  {
    int use_uri = (Data[0] & 1);
    int use_memory = (Data[0] & 2);
    int use_temp = (Data[0] & 4);
    const char *open_name = "./dummy_file";

    if(use_memory){
      open_name = ":memory:";
    }else if(use_temp){
      open_name = "";
    }else if(use_uri){
      open_name = "file:./dummy_file?mode=rwc&cache=shared&psow=1&_journal=testj&_wal=testw";
    }

    rc = sqlite3_open_v2(open_name, &db,
                         SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
                         NULL);
    if(rc != SQLITE_OK){
      if(db) sqlite3_close(db);
      db = NULL;
    }
  }

  if(db){
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(x);", NULL, NULL, NULL);
    sqlite3_exec(db, "ATTACH DATABASE 'file:./dummy_file?mode=rwc&cache=private&k1=v1&k2=0x10' AS aux;", NULL, NULL, NULL);
    sqlite3_exec(db, "ATTACH DATABASE ':memory:' AS memdb;", NULL, NULL, NULL);
    sqlite3_exec(db, "ATTACH DATABASE '' AS tempish;", NULL, NULL, NULL);
    sqlite3_prepare_v2(db, "PRAGMA database_list;", -1, &stmt, NULL);
    if(stmt){
      while(sqlite3_step(stmt) == SQLITE_ROW){}
      sqlite3_finalize(stmt);
      stmt = NULL;
    }

    {
      char dbname1[64], dbname2[64], param1[64], param2[64];
      const char *fn;
      sqlite3_int64 defv = 0;
      size_t off = 1;
      size_t rem = Size > off ? Size - off : 0;

      if(Size >= 8){
        defv = rd64(Data);
      }

      bounded_copy(dbname1, sizeof(dbname1), rem ? Data + off : (const uint8_t *)"", rem > 20 ? 20 : rem);
      off += rem > 20 ? 20 : rem;
      rem = Size > off ? Size - off : 0;
      bounded_copy(dbname2, sizeof(dbname2), rem ? Data + off : (const uint8_t *)"", rem > 20 ? 20 : rem);
      off += rem > 20 ? 20 : rem;
      rem = Size > off ? Size - off : 0;
      bounded_copy(param1, sizeof(param1), rem ? Data + off : (const uint8_t *)"", rem > 20 ? 20 : rem);
      off += rem > 20 ? 20 : rem;
      rem = Size > off ? Size - off : 0;
      bounded_copy(param2, sizeof(param2), rem ? Data + off : (const uint8_t *)"", rem > 20 ? 20 : rem);

      fn = sqlite3_db_filename(db, "main");
      if(fn){
        sqlite3_uri_parameter(fn, "mode");
        sqlite3_uri_parameter(fn, "cache");
        sqlite3_uri_parameter(fn, param1);
        sqlite3_uri_int64(fn, "psow", defv);
        sqlite3_uri_int64(fn, param2, defv);
        sqlite3_uri_key(fn, 0);
        sqlite3_uri_key(fn, 1);
        sqlite3_uri_key(fn, 2);
        sqlite3_filename_journal(fn);
      }

      fn = sqlite3_db_filename(db, "aux");
      if(fn){
        sqlite3_uri_parameter(fn, "k1");
        sqlite3_uri_parameter(fn, "k2");
        sqlite3_uri_parameter(fn, param1);
        sqlite3_uri_int64(fn, "k2", defv);
        sqlite3_uri_int64(fn, param2, defv);
        sqlite3_uri_key(fn, 0);
        sqlite3_uri_key(fn, 1);
        sqlite3_uri_key(fn, 3);
        sqlite3_filename_journal(fn);
      }

      fn = sqlite3_db_filename(db, "memdb");
      if(fn && fn[0] != '\0'){
        sqlite3_uri_parameter(fn, "x");
        sqlite3_uri_int64(fn, "x", defv);
        sqlite3_uri_key(fn, 0);
        sqlite3_filename_journal(fn);
      }

      fn = sqlite3_db_filename(db, "temp");
      if(fn && fn[0] != '\0'){
        sqlite3_uri_parameter(fn, "x");
        sqlite3_uri_int64(fn, "x", defv);
        sqlite3_uri_key(fn, 0);
        sqlite3_filename_journal(fn);
      }

      fn = sqlite3_db_filename(db, dbname1);
      if(fn && fn[0] != '\0'){
        sqlite3_uri_parameter(fn, param1);
        sqlite3_uri_int64(fn, param2, defv);
        sqlite3_uri_key(fn, 0);
        sqlite3_filename_journal(fn);
      }else{
        sqlite3_uri_parameter(fn, param1);
        sqlite3_uri_int64(fn, param2, defv);
        sqlite3_uri_key(fn, -1);
      }

      fn = sqlite3_db_filename(db, dbname2);
      if(fn && fn[0] != '\0'){
        sqlite3_uri_parameter(fn, param2);
        sqlite3_uri_int64(fn, param1, defv);
        sqlite3_uri_key(fn, 1);
        sqlite3_filename_journal(fn);
      }else{
        sqlite3_uri_parameter(fn, param1);
        sqlite3_uri_int64(fn, param2, defv);
        sqlite3_uri_key(fn, -1);
      }
    }
  }

  {
    char zDb[128], zJ[128], zW[128];
    char k1[64], v1[64], k2[64], v2[64];
    const char *params[4];
    size_t q = 0;

    if(Size >= 4){
      q = rd32(Data) % Size;
    }

    bounded_copy(zDb, sizeof(zDb), Data + q, Size - q);
    bounded_copy(zJ, sizeof(zJ), Data, Size > 40 ? 40 : Size);
    bounded_copy(zW, sizeof(zW), Data + (Size / 3), Size - (Size / 3));
    bounded_copy(k1, sizeof(k1), Data, Size > 15 ? 15 : Size);
    bounded_copy(v1, sizeof(v1), Data + (Size / 4), Size - (Size / 4));
    bounded_copy(k2, sizeof(k2), Data + (Size / 2), Size - (Size / 2));
    bounded_copy(v2, sizeof(v2), Data + ((Size * 3) / 4), Size - ((Size * 3) / 4));

    if(k1[0] == '\0') strcpy(k1, "a");
    if(k2[0] == '\0') strcpy(k2, "b");

    params[0] = k1;
    params[1] = v1;
    params[2] = k2;
    params[3] = v2;

    created = sqlite3_create_filename(zDb, zJ, zW, 2, params);
    if(created){
      sqlite3_uri_parameter(created, k1);
      sqlite3_uri_parameter(created, k2);
      sqlite3_uri_parameter(created, "missing");
      sqlite3_uri_int64(created, k1, 123);
      sqlite3_uri_int64(created, k2, -456);
      sqlite3_uri_key(created, 0);
      sqlite3_uri_key(created, 1);
      sqlite3_uri_key(created, 2);
      sqlite3_filename_journal(created);
      sqlite3_free_filename(created);
    }
  }

  sqlite3_uri_parameter(NULL, "nulltest");
  sqlite3_uri_int64(NULL, "nulltest", 777);
  sqlite3_uri_key(NULL, 0);

  if(db){
    sqlite3_exec(db, "DETACH DATABASE aux;", NULL, NULL, NULL);
    sqlite3_exec(db, "DETACH DATABASE memdb;", NULL, NULL, NULL);
    sqlite3_exec(db, "DETACH DATABASE tempish;", NULL, NULL, NULL);
    sqlite3_close(db);
  }

  return 0;
}