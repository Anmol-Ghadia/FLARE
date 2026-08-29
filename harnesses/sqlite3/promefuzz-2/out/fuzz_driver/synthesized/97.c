// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_drop_modules at sqlite3.c:130444:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_drop_modules at sqlite3.c:130444:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_declare_vtab at sqlite3.c:131100:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_load_extension at sqlite3.c:114732:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_load_extension at sqlite3.c:114732:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_declare_vtab at sqlite3.c:131100:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"

static uint32_t fuzz_u32(const uint8_t **pData, size_t *pSize){
  uint32_t v = 0;
  size_t n = *pSize < 4 ? *pSize : 4;
  for(size_t i = 0; i < n; i++){
    v = (v << 8) | (*pData)[i];
  }
  *pData += n;
  *pSize -= n;
  return v;
}

static int fuzz_int(const uint8_t **pData, size_t *pSize){
  return (int)fuzz_u32(pData, pSize);
}

static unsigned char fuzz_byte(const uint8_t **pData, size_t *pSize){
  if(*pSize == 0) return 0;
  {
    unsigned char c = **pData;
    (*pData)++;
    (*pSize)--;
    return c;
  }
}

static char *fuzz_string(const uint8_t **pData, size_t *pSize, size_t maxlen){
  size_t n = 0;
  char *s;
  if(*pSize == 0){
    s = (char *)malloc(1);
    if(s) s[0] = '\0';
    return s;
  }
  n = fuzz_byte(pData, pSize);
  if(n > maxlen) n = maxlen;
  if(n > *pSize) n = *pSize;

  s = (char *)malloc(n + 1);
  if(!s) return NULL;

  for(size_t i = 0; i < n; i++){
    unsigned char c = (*pData)[i];
    if(c == 0) c = 'A';
    s[i] = (char)c;
  }
  s[n] = '\0';
  *pData += n;
  *pSize -= n;
  return s;
}

static char *dup_cstr(const char *z){
  size_t n = z ? strlen(z) : 0;
  char *out = (char *)malloc(n + 1);
  if(!out) return NULL;
  if(n) memcpy(out, z, n);
  out[n] = '\0';
  return out;
}

static int vtCreateOrConnect(
  sqlite3 *db, void *pAux, int argc, const char *const *argv,
  sqlite3_vtab **ppVTab, char **pzErr
){
  sqlite3_vtab *pVTab;
  (void)pAux;
  (void)argc;
  (void)argv;
  (void)pzErr;

  pVTab = (sqlite3_vtab *)sqlite3_malloc((int)sizeof(sqlite3_vtab));
  if(!pVTab){
    *ppVTab = NULL;
    return SQLITE_NOMEM;
  }
  memset(pVTab, 0, sizeof(*pVTab));
  *ppVTab = pVTab;

  sqlite3_declare_vtab(db, "CREATE TABLE x(a,b,c)");
  return SQLITE_OK;
}

static int vtBestIndex(sqlite3_vtab *pVTab, sqlite3_index_info *pInfo){
  (void)pVTab;
  (void)pInfo;
  return SQLITE_OK;
}

static int vtDisconnect(sqlite3_vtab *pVTab){
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

static int vtDestroy(sqlite3_vtab *pVTab){
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

static int vtOpen(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor){
  sqlite3_vtab_cursor *pCur;
  (void)pVTab;
  pCur = (sqlite3_vtab_cursor *)sqlite3_malloc((int)sizeof(sqlite3_vtab_cursor));
  if(!pCur){
    *ppCursor = NULL;
    return SQLITE_NOMEM;
  }
  memset(pCur, 0, sizeof(*pCur));
  *ppCursor = pCur;
  return SQLITE_OK;
}

static int vtClose(sqlite3_vtab_cursor *pCur){
  sqlite3_free(pCur);
  return SQLITE_OK;
}

static int vtFilter(sqlite3_vtab_cursor *pCur, int idxNum, const char *idxStr,
                    int argc, sqlite3_value **argv){
  (void)pCur;
  (void)idxNum;
  (void)idxStr;
  (void)argc;
  (void)argv;
  return SQLITE_OK;
}

static int vtNext(sqlite3_vtab_cursor *pCur){
  (void)pCur;
  return SQLITE_OK;
}

static int vtEof(sqlite3_vtab_cursor *pCur){
  (void)pCur;
  return 1;
}

static int vtColumn(sqlite3_vtab_cursor *pCur, sqlite3_context *ctx, int i){
  (void)pCur;
  sqlite3_result_int(ctx, i);
  return SQLITE_OK;
}

static int vtRowid(sqlite3_vtab_cursor *pCur, sqlite3_int64 *pRowid){
  (void)pCur;
  if(pRowid) *pRowid = 1;
  return SQLITE_OK;
}

static int vtUpdate(sqlite3_vtab *pVTab, int argc, sqlite3_value **argv, sqlite3_int64 *pRowid){
  (void)pVTab;
  (void)argc;
  (void)argv;
  if(pRowid) *pRowid = 1;
  return SQLITE_OK;
}

static int vtBegin(sqlite3_vtab *pVTab){ (void)pVTab; return SQLITE_OK; }
static int vtSync(sqlite3_vtab *pVTab){ (void)pVTab; return SQLITE_OK; }
static int vtCommit(sqlite3_vtab *pVTab){ (void)pVTab; return SQLITE_OK; }
static int vtRollback(sqlite3_vtab *pVTab){ (void)pVTab; return SQLITE_OK; }

static int vtFindFunction(sqlite3_vtab *pVtab, int nArg, const char *zName,
                          void (**pxFunc)(sqlite3_context *, int, sqlite3_value **),
                          void **ppArg){
  (void)pVtab;
  (void)nArg;
  (void)zName;
  (void)pxFunc;
  (void)ppArg;
  return 0;
}

static int vtRename(sqlite3_vtab *pVtab, const char *zNew){
  (void)pVtab;
  (void)zNew;
  return SQLITE_OK;
}

static int vtSavepoint(sqlite3_vtab *pVTab, int i){ (void)pVTab; (void)i; return SQLITE_OK; }
static int vtRelease(sqlite3_vtab *pVTab, int i){ (void)pVTab; (void)i; return SQLITE_OK; }
static int vtRollbackTo(sqlite3_vtab *pVTab, int i){ (void)pVTab; (void)i; return SQLITE_OK; }
static int vtShadowName(const char *zName){ (void)zName; return 0; }

static const sqlite3_module gModule = {
  3,
  vtCreateOrConnect,
  vtCreateOrConnect,
  vtBestIndex,
  vtDisconnect,
  vtDestroy,
  vtOpen,
  vtClose,
  vtFilter,
  vtNext,
  vtEof,
  vtColumn,
  vtRowid,
  vtUpdate,
  vtBegin,
  vtSync,
  vtCommit,
  vtRollback,
  vtFindFunction,
  vtRename,
  vtSavepoint,
  vtRelease,
  vtRollbackTo,
  vtShadowName
};

int LLVMFuzzerTestOneInput_97(const uint8_t *Data, size_t Size){
  const uint8_t *p = Data;
  size_t n = Size;
  sqlite3 *db = NULL;
  int rc;
  FILE *fp;

  char *modName1 = NULL;
  char *modName2 = NULL;
  char *keep1 = NULL;
  char *keep2 = NULL;
  char *mainDbName = NULL;
  char *entryProc = NULL;

  fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if(rc != SQLITE_OK || db == NULL){
    if(db) sqlite3_close(db);
    return 0;
  }

  sqlite3_extended_result_codes(db, fuzz_byte(&p, &n) & 1);

  modName1 = fuzz_string(&p, &n, 32);
  modName2 = fuzz_string(&p, &n, 32);
  keep1 = fuzz_string(&p, &n, 32);
  keep2 = fuzz_string(&p, &n, 32);
  mainDbName = fuzz_string(&p, &n, 32);
  entryProc = fuzz_string(&p, &n, 32);

  if(!modName1 || !modName2 || !keep1 || !keep2 || !mainDbName || !entryProc){
    free(modName1);
    free(modName2);
    free(keep1);
    free(keep2);
    free(mainDbName);
    free(entryProc);
    sqlite3_close(db);
    return 0;
  }

  if(modName1[0] == '\0'){
    free(modName1);
    modName1 = dup_cstr("m1");
    if(!modName1) goto cleanup;
  }
  if(modName2[0] == '\0'){
    free(modName2);
    modName2 = dup_cstr("m2");
    if(!modName2) goto cleanup;
  }

  sqlite3_create_module(db, modName1, &gModule, NULL);
  sqlite3_create_module(db, modName2, &gModule, db);

  {
    int onoff = fuzz_byte(&p, &n) & 1;
    int out = 0;
    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, onoff, &out);
    sqlite3_errcode(db);
  }

#ifdef SQLITE_DBCONFIG_LOOKASIDE
  {
    int sz = (fuzz_int(&p, &n) % 256);
    int cnt = (fuzz_int(&p, &n) % 32);
    if(sz < 0) sz = -sz;
    if(cnt < 0) cnt = -cnt;
    sqlite3_db_config(db, SQLITE_DBCONFIG_LOOKASIDE, NULL, sz, cnt);
    sqlite3_errcode(db);
  }
#endif

#ifdef SQLITE_DBCONFIG_MAINDBNAME
  {
    sqlite3_db_config(db, SQLITE_DBCONFIG_MAINDBNAME, mainDbName);
    sqlite3_errcode(db);
  }
#endif

  {
    int opChoices[] = {
#ifdef SQLITE_DBCONFIG_DEFENSIVE
      SQLITE_DBCONFIG_DEFENSIVE,
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_FKEY
      SQLITE_DBCONFIG_ENABLE_FKEY,
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_TRIGGER
      SQLITE_DBCONFIG_ENABLE_TRIGGER,
#endif
#ifdef SQLITE_DBCONFIG_WRITABLE_SCHEMA
      SQLITE_DBCONFIG_WRITABLE_SCHEMA,
#endif
#ifdef SQLITE_DBCONFIG_LEGACY_ALTER_TABLE
      SQLITE_DBCONFIG_LEGACY_ALTER_TABLE,
#endif
#ifdef SQLITE_DBCONFIG_DQS_DML
      SQLITE_DBCONFIG_DQS_DML,
#endif
#ifdef SQLITE_DBCONFIG_DQS_DDL
      SQLITE_DBCONFIG_DQS_DDL,
#endif
#ifdef SQLITE_DBCONFIG_TRUSTED_SCHEMA
      SQLITE_DBCONFIG_TRUSTED_SCHEMA,
#endif
#ifdef SQLITE_DBCONFIG_ENABLE_VIEW
      SQLITE_DBCONFIG_ENABLE_VIEW,
#endif
#ifdef SQLITE_DBCONFIG_STMT_SCANSTATUS
      SQLITE_DBCONFIG_STMT_SCANSTATUS,
#endif
    };
    size_t numOps = sizeof(opChoices) / sizeof(opChoices[0]);
    if(numOps > 0){
      int op = opChoices[fuzz_u32(&p, &n) % numOps];
      int val = fuzz_byte(&p, &n) & 1;
      int out = 0;
      sqlite3_db_config(db, op, val, &out);
      sqlite3_errcode(db);
    }
  }

  {
    const char *keepList1[2];
    keepList1[0] = keep1[0] ? keep1 : modName1;
    keepList1[1] = NULL;
    sqlite3_drop_modules(db, keepList1);
    sqlite3_errcode(db);
  }

  sqlite3_create_module(db, modName1, &gModule, NULL);
  sqlite3_create_module(db, modName2, &gModule, NULL);

  {
    const char *keepList2[3];
    keepList2[0] = (fuzz_byte(&p, &n) & 1) ? modName1 : keep1;
    keepList2[1] = (fuzz_byte(&p, &n) & 1) ? modName2 : keep2;
    keepList2[2] = NULL;
    sqlite3_drop_modules(db, (fuzz_byte(&p, &n) & 1) ? keepList2 : NULL);
    sqlite3_errcode(db);
  }

  sqlite3_create_module(db, modName1, NULL, NULL);
  sqlite3_errcode(db);

  sqlite3_exec(db, "CREATE VIRTUAL TABLE temp.t1 USING ", NULL, NULL, NULL);
  sqlite3_errcode(db);

  {
    char sql[256];
    snprintf(sql, sizeof(sql), "CREATE VIRTUAL TABLE temp.t1 USING \"%s\"", modName1);
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    sqlite3_errcode(db);
  }

  {
    char sql[256];
    snprintf(sql, sizeof(sql), "CREATE VIRTUAL TABLE temp.t2 USING \"%s\"", modName2);
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    sqlite3_errcode(db);
  }

  sqlite3_declare_vtab(db, "CREATE TABLE misuse(x)");
  sqlite3_errcode(db);

  {
    char *loadErr = NULL;
    const char *proc = (fuzz_byte(&p, &n) & 1) ? NULL : entryProc;
    sqlite3_load_extension(db, "./dummy_file", proc, &loadErr);
    sqlite3_errcode(db);
    if(loadErr) sqlite3_free(loadErr);
  }

  {
    char *loadErr = NULL;
    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, 1, NULL);
    sqlite3_load_extension(db, "./dummy_file", NULL, &loadErr);
    sqlite3_errcode(db);
    if(loadErr) sqlite3_free(loadErr);
  }

cleanup:
  free(modName1);
  free(modName2);
  free(keep1);
  free(keep2);
  free(mainDbName);
  free(entryProc);
  sqlite3_close(db);
  return 0;
}