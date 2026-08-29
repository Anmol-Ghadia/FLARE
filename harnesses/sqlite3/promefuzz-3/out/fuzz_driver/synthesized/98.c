// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_create_module_v2 at sqlite3.c:130427:16 in sqlite3.h
// sqlite3_overload_function at sqlite3.c:154335:16 in sqlite3.h
// sqlite3_overload_function at sqlite3.c:154335:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_drop_modules at sqlite3.c:130444:16 in sqlite3.h
// sqlite3_create_module_v2 at sqlite3.c:130427:16 in sqlite3.h
// sqlite3_overload_function at sqlite3.c:154335:16 in sqlite3.h
// sqlite3_overload_function at sqlite3.c:154335:16 in sqlite3.h
// sqlite3_overload_function at sqlite3.c:154335:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_vtab_config at sqlite3.c:131583:16 in sqlite3.h
// sqlite3_vtab_config at sqlite3.c:131583:16 in sqlite3.h
// sqlite3_vtab_config at sqlite3.c:131583:16 in sqlite3.h
// sqlite3_declare_vtab at sqlite3.c:131100:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

typedef struct {
  sqlite3_vtab base;
} FuzzVTab;

typedef struct {
  sqlite3_vtab_cursor base;
  int row_emitted;
} FuzzCursor;

static void fuzz_destroy_aux(void *p){
  free(p);
}

static int fuzz_vtab_create_or_connect(
    sqlite3 *db, void *pAux, int argc, const char *const *argv,
    sqlite3_vtab **ppVTab, char **pzErr
){
  (void)pAux;
  (void)argc;
  (void)argv;
  (void)pzErr;

  int rc;
  FuzzVTab *v = (FuzzVTab *)sqlite3_malloc(sizeof(FuzzVTab));
  if(!v) return SQLITE_NOMEM;
  memset(v, 0, sizeof(*v));

  rc = sqlite3_vtab_config(db, SQLITE_VTAB_CONSTRAINT_SUPPORT, 1);
  (void)rc;
#ifdef SQLITE_VTAB_INNOCUOUS
  rc = sqlite3_vtab_config(db, SQLITE_VTAB_INNOCUOUS);
  (void)rc;
#endif
#ifdef SQLITE_VTAB_DIRECTONLY
  rc = sqlite3_vtab_config(db, SQLITE_VTAB_DIRECTONLY);
  (void)rc;
#endif

  rc = sqlite3_declare_vtab(db, "CREATE TABLE x(a,b,c)");
  if(rc != SQLITE_OK){
    sqlite3_free(v);
    return rc;
  }

  *ppVTab = &v->base;
  return SQLITE_OK;
}

static int fuzz_vtab_bestindex(sqlite3_vtab *pVTab, sqlite3_index_info *pIdxInfo){
  (void)pVTab;
  if(pIdxInfo){
    pIdxInfo->estimatedCost = 1.0;
    pIdxInfo->estimatedRows = 1;
  }
  return SQLITE_OK;
}

static int fuzz_vtab_disconnect(sqlite3_vtab *pVTab){
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

static int fuzz_vtab_destroy(sqlite3_vtab *pVTab){
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

static int fuzz_vtab_open(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor){
  (void)pVTab;
  FuzzCursor *c = (FuzzCursor *)sqlite3_malloc(sizeof(FuzzCursor));
  if(!c) return SQLITE_NOMEM;
  memset(c, 0, sizeof(*c));
  *ppCursor = &c->base;
  return SQLITE_OK;
}

static int fuzz_vtab_close(sqlite3_vtab_cursor *cur){
  sqlite3_free(cur);
  return SQLITE_OK;
}

static int fuzz_vtab_filter(sqlite3_vtab_cursor *pVtabCursor, int idxNum,
                            const char *idxStr, int argc, sqlite3_value **argv){
  (void)idxNum;
  (void)idxStr;
  (void)argc;
  (void)argv;
  FuzzCursor *c = (FuzzCursor *)pVtabCursor;
  c->row_emitted = 0;
  return SQLITE_OK;
}

static int fuzz_vtab_next(sqlite3_vtab_cursor *cur){
  FuzzCursor *c = (FuzzCursor *)cur;
  c->row_emitted = 1;
  return SQLITE_OK;
}

static int fuzz_vtab_eof(sqlite3_vtab_cursor *cur){
  FuzzCursor *c = (FuzzCursor *)cur;
  return c->row_emitted ? 1 : 0;
}

static int fuzz_vtab_column(sqlite3_vtab_cursor *cur, sqlite3_context *ctx, int i){
  (void)cur;
  sqlite3_result_int(ctx, i);
  return SQLITE_OK;
}

static int fuzz_vtab_rowid(sqlite3_vtab_cursor *cur, sqlite3_int64 *pRowid){
  (void)cur;
  *pRowid = 1;
  return SQLITE_OK;
}

static int fuzz_vtab_findfunction(sqlite3_vtab *pVtab, int nArg, const char *zName,
                                  void (**pxFunc)(sqlite3_context *, int, sqlite3_value **),
                                  void **ppArg){
  (void)pVtab;
  (void)nArg;
  (void)zName;
  if(pxFunc) *pxFunc = NULL;
  if(ppArg) *ppArg = NULL;
  return 0;
}

static const sqlite3_module fuzz_module = {
  3,
  fuzz_vtab_create_or_connect,
  fuzz_vtab_create_or_connect,
  fuzz_vtab_bestindex,
  fuzz_vtab_disconnect,
  fuzz_vtab_destroy,
  fuzz_vtab_open,
  fuzz_vtab_close,
  fuzz_vtab_filter,
  fuzz_vtab_next,
  fuzz_vtab_eof,
  fuzz_vtab_column,
  fuzz_vtab_rowid,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  fuzz_vtab_findfunction,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

static unsigned consume_u32(const uint8_t **data, size_t *size){
  unsigned v = 0;
  size_t n = *size < 4 ? *size : 4;
  for(size_t i = 0; i < n; i++){
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static int consume_int(const uint8_t **data, size_t *size){
  return (int)consume_u32(data, size);
}

static char *consume_string(const uint8_t **data, size_t *size){
  if(*size == 0){
    char *s = (char *)malloc(1);
    if(s) s[0] = '\0';
    return s;
  }
  size_t len = (*data)[0] % (*size + 1);
  *data += 1;
  *size -= 1;
  if(len > *size) len = *size;

  char *s = (char *)malloc(len + 1);
  if(!s) return NULL;
  memcpy(s, *data, len);
  for(size_t i = 0; i < len; i++){
    if(s[i] == '\0') s[i] = 'A';
  }
  s[len] = '\0';
  *data += len;
  *size -= len;
  return s;
}

static void exec_sql(sqlite3 *db, const char *sql){
  char *err = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &err);
  sqlite3_free(err);
}

int LLVMFuzzerTestOneInput_98(const uint8_t *Data, size_t Size){
  FILE *fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  if(sqlite3_open(":memory:", &db) != SQLITE_OK){
    if(db) sqlite3_close(db);
    return 0;
  }

  const uint8_t *p = Data;
  size_t remaining = Size;

  char *modname1 = consume_string(&p, &remaining);
  char *modname2 = consume_string(&p, &remaining);
  char *funcname1 = consume_string(&p, &remaining);
  char *funcname2 = consume_string(&p, &remaining);
  char *keep1 = consume_string(&p, &remaining);
  char *keep2 = consume_string(&p, &remaining);

  if(!modname1 || !modname2 || !funcname1 || !funcname2 || !keep1 || !keep2){
    free(modname1);
    free(modname2);
    free(funcname1);
    free(funcname2);
    free(keep1);
    free(keep2);
    sqlite3_close(db);
    return 0;
  }

  int nArg1 = consume_int(&p, &remaining) % 8 - 4;
  int nArg2 = consume_int(&p, &remaining);
  int op_choice = consume_int(&p, &remaining);

  char *aux1 = (char *)malloc(16);
  char *aux2 = (char *)malloc(16);
  if(aux1) memset(aux1, 0x41, 16);
  if(aux2) memset(aux2, 0x42, 16);

  sqlite3_create_module(db, modname1, &fuzz_module, aux1);
  sqlite3_create_module_v2(db, modname2, &fuzz_module, aux2, fuzz_destroy_aux);

  sqlite3_overload_function(db, funcname1, nArg1);
  sqlite3_overload_function(db, funcname2, nArg2);

  sqlite3_create_module(db, modname1, NULL, NULL);
  free(aux1);
  aux1 = NULL;

  {
    const char *keep_list1[3];
    keep_list1[0] = keep1[0] ? keep1 : NULL;
    keep_list1[1] = keep2[0] ? keep2 : NULL;
    keep_list1[2] = NULL;
    sqlite3_drop_modules(db, (op_choice & 1) ? keep_list1 : NULL);
  }

  sqlite3_create_module_v2(db, "fuzzmod", &fuzz_module, NULL, NULL);
  exec_sql(db, "CREATE VIRTUAL TABLE temp.t1 USING fuzzmod");
  exec_sql(db, "SELECT * FROM temp.t1");
  exec_sql(db, "DROP TABLE IF EXISTS temp.t1");

  sqlite3_overload_function(db, "match", 2);
  sqlite3_overload_function(db, "glob", 2);
  sqlite3_overload_function(db, "like", 2);

  free(modname1);
  free(modname2);
  free(funcname1);
  free(funcname2);
  free(keep1);
  free(keep2);

  sqlite3_close(db);
  return 0;
}