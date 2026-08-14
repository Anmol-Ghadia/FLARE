#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

typedef struct {
  sqlite3_vtab base;
  sqlite3 *db;
} FuzzVTab;

typedef struct {
  sqlite3_vtab_cursor base;
  sqlite3_int64 rowid;
  int eof;
} FuzzCursor;

typedef struct {
  int marker;
} AuxData;

static void aux_destructor(void *p) {
  free(p);
}

static int fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  (void)argv;
  sqlite3_result_int(ctx, 1);
  return SQLITE_OK;
}

static void fuzz_scalar_void(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  (void)argv;
  sqlite3_result_text(ctx, "ok", -1, SQLITE_STATIC);
}

static int fuzzDeclareSchema(sqlite3 *db) {
  return sqlite3_declare_vtab(db, "CREATE TABLE x(a,b)");
}

static int fuzzBestIndex(sqlite3_vtab *pVTab, sqlite3_index_info *pInfo) {
  (void)pVTab;
  if (pInfo) {
    pInfo->estimatedCost = 1.0;
    pInfo->estimatedRows = 1;
  }
  return SQLITE_OK;
}

static int fuzzDisconnect(sqlite3_vtab *pVTab) {
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

static int fuzzDestroy(sqlite3_vtab *pVTab) {
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

static int fuzzOpen(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor) {
  (void)pVTab;
  FuzzCursor *c = (FuzzCursor *)sqlite3_malloc(sizeof(FuzzCursor));
  if (!c) return SQLITE_NOMEM;
  memset(c, 0, sizeof(*c));
  c->rowid = 1;
  c->eof = 0;
  *ppCursor = &c->base;
  return SQLITE_OK;
}

static int fuzzClose(sqlite3_vtab_cursor *cur) {
  sqlite3_free(cur);
  return SQLITE_OK;
}

static int fuzzFilter(sqlite3_vtab_cursor *cur, int idxNum, const char *idxStr,
                      int argc, sqlite3_value **argv) {
  (void)idxNum;
  (void)idxStr;
  (void)argc;
  (void)argv;
  FuzzCursor *c = (FuzzCursor *)cur;
  c->rowid = 1;
  c->eof = 0;
  return SQLITE_OK;
}

static int fuzzNext(sqlite3_vtab_cursor *cur) {
  FuzzCursor *c = (FuzzCursor *)cur;
  c->rowid++;
  c->eof = 1;
  return SQLITE_OK;
}

static int fuzzEof(sqlite3_vtab_cursor *cur) {
  FuzzCursor *c = (FuzzCursor *)cur;
  return c->eof;
}

static int fuzzColumn(sqlite3_vtab_cursor *cur, sqlite3_context *ctx, int i) {
  FuzzCursor *c = (FuzzCursor *)cur;
  if (i == 0) {
    sqlite3_result_int64(ctx, c->rowid);
  } else {
    sqlite3_result_text(ctx, "v", -1, SQLITE_STATIC);
  }
  return SQLITE_OK;
}

static int fuzzRowid(sqlite3_vtab_cursor *cur, sqlite3_int64 *pRowid) {
  FuzzCursor *c = (FuzzCursor *)cur;
  *pRowid = c->rowid;
  return SQLITE_OK;
}

static int fuzzUpdate(sqlite3_vtab *pVTab, int argc, sqlite3_value **argv, sqlite3_int64 *pRowid) {
  FuzzVTab *v = (FuzzVTab *)pVTab;
  int conflict = sqlite3_vtab_on_conflict(v->db);
  if (pRowid) {
    *pRowid = (sqlite3_int64)conflict + argc;
  }
  (void)argv;
  return SQLITE_OK;
}

static int fuzzBegin(sqlite3_vtab *pVTab) {
  (void)pVTab;
  return SQLITE_OK;
}

static int fuzzSync(sqlite3_vtab *pVTab) {
  (void)pVTab;
  return SQLITE_OK;
}

static int fuzzCommit(sqlite3_vtab *pVTab) {
  (void)pVTab;
  return SQLITE_OK;
}

static int fuzzRollback(sqlite3_vtab *pVTab) {
  (void)pVTab;
  return SQLITE_OK;
}

static int fuzzFindFunction(sqlite3_vtab *pVtab, int nArg, const char *zName,
                            void (**pxFunc)(sqlite3_context *, int, sqlite3_value **),
                            void **ppArg) {
  (void)pVtab;
  (void)nArg;
  if (zName && pxFunc && ppArg) {
    *pxFunc = fuzz_scalar_void;
    *ppArg = NULL;
    return 1;
  }
  return 0;
}

static int fuzzRename(sqlite3_vtab *pVtab, const char *zNew) {
  (void)pVtab;
  (void)zNew;
  return SQLITE_OK;
}

static int fuzzSavepoint(sqlite3_vtab *pVTab, int i) {
  (void)pVTab;
  (void)i;
  return SQLITE_OK;
}

static int fuzzRelease(sqlite3_vtab *pVTab, int i) {
  (void)pVTab;
  (void)i;
  return SQLITE_OK;
}

static int fuzzRollbackTo(sqlite3_vtab *pVTab, int i) {
  (void)pVTab;
  (void)i;
  return SQLITE_OK;
}

static int fuzzShadowName(const char *zName) {
  return zName && zName[0] == 's';
}

static int fuzzConnectCreate(sqlite3 *db, void *pAux, int argc, const char *const *argv,
                             sqlite3_vtab **ppVTab, char **pzErr) {
  (void)pAux;
  (void)argc;
  (void)argv;
  (void)pzErr;

  (void)sqlite3_vtab_config(db, SQLITE_VTAB_CONSTRAINT_SUPPORT, 1);
#ifdef SQLITE_VTAB_INNOCUOUS
  (void)sqlite3_vtab_config(db, SQLITE_VTAB_INNOCUOUS);
#endif
#ifdef SQLITE_VTAB_DIRECTONLY
  (void)sqlite3_vtab_config(db, SQLITE_VTAB_DIRECTONLY);
#endif

  if (fuzzDeclareSchema(db) != SQLITE_OK) {
    return SQLITE_ERROR;
  }

  FuzzVTab *v = (FuzzVTab *)sqlite3_malloc(sizeof(FuzzVTab));
  if (!v) return SQLITE_NOMEM;
  memset(v, 0, sizeof(*v));
  v->db = db;
  *ppVTab = &v->base;
  return SQLITE_OK;
}

static const sqlite3_module fuzzModule = {
  3,
  fuzzConnectCreate,
  fuzzConnectCreate,
  fuzzBestIndex,
  fuzzDisconnect,
  fuzzDestroy,
  fuzzOpen,
  fuzzClose,
  fuzzFilter,
  fuzzNext,
  fuzzEof,
  fuzzColumn,
  fuzzRowid,
  fuzzUpdate,
  fuzzBegin,
  fuzzSync,
  fuzzCommit,
  fuzzRollback,
  fuzzFindFunction,
  fuzzRename,
  fuzzSavepoint,
  fuzzRelease,
  fuzzRollbackTo,
  fuzzShadowName
};

static int exec_sql(sqlite3 *db, const char *sql) {
  char *err = NULL;
  int rc = sqlite3_exec(db, sql, NULL, NULL, &err);
  sqlite3_free(err);
  return rc;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  char *sql = NULL;
  char *fname = NULL;
  size_t i;
  int rc;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || !db) {
    if (db) sqlite3_close(db);
    return 0;
  }

  (void)sqlite3_enable_load_extension(db, (Size > 0) ? (Data[0] & 1) : 0);
  (void)sqlite3_enable_load_extension(db, (Size > 1) ? (Data[1] & 1) : 1);

  {
    size_t n = Size > 0 ? ((size_t)Data[0] % 32) : 0;
    fname = (char *)malloc(n + 2);
    if (fname) {
      for (i = 0; i < n; i++) {
        unsigned char c = (Size > i + 1) ? Data[i + 1] : 0;
        fname[i] = (char)('a' + (c % 26));
      }
      fname[n] = '\0';
      if (n == 0) {
        fname[0] = 'f';
        fname[1] = '\0';
      }
    }
  }

  if (fname) {
    int nArg = (Size > 2) ? ((int)(Data[2] % 8) - 1) : 0;
    (void)sqlite3_overload_function(db, fname, nArg);
    (void)sqlite3_overload_function(db, fname, (Size > 3) ? (int)(Data[3] % 16) : 1);
  }

  {
    AuxData *aux1 = (AuxData *)malloc(sizeof(AuxData));
    AuxData *aux2 = (AuxData *)malloc(sizeof(AuxData));
    if (aux1) aux1->marker = (Size > 4) ? Data[4] : 0;
    if (aux2) aux2->marker = (Size > 5) ? Data[5] : 0;

    (void)sqlite3_create_module(db, "fuzzmod", &fuzzModule, aux1);
    (void)sqlite3_create_module_v2(db, "fuzzmod2", &fuzzModule, aux2, aux_destructor);

    if ((Size > 6) && (Data[6] & 1)) {
      (void)sqlite3_create_module(db, "fuzzmod", NULL, NULL);
    }
    if ((Size > 7) && (Data[7] & 1)) {
      (void)sqlite3_create_module_v2(db, "fuzzmod2", NULL, NULL, NULL);
    }
  }

  (void)exec_sql(db, "PRAGMA writable_schema=OFF;");
  (void)exec_sql(db, "CREATE VIRTUAL TABLE IF NOT EXISTS vt USING fuzzmod;");
  (void)exec_sql(db, "CREATE VIRTUAL TABLE IF NOT EXISTS vt2 USING fuzzmod2;");

  if (fname) {
    size_t sql_len = strlen(fname) + 256;
    sql = (char *)malloc(sql_len);
    if (sql) {
      snprintf(sql, sql_len,
               "SELECT %s(a) FROM vt;"
               "INSERT OR REPLACE INTO vt(rowid,a,b) VALUES(1,2,3);"
               "UPDATE OR IGNORE vt SET a=4 WHERE rowid=1;"
               "DELETE FROM vt WHERE rowid=1;"
               "SELECT * FROM vt2;",
               fname);
      (void)exec_sql(db, sql);
      free(sql);
      sql = NULL;
    }
  } else {
    (void)exec_sql(db,
                   "INSERT OR ABORT INTO vt(rowid,a,b) VALUES(1,2,3);"
                   "UPDATE OR FAIL vt SET a=9 WHERE rowid=1;"
                   "SELECT * FROM vt;");
  }

  (void)sqlite3_vtab_config(db, SQLITE_VTAB_CONSTRAINT_SUPPORT, 1);

  sqlite3_create_function(db, "fuzz_scalar", 0, SQLITE_UTF8, NULL,
                          fuzz_scalar_void, NULL, NULL);
  (void)exec_sql(db, "SELECT fuzz_scalar();");

  free(fname);
  sqlite3_close(db);
  return 0;
}