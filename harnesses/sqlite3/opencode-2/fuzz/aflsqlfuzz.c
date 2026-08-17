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
** This is a libFuzzer-style SQL-input harness intended to be linked with
** AFL++'s libFuzzer-compatible driver.  The fuzz input is always treated as
** SQL text executed against a fixed in-memory database.  No filename or path
** is taken from the fuzz input.
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

#define AFL_SQL_MAX_INPUT       65536
#define AFL_SQL_MAX_PROGRESS    10000
#define AFL_SQL_MAX_ROWS        1000

typedef struct FuzzCtx FuzzCtx;
struct FuzzCtx {
  int nProgress;                    /* Progress-handler callback count */
  int nRow;                         /* Rows returned by sqlite3_exec() */
};

/* Abort long-running executions. */
static int fuzzProgress(void *pCtx){
  FuzzCtx *p = (FuzzCtx*)pCtx;
  p->nProgress++;
  return p->nProgress>AFL_SQL_MAX_PROGRESS;
}

/* Limit the amount of output materialized through sqlite3_exec(). */
static int fuzzExecCb(void *pCtx, int argc, char **argv, char **azCol){
  FuzzCtx *p = (FuzzCtx*)pCtx;
  (void)argc;
  (void)argv;
  (void)azCol;
  p->nRow++;
  return p->nRow>AFL_SQL_MAX_ROWS;
}

/* Keep the fuzz target away from filename and debug-output surfaces. */
static int fuzzAuthorizer(
  void *pUnused,
  int eCode,
  const char *zArg1,
  const char *zArg2,
  const char *zArg3,
  const char *zArg4
){
  (void)pUnused;
  (void)zArg2;
  (void)zArg3;
  (void)zArg4;
  if( eCode==SQLITE_ATTACH || eCode==SQLITE_DETACH ){
    return SQLITE_DENY;
  }
  if( eCode==SQLITE_PRAGMA && zArg1!=0 ){
    if( sqlite3_stricmp(zArg1, "data_store_directory")==0
     || sqlite3_stricmp(zArg1, "temp_store_directory")==0
     || sqlite3_stricmp(zArg1, "parser_trace")==0
     || sqlite3_strnicmp(zArg1, "vdbe_", 5)==0
    ){
      return SQLITE_DENY;
    }
  }
  return SQLITE_OK;
}

/* Seed a small schema so DDL, DML, and planner code all have something to hit. */
static void fuzzSetup(sqlite3 *db){
  sqlite3_exec(db,
    "PRAGMA temp_store=MEMORY;"
    "CREATE TABLE IF NOT EXISTS t1(a INTEGER PRIMARY KEY, b TEXT, c BLOB);"
    "CREATE TABLE IF NOT EXISTS t2(x INTEGER, y TEXT);"
    "CREATE INDEX IF NOT EXISTS t2x ON t2(x);"
    "CREATE VIEW IF NOT EXISTS v1 AS SELECT a, b FROM t1;"
    "INSERT OR IGNORE INTO t1(a, b, c) VALUES"
    "(1, 'one', x'01'),"
    "(2, 'two', x'0203'),"
    "(3, NULL, x'');"
    "INSERT OR IGNORE INTO t2(rowid, x, y) VALUES"
    "(1, 1, 'alpha'),"
    "(2, 2, 'beta'),"
    "(3, 3, 'gamma');",
    0, 0, 0
  );
}

int LLVMFuzzerInitialize(int *pArgc, char ***pArgv){
  (void)pArgc;
  (void)pArgv;
  sqlite3_initialize();
  sqlite3_hard_heap_limit64(67108864);
  return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *aData, size_t nByte){
  char *zSql = 0;
  sqlite3 *db = 0;
  FuzzCtx x;
  int rc;
  int iDbCfg = 0;

  if( nByte==0 ) return 0;
  if( nByte>AFL_SQL_MAX_INPUT ) nByte = AFL_SQL_MAX_INPUT;
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
  sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, 1, &iDbCfg);
#ifndef SQLITE_OMIT_LOAD_EXTENSION
  sqlite3_enable_load_extension(db, 0);
#endif
  sqlite3_set_authorizer(db, fuzzAuthorizer, 0);
#ifndef SQLITE_OMIT_PROGRESS_CALLBACK
  sqlite3_progress_handler(db, 1000, fuzzProgress, &x);
#endif
  sqlite3_limit(db, SQLITE_LIMIT_ATTACHED, 0);
  sqlite3_limit(db, SQLITE_LIMIT_LENGTH, 1<<20);
  sqlite3_limit(db, SQLITE_LIMIT_SQL_LENGTH, AFL_SQL_MAX_INPUT);
  sqlite3_limit(db, SQLITE_LIMIT_VDBE_OP, 25000);

  fuzzSetup(db);

  zSql = sqlite3_malloc64(nByte+1);
  if( zSql==0 ) goto end;
  memcpy(zSql, aData, nByte);
  zSql[nByte] = 0;

  sqlite3_exec(db, zSql, fuzzExecCb, &x, 0);

end:
  sqlite3_free(zSql);
  if( db ) sqlite3_close(db);
  return 0;
}
