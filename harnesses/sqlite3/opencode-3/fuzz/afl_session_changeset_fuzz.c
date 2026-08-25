/*
** AFL++/libFuzzer-style harness for fuzzing the SQLite session module.
**
** Compile with SQLITE_ENABLE_SESSION and SQLITE_ENABLE_PREUPDATE_HOOK.
** The harness uses only in-memory state, so the fuzz input never controls
** a filename or path.
*/

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "sqlite3.h"

#define SESSION_FUZZ_PROGRESS_LIMIT 100000
#define SESSION_FUZZ_HEAP_LIMIT (64*1024*1024)

typedef struct SessionFuzzCtx SessionFuzzCtx;
struct SessionFuzzCtx {
  unsigned selector;
  int nProgress;
};

static int sessionFuzzProgress(void *pCtx){
  SessionFuzzCtx *p = (SessionFuzzCtx*)pCtx;
  p->nProgress++;
  return p->nProgress > SESSION_FUZZ_PROGRESS_LIMIT;
}

static int sessionFuzzFilter(void *pCtx, const char *zTab){
  SessionFuzzCtx *p = (SessionFuzzCtx*)pCtx;
  if( (p->selector & 4)==0 ) return 1;
  return zTab!=0 && ((unsigned char)zTab[0] & 1)==((p->selector >> 3) & 1);
}

static int sessionFuzzConflict(
  void *pCtx,
  int eConflict,
  sqlite3_changeset_iter *pIter
){
  SessionFuzzCtx *p = (SessionFuzzCtx*)pCtx;
  unsigned mode = p->selector & 3;
  (void)pIter;
  if( mode==0 ) return SQLITE_CHANGESET_OMIT;
  if( mode==1 ){
    if( eConflict==SQLITE_CHANGESET_DATA || eConflict==SQLITE_CHANGESET_CONFLICT ){
      return SQLITE_CHANGESET_REPLACE;
    }
    return SQLITE_CHANGESET_OMIT;
  }
  return SQLITE_CHANGESET_ABORT;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size){
  static const char zSchema[] =
    "PRAGMA foreign_keys=ON;"
    "CREATE TABLE t1(id INTEGER PRIMARY KEY, value TEXT, extra BLOB);"
    "CREATE TABLE t2(id INTEGER PRIMARY KEY, ref INTEGER REFERENCES t1(id),"
    " score REAL);"
    "INSERT INTO t1 VALUES(1,'one',x'0102');"
    "INSERT INTO t1 VALUES(2,'two',x'0304');"
    "INSERT INTO t2 VALUES(1,1,1.5);"
    "INSERT INTO t2 VALUES(2,2,2.5);";
  sqlite3 *db = 0;
  sqlite3_changegroup *pGroup = 0;
  SessionFuzzCtx ctx;
  void *pGrouped = 0;
  void *pRebase = 0;
  int nGrouped = 0;
  int nRebase = 0;
  int rc;
  int rcGroup;
  int iDummy = 0;
  int flags = 0;
  size_t nLeft;
  size_t nRight;

  if( size==0 ) return 0;
  memset(&ctx, 0, sizeof(ctx));
  ctx.selector = data[0];
  data++;
  size--;

  if( sqlite3_initialize()!=SQLITE_OK ) return 0;
  sqlite3_hard_heap_limit64(SESSION_FUZZ_HEAP_LIMIT);

  rc = sqlite3_open_v2(":memory:", &db,
                       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
  if( rc!=SQLITE_OK || db==0 ){
    sqlite3_close(db);
    return 0;
  }

  sqlite3_progress_handler(db, 10, sessionFuzzProgress, &ctx);
  sqlite3_enable_load_extension(db, 0);
  sqlite3_db_config(db, SQLITE_DBCONFIG_DEFENSIVE, 1, &iDummy);
  sqlite3_db_config(db, SQLITE_DBCONFIG_TRUSTED_SCHEMA, 0, &iDummy);
  sqlite3_exec(db, "PRAGMA temp_store=MEMORY;", 0, 0, 0);
  sqlite3_exec(db, zSchema, 0, 0, 0);

  if( ctx.selector & 8 ){
    flags |= SQLITE_CHANGESETAPPLY_NOSAVEPOINT;
  }
  if( ctx.selector & 16 ){
    flags |= SQLITE_CHANGESETAPPLY_INVERT;
  }

  if( sqlite3changegroup_new(&pGroup)==SQLITE_OK ){
    nLeft = size/2;
    nRight = size - nLeft;
    rcGroup = SQLITE_OK;
    if( nLeft>0 ){
      rcGroup = sqlite3changegroup_add(pGroup, (int)nLeft, (void*)data);
    }
    if( rcGroup==SQLITE_OK && nRight>0 ){
      rcGroup = sqlite3changegroup_add(pGroup, (int)nRight, (void*)(data+nLeft));
    }
    if( rcGroup==SQLITE_OK
     && sqlite3changegroup_output(pGroup, &nGrouped, &pGrouped)==SQLITE_OK
     && pGrouped!=0 ){
      ctx.nProgress = 0;
      sqlite3changeset_apply_v2(db, nGrouped, pGrouped,
                                sessionFuzzFilter, sessionFuzzConflict, &ctx,
                                &pRebase, &nRebase, flags);
      sqlite3_free(pRebase);
      pRebase = 0;
      nRebase = 0;
    }
  }

  ctx.nProgress = 0;
  sqlite3changeset_apply_v2(db, (int)size, (void*)data,
                            sessionFuzzFilter, sessionFuzzConflict, &ctx,
                            &pRebase, &nRebase, flags);

  sqlite3_free(pRebase);
  sqlite3_free(pGrouped);
  if( pGroup ) sqlite3changegroup_delete(pGroup);
  sqlite3_close(db);
  return 0;
}
