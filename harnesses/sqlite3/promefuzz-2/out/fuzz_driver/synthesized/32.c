// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_memory_used at sqlite3.c:15633:26 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

static int read_u32(const uint8_t *Data, size_t Size, size_t *Off){
  uint32_t v = 0;
  size_t i;
  for(i = 0; i < 4 && *Off < Size; i++, (*Off)++){
    v = (v << 8) | Data[*Off];
  }
  return (int)v;
}

int LLVMFuzzerTestOneInput_32(const uint8_t *Data, size_t Size){
  size_t off = 0;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  FILE *fp;
  int rc;
  int open_flags;
  int config_choice;
  int config_rc;
  sqlite3_int64 mem_used;

  fp = fopen("./dummy_file", "wb");
  if(fp != NULL){
    if(Size > 0){
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  open_flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
#ifdef SQLITE_OPEN_URI
  if(Size > 0 && (Data[0] & 1)) open_flags |= SQLITE_OPEN_URI;
#endif
#ifdef SQLITE_OPEN_MEMORY
  if(Size > 0 && (Data[0] & 2)) open_flags |= SQLITE_OPEN_MEMORY;
#endif
#ifdef SQLITE_OPEN_NOMUTEX
  if(Size > 0 && (Data[0] & 4)) open_flags |= SQLITE_OPEN_NOMUTEX;
#endif
#ifdef SQLITE_OPEN_FULLMUTEX
  if(Size > 0 && (Data[0] & 8)) open_flags |= SQLITE_OPEN_FULLMUTEX;
#endif

  rc = sqlite3_open_v2("./dummy_file", &db, open_flags, NULL);
  if(rc == SQLITE_OK && db != NULL){
    const char *sqls[] = {
      "CREATE TABLE IF NOT EXISTS t(a,b);",
      "INSERT INTO t(a,b) VALUES(1,'x');",
      "SELECT * FROM t;",
      "PRAGMA journal_mode=WAL;",
      "BEGIN; INSERT INTO t(a,b) VALUES(2,'y');"
    };
    size_t idx = (Size > 1) ? (Data[1] % (sizeof(sqls)/sizeof(sqls[0]))) : 0;
    sqlite3_prepare_v2(db, sqls[idx], -1, &stmt, NULL);
    if(stmt != NULL && Size > 2){
      int steps = (Data[2] % 4);
      while(steps-- > 0){
        int s = sqlite3_step(stmt);
        if(s != SQLITE_ROW && s != SQLITE_DONE){
          break;
        }
      }
    }
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  mem_used = sqlite3_memory_used();
  (void)mem_used;
  sqlite3_shutdown();

  config_choice = read_u32(Data, Size, &off) % 6;
  switch(config_choice){
    case 0:
      config_rc = sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
      break;
    case 1:
      config_rc = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
      break;
    case 2:
      config_rc = sqlite3_config(SQLITE_CONFIG_SERIALIZED);
      break;
    case 3:
      config_rc = sqlite3_config(SQLITE_CONFIG_MEMSTATUS, (Size > off) ? (int)(Data[off] & 1) : 0);
      break;
    case 4:
      config_rc = sqlite3_config(SQLITE_CONFIG_URI, (Size > off) ? (int)(Data[off] & 1) : 0);
      break;
    default:
      config_rc = sqlite3_config(SQLITE_CONFIG_COVERING_INDEX_SCAN, (Size > off) ? (int)(Data[off] & 1) : 0);
      break;
  }
  (void)config_rc;

  return 0;
}