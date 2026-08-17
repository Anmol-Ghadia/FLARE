/*
** AFL++/libFuzzer-style harness for fuzzing SQLite SQL text handling.
**
** Compile this file with a libFuzzer-compatible driver (for example,
** AFL++'s libAFLDriver) and link it against SQLite. The harness never lets
** the input control a filename or path; all work happens on an in-memory
** database.
*/

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "sqlite3.h"

#define SQL_FUZZ_PROGRESS_LIMIT 100000
#define SQL_FUZZ_ROW_LIMIT 128
#define SQL_FUZZ_HEAP_LIMIT (64*1024*1024)

typedef struct SqlFuzzCtx SqlFuzzCtx;
struct SqlFuzzCtx {
  int nProgress;
  int nRows;
};

static int sqlFuzzProgress(void *pCtx){
  SqlFuzzCtx *p = (SqlFuzzCtx*)pCtx;
  p->nProgress++;
  return p->nProgress > SQL_FUZZ_PROGRESS_LIMIT;
}

static int sqlFuzzRow(void *pCtx, int argc, char **argv, char **azCol){
  SqlFuzzCtx *p = (SqlFuzzCtx*)pCtx;
  int i;
  (void)azCol;
  if( argv ){
    for(i=0; i<argc; i++){
      sqlite3_free(sqlite3_mprintf("%s", argv[i]));
    }
  }
  p->nRows++;
  return p->nRows > SQL_FUZZ_ROW_LIMIT;
}

static int sqlFuzzAuthorizer(
  void *pUnused,
  int eAction,
  const char *zArg1,
  const char *zArg2,
  const char *zArg3,
  const char *zArg4
){
  (void)pUnused;
  (void)zArg2;
  (void)zArg3;
  (void)zArg4;
  if( eAction==SQLITE_ATTACH ) return SQLITE_DENY;
  if( eAction==SQLITE_PRAGMA && zArg1!=0 ){
    if( sqlite3_stricmp(zArg1, "temp_store_directory")==0
     || sqlite3_stricmp(zArg1, "data_store_directory")==0
     || sqlite3_stricmp(zArg1, "parser_trace")==0
     || sqlite3_strnicmp(zArg1, "vdbe_", 5)==0
    ){
      return SQLITE_DENY;
    }
  }
  return SQLITE_OK;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size){
  sqlite3 *db = 0;
  char *zSql = 0;
  char *zErr = 0;
  SqlFuzzCtx ctx;
  unsigned selector = 0;
  int rc;
  int iDummy = 0;

  if( size==0 ) return 0;
  memset(&ctx, 0, sizeof(ctx));

  selector = data[0];
  data++;
  size--;

  if( sqlite3_initialize()!=SQLITE_OK ) return 0;
  sqlite3_hard_heap_limit64(SQL_FUZZ_HEAP_LIMIT);

  rc = sqlite3_open_v2(":memory:", &db,
                       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
  if( rc!=SQLITE_OK || db==0 ){
    sqlite3_close(db);
    return 0;
  }

  sqlite3_progress_handler(db, 10, sqlFuzzProgress, &ctx);
  sqlite3_set_authorizer(db, sqlFuzzAuthorizer, 0);
  sqlite3_enable_load_extension(db, 0);
  sqlite3_limit(db, SQLITE_LIMIT_LENGTH, 1<<20);
  sqlite3_limit(db, SQLITE_LIMIT_SQL_LENGTH, 1<<20);
  sqlite3_limit(db, SQLITE_LIMIT_VDBE_OP, 25000);
  sqlite3_limit(db, SQLITE_LIMIT_COMPOUND_SELECT, 32);
  sqlite3_db_config(db, SQLITE_DBCONFIG_DEFENSIVE, 1, &iDummy);
  sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, selector & 1, &iDummy);
  sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_TRIGGER, (selector >> 1) & 1,
                    &iDummy);
  sqlite3_db_config(db, SQLITE_DBCONFIG_TRUSTED_SCHEMA, 0, &iDummy);
  sqlite3_exec(db, "PRAGMA temp_store=MEMORY;", 0, 0, 0);

  zSql = sqlite3_mprintf("%.*s", (int)size, data);
  if( zSql!=0 ){
#ifndef SQLITE_OMIT_COMPLETE
    sqlite3_complete(zSql);
#endif
    sqlite3_exec(db, zSql, sqlFuzzRow, &ctx, &zErr);
  }

  sqlite3_free(zErr);
  sqlite3_free(zSql);
  sqlite3_close(db);
  return 0;
}
