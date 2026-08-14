// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_vtab_on_conflict at sqlite3.c:131565:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_create_module_v2 at sqlite3.c:130427:16 in sqlite3.h
// sqlite3_create_module_v2 at sqlite3.c:130427:16 in sqlite3.h
// sqlite3_overload_function at sqlite3.c:154335:16 in sqlite3.h
// sqlite3_overload_function at sqlite3.c:154335:16 in sqlite3.h
// sqlite3_overload_function at sqlite3.c:154335:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_vtab_config at sqlite3.c:131583:16 in sqlite3.h
// sqlite3_vtab_config at sqlite3.c:131583:16 in sqlite3.h
// sqlite3_vtab_config at sqlite3.c:131583:16 in sqlite3.h
// sqlite3_declare_vtab at sqlite3.c:131100:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_vtab_collation at sqlite3.c:139787:24 in sqlite3.h
// sqlite3_libversion_number at sqlite3.c:152370:16 in sqlite3.h
// sqlite3_libversion_number at sqlite3.c:152370:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

typedef struct {
  sqlite3_vtab base;
  int nRows;
  sqlite3_int64 rowid;
  sqlite3 *db;
} FuzzVTab;

typedef struct {
  sqlite3_vtab_cursor base;
  int row;
} FuzzCursor;

static void fuzz_destroy_aux(void *p) {
  sqlite3_free(p);
}

static int fuzz_xCreateOrConnect(
    sqlite3 *db, void *pAux, int argc, const char *const *argv,
    sqlite3_vtab **ppVTab, char **pzErr) {
  FuzzVTab *vtab = NULL;
  int rc;

  (void)pAux;
  (void)argc;
  (void)argv;
  (void)pzErr;

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
  if (rc != SQLITE_OK) return rc;

  vtab = (FuzzVTab *)sqlite3_malloc(sizeof(*vtab));
  if (!vtab) return SQLITE_NOMEM;
  memset(vtab, 0, sizeof(*vtab));
  vtab->db = db;
  vtab->nRows = 3;
  vtab->rowid = 1;
  *ppVTab = (sqlite3_vtab *)vtab;
  return SQLITE_OK;
}

static int fuzz_xCreate(
    sqlite3 *db, void *pAux, int argc, const char *const *argv,
    sqlite3_vtab **ppVTab, char **pzErr) {
  return fuzz_xCreateOrConnect(db, pAux, argc, argv, ppVTab, pzErr);
}

static int fuzz_xConnect(
    sqlite3 *db, void *pAux, int argc, const char *const *argv,
    sqlite3_vtab **ppVTab, char **pzErr) {
  return fuzz_xCreateOrConnect(db, pAux, argc, argv, ppVTab, pzErr);
}

static int fuzz_xBestIndex(sqlite3_vtab *pVTab, sqlite3_index_info *pIdxInfo) {
  int i;
  (void)pVTab;
  if (!pIdxInfo) return SQLITE_OK;

  for (i = 0; i < pIdxInfo->nConstraint; i++) {
    const char *coll = sqlite3_vtab_collation(pIdxInfo, i);
    (void)coll;
    if (pIdxInfo->aConstraint && pIdxInfo->aConstraintUsage &&
        pIdxInfo->aConstraint[i].usable) {
      pIdxInfo->aConstraintUsage[i].argvIndex = i + 1;
      pIdxInfo->aConstraintUsage[i].omit = (unsigned char)(i & 1);
    }
  }

  pIdxInfo->idxNum = pIdxInfo->nConstraint;
  pIdxInfo->orderByConsumed = (pIdxInfo->nOrderBy > 0);
  pIdxInfo->estimatedCost = (double)(pIdxInfo->nConstraint + 1);
  if (sqlite3_libversion_number() >= 3008002) {
    pIdxInfo->estimatedRows = 3;
  }
  if (sqlite3_libversion_number() >= 3009000) {
    pIdxInfo->idxFlags = 0;
  }
  return SQLITE_OK;
}

static int fuzz_xDisconnect(sqlite3_vtab *pVTab) {
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

static int fuzz_xDestroy(sqlite3_vtab *pVTab) {
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

static int fuzz_xOpen(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor) {
  FuzzCursor *c = (FuzzCursor *)sqlite3_malloc(sizeof(*c));
  (void)pVTab;
  if (!c) return SQLITE_NOMEM;
  memset(c, 0, sizeof(*c));
  *ppCursor = (sqlite3_vtab_cursor *)c;
  return SQLITE_OK;
}

static int fuzz_xClose(sqlite3_vtab_cursor *cur) {
  sqlite3_free(cur);
  return SQLITE_OK;
}

static int fuzz_xFilter(sqlite3_vtab_cursor *cur, int idxNum, const char *idxStr,
                        int argc, sqlite3_value **argv) {
  FuzzCursor *c = (FuzzCursor *)cur;
  (void)idxNum;
  (void)idxStr;
  (void)argc;
  (void)argv;
  c->row = 0;
  return SQLITE_OK;
}

static int fuzz_xNext(sqlite3_vtab_cursor *cur) {
  FuzzCursor *c = (FuzzCursor *)cur;
  c->row++;
  return SQLITE_OK;
}

static int fuzz_xEof(sqlite3_vtab_cursor *cur) {
  FuzzCursor *c = (FuzzCursor *)cur;
  return c->row >= 3;
}

static int fuzz_xColumn(sqlite3_vtab_cursor *cur, sqlite3_context *ctx, int i) {
  FuzzCursor *c = (FuzzCursor *)cur;
  switch (i) {
    case 0:
      sqlite3_result_int(ctx, c->row);
      break;
    case 1:
      sqlite3_result_text(ctx, "fuzz", -1, SQLITE_STATIC);
      break;
    default:
      sqlite3_result_null(ctx);
      break;
  }
  return SQLITE_OK;
}

static int fuzz_xRowid(sqlite3_vtab_cursor *cur, sqlite3_int64 *pRowid) {
  FuzzCursor *c = (FuzzCursor *)cur;
  *pRowid = (sqlite3_int64)(c->row + 1);
  return SQLITE_OK;
}

static int fuzz_xUpdate(sqlite3_vtab *pVTab, int argc, sqlite3_value **argv,
                        sqlite3_int64 *pRowid) {
  FuzzVTab *v = (FuzzVTab *)pVTab;
  int conflict = sqlite3_vtab_on_conflict(v->db);
  (void)argc;
  (void)argv;
  if (pRowid) {
    *pRowid = ++v->rowid;
  }
  if (conflict == SQLITE_IGNORE) {
    return SQLITE_CONSTRAINT;
  }
  return SQLITE_OK;
}

static int fuzz_xBegin(sqlite3_vtab *pVTab) {
  (void)pVTab;
  return SQLITE_OK;
}

static int fuzz_xSync(sqlite3_vtab *pVTab) {
  (void)pVTab;
  return SQLITE_OK;
}

static int fuzz_xCommit(sqlite3_vtab *pVTab) {
  (void)pVTab;
  return SQLITE_OK;
}

static int fuzz_xRollback(sqlite3_vtab *pVTab) {
  (void)pVTab;
  return SQLITE_OK;
}

static int fuzz_xFindFunction(sqlite3_vtab *pVtab, int nArg, const char *zName,
                              void (**pxFunc)(sqlite3_context *, int, sqlite3_value **),
                              void **ppArg) {
  (void)pVtab;
  (void)nArg;
  (void)zName;
  if (pxFunc) *pxFunc = NULL;
  if (ppArg) *ppArg = NULL;
  return 0;
}

static int fuzz_xRename(sqlite3_vtab *pVtab, const char *zNew) {
  (void)pVtab;
  (void)zNew;
  return SQLITE_OK;
}

static int fuzz_xSavepoint(sqlite3_vtab *pVTab, int i) {
  (void)pVTab;
  (void)i;
  return SQLITE_OK;
}

static int fuzz_xRelease(sqlite3_vtab *pVTab, int i) {
  (void)pVTab;
  (void)i;
  return SQLITE_OK;
}

static int fuzz_xRollbackTo(sqlite3_vtab *pVTab, int i) {
  (void)pVTab;
  (void)i;
  return SQLITE_OK;
}

static int fuzz_xShadowName(const char *zName) {
  return zName && zName[0] == 's';
}

static const sqlite3_module fuzzModule = {
  3,
  fuzz_xCreate,
  fuzz_xConnect,
  fuzz_xBestIndex,
  fuzz_xDisconnect,
  fuzz_xDestroy,
  fuzz_xOpen,
  fuzz_xClose,
  fuzz_xFilter,
  fuzz_xNext,
  fuzz_xEof,
  fuzz_xColumn,
  fuzz_xRowid,
  fuzz_xUpdate,
  fuzz_xBegin,
  fuzz_xSync,
  fuzz_xCommit,
  fuzz_xRollback,
  fuzz_xFindFunction,
  fuzz_xRename,
  fuzz_xSavepoint,
  fuzz_xRelease,
  fuzz_xRollbackTo,
  fuzz_xShadowName
};

static uint32_t rd32(const uint8_t *p, size_t n, size_t *off) {
  uint32_t v = 0;
  size_t i;
  for (i = 0; i < 4 && *off < n; i++, (*off)++) {
    v = (v << 8) | p[*off];
  }
  return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  char *errmsg = NULL;
  sqlite3_stmt *stmt = NULL;
  size_t off = 0;
  int rc;
  char modname1[32];
  char modname2[32];
  char funcname[32];
  int narg;
  void *aux1 = NULL;
  void *aux2 = NULL;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK || !db) {
    if (db) sqlite3_close(db);
    return 0;
  }

  snprintf(modname1, sizeof(modname1), "m%08x", rd32(Data, Size, &off));
  snprintf(modname2, sizeof(modname2), "n%08x", rd32(Data, Size, &off));
  snprintf(funcname, sizeof(funcname), "f%08x", rd32(Data, Size, &off));
  narg = (int)(rd32(Data, Size, &off) % 8) - 2;

  aux1 = sqlite3_malloc(16);
  aux2 = sqlite3_malloc(16);
  if (aux1) memset(aux1, 0x41, 16);
  if (aux2) memset(aux2, 0x42, 16);

  (void)sqlite3_create_module(db, modname1, &fuzzModule, aux1);
  (void)sqlite3_create_module(db, modname1, NULL, NULL);
  (void)sqlite3_create_module(db, modname1, &fuzzModule, NULL);
  (void)sqlite3_create_module_v2(db, modname2, &fuzzModule, aux2, fuzz_destroy_aux);
  aux2 = NULL;
  (void)sqlite3_create_module_v2(db, modname2, NULL, NULL, NULL);

  (void)sqlite3_overload_function(db, funcname, narg);
  (void)sqlite3_overload_function(db, "abs", 1);
  (void)sqlite3_overload_function(db, "", 0);

  rc = sqlite3_exec(db, "PRAGMA writable_schema=ON;", NULL, NULL, &errmsg);
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }
  (void)rc;

  {
    char sql[512];
    snprintf(sql, sizeof(sql),
             "CREATE VIRTUAL TABLE t1 USING %s; "
             "SELECT * FROM t1 WHERE a=1 COLLATE NOCASE ORDER BY a; "
             "INSERT OR IGNORE INTO t1 VALUES(1,2,3); "
             "UPDATE OR REPLACE t1 SET a=2 WHERE a=1; "
             "DELETE FROM t1 WHERE a=2; "
             "DROP TABLE IF EXISTS t1; "
             "CREATE VIRTUAL TABLE t2 USING %s; "
             "SELECT %s(a) FROM t2 WHERE b>3 COLLATE RTRIM;",
             modname1, modname2, funcname);
    rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (errmsg) {
      sqlite3_free(errmsg);
      errmsg = NULL;
    }
    (void)rc;
  }

  if (sqlite3_prepare_v2(db, "SELECT * FROM sqlite_master;", -1, &stmt, NULL) == SQLITE_OK) {
    while (sqlite3_step(stmt) == SQLITE_ROW) {
    }
  }
  sqlite3_finalize(stmt);
  stmt = NULL;

  if (aux1) sqlite3_free(aux1);
  if (aux2) sqlite3_free(aux2);
  sqlite3_close(db);
  return 0;
}