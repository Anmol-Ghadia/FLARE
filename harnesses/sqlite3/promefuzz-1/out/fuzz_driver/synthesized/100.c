// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_auto_extension at sqlite3.c:114809:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_cancel_auto_extension at sqlite3.c:114856:16 in sqlite3.h
// sqlite3_threadsafe at sqlite3.c:152376:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_cancel_auto_extension at sqlite3.c:114856:16 in sqlite3.h
// sqlite3_cancel_auto_extension at sqlite3.c:114856:16 in sqlite3.h
// sqlite3_cancel_auto_extension at sqlite3.c:114856:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
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

static void (*pick_ext(uint8_t v))(void){
  switch(v % 3){
    case 0: return (void(*)(void))fuzz_ext_ok;
    case 1: return (void(*)(void))fuzz_ext_err;
    default: return (void(*)(void))fuzz_ext_toggle;
  }
}

int LLVMFuzzerTestOneInput_100(const uint8_t *Data, size_t Size){
  size_t idx = 0;
  int i;
  int rc = SQLITE_OK;
  int ts;
  int initialized = 0;
  sqlite3 *db = NULL;

  write_dummy_file(Data, Size);
  g_ext_mode = (Size > 0) ? Data[0] : 0;

  /* Start from a clean state. */
  (void)sqlite3_shutdown();

  for(i = 0; i < 16; i++){
    uint8_t op = (idx < Size) ? Data[idx++] : (uint8_t)i;

    switch(op % 10){
      case 0:
        rc = sqlite3_initialize();
        if( rc == SQLITE_OK ) initialized = 1;
        break;

      case 1:
        if( initialized ){
          (void)sqlite3_shutdown();
          initialized = 0;
        }
        break;

      case 2:
        if( !initialized ){
          rc = sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
          (void)rc;
        }
        break;

      case 3:
        if( !initialized ){
          rc = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
          (void)rc;
        }
        break;

      case 4:
        if( !initialized ){
          rc = sqlite3_config(SQLITE_CONFIG_SERIALIZED);
          (void)rc;
        }
        break;

      case 5: {
        void (*x)(void) = pick_ext(idx < Size ? Data[idx++] : 0);
        rc = sqlite3_auto_extension(x);
        if( rc == SQLITE_OK ) initialized = 1; /* auto_extension may initialize SQLite */
        break;
      }

      case 6: {
        void (*x)(void) = pick_ext(idx < Size ? Data[idx++] : 0);
        /* sqlite3_cancel_auto_extension() uses SQLite mutexes; ensure initialized. */
        if( !initialized ){
          rc = sqlite3_initialize();
          if( rc == SQLITE_OK ) initialized = 1;
        }
        if( initialized ){
          (void)sqlite3_cancel_auto_extension(x);
        }
        break;
      }

      case 7:
        ts = sqlite3_threadsafe();
        (void)ts;
        break;

      case 8:
        rc = sqlite3_initialize();
        if( rc == SQLITE_OK ){
          initialized = 1;
          if( sqlite3_open("./dummy_file", &db) == SQLITE_OK && db ){
            sqlite3_close(db);
            db = NULL;
          }else if( db ){
            sqlite3_close(db);
            db = NULL;
          }
        }
        break;

      case 9:
      default:
        rc = sqlite3_initialize();
        if( rc == SQLITE_OK ) initialized = 1;
        if( !initialized ){
          rc = sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
          (void)rc;
        }
        break;
    }
  }

  if( db ){
    sqlite3_close(db);
    db = NULL;
  }

  if( !initialized ){
    rc = sqlite3_initialize();
    if( rc == SQLITE_OK ) initialized = 1;
  }

  if( initialized ){
    (void)sqlite3_cancel_auto_extension((void(*)(void))fuzz_ext_ok);
    (void)sqlite3_cancel_auto_extension((void(*)(void))fuzz_ext_err);
    (void)sqlite3_cancel_auto_extension((void(*)(void))fuzz_ext_toggle);
    (void)sqlite3_shutdown();
  }

  return 0;
}