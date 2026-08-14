// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_collation_needed at sqlite3.c:155803:16 in sqlite3.h
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_create_collation16 at sqlite3.c:155773:16 in sqlite3.h
// sqlite3_create_function_v2 at sqlite3.c:154242:16 in sqlite3.h
// sqlite3_create_function_v2 at sqlite3.c:154242:16 in sqlite3.h
// sqlite3_create_module_v2 at sqlite3.c:130427:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
// sqlite3_create_collation at sqlite3.c:155735:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_value_bytes at sqlite3.c:72091:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_aggregate_context at sqlite3.c:72792:18 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_create_collation_v2 at sqlite3.c:155748:16 in sqlite3.h
// sqlite3_declare_vtab at sqlite3.c:131100:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
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

static int fuzz_compare(void *pArg, int n1, const void *s1, int n2, const void *s2) {
  unsigned seed = pArg ? *(unsigned *)pArg : 0U;
  const unsigned char *a = (const unsigned char *)s1;
  const unsigned char *b = (const unsigned char *)s2;
  int min = n1 < n2 ? n1 : n2;
  int i;
  for (i = 0; i < min; i++) {
    unsigned av = a ? a[i] : 0U;
    unsigned bv = b ? b[i] : 0U;
    if (av != bv) return (int)((av ^ seed) - (bv ^ seed));
  }
  if (n1 != n2) return n1 - n2;
  return (int)(seed & 1U) ? 0 : 0;
}

static void fuzz_destroy(void *p) {
  free(p);
}

static void fuzz_scalar(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int i;
  sqlite3_int64 acc = 0;
  for (i = 0; i < argc; i++) {
    acc += sqlite3_value_bytes(argv[i]);
    acc += sqlite3_value_type(argv[i]);
  }
  sqlite3_result_int64(ctx, acc);
}

static void fuzz_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3_int64 *state = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, sizeof(sqlite3_int64));
  int i;
  if (!state) return;
  for (i = 0; i < argc; i++) {
    *state += sqlite3_value_bytes(argv[i]);
    *state += sqlite3_value_type(argv[i]);
  }
}

static void fuzz_final(sqlite3_context *ctx) {
  sqlite3_int64 *state = (sqlite3_int64 *)sqlite3_aggregate_context(ctx, 0);
  sqlite3_result_int64(ctx, state ? *state : 0);
}

static void fuzz_collation_needed(void *pArg, sqlite3 *db, int eTextRep, const char *zName) {
  unsigned *seed = (unsigned *)pArg;
  unsigned *heap_seed;
  if (!db || !zName) return;
  heap_seed = (unsigned *)malloc(sizeof(unsigned));
  if (!heap_seed) return;
  *heap_seed = seed ? *seed : 0U;
  if (sqlite3_create_collation_v2(db, zName, eTextRep, heap_seed, fuzz_compare, fuzz_destroy) != SQLITE_OK) {
    free(heap_seed);
  }
}

static int vtabCreateOrConnect(sqlite3 *db, void *pAux, int argc, const char *const *argv,
                               sqlite3_vtab **ppVTab, char **pzErr) {
  (void)pAux;
  (void)argc;
  (void)argv;
  (void)pzErr;
  *ppVTab = NULL;
  return sqlite3_declare_vtab(db, "CREATE TABLE x(value)");
}

static int vtabBestIndex(sqlite3_vtab *pVTab, sqlite3_index_info *pInfo) {
  (void)pVTab;
  (void)pInfo;
  return SQLITE_OK;
}

static int vtabDisconnect(sqlite3_vtab *pVTab) {
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

static int vtabDestroy(sqlite3_vtab *pVTab) {
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

static int vtabOpen(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor) {
  (void)pVTab;
  *ppCursor = (sqlite3_vtab_cursor *)sqlite3_malloc(sizeof(sqlite3_vtab_cursor));
  if (!*ppCursor) return SQLITE_NOMEM;
  memset(*ppCursor, 0, sizeof(sqlite3_vtab_cursor));
  return SQLITE_OK;
}

static int vtabClose(sqlite3_vtab_cursor *cur) {
  sqlite3_free(cur);
  return SQLITE_OK;
}

static int vtabFilter(sqlite3_vtab_cursor *cur, int idxNum, const char *idxStr,
                      int argc, sqlite3_value **argv) {
  (void)cur;
  (void)idxNum;
  (void)idxStr;
  (void)argc;
  (void)argv;
  return SQLITE_OK;
}

static int vtabNext(sqlite3_vtab_cursor *cur) {
  (void)cur;
  return SQLITE_OK;
}

static int vtabEof(sqlite3_vtab_cursor *cur) {
  (void)cur;
  return 1;
}

static int vtabColumn(sqlite3_vtab_cursor *cur, sqlite3_context *ctx, int i) {
  (void)cur;
  sqlite3_result_int(ctx, i);
  return SQLITE_OK;
}

static int vtabRowid(sqlite3_vtab_cursor *cur, sqlite3_int64 *pRowid) {
  (void)cur;
  *pRowid = 0;
  return SQLITE_OK;
}

static void write_dummy_file(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

int LLVMFuzzerTestOneInput_49(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  char *name = NULL;
  char *funcname = NULL;
  void *name16 = NULL;
  unsigned *seed1 = NULL;
  unsigned *seed2 = NULL;
  unsigned *seed3 = NULL;
  unsigned *seed4 = NULL;
  sqlite3_module module;
  sqlite3_stmt *stmt = NULL;
  int rc;
  int encs[] = {SQLITE_UTF8, SQLITE_UTF16LE, SQLITE_UTF16BE, SQLITE_UTF16, SQLITE_UTF16_ALIGNED};
  int enc1, enc2, enc3, funcEnc;
  int nArg;
  size_t name_len, func_len, name16_len;
  unsigned base_seed = 0;

  write_dummy_file(Data, Size);

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || !db) {
    if (db) sqlite3_close(db);
    return 0;
  }

  if (Size > 0) base_seed = Data[0];
  if (Size > 1) base_seed |= ((unsigned)Data[1] << 8);
  if (Size > 2) base_seed |= ((unsigned)Data[2] << 16);
  if (Size > 3) base_seed |= ((unsigned)Data[3] << 24);

  name_len = Size > 4 ? ((size_t)Data[4] % 32U) + 1U : 8U;
  func_len = Size > 5 ? ((size_t)Data[5] % 32U) + 1U : 8U;
  name16_len = Size > 6 ? ((size_t)Data[6] % 32U) + 2U : 10U;

  name = (char *)malloc(name_len + 1);
  funcname = (char *)malloc(func_len + 1);
  name16 = malloc(name16_len + 2);
  seed1 = (unsigned *)malloc(sizeof(unsigned));
  seed2 = (unsigned *)malloc(sizeof(unsigned));
  seed3 = (unsigned *)malloc(sizeof(unsigned));
  seed4 = (unsigned *)malloc(sizeof(unsigned));

  if (!name || !funcname || !name16 || !seed1 || !seed2 || !seed3 || !seed4) {
    free(name);
    free(funcname);
    free(name16);
    free(seed1);
    free(seed2);
    free(seed3);
    free(seed4);
    sqlite3_close(db);
    return 0;
  }

  memset(name, 0, name_len + 1);
  memset(funcname, 0, func_len + 1);
  memset(name16, 0, name16_len + 2);

  {
    size_t i;
    for (i = 0; i < name_len; i++) {
      unsigned char c = (i + 8 < Size) ? Data[i + 8] : (unsigned char)('A' + (i % 26));
      name[i] = (char)('A' + (c % 26));
    }
    for (i = 0; i < func_len; i++) {
      unsigned char c = (i + 40 < Size) ? Data[i + 40] : (unsigned char)('a' + (i % 26));
      funcname[i] = (char)('a' + (c % 26));
    }
    for (i = 0; i < name16_len; i++) {
      ((unsigned char *)name16)[i] = (i + 72 < Size) ? Data[i + 72] : (unsigned char)(i + 1);
    }
    ((unsigned char *)name16)[name16_len] = 0;
    ((unsigned char *)name16)[name16_len + 1] = 0;
  }

  *seed1 = base_seed ^ 0x11111111U;
  *seed2 = base_seed ^ 0x22222222U;
  *seed3 = base_seed ^ 0x33333333U;
  *seed4 = base_seed ^ 0x44444444U;

  enc1 = encs[(Size > 7 ? Data[7] : 0) % (sizeof(encs) / sizeof(encs[0]))];
  enc2 = encs[(Size > 8 ? Data[8] : 1) % (sizeof(encs) / sizeof(encs[0]))];
  enc3 = encs[(Size > 9 ? Data[9] : 2) % (sizeof(encs) / sizeof(encs[0]))];
  funcEnc = encs[(Size > 10 ? Data[10] : 3) % (sizeof(encs) / sizeof(encs[0]))];
  nArg = (Size > 11) ? ((int)(Data[11] % 8) - 1) : -1;

  sqlite3_collation_needed(db, seed4, fuzz_collation_needed);

  rc = sqlite3_create_collation_v2(db, name, enc1, seed1, fuzz_compare, fuzz_destroy);
  if (rc != SQLITE_OK) {
    free(seed1);
    seed1 = NULL;
  }

  rc = sqlite3_create_collation(db, name, enc2, seed2, fuzz_compare);
  if (rc != SQLITE_OK) {
    free(seed2);
    seed2 = NULL;
  }

  rc = sqlite3_create_collation16(db, name16, enc3, seed3, fuzz_compare);
  if (rc != SQLITE_OK) {
    free(seed3);
    seed3 = NULL;
  }

  rc = sqlite3_create_function_v2(
      db, funcname, nArg, funcEnc, NULL,
      fuzz_scalar,
      NULL,
      NULL,
      NULL);
  (void)rc;

  rc = sqlite3_create_function_v2(
      db, "aggfuzz", nArg, funcEnc, NULL,
      NULL,
      fuzz_step,
      fuzz_final,
      NULL);
  (void)rc;

  memset(&module, 0, sizeof(module));
  module.iVersion = 1;
  module.xCreate = vtabCreateOrConnect;
  module.xConnect = vtabCreateOrConnect;
  module.xBestIndex = vtabBestIndex;
  module.xDisconnect = vtabDisconnect;
  module.xDestroy = vtabDestroy;
  module.xOpen = vtabOpen;
  module.xClose = vtabClose;
  module.xFilter = vtabFilter;
  module.xNext = vtabNext;
  module.xEof = vtabEof;
  module.xColumn = vtabColumn;
  module.xRowid = vtabRowid;

  rc = sqlite3_create_module_v2(db, name, &module, NULL, NULL);
  (void)rc;

  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(x TEXT, y INTEGER);", NULL, NULL, NULL);
  sqlite3_exec(db, "INSERT INTO t(x,y) VALUES('a',1),('b',2),('c',3);", NULL, NULL, NULL);

  {
    char *sql = sqlite3_mprintf(
        "SELECT x FROM t ORDER BY x COLLATE \"%q\"; "
        "SELECT \"%q\"('abc'); "
        "SELECT aggfuzz(y) FROM t;",
        name, funcname);
    if (sql) {
      sqlite3_exec(db, sql, NULL, NULL, NULL);
      sqlite3_free(sql);
    }
  }

  if (sqlite3_prepare_v2(db, "SELECT x FROM t ORDER BY x COLLATE missingcoll;", -1, &stmt, NULL) == SQLITE_OK) {
    while (sqlite3_step(stmt) == SQLITE_ROW) {
    }
  }
  if (stmt) {
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  sqlite3_create_collation_v2(db, name, enc1, NULL, NULL, NULL);
  sqlite3_create_collation(db, name, enc2, NULL, NULL);

  free(name);
  free(funcname);
  free(name16);
  if (seed2) free(seed2);
  if (seed3) free(seed3);
  if (seed4) free(seed4);

  sqlite3_close(db);
  return 0;
}