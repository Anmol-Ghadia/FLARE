/*
** AFL++/libFuzzer-style harness for fuzzing SQLite database images.
**
** The fuzz input is deserialized directly into the main database, so no
** filename or path is ever controlled by the fuzzer.
*/

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "sqlite3.h"

#define DB_FUZZ_PROGRESS_LIMIT 150000
#define DB_FUZZ_HEAP_LIMIT (64*1024*1024)
#define DB_FUZZ_MAX_SIZE (4*1024*1024)

typedef struct DbFuzzCtx DbFuzzCtx;
struct DbFuzzCtx {
  int nProgress;
};

static int dbFuzzProgress(void *pCtx){
  DbFuzzCtx *p = (DbFuzzCtx*)pCtx;
  p->nProgress++;
  return p->nProgress > DB_FUZZ_PROGRESS_LIMIT;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size){
  static const char *azSql[] = {
    "PRAGMA quick_check;",
    "PRAGMA foreign_key_check;",
    "SELECT * FROM sqlite_schema;",
    "SELECT count(*) FROM sqlite_schema;",
    "REINDEX;",
    "VACUUM;"
  };
  sqlite3 *db = 0;
  unsigned char *aDb = 0;
  DbFuzzCtx ctx;
  sqlite3_int64 szLimit;
  int iDummy = 0;
  int rc;
  size_t i;

  if( size==0 || size>DB_FUZZ_MAX_SIZE ) return 0;
  memset(&ctx, 0, sizeof(ctx));

  if( sqlite3_initialize()!=SQLITE_OK ) return 0;
  sqlite3_hard_heap_limit64(DB_FUZZ_HEAP_LIMIT);

  rc = sqlite3_open_v2(":memory:", &db,
                       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
  if( rc!=SQLITE_OK || db==0 ){
    sqlite3_close(db);
    return 0;
  }

  aDb = sqlite3_malloc64(size ? size : 1);
  if( aDb==0 ){
    sqlite3_close(db);
    return 0;
  }
  memcpy(aDb, data, size);

  sqlite3_progress_handler(db, 10, dbFuzzProgress, &ctx);
  sqlite3_enable_load_extension(db, 0);
  sqlite3_db_config(db, SQLITE_DBCONFIG_DEFENSIVE, 1, &iDummy);
  sqlite3_db_config(db, SQLITE_DBCONFIG_TRUSTED_SCHEMA, 0, &iDummy);
  sqlite3_exec(db, "PRAGMA temp_store=MEMORY;", 0, 0, 0);

  rc = sqlite3_deserialize(db, "main", aDb, (sqlite3_int64)size,
                           (sqlite3_int64)size,
                           SQLITE_DESERIALIZE_RESIZEABLE |
                           SQLITE_DESERIALIZE_FREEONCLOSE);
  if( rc!=SQLITE_OK ){
    sqlite3_free(aDb);
    sqlite3_close(db);
    return 0;
  }

  szLimit = DB_FUZZ_MAX_SIZE;
#ifdef SQLITE_FCNTL_SIZE_LIMIT
  sqlite3_file_control(db, "main", SQLITE_FCNTL_SIZE_LIMIT, &szLimit);
#endif

  for(i=0; i<sizeof(azSql)/sizeof(azSql[0]); i++){
    char *zErr = 0;
    ctx.nProgress = 0;
    sqlite3_exec(db, azSql[i], 0, 0, &zErr);
    sqlite3_free(zErr);
  }

  sqlite3_close(db);
  return 0;
}
