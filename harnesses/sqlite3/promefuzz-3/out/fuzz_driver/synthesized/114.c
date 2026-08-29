// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_vtab_nochange at sqlite3.c:72738:16 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_vtab_on_conflict at sqlite3.c:131565:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_db_mutex at sqlite3.c:153096:27 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_db_mutex at sqlite3.c:153096:27 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_db_mutex at sqlite3.c:153096:27 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_context_db_handle at sqlite3.c:72719:21 in sqlite3.h
// sqlite3_db_mutex at sqlite3.c:153096:27 in sqlite3.h
// sqlite3_changes at sqlite3.c:153344:16 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_null at sqlite3.c:72352:17 in sqlite3.h
// sqlite3_vtab_config at sqlite3.c:131583:16 in sqlite3.h
// sqlite3_vtab_config at sqlite3.c:131583:16 in sqlite3.h
// sqlite3_vtab_config at sqlite3.c:131583:16 in sqlite3.h
// sqlite3_declare_vtab at sqlite3.c:131100:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; i++) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
  return rc;
}

static void udf_ctx_db_handle(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  (void)argv;
  sqlite3 *db = sqlite3_context_db_handle(ctx);
  if (db) {
    (void)sqlite3_db_mutex(db);
    sqlite3_result_int(ctx, sqlite3_changes(db));
  } else {
    sqlite3_result_null(ctx);
  }
}

typedef struct FuzzVTab {
  sqlite3_vtab base;
  sqlite3 *db;
} FuzzVTab;

typedef struct FuzzCursor {
  sqlite3_vtab_cursor base;
  sqlite3_int64 rowid;
  int eof;
} FuzzCursor;

static int fuzz_vtab_connect(
    sqlite3 *db, void *pAux, int argc, const char *const *argv,
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

  if (sqlite3_declare_vtab(db, "CREATE TABLE x(a,b,c)") != SQLITE_OK) {
    return SQLITE_ERROR;
  }

  FuzzVTab *v = (FuzzVTab *)sqlite3_malloc(sizeof(*v));
  if (!v) return SQLITE_NOMEM;
  memset(v, 0, sizeof(*v));
  v->db = db;
  *ppVTab = &v->base;
  return SQLITE_OK;
}

static int fuzz_vtab_disconnect(sqlite3_vtab *pVTab) {
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

static int fuzz_vtab_bestindex(sqlite3_vtab *tab, sqlite3_index_info *pIdxInfo) {
  (void)tab;
  if (pIdxInfo) {
    pIdxInfo->estimatedCost = 1.0;
    pIdxInfo->estimatedRows = 1;
  }
  return SQLITE_OK;
}

static int fuzz_vtab_open(sqlite3_vtab *p, sqlite3_vtab_cursor **ppCursor) {
  (void)p;
  FuzzCursor *c = (FuzzCursor *)sqlite3_malloc(sizeof(*c));
  if (!c) return SQLITE_NOMEM;
  memset(c, 0, sizeof(*c));
  *ppCursor = &c->base;
  return SQLITE_OK;
}

static int fuzz_vtab_close(sqlite3_vtab_cursor *cur) {
  sqlite3_free(cur);
  return SQLITE_OK;
}

static int fuzz_vtab_filter(
    sqlite3_vtab_cursor *pVtabCursor, int idxNum, const char *idxStr,
    int argc, sqlite3_value **argv) {
  (void)idxNum;
  (void)idxStr;
  (void)argc;
  (void)argv;
  FuzzCursor *c = (FuzzCursor *)pVtabCursor;
  c->rowid = 1;
  c->eof = 0;
  return SQLITE_OK;
}

static int fuzz_vtab_next(sqlite3_vtab_cursor *cur) {
  FuzzCursor *c = (FuzzCursor *)cur;
  c->eof = 1;
  c->rowid++;
  return SQLITE_OK;
}

static int fuzz_vtab_eof(sqlite3_vtab_cursor *cur) {
  FuzzCursor *c = (FuzzCursor *)cur;
  return c->eof;
}

static int fuzz_vtab_column(sqlite3_vtab_cursor *cur, sqlite3_context *ctx, int i) {
  (void)cur;
  if (sqlite3_vtab_nochange(ctx)) {
    return SQLITE_OK;
  }
  sqlite3_result_int(ctx, i);
  return SQLITE_OK;
}

static int fuzz_vtab_rowid(sqlite3_vtab_cursor *cur, sqlite3_int64 *pRowid) {
  FuzzCursor *c = (FuzzCursor *)cur;
  *pRowid = c->rowid;
  return SQLITE_OK;
}

static int fuzz_vtab_update(
    sqlite3_vtab *pVTab, int argc, sqlite3_value **argv, sqlite3_int64 *pRowid) {
  FuzzVTab *v = (FuzzVTab *)pVTab;
  (void)sqlite3_vtab_on_conflict(v->db);
  if (pRowid) *pRowid = 1;
  (void)argc;
  (void)argv;
  return SQLITE_OK;
}

static sqlite3_module fuzz_module = {
  0,
  0,
  fuzz_vtab_connect,
  fuzz_vtab_bestindex,
  fuzz_vtab_disconnect,
  fuzz_vtab_disconnect,
  fuzz_vtab_open,
  fuzz_vtab_close,
  fuzz_vtab_filter,
  fuzz_vtab_next,
  fuzz_vtab_eof,
  fuzz_vtab_column,
  fuzz_vtab_rowid,
  fuzz_vtab_update,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};

int LLVMFuzzerTestOneInput_114(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  if (sqlite3_open_v2("./dummy_file", &db, flags, NULL) != SQLITE_OK || !db) {
    if (db) sqlite3_close(db);
    return 0;
  }

  (void)sqlite3_db_mutex(db);
  (void)sqlite3_changes(db);

  (void)sqlite3_create_function(db, "f_ctx_db", 0, SQLITE_UTF8, NULL,
                                udf_ctx_db_handle, NULL, NULL);

  (void)sqlite3_create_module(db, "fuzzmod", &fuzz_module, NULL);

  (void)exec_sql(db, "PRAGMA foreign_keys=OFF;");
  (void)exec_sql(db, "CREATE TABLE IF NOT EXISTS t1(x INTEGER, y TEXT);");
  (void)exec_sql(db, "CREATE VIRTUAL TABLE IF NOT EXISTS vt USING fuzzmod;");

  for (int iter = 0; iter < 8 && n > 0; iter++) {
    uint32_t op = read_u32(&p, &n) % 9;
    switch (op) {
      case 0:
        (void)exec_sql(db, "INSERT INTO t1(x,y) VALUES(1,'a');");
        break;
      case 1:
        (void)exec_sql(db, "UPDATE t1 SET y='b' WHERE x=1;");
        break;
      case 2:
        (void)exec_sql(db, "DELETE FROM t1 WHERE x=1;");
        break;
      case 3:
        (void)exec_sql(db, "SELECT f_ctx_db();");
        break;
      case 4:
        (void)exec_sql(db, "INSERT OR REPLACE INTO vt(rowid,a,b,c) VALUES(1,2,3,4);");
        break;
      case 5:
        (void)exec_sql(db, "UPDATE OR IGNORE vt SET a=5,b=6 WHERE rowid=1;");
        break;
      case 6:
        (void)exec_sql(db, "SELECT a,b,c FROM vt;");
        break;
      case 7:
        (void)sqlite3_db_mutex(db);
        (void)sqlite3_changes(db);
        break;
      case 8:
      default:
        (void)exec_sql(db, "BEGIN;");
        (void)exec_sql(db, "INSERT INTO t1(x,y) VALUES(2,'z');");
        (void)exec_sql(db, "COMMIT;");
        break;
    }
  }

  (void)sqlite3_changes(db);
  (void)sqlite3_db_mutex(db);

  sqlite3_close(db);
  return 0;
}