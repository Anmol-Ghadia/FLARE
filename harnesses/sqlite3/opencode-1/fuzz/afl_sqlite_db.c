/*
** libFuzzer-style database-image harness for SQLite.
**
** Intended to be built with an AFL++ libFuzzer-compatible driver.
** The database image is deserialized into memory and exercised only with
** fixed SQL so that no fuzzed filename or path is ever used.
*/
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "sqlite3.h"

#define DB_HEAP_LIMIT ((sqlite3_int64)32 * 1024 * 1024)
#define DB_IMAGE_LIMIT (1024 * 1024)
#define DB_TEXT_LIMIT 65536
#define DB_VDBE_OP_LIMIT 25000
#define DB_PROGRESS_LIMIT 250000
#define DB_ROW_LIMIT 128

typedef struct DbFuzzCtx DbFuzzCtx;
struct DbFuzzCtx {
  int nProgress;
  unsigned int nRowBudget;
};

static const char *azDbSql[] = {
  "PRAGMA quick_check(1);",
  "PRAGMA integrity_check(1);",
  "SELECT * FROM sqlite_schema;",
  "SELECT name, type FROM sqlite_schema ORDER BY 1;",
  "ANALYZE;",
  "REINDEX;",
  "BEGIN IMMEDIATE; ROLLBACK;",
  "CREATE TEMP TABLE IF NOT EXISTS fuzz_temp(x);"
  "INSERT INTO fuzz_temp VALUES(1);"
  "DELETE FROM fuzz_temp;"
};

static int dbProgressHandler(void *pArg){
  DbFuzzCtx *p = (DbFuzzCtx*)pArg;
  p->nProgress++;
  return p->nProgress>DB_PROGRESS_LIMIT;
}

static int dbExecCallback(void *pArg, int argc, char **argv, char **azCol){
  DbFuzzCtx *p = (DbFuzzCtx*)pArg;
  int i;
  (void)azCol;
  if( p->nRowBudget==0 ) return 1;
  p->nRowBudget--;
  for(i=0; i<argc; i++){
    if( argv[i]!=0 ) sqlite3_free(sqlite3_mprintf("%s", argv[i]));
  }
  return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size){
#ifndef SQLITE_OMIT_DESERIALIZE
  sqlite3 *db = 0;
  unsigned char *aDb;
  DbFuzzCtx cx;
  sqlite3_int64 iMax;
  int rc;
  unsigned int i;

  memset(&cx, 0, sizeof(cx));
  if( size>DB_IMAGE_LIMIT ) size = DB_IMAGE_LIMIT;

  if( sqlite3_initialize()!=SQLITE_OK ) return 0;
  sqlite3_hard_heap_limit64(DB_HEAP_LIMIT);
  rc = sqlite3_open(":memory:", &db);
  if( rc!=SQLITE_OK ){
    sqlite3_close(db);
    return 0;
  }

  aDb = sqlite3_malloc64(size>0 ? size : 1);
  if( aDb==0 ){
    sqlite3_close(db);
    return 0;
  }
  if( size>0 ) memcpy(aDb, data, size);

  /* Force rollback-journal mode so malformed WAL headers do not depend
  ** on sidecar files that are not part of the fuzz input. */
  if( size>=20 && aDb[18]==2 && aDb[19]==2 ){
    aDb[18] = 1;
    aDb[19] = 1;
  }

  rc = sqlite3_deserialize(
    db,
    "main",
    aDb,
    (sqlite3_int64)size,
    (sqlite3_int64)size,
    SQLITE_DESERIALIZE_FREEONCLOSE | SQLITE_DESERIALIZE_RESIZEABLE
  );
  if( rc!=SQLITE_OK ){
    sqlite3_close(db);
    return 0;
  }

  iMax = DB_IMAGE_LIMIT;
#ifdef SQLITE_FCNTL_SIZE_LIMIT
  sqlite3_file_control(db, "main", SQLITE_FCNTL_SIZE_LIMIT, &iMax);
#endif
  sqlite3_exec(db, "PRAGMA temp_store=MEMORY", 0, 0, 0);
  sqlite3_limit(db, SQLITE_LIMIT_LENGTH, DB_TEXT_LIMIT);
  sqlite3_limit(db, SQLITE_LIMIT_VDBE_OP, DB_VDBE_OP_LIMIT);
  sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, 0, &rc);
#ifndef SQLITE_OMIT_PROGRESS_CALLBACK
  sqlite3_progress_handler(db, 10, dbProgressHandler, &cx);
#endif

  for(i=0; i<sizeof(azDbSql)/sizeof(azDbSql[0]); i++){
    cx.nProgress = 0;
    cx.nRowBudget = DB_ROW_LIMIT;
    sqlite3_exec(db, azDbSql[i], dbExecCallback, &cx, 0);
  }

  sqlite3_close(db);
#else
  (void)data;
  (void)size;
#endif
  return 0;
}
