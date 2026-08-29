// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_user_data at sqlite3.c:72704:18 in sqlite3.h
// sqlite3_result_text64 at sqlite3.c:72383:17 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_result_text16le at sqlite3.c:72418:17 in sqlite3.h
// sqlite3_result_text64 at sqlite3.c:72383:17 in sqlite3.h
// sqlite3_result_blob64 at sqlite3.c:72314:17 in sqlite3.h
// sqlite3_result_text16 at sqlite3.c:72400:17 in sqlite3.h
// sqlite3_result_pointer at sqlite3.c:72356:17 in sqlite3.h
// sqlite3_result_text64 at sqlite3.c:72383:17 in sqlite3.h
// sqlite3_result_blob64 at sqlite3.c:72314:17 in sqlite3.h
// sqlite3_result_pointer at sqlite3.c:72356:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

static void udf_target(sqlite3_context *ctx, int argc, sqlite3_value **argv){
  (void)argc;
  (void)argv;

  void *ud = sqlite3_user_data(ctx);
  const uint8_t *bytes = (const uint8_t *)ud;
  if(!bytes){
    static const char fallback[] = "x";
    sqlite3_result_text64(ctx, fallback, 1, SQLITE_STATIC, SQLITE_UTF8);
    return;
  }

  uint8_t mode = bytes[0];
  sqlite3_uint64 n64 = 0;
  if(bytes[1] & 1){
    n64 = ((sqlite3_uint64)0x80000000ULL) + (sqlite3_uint64)(bytes[2]);
  }else{
    n64 = (sqlite3_uint64)(bytes[2] % 32);
  }

  {
    char *p = (char *)sqlite3_malloc64((sqlite3_uint64)64);
    if(p){
      memcpy(p, bytes, 64);
      switch(mode % 6){
        case 0:
          sqlite3_result_text16le(ctx, p, (int)(bytes[3] % 32), sqlite3_free);
          break;
        case 1:
          sqlite3_result_text64(
              ctx, p, n64, sqlite3_free,
              (unsigned char)((bytes[4] % 4) == 0 ? SQLITE_UTF8 :
                              (bytes[4] % 4) == 1 ? SQLITE_UTF16LE :
                              (bytes[4] % 4) == 2 ? SQLITE_UTF16BE :
                                                    SQLITE_UTF16));
          break;
        case 2:
          sqlite3_result_blob64(ctx, p, n64, sqlite3_free);
          break;
        case 3:
          sqlite3_result_text16(ctx, p, (bytes[5] & 1) ? -1 : (int)(bytes[6] % 32), sqlite3_free);
          break;
        case 4:
          sqlite3_result_pointer(ctx, p, "fuzz.ptr", sqlite3_free);
          break;
        default:
          sqlite3_result_text64(ctx, p, (sqlite3_uint64)(bytes[7] % 16), sqlite3_free, SQLITE_UTF8);
          break;
      }
    }else{
      sqlite3_result_blob64(ctx, bytes, 0, SQLITE_STATIC);
    }
  }

  if((mode & 0x40) != 0){
    sqlite3_result_pointer(ctx, NULL, "null.ptr", NULL);
  }
}

int LLVMFuzzerTestOneInput_76(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *errmsg = NULL;
  int rc;

  if(Data == NULL){
    return 0;
  }

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if(fp){
      if(Size > 0){
        fwrite(Data, 1, Size, fp);
      }
      fclose(fp);
    }
  }

  rc = sqlite3_open(":memory:", &db);
  if(rc != SQLITE_OK || db == NULL){
    if(db) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db, "CREATE TABLE t(x);", NULL, NULL, NULL);
  sqlite3_exec(db, "INSERT INTO t VALUES(1);", NULL, NULL, NULL);

  {
    unsigned char userbuf[64];
    size_t copy_n = Size < sizeof(userbuf) ? Size : sizeof(userbuf);
    memset(userbuf, 0, sizeof(userbuf));
    if(copy_n > 0){
      memcpy(userbuf, Data, copy_n);
    }

    rc = sqlite3_create_function(
        db, "fuzzfunc", 1, SQLITE_UTF8, userbuf, udf_target, NULL, NULL);
    if(rc == SQLITE_OK){
      const char *sqls[] = {
        "SELECT fuzzfunc(x) FROM t;",
        "SELECT fuzzfunc(NULL);",
        "SELECT fuzzfunc(zeroblob(8));",
        "SELECT fuzzfunc('abc');"
      };
      size_t i;
      for(i = 0; i < sizeof(sqls)/sizeof(sqls[0]); i++){
        stmt = NULL;
        rc = sqlite3_prepare_v2(db, sqls[i], -1, &stmt, NULL);
        if(rc == SQLITE_OK && stmt){
          while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
          }
        }
        if(stmt){
          sqlite3_finalize(stmt);
          stmt = NULL;
        }
      }
    }
  }

  if(Size > 0){
    const char *tail = NULL;
    char *sql = (char *)malloc(Size + 1);
    if(sql){
      memcpy(sql, Data, Size);
      sql[Size] = '\0';
      stmt = NULL;
      rc = sqlite3_prepare_v2(db, sql, (int)Size, &stmt, &tail);
      if(rc == SQLITE_OK && stmt){
        while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
        }
      }
      if(stmt){
        sqlite3_finalize(stmt);
        stmt = NULL;
      }
      free(sql);
    }
  }

  sqlite3_exec(db, "SELECT readfile('./dummy_file');", NULL, NULL, &errmsg);
  if(errmsg){
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  sqlite3_close(db);
  return 0;
}