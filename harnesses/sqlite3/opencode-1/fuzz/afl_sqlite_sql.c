/*
** libFuzzer-style SQL harness for SQLite.
**
** Intended to be built with an AFL++ libFuzzer-compatible driver.
** The harness keeps all state in memory and blocks SQL that would fuzz
** filesystem paths.
*/
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "sqlite3.h"

#define SQL_HEAP_LIMIT ((sqlite3_int64)32 * 1024 * 1024)
#define SQL_TEXT_LIMIT 65536
#define SQL_VDBE_OP_LIMIT 25000
#define SQL_PROGRESS_LIMIT 250000

typedef struct SqlFuzzCtx SqlFuzzCtx;
struct SqlFuzzCtx {
  int nProgress;
  unsigned int nRowBudget;
};

static int isSqlIdentChar(char c){
  return (c>='0' && c<='9')
      || (c>='A' && c<='Z')
      || (c>='a' && c<='z')
      || c=='_';
}

static char asciiLower(char c){
  if( c>='A' && c<='Z' ) return (char)(c - 'A' + 'a');
  return c;
}

static int isVacuumToken(const char *zSql){
  static const char zToken[] = "vacuum";
  int i;
  for(i=0; zToken[i]!=0; i++){
    if( zSql[i]==0 || asciiLower(zSql[i])!=zToken[i] ) return 0;
  }
  return !isSqlIdentChar(zSql[i]);
}

static int containsForbiddenSql(const char *zSql){
  int i;
  for(i=0; zSql[i]!=0; i++){
    if( (i==0 || !isSqlIdentChar(zSql[i-1])) && isVacuumToken(&zSql[i]) ){
      return 1;
    }
  }
  return 0;
}

static int sqlProgressHandler(void *pArg){
  SqlFuzzCtx *p = (SqlFuzzCtx*)pArg;
  p->nProgress++;
  return p->nProgress>SQL_PROGRESS_LIMIT;
}

static int sqlExecCallback(void *pArg, int argc, char **argv, char **azCol){
  SqlFuzzCtx *p = (SqlFuzzCtx*)pArg;
  int i;
  (void)azCol;
  if( p->nRowBudget==0 ) return 1;
  p->nRowBudget--;
  for(i=0; i<argc; i++){
    if( argv[i]!=0 ) sqlite3_free(sqlite3_mprintf("%s", argv[i]));
  }
  return 0;
}

static int sqlAuthorizer(
  void *pNotUsed,
  int eCode,
  const char *zArg1,
  const char *zArg2,
  const char *zArg3,
  const char *zArg4
){
  (void)pNotUsed;
  (void)zArg2;
  (void)zArg3;
  (void)zArg4;
  if( eCode==SQLITE_ATTACH || eCode==SQLITE_DETACH ){
    return SQLITE_DENY;
  }
  if( eCode==SQLITE_PRAGMA && zArg1!=0 ){
    if( sqlite3_strnicmp("vdbe_", zArg1, 5)==0
     || sqlite3_stricmp("parser_trace", zArg1)==0
     || sqlite3_stricmp("temp_store_directory", zArg1)==0
     || sqlite3_stricmp("data_store_directory", zArg1)==0
    ){
      return SQLITE_DENY;
    }
  }
  return SQLITE_OK;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size){
  sqlite3 *db = 0;
  SqlFuzzCtx cx;
  char *zErr = 0;
  char *zSql = 0;
  unsigned int selector;
  int rc;

  if( size==0 ) return 0;
  memset(&cx, 0, sizeof(cx));
  selector = data[0];
  data++;
  size--;
  if( size>SQL_TEXT_LIMIT ) size = SQL_TEXT_LIMIT;

  if( sqlite3_initialize()!=SQLITE_OK ) return 0;
  sqlite3_hard_heap_limit64(SQL_HEAP_LIMIT);
  rc = sqlite3_open_v2(
    ":memory:",
    &db,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
    0
  );
  if( rc!=SQLITE_OK ){
    sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db, "PRAGMA temp_store=MEMORY", 0, 0, 0);
  sqlite3_limit(db, SQLITE_LIMIT_LENGTH, SQL_TEXT_LIMIT);
  sqlite3_limit(db, SQLITE_LIMIT_SQL_LENGTH, SQL_TEXT_LIMIT);
  sqlite3_limit(db, SQLITE_LIMIT_VDBE_OP, SQL_VDBE_OP_LIMIT);
  sqlite3_limit(db, SQLITE_LIMIT_EXPR_DEPTH, 64);
  sqlite3_limit(db, SQLITE_LIMIT_LIKE_PATTERN_LENGTH, 256);
  sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, 0, &rc);
#ifndef SQLITE_OMIT_PROGRESS_CALLBACK
  sqlite3_progress_handler(db, 10, sqlProgressHandler, &cx);
#endif
  sqlite3_set_authorizer(db, sqlAuthorizer, 0);

  if( selector&1 ) sqlite3_exec(db, "PRAGMA foreign_keys=ON", 0, 0, 0);
  if( selector&2 ) sqlite3_exec(db, "PRAGMA recursive_triggers=ON", 0, 0, 0);
  if( selector&4 ) sqlite3_exec(db, "PRAGMA automatic_index=OFF", 0, 0, 0);
  cx.nRowBudget = 1u + (selector>>3);

  zSql = sqlite3_malloc64(size + 1);
  if( zSql!=0 ){
    if( size>0 ) memcpy(zSql, data, size);
    zSql[size] = 0;
#ifndef SQLITE_OMIT_COMPLETE
    sqlite3_complete(zSql);
#endif
    if( !containsForbiddenSql(zSql) ){
      sqlite3_exec(db, zSql, sqlExecCallback, &cx, &zErr);
    }
  }

  sqlite3_free(zErr);
  sqlite3_free(zSql);
  sqlite3_close(db);
  return 0;
}
