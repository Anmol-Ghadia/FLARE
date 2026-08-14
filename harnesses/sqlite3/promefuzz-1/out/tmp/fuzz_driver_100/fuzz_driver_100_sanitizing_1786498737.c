#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static int g_ext_mode = 0;

static int fuzz_ext_ok(sqlite3 *db, const char **pzErrMsg, const sqlite3_api_routines *pApi){
  (void)db;
  (void)pzErrMsg;
  (void)pApi;
  return SQLITE_OK;
}

static int fuzz_ext_err(sqlite3 *db, const char **pzErrMsg, const sqlite3_api_routines *pApi){
  (void)db;
  (void)pApi;
  if( pzErrMsg ){
    *pzErrMsg = sqlite3_mprintf("fuzz extension error");
  }
  return SQLITE_ERROR;
}

static int fuzz_ext_toggle(sqlite3 *db, const char **pzErrMsg, const sqlite3_api_routines *pApi){
  (void)db;
  (void)pApi;
  if( g_ext_mode & 1 ){
    if( pzErrMsg ){
      *pzErrMsg = sqlite3_mprintf("toggle error");
    }
    return SQLITE_ERROR;
  }
  return SQLITE_OK;
}

static void write_dummy_file(const uint8_t *Data, size_t Size){
  FILE *fp = fopen("./dummy_file", "wb");
  if( !fp ) return;
  if( Size ) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  size_t idx = 0;
  int i;
  sqlite3 *db = NULL;
  int rc;
  int ts;
  void (*exts[3])(void);

  write_dummy_file(Data, Size);

  exts[0] = (void(*)(void))fuzz_ext_ok;
  exts[1] = (void(*)(void))fuzz_ext_err;
  exts[2] = (void(*)(void))fuzz_ext_toggle;

  if( Size > 0 ){
    g_ext_mode = Data[0];
  }else{
    g_ext_mode = 0;
  }

  sqlite3_shutdown();

  for(i = 0; i < 8; i++){
    uint8_t op = (idx < Size) ? Data[idx++] : (uint8_t)i;
    switch(op % 10){
      case 0:
        (void)sqlite3_shutdown();
        break;

      case 1:
        rc = sqlite3_initialize();
        (void)rc;
        break;

      case 2:
        rc = sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
        (void)rc;
        break;

      case 3:
        rc = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
        (void)rc;
        break;

      case 4:
        rc = sqlite3_config(SQLITE_CONFIG_SERIALIZED);
        (void)rc;
        break;

      case 5: {
        void (*x)(void) = exts[(idx < Size ? Data[idx++] : 0) % 3];
        rc = sqlite3_auto_extension(x);
        (void)rc;
        break;
      }

      case 6: {
        void (*x)(void) = exts[(idx < Size ? Data[idx++] : 0) % 3];
        rc = sqlite3_cancel_auto_extension(x);
        (void)rc;
        break;
      }

      case 7:
        ts = sqlite3_threadsafe();
        (void)ts;
        break;

      case 8:
        rc = sqlite3_initialize();
        if( rc == SQLITE_OK ){
          sqlite3_open("./dummy_file", &db);
          if( db ){
            sqlite3_close(db);
            db = NULL;
          }
        }
        break;

      case 9:
      default:
        rc = sqlite3_initialize();
        (void)rc;
        rc = sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
        (void)rc;
        break;
    }
  }

  if( db ){
    sqlite3_close(db);
    db = NULL;
  }

  (void)sqlite3_cancel_auto_extension((void(*)(void))fuzz_ext_ok);
  (void)sqlite3_cancel_auto_extension((void(*)(void))fuzz_ext_err);
  (void)sqlite3_cancel_auto_extension((void(*)(void))fuzz_ext_toggle);
  (void)sqlite3_shutdown();

  return 0;
}