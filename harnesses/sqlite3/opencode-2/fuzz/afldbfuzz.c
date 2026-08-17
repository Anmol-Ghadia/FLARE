/*
** 2026-08-17
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
**
** This is a libFuzzer-style database-image harness intended to be linked with
** AFL++'s libFuzzer-compatible driver.  The fuzz input is deserialized into
** the main in-memory database directly, so no filename or path is ever fuzzed.
*/
#include <stddef.h>
#if !defined(_MSC_VER)
# include <stdint.h>
#endif
#include <string.h>
#include "sqlite3.h"

#if defined(_MSC_VER)
typedef unsigned char uint8_t;
#endif

#define AFL_DB_MAX_INPUT        1048576
#define AFL_DB_MAX_PROGRESS     10000
#define AFL_DB_MAX_ROWS         1000
#define AFL_DB_SIZE_LIMIT       4194304

static const char *azSql[] = {
  "PRAGMA integrity_check;",
  "PRAGMA quick_check;",
  "SELECT * FROM sqlite_schema;",
  "SELECT count(*) FROM sqlite_schema;",
  "SELECT sql FROM sqlite_schema ORDER BY name;",
  "CREATE TEMP TABLE IF NOT EXISTS tt(x);"
  "INSERT INTO tt VALUES(1);"
  "DELETE FROM tt;",
  "REINDEX;",
  "VACUUM;",
};

typedef struct FuzzCtx FuzzCtx;
struct FuzzCtx {
  int nProgress;                    /* Progress-handler callback count */
  int nRow;                         /* Rows returned by sqlite3_exec() */
};

/* Abort long-running executions. */
static int fuzzProgress(void *pCtx){
  FuzzCtx *p = (FuzzCtx*)pCtx;
  p->nProgress++;
  return p->nProgress>AFL_DB_MAX_PROGRESS;
}

/* Limit the amount of output materialized through sqlite3_exec(). */
static int fuzzExecCb(void *pCtx, int argc, char **argv, char **azCol){
  FuzzCtx *p = (FuzzCtx*)pCtx;
  (void)argc;
  (void)argv;
  (void)azCol;
  p->nRow++;
  return p->nRow>AFL_DB_MAX_ROWS;
}

int LLVMFuzzerInitialize(int *pArgc, char ***pArgv){
  (void)pArgc;
  (void)pArgv;
  sqlite3_initialize();
  sqlite3_hard_heap_limit64(67108864);
  return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *aData, size_t nByte){
  unsigned char *aDb = 0;
  sqlite3 *db = 0;
  sqlite3_int64 mxSize = AFL_DB_SIZE_LIMIT;
  FuzzCtx x;
  int i;
  int rc;
  int iDbCfg = 0;

  if( nByte==0 || nByte>AFL_DB_MAX_INPUT ) return 0;
  if( sqlite3_initialize() ) return 0;
  memset(&x, 0, sizeof(x));

  rc = sqlite3_open(":memory:", &db);
  if( rc!=SQLITE_OK ) goto end;

#ifdef SQLITE_DBCONFIG_DEFENSIVE
  sqlite3_db_config(db, SQLITE_DBCONFIG_DEFENSIVE, 1, &iDbCfg);
#endif
#ifdef SQLITE_DBCONFIG_TRUSTED_SCHEMA
  sqlite3_db_config(db, SQLITE_DBCONFIG_TRUSTED_SCHEMA, 0, &iDbCfg);
#endif
#ifndef SQLITE_OMIT_PROGRESS_CALLBACK
  sqlite3_progress_handler(db, 1000, fuzzProgress, &x);
#endif
  sqlite3_limit(db, SQLITE_LIMIT_LENGTH, 1<<20);
  sqlite3_limit(db, SQLITE_LIMIT_VDBE_OP, 25000);
  sqlite3_exec(db, "PRAGMA temp_store=MEMORY;", 0, 0, 0);

  aDb = sqlite3_malloc64(nByte);
  if( aDb==0 ) goto end;
  memcpy(aDb, aData, nByte);
  rc = sqlite3_deserialize(db, "main", aDb, nByte, nByte,
         SQLITE_DESERIALIZE_RESIZEABLE |
         SQLITE_DESERIALIZE_FREEONCLOSE);
  if( rc!=SQLITE_OK ) goto end;
  aDb = 0;

#ifdef SQLITE_FCNTL_SIZE_LIMIT
  sqlite3_file_control(db, "main", SQLITE_FCNTL_SIZE_LIMIT, &mxSize);
#endif

  for(i=0; i<(int)(sizeof(azSql)/sizeof(azSql[0])); i++){
    x.nRow = 0;
    sqlite3_exec(db, azSql[i], fuzzExecCb, &x, 0);
    if( x.nProgress>AFL_DB_MAX_PROGRESS ) break;
  }

end:
  sqlite3_free(aDb);
  if( db ) sqlite3_close(db);
  return 0;
}
