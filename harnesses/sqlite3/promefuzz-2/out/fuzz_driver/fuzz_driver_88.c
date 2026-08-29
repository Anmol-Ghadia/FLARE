// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_vtab_config at sqlite3.c:131583:16 in sqlite3.h
// sqlite3_vtab_config at sqlite3.c:131583:16 in sqlite3.h
// sqlite3_vtab_config at sqlite3.c:131583:16 in sqlite3.h
// sqlite3_declare_vtab at sqlite3.c:131100:16 in sqlite3.h
// sqlite3_declare_vtab at sqlite3.c:131100:16 in sqlite3.h
// sqlite3_declare_vtab at sqlite3.c:131100:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_declare_vtab at sqlite3.c:131100:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_create_module_v2 at sqlite3.c:130427:16 in sqlite3.h
// sqlite3_create_module_v2 at sqlite3.c:130427:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_create_module_v2 at sqlite3.c:130427:16 in sqlite3.h
// sqlite3_overload_function at sqlite3.c:154335:16 in sqlite3.h
// sqlite3_overload_function at sqlite3.c:154335:16 in sqlite3.h
// sqlite3_overload_function at sqlite3.c:154335:16 in sqlite3.h
// sqlite3_drop_modules at sqlite3.c:130444:16 in sqlite3.h
// sqlite3_drop_modules at sqlite3.c:130444:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_declare_vtab at sqlite3.c:131100:16 in sqlite3.h
// sqlite3_vtab_config at sqlite3.c:131583:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

typedef struct {
  const uint8_t *data;
  size_t size;
  size_t off;
} FuzzInput;

static uint8_t fi_u8(FuzzInput *in){
  if(in->off >= in->size) return 0;
  return in->data[in->off++];
}

static uint32_t fi_u32(FuzzInput *in){
  uint32_t v = 0;
  int i;
  for(i = 0; i < 4; i++){
    v = (v << 8) | fi_u8(in);
  }
  return v;
}

static int fi_bool(FuzzInput *in){
  return (fi_u8(in) & 1) != 0;
}

static int fi_int_range(FuzzInput *in, int minv, int maxv){
  uint32_t span;
  if(maxv <= minv) return minv;
  span = (uint32_t)(maxv - minv + 1);
  return minv + (int)(fi_u32(in) % span);
}

static char *fi_string(FuzzInput *in, size_t maxlen){
  size_t remain = (in->off < in->size) ? (in->size - in->off) : 0;
  size_t len = remain ? (fi_u8(in) % (maxlen + 1)) : 0;
  char *s;
  size_t i;
  if(len > remain - (remain ? 1 : 0)) len = remain ? (remain - 1) : 0;
  s = (char *)malloc(len + 1);
  if(!s) return NULL;
  for(i = 0; i < len; i++){
    uint8_t c = fi_u8(in);
    s[i] = (char)('a' + (c % 26));
  }
  s[len] = '\0';
  return s;
}

static int dummy_vtab_disconnect(sqlite3_vtab *pVtab){
  sqlite3_free(pVtab);
  return SQLITE_OK;
}

static int dummy_vtab_destroy(sqlite3_vtab *pVtab){
  sqlite3_free(pVtab);
  return SQLITE_OK;
}

static int dummy_vtab_open(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor){
  sqlite3_vtab_cursor *pCur = (sqlite3_vtab_cursor *)sqlite3_malloc(sizeof(*pCur));
  (void)pVTab;
  if(!pCur) return SQLITE_NOMEM;
  memset(pCur, 0, sizeof(*pCur));
  pCur->pVtab = pVTab;
  *ppCursor = pCur;
  return SQLITE_OK;
}

static int dummy_vtab_close(sqlite3_vtab_cursor *cur){
  sqlite3_free(cur);
  return SQLITE_OK;
}

static int dummy_vtab_bestindex(sqlite3_vtab *tab, sqlite3_index_info *pIdxInfo){
  (void)tab;
  if(pIdxInfo){
    pIdxInfo->estimatedCost = 1.0;
    pIdxInfo->estimatedRows = 1;
  }
  return SQLITE_OK;
}

static int dummy_vtab_filter(sqlite3_vtab_cursor *pVtabCursor, int idxNum, const char *idxStr,
                             int argc, sqlite3_value **argv){
  (void)pVtabCursor;
  (void)idxNum;
  (void)idxStr;
  (void)argc;
  (void)argv;
  return SQLITE_OK;
}

static int dummy_vtab_next(sqlite3_vtab_cursor *cur){
  (void)cur;
  return SQLITE_OK;
}

static int dummy_vtab_eof(sqlite3_vtab_cursor *cur){
  (void)cur;
  return 1;
}

static int dummy_vtab_column(sqlite3_vtab_cursor *cur, sqlite3_context *ctx, int i){
  (void)cur;
  sqlite3_result_int(ctx, i);
  return SQLITE_OK;
}

static int dummy_vtab_rowid(sqlite3_vtab_cursor *cur, sqlite3_int64 *pRowid){
  (void)cur;
  if(pRowid) *pRowid = 1;
  return SQLITE_OK;
}

static int dummy_vtab_create_or_connect(
    sqlite3 *db, void *pAux, int argc, const char *const *argv,
    sqlite3_vtab **ppVtab, char **pzErr){
  sqlite3_vtab *pVtab;
  int rc;
  int mode = (int)(intptr_t)pAux;
  (void)argc;
  (void)argv;
  (void)pzErr;

  pVtab = (sqlite3_vtab *)sqlite3_malloc(sizeof(*pVtab));
  if(!pVtab) return SQLITE_NOMEM;
  memset(pVtab, 0, sizeof(*pVtab));

  if(mode & 1){
#ifdef SQLITE_VTAB_CONSTRAINT_SUPPORT
    sqlite3_vtab_config(db, SQLITE_VTAB_CONSTRAINT_SUPPORT, (mode >> 1) & 1);
#endif
  }
#ifdef SQLITE_VTAB_INNOCUOUS
  if(mode & 4){
    sqlite3_vtab_config(db, SQLITE_VTAB_INNOCUOUS);
  }
#endif
#ifdef SQLITE_VTAB_DIRECTONLY
  if(mode & 8){
    sqlite3_vtab_config(db, SQLITE_VTAB_DIRECTONLY);
  }
#endif

  if(mode & 16){
    rc = sqlite3_declare_vtab(db, "CREATE TABLE x(a,b,c)");
  }else if(mode & 32){
    rc = sqlite3_declare_vtab(db, "CREATE TABLE x(a HIDDEN,b,c)");
  }else{
    rc = sqlite3_declare_vtab(db, "CREATE TABLE x(a)");
  }

  if(rc != SQLITE_OK){
    sqlite3_free(pVtab);
    return rc;
  }

  if(mode & 64){
    (void)sqlite3_declare_vtab(db, "CREATE TABLE y(z)");
  }

  *ppVtab = pVtab;
  return SQLITE_OK;
}

static void dummy_destroy(void *p){
  free(p);
}

static const sqlite3_module dummy_module = {
  1,
  dummy_vtab_create_or_connect,
  dummy_vtab_create_or_connect,
  dummy_vtab_bestindex,
  dummy_vtab_disconnect,
  dummy_vtab_destroy,
  dummy_vtab_open,
  dummy_vtab_close,
  dummy_vtab_filter,
  dummy_vtab_next,
  dummy_vtab_eof,
  dummy_vtab_column,
  dummy_vtab_rowid,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  FuzzInput in;
  sqlite3 *db = NULL;
  char *err = NULL;
  char *mod1 = NULL, *mod2 = NULL, *mod3 = NULL;
  char *func1 = NULL, *func2 = NULL;
  const char *keep[4];
  int rc;
  FILE *fp;

  in.data = Data;
  in.size = Size;
  in.off = 0;

  fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if(rc != SQLITE_OK || db == NULL){
    if(db) sqlite3_close(db);
    return 0;
  }

  mod1 = fi_string(&in, 32);
  mod2 = fi_string(&in, 32);
  mod3 = fi_string(&in, 32);
  func1 = fi_string(&in, 32);
  func2 = fi_string(&in, 32);

  if(!mod1){ mod1 = strdup("m1"); }
  if(!mod2){ mod2 = strdup("m2"); }
  if(!mod3){ mod3 = strdup("m3"); }
  if(!func1){ func1 = strdup("f1"); }
  if(!func2){ func2 = strdup("f2"); }

  if(mod1 && mod1[0] == '\0'){ free(mod1); mod1 = strdup("m1"); }
  if(mod2 && mod2[0] == '\0'){ free(mod2); mod2 = strdup("m2"); }
  if(mod3 && mod3[0] == '\0'){ free(mod3); mod3 = strdup("m3"); }
  if(func1 && func1[0] == '\0'){ free(func1); func1 = strdup("f1"); }
  if(func2 && func2[0] == '\0'){ free(func2); func2 = strdup("f2"); }

  {
    int mode1 = fi_int_range(&in, 0, 127);
    int mode2 = fi_int_range(&in, 0, 127);
    int mode3 = fi_int_range(&in, 0, 127);
    int *aux2 = (int *)malloc(sizeof(int));
    int *aux3 = (int *)malloc(sizeof(int));

    if(aux2) *aux2 = mode2;
    if(aux3) *aux3 = mode3;

    (void)sqlite3_create_module(db, mod1, &dummy_module, (void *)(intptr_t)mode1);
    (void)sqlite3_create_module_v2(db, mod2, &dummy_module, aux2, dummy_destroy);
    (void)sqlite3_create_module_v2(db, mod3, &dummy_module, aux3, dummy_destroy);

    if(fi_bool(&in)){
      (void)sqlite3_create_module(db, mod1, NULL, NULL);
    }
    if(fi_bool(&in)){
      (void)sqlite3_create_module_v2(db, mod2, NULL, NULL, NULL);
    }
  }

  (void)sqlite3_overload_function(db, func1, fi_int_range(&in, -2, 8));
  (void)sqlite3_overload_function(db, func2, fi_int_range(&in, -2, 8));
  (void)sqlite3_overload_function(db, func1, fi_int_range(&in, -2, 8));

  keep[0] = fi_bool(&in) ? mod1 : NULL;
  keep[1] = fi_bool(&in) ? mod2 : NULL;
  keep[2] = fi_bool(&in) ? mod3 : NULL;
  keep[3] = NULL;

  if(fi_bool(&in)){
    (void)sqlite3_drop_modules(db, NULL);
  }else{
    (void)sqlite3_drop_modules(db, keep);
  }

  (void)sqlite3_create_module(db, mod1, &dummy_module, (void *)(intptr_t)fi_int_range(&in, 0, 127));
  (void)sqlite3_exec(db, "CREATE VIRTUAL TABLE IF NOT EXISTS temp.t1 USING m1", NULL, NULL, &err);
  if(err){ sqlite3_free(err); err = NULL; }

  (void)sqlite3_exec(db, "CREATE VIRTUAL TABLE IF NOT EXISTS temp.t2 USING m2", NULL, NULL, &err);
  if(err){ sqlite3_free(err); err = NULL; }

  (void)sqlite3_exec(db, "SELECT * FROM temp.t1", NULL, NULL, &err);
  if(err){ sqlite3_free(err); err = NULL; }

  (void)sqlite3_exec(db, "SELECT * FROM temp.t2", NULL, NULL, &err);
  if(err){ sqlite3_free(err); err = NULL; }

  (void)sqlite3_declare_vtab(db, "CREATE TABLE misuse(x)");
#ifdef SQLITE_VTAB_CONSTRAINT_SUPPORT
  (void)sqlite3_vtab_config(db, SQLITE_VTAB_CONSTRAINT_SUPPORT, 1);
#endif

  free(mod1);
  free(mod2);
  free(mod3);
  free(func1);
  free(func2);
  sqlite3_close(db);
  return 0;
}