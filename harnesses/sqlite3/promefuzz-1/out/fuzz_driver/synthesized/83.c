// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_declare_vtab at sqlite3.c:131100:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_vtab_collation at sqlite3.c:139787:24 in sqlite3.h
// sqlite3_libversion_number at sqlite3.c:152370:16 in sqlite3.h
// sqlite3_libversion_number at sqlite3.c:152370:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_stricmp at sqlite3.c:19635:16 in sqlite3.h
// sqlite3_collation_needed at sqlite3.c:155803:16 in sqlite3.h
// sqlite3_collation_needed at sqlite3.c:155803:16 in sqlite3.h
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
// sqlite3_create_module_v2 at sqlite3.c:130427:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
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

typedef struct {
  int id;
  unsigned seed;
} CollCtx;

typedef struct {
  int id;
} ModuleCtx;

typedef struct DummyVtab {
  sqlite3_vtab base;
} DummyVtab;

typedef struct DummyCursor {
  sqlite3_vtab_cursor base;
  int row;
} DummyCursor;

static uint8_t fi_u8(FuzzInput *in) {
  if (in->off >= in->size) return 0;
  return in->data[in->off++];
}

static uint32_t fi_u32(FuzzInput *in) {
  uint32_t v = 0;
  int i;
  for (i = 0; i < 4; i++) {
    v = (v << 8) | fi_u8(in);
  }
  return v;
}

static int fi_bool(FuzzInput *in) {
  return (int)(fi_u8(in) & 1);
}

static size_t fi_len(FuzzInput *in, size_t maxlen) {
  if (maxlen == 0) return 0;
  return (size_t)(fi_u8(in) % (maxlen + 1));
}

static char *fi_string(FuzzInput *in, size_t maxlen) {
  size_t n = fi_len(in, maxlen);
  char *s = (char *)malloc(n + 1);
  if (!s) return NULL;
  if (n > 0) {
    size_t i;
    for (i = 0; i < n; i++) {
      uint8_t c = fi_u8(in);
      if (c == 0) c = (uint8_t)('A' + (i % 26));
      s[i] = (char)c;
    }
  }
  s[n] = '\0';
  return s;
}

static int pick_encoding(FuzzInput *in) {
  static const int encs[] = {
    SQLITE_UTF8, SQLITE_UTF16LE, SQLITE_UTF16BE,
    SQLITE_UTF16, SQLITE_UTF16_ALIGNED
  };
  return encs[fi_u8(in) % (sizeof(encs) / sizeof(encs[0]))];
}

static int fuzz_compare(void *pArg, int n1, const void *s1, int n2, const void *s2) {
  const unsigned char *a = (const unsigned char *)s1;
  const unsigned char *b = (const unsigned char *)s2;
  const CollCtx *ctx = (const CollCtx *)pArg;
  int n = n1 < n2 ? n1 : n2;
  int i;
  unsigned tweak = ctx ? ctx->seed : 0U;

  for (i = 0; i < n; i++) {
    unsigned char ca = a ? a[i] : 0;
    unsigned char cb = b ? b[i] : 0;
    if ((tweak & 1U) != 0) {
      if (ca >= 'A' && ca <= 'Z') ca = (unsigned char)(ca - 'A' + 'a');
      if (cb >= 'A' && cb <= 'Z') cb = (unsigned char)(cb - 'A' + 'a');
    }
    if (ca != cb) return (int)ca - (int)cb;
  }
  if ((tweak & 2U) != 0) {
    if (n1 == n2) return 0;
    return n1 < n2 ? -1 : 1;
  }
  return (n1 - n2);
}

static void coll_destroy(void *p) {
  free(p);
}

static int dummyCreateOrConnect(
    sqlite3 *db, void *pAux, int argc, const char *const *argv,
    sqlite3_vtab **ppVTab, char **pzErr) {
  DummyVtab *vtab;
  (void)pAux;
  (void)argc;
  (void)argv;
  (void)pzErr;

  vtab = (DummyVtab *)sqlite3_malloc(sizeof(*vtab));
  if (!vtab) return SQLITE_NOMEM;
  memset(vtab, 0, sizeof(*vtab));
  if (sqlite3_declare_vtab(db, "CREATE TABLE x(a TEXT, b TEXT)") != SQLITE_OK) {
    sqlite3_free(vtab);
    return SQLITE_ERROR;
  }
  *ppVTab = &vtab->base;
  return SQLITE_OK;
}

static int dummyBestIndex(sqlite3_vtab *pVTab, sqlite3_index_info *pIdxInfo) {
  int i;
  (void)pVTab;
  if (!pIdxInfo) return SQLITE_ERROR;

  for (i = 0; i < pIdxInfo->nConstraint; i++) {
    const char *coll = sqlite3_vtab_collation(pIdxInfo, i);
    if (pIdxInfo->aConstraintUsage) {
      pIdxInfo->aConstraintUsage[i].argvIndex = pIdxInfo->aConstraint[i].usable ? (i + 1) : 0;
      pIdxInfo->aConstraintUsage[i].omit = (unsigned char)(coll != NULL);
    }
  }
  pIdxInfo->idxNum = pIdxInfo->nConstraint;
  pIdxInfo->orderByConsumed = 0;
  pIdxInfo->estimatedCost = (double)(pIdxInfo->nConstraint + 1);
  if (sqlite3_libversion_number() >= 3008002) {
    pIdxInfo->estimatedRows = 1;
  }
  if (sqlite3_libversion_number() >= 3009000) {
    pIdxInfo->idxFlags = 0;
  }
  return SQLITE_OK;
}

static int dummyDisconnectOrDestroy(sqlite3_vtab *pVTab) {
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

static int dummyOpen(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor) {
  DummyCursor *c;
  (void)pVTab;
  c = (DummyCursor *)sqlite3_malloc(sizeof(*c));
  if (!c) return SQLITE_NOMEM;
  memset(c, 0, sizeof(*c));
  *ppCursor = &c->base;
  return SQLITE_OK;
}

static int dummyClose(sqlite3_vtab_cursor *cur) {
  sqlite3_free(cur);
  return SQLITE_OK;
}

static int dummyFilter(sqlite3_vtab_cursor *cur, int idxNum, const char *idxStr,
                       int argc, sqlite3_value **argv) {
  DummyCursor *c = (DummyCursor *)cur;
  (void)idxNum;
  (void)idxStr;
  (void)argc;
  (void)argv;
  c->row = 0;
  return SQLITE_OK;
}

static int dummyNext(sqlite3_vtab_cursor *cur) {
  DummyCursor *c = (DummyCursor *)cur;
  c->row++;
  return SQLITE_OK;
}

static int dummyEof(sqlite3_vtab_cursor *cur) {
  DummyCursor *c = (DummyCursor *)cur;
  return c->row > 0;
}

static int dummyColumn(sqlite3_vtab_cursor *cur, sqlite3_context *ctx, int i) {
  (void)cur;
  sqlite3_result_int(ctx, i);
  return SQLITE_OK;
}

static int dummyRowid(sqlite3_vtab_cursor *cur, sqlite3_int64 *pRowid) {
  DummyCursor *c = (DummyCursor *)cur;
  *pRowid = (sqlite3_int64)c->row;
  return SQLITE_OK;
}

static void module_destroy(void *p) {
  free(p);
}

static void coll_needed_cb(void *pArg, sqlite3 *db, int eTextRep, const char *zName) {
  CollCtx *ctx;
  int rc;
  (void)eTextRep;

  if (!db || !zName) return;
  ctx = (CollCtx *)malloc(sizeof(*ctx));
  if (!ctx) return;
  ctx->id = 1234;
  ctx->seed = 0xC0U;
  rc = sqlite3_create_collation_v2(db, zName, SQLITE_UTF8, ctx, fuzz_compare, coll_destroy);
  if (rc != SQLITE_OK) {
    free(ctx);
  }
  (void)pArg;
}

int LLVMFuzzerTestOneInput_83(const uint8_t *Data, size_t Size) {
  FuzzInput in;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *name1 = NULL, *name2 = NULL, *name3 = NULL, *modname = NULL;
  char *s1 = NULL, *s2 = NULL, *sql = NULL;
  int rc;
  sqlite3_module module;
  ModuleCtx *mctx = NULL;

  in.data = Data;
  in.size = Size;
  in.off = 0;

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
      if (Size > 0) fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || !db) {
    if (db) sqlite3_close(db);
    return 0;
  }

  memset(&module, 0, sizeof(module));
  module.iVersion = 1;
  module.xCreate = dummyCreateOrConnect;
  module.xConnect = dummyCreateOrConnect;
  module.xBestIndex = dummyBestIndex;
  module.xDisconnect = dummyDisconnectOrDestroy;
  module.xDestroy = dummyDisconnectOrDestroy;
  module.xOpen = dummyOpen;
  module.xClose = dummyClose;
  module.xFilter = dummyFilter;
  module.xNext = dummyNext;
  module.xEof = dummyEof;
  module.xColumn = dummyColumn;
  module.xRowid = dummyRowid;

  name1 = fi_string(&in, 32);
  name2 = fi_string(&in, 32);
  name3 = fi_string(&in, 32);
  modname = fi_string(&in, 32);
  s1 = fi_string(&in, 64);
  s2 = fi_string(&in, 64);

  if (!name1 || !name2 || !name3 || !modname || !s1 || !s2) {
    goto cleanup;
  }

  sqlite3_stricmp(s1, s2);
  sqlite3_stricmp(fi_bool(&in) ? NULL : s1, fi_bool(&in) ? NULL : s2);

  sqlite3_collation_needed(db, NULL, coll_needed_cb);
  sqlite3_collation_needed(db, db, fi_bool(&in) ? coll_needed_cb : NULL);

  {
    CollCtx *ctx1 = (CollCtx *)malloc(sizeof(*ctx1));
    if (ctx1) {
      ctx1->id = 1;
      ctx1->seed = fi_u32(&in);
      rc = sqlite3_create_collation_v2(
          db,
          fi_bool(&in) ? name1 : "fuzzcoll",
          pick_encoding(&in),
          ctx1,
          fi_bool(&in) ? fuzz_compare : NULL,
          coll_destroy);
      if (rc != SQLITE_OK) {
        free(ctx1);
      }
    }
  }

  {
    CollCtx *ctx2 = (CollCtx *)malloc(sizeof(*ctx2));
    if (ctx2) {
      ctx2->id = 2;
      ctx2->seed = fi_u32(&in);
      rc = sqlite3_create_collation(
          db,
          fi_bool(&in) ? name2 : "fuzzcoll2",
          pick_encoding(&in),
          ctx2,
          fi_bool(&in) ? fuzz_compare : NULL);
      if (rc != SQLITE_OK || !fi_bool(&in)) {
        free(ctx2);
      }
    }
  }

  {
    CollCtx *ctx3 = (CollCtx *)malloc(sizeof(*ctx3));
    if (ctx3) {
      ctx3->id = 3;
      ctx3->seed = fi_u32(&in);
      rc = sqlite3_create_collation_v2(
          db,
          fi_bool(&in) ? name1 : "fuzzcoll",
          pick_encoding(&in),
          ctx3,
          fuzz_compare,
          coll_destroy);
      if (rc != SQLITE_OK) {
        free(ctx3);
      }
    }
  }

  mctx = (ModuleCtx *)malloc(sizeof(*mctx));
  if (mctx) {
    mctx->id = (int)fi_u32(&in);
    rc = sqlite3_create_module_v2(
        db,
        fi_bool(&in) ? modname : "fuzzmod",
        &module,
        mctx,
        module_destroy);
    if (rc != SQLITE_OK) {
      free(mctx);
      mctx = NULL;
    } else {
      mctx = NULL;
    }
  }

  sqlite3_exec(db, "CREATE VIRTUAL TABLE IF NOT EXISTS vt USING fuzzmod", NULL, NULL, NULL);
  sqlite3_exec(db, "SELECT * FROM vt WHERE a = 'x' COLLATE fuzzcoll", NULL, NULL, NULL);
  sqlite3_exec(db, "SELECT * FROM vt WHERE b > 'y' COLLATE fuzzcoll2", NULL, NULL, NULL);

  {
    const char *coll = fi_bool(&in) ? name3 : "missing_collation";
    size_t sql_len = strlen(coll) + 64;
    sql = (char *)malloc(sql_len);
    if (sql) {
      snprintf(sql, sql_len, "SELECT 'a' = 'b' COLLATE \"%s\"", coll);
      if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_step(stmt);
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

cleanup:
  if (stmt) sqlite3_finalize(stmt);
  free(name1);
  free(name2);
  free(name3);
  free(modname);
  free(s1);
  free(s2);
  free(sql);
  sqlite3_close(db);
  return 0;
}