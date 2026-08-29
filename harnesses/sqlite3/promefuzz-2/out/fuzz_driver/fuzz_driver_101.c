// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_declare_vtab at sqlite3.c:131100:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_vtab_collation at sqlite3.c:139787:24 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_log at sqlite3.c:17539:17 in sqlite3.h
// sqlite3_create_function_v2 at sqlite3.c:154242:16 in sqlite3.h
// sqlite3_create_function_v2 at sqlite3.c:154242:16 in sqlite3.h
// sqlite3_create_function_v2 at sqlite3.c:154242:16 in sqlite3.h
// sqlite3_create_function_v2 at sqlite3.c:154242:16 in sqlite3.h
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_overload_function at sqlite3.c:154335:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_int64 at sqlite3.c:72103:25 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_value_text at sqlite3.c:72123:33 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
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

static uint32_t read_u32(const uint8_t **data, size_t *size) {
  uint32_t v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; ++i) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static int read_int(const uint8_t **data, size_t *size) {
  return (int)read_u32(data, size);
}

static unsigned char read_byte(const uint8_t **data, size_t *size) {
  if (*size == 0) return 0;
  unsigned char c = **data;
  (*data)++;
  (*size)--;
  return c;
}

static char *make_string(const uint8_t **data, size_t *size, size_t max_len) {
  size_t len = 0;
  if (*size > 0) {
    len = read_byte(data, size);
    if (len > max_len) len = max_len;
    if (len > *size) len = *size;
  }
  char *s = (char *)malloc(len + 1);
  if (!s) return NULL;
  if (len > 0) memcpy(s, *data, len);
  s[len] = '\0';
  *data += len;
  *size -= len;
  return s;
}

static void udf_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int sum = argc;
  for (int i = 0; i < argc; ++i) {
    sum ^= sqlite3_value_type(argv[i]);
    if (sqlite3_value_type(argv[i]) == SQLITE_INTEGER) {
      sum ^= (int)sqlite3_value_int64(argv[i]);
    } else if (sqlite3_value_type(argv[i]) == SQLITE_TEXT) {
      const unsigned char *t = sqlite3_value_text(argv[i]);
      if (t) sum ^= (int)t[0];
    }
  }
  sqlite3_result_int(ctx, sum);
}

static void udf_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int *acc = (int *)sqlite3_aggregate_context(ctx, sizeof(int));
  if (!acc) return;
  *acc += argc;
  for (int i = 0; i < argc; ++i) {
    *acc ^= sqlite3_value_type(argv[i]);
  }
}

static void udf_final(sqlite3_context *ctx) {
  int *acc = (int *)sqlite3_aggregate_context(ctx, 0);
  sqlite3_result_int(ctx, acc ? *acc : 0);
}

static void destroy_ptr(void *p) {
  free(p);
}

static int coll_compare(void *pArg, int n1, const void *p1, int n2, const void *p2) {
  (void)pArg;
  const unsigned char *a = (const unsigned char *)p1;
  const unsigned char *b = (const unsigned char *)p2;
  int n = n1 < n2 ? n1 : n2;
  int rc = memcmp(a, b, (size_t)n);
  if (rc != 0) return rc;
  if (n1 < n2) return -1;
  if (n1 > n2) return 1;
  return 0;
}

typedef struct {
  sqlite3_vtab base;
} DummyVTab;

typedef struct {
  sqlite3_vtab_cursor base;
  int row;
} DummyCursor;

static int modCreate(sqlite3 *db, void *pAux, int argc, const char *const *argv,
                     sqlite3_vtab **ppVTab, char **pzErr) {
  (void)pAux;
  (void)argc;
  (void)argv;
  (void)pzErr;
  DummyVTab *v = (DummyVTab *)sqlite3_malloc(sizeof(DummyVTab));
  if (!v) return SQLITE_NOMEM;
  memset(v, 0, sizeof(*v));
  int rc = sqlite3_declare_vtab(db, "CREATE TABLE x(a TEXT, b INT)");
  if (rc != SQLITE_OK) {
    sqlite3_free(v);
    return rc;
  }
  *ppVTab = &v->base;
  return SQLITE_OK;
}

static int modConnect(sqlite3 *db, void *pAux, int argc, const char *const *argv,
                      sqlite3_vtab **ppVTab, char **pzErr) {
  return modCreate(db, pAux, argc, argv, ppVTab, pzErr);
}

static int modBestIndex(sqlite3_vtab *pVTab, sqlite3_index_info *pIdxInfo) {
  (void)pVTab;
  if (!pIdxInfo) return SQLITE_OK;
  for (int i = 0; i < pIdxInfo->nConstraint; ++i) {
    const char *coll = sqlite3_vtab_collation(pIdxInfo, i);
    if (pIdxInfo->aConstraintUsage) {
      pIdxInfo->aConstraintUsage[i].argvIndex = (i < 8) ? (i + 1) : 0;
      pIdxInfo->aConstraintUsage[i].omit = (unsigned char)((coll && coll[0]) ? 1 : 0);
    }
  }
  pIdxInfo->idxNum = pIdxInfo->nConstraint;
  pIdxInfo->orderByConsumed = (pIdxInfo->nOrderBy > 0) ? 1 : 0;
  pIdxInfo->estimatedCost = (double)(pIdxInfo->nConstraint + 1);
#if SQLITE_VERSION_NUMBER >= 3008002
  pIdxInfo->estimatedRows = 1;
#endif
#if SQLITE_VERSION_NUMBER >= 3009000
  pIdxInfo->idxFlags = 0;
#endif
  return SQLITE_OK;
}

static int modDisconnect(sqlite3_vtab *pVTab) {
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

static int modDestroy(sqlite3_vtab *pVTab) {
  return modDisconnect(pVTab);
}

static int modOpen(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor) {
  (void)pVTab;
  DummyCursor *c = (DummyCursor *)sqlite3_malloc(sizeof(DummyCursor));
  if (!c) return SQLITE_NOMEM;
  memset(c, 0, sizeof(*c));
  *ppCursor = &c->base;
  return SQLITE_OK;
}

static int modClose(sqlite3_vtab_cursor *cur) {
  sqlite3_free(cur);
  return SQLITE_OK;
}

static int modFilter(sqlite3_vtab_cursor *pVtabCursor, int idxNum, const char *idxStr,
                     int argc, sqlite3_value **argv) {
  (void)idxNum;
  (void)idxStr;
  (void)argc;
  (void)argv;
  DummyCursor *c = (DummyCursor *)pVtabCursor;
  c->row = 0;
  return SQLITE_OK;
}

static int modNext(sqlite3_vtab_cursor *cur) {
  DummyCursor *c = (DummyCursor *)cur;
  c->row++;
  return SQLITE_OK;
}

static int modEof(sqlite3_vtab_cursor *cur) {
  DummyCursor *c = (DummyCursor *)cur;
  return c->row > 0;
}

static int modColumn(sqlite3_vtab_cursor *cur, sqlite3_context *ctx, int i) {
  DummyCursor *c = (DummyCursor *)cur;
  if (i == 0) {
    sqlite3_result_text(ctx, "x", -1, SQLITE_STATIC);
  } else {
    sqlite3_result_int(ctx, c->row);
  }
  return SQLITE_OK;
}

static int modRowid(sqlite3_vtab_cursor *cur, sqlite3_int64 *pRowid) {
  DummyCursor *c = (DummyCursor *)cur;
  *pRowid = c->row + 1;
  return SQLITE_OK;
}

static int modUpdate(sqlite3_vtab *pVTab, int argc, sqlite3_value **argv, sqlite3_int64 *pRowid) {
  (void)pVTab;
  (void)argc;
  (void)argv;
  if (pRowid) *pRowid = 1;
  return SQLITE_OK;
}

static int modBegin(sqlite3_vtab *pVTab) { (void)pVTab; return SQLITE_OK; }
static int modSync(sqlite3_vtab *pVTab) { (void)pVTab; return SQLITE_OK; }
static int modCommit(sqlite3_vtab *pVTab) { (void)pVTab; return SQLITE_OK; }
static int modRollback(sqlite3_vtab *pVTab) { (void)pVTab; return SQLITE_OK; }
static int modRename(sqlite3_vtab *pVTab, const char *zNew) { (void)pVTab; (void)zNew; return SQLITE_OK; }
static int modSavepoint(sqlite3_vtab *pVTab, int i) { (void)pVTab; (void)i; return SQLITE_OK; }
static int modRelease(sqlite3_vtab *pVTab, int i) { (void)pVTab; (void)i; return SQLITE_OK; }
static int modRollbackTo(sqlite3_vtab *pVTab, int i) { (void)pVTab; (void)i; return SQLITE_OK; }
static int modFindFunction(sqlite3_vtab *pVtab, int nArg, const char *zName,
                           void (**pxFunc)(sqlite3_context *, int, sqlite3_value **),
                           void **ppArg) {
  (void)pVtab;
  (void)nArg;
  (void)zName;
  if (pxFunc) *pxFunc = udf_scalar;
  if (ppArg) *ppArg = NULL;
  return 1;
}

static const sqlite3_module dummy_module = {
  3,
  modCreate,
  modConnect,
  modBestIndex,
  modDisconnect,
  modDestroy,
  modOpen,
  modClose,
  modFilter,
  modNext,
  modEof,
  modColumn,
  modRowid,
  modUpdate,
  modBegin,
  modSync,
  modCommit,
  modRollback,
  modFindFunction,
  modRename,
  modSavepoint,
  modRelease,
  modRollbackTo,
  NULL
};

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *data = Data;
  size_t size = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  if (sqlite3_open("./dummy_file", &db) != SQLITE_OK) {
    if (db) sqlite3_close(db);
    return 0;
  }

  char *logfmt = make_string(&data, &size, 128);
  if (!logfmt || logfmt[0] == '\0') {
    free(logfmt);
    logfmt = (char *)malloc(8);
    if (logfmt) memcpy(logfmt, "log:%d%s", 8);
  }
  if (logfmt) {
    int errcode = read_int(&data, &size);
    int num = read_int(&data, &size);
    char *arg = make_string(&data, &size, 64);
    if (!arg) arg = strdup("x");
    sqlite3_log(errcode, logfmt, num, arg ? arg : "");
    free(arg);
    free(logfmt);
  }

  for (int iter = 0; iter < 4 && db; ++iter) {
    char *fname = make_string(&data, &size, 32);
    char *cname = make_string(&data, &size, 32);
    char *mname = make_string(&data, &size, 32);
    char *oname = make_string(&data, &size, 32);

    if (!fname || fname[0] == '\0') {
      free(fname);
      fname = strdup("f");
    }
    if (!cname || cname[0] == '\0') {
      free(cname);
      cname = strdup("c");
    }
    if (!mname || mname[0] == '\0') {
      free(mname);
      mname = strdup("mod");
    }
    if (!oname || oname[0] == '\0') {
      free(oname);
      oname = strdup("over");
    }

    int nArg = (read_int(&data, &size) % 10) - 3;
    int eTextRep1 = read_int(&data, &size);
    int eTextRep2 = read_int(&data, &size);
    int collRep = read_int(&data, &size);
    int overNArg = (read_int(&data, &size) % 10) - 3;

    void *app1 = malloc(16);
    void *app2 = malloc(16);
    if (app1) memset(app1, 0xA5, 16);
    if (app2) memset(app2, 0x5A, 16);

    switch (read_byte(&data, &size) % 3) {
      case 0:
        sqlite3_create_function_v2(db, fname, nArg, eTextRep1, app1,
                                   udf_scalar, NULL, NULL, destroy_ptr);
        app1 = NULL;
        break;
      case 1:
        sqlite3_create_function_v2(db, fname, nArg, eTextRep1, app1,
                                   NULL, udf_step, udf_final, destroy_ptr);
        app1 = NULL;
        break;
      default:
        sqlite3_create_function_v2(db, fname, nArg, eTextRep1, app1,
                                   udf_scalar, udf_step, udf_final, destroy_ptr);
        app1 = NULL;
        break;
    }

    sqlite3_create_function_v2(db, fname, nArg, eTextRep2, app2,
                               udf_scalar, NULL, NULL, destroy_ptr);
    app2 = NULL;

    void *collArg = malloc(8);
    sqlite3_create_collation_v2(db, cname, collRep, collArg, coll_compare, destroy_ptr);
    collArg = NULL;

    if ((read_byte(&data, &size) & 1) == 0) {
      sqlite3_create_module(db, mname, &dummy_module, NULL);
    } else {
      sqlite3_create_module(db, mname, NULL, NULL);
    }

    sqlite3_overload_function(db, oname, overNArg);

    free(app1);
    free(app2);
    free(collArg);
    free(fname);
    free(cname);
    free(mname);
    free(oname);
  }

  sqlite3_exec(db, "CREATE VIRTUAL TABLE IF NOT EXISTS vt USING mod;", NULL, NULL, NULL);
  sqlite3_exec(db, "SELECT * FROM vt WHERE a='x' COLLATE BINARY;", NULL, NULL, NULL);
  sqlite3_exec(db, "SELECT over(1);", NULL, NULL, NULL);
  sqlite3_exec(db, "SELECT f(1), f('x');", NULL, NULL, NULL);

  sqlite3_close(db);
  return 0;
}