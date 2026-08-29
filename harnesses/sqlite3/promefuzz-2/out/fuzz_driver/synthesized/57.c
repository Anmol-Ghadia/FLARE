// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_vtab_on_conflict at sqlite3.c:131565:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_update_hook at sqlite3.c:154474:18 in sqlite3.h
// sqlite3_update_hook at sqlite3.c:154474:18 in sqlite3.h
// sqlite3_update_hook at sqlite3.c:154474:18 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_interrupt at sqlite3.c:154037:17 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
// sqlite3_context_db_handle at sqlite3.c:72719:21 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_declare_vtab at sqlite3.c:131100:16 in sqlite3.h
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

static void update_hook_cb(void *arg, int op, const char *dbName,
                           const char *tblName, sqlite3_int64 rowid) {
  volatile unsigned *state = (volatile unsigned *)arg;
  if (state) {
    *state ^= (unsigned)op;
    *state ^= (unsigned)rowid;
    if (dbName) *state ^= (unsigned)(unsigned char)dbName[0];
    if (tblName) *state ^= (unsigned)(unsigned char)tblName[0];
  }
}

static void fuzz_sql_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3 *db = sqlite3_context_db_handle(ctx);
  sqlite3_int64 v = db ? sqlite3_last_insert_rowid(db) : 0;
  if (argc > 0 && argv && argv[0]) {
    int t = sqlite3_value_type(argv[0]);
    v ^= (sqlite3_int64)t;
  }
  sqlite3_result_int64(ctx, v);
}

typedef struct {
  sqlite3_vtab base;
  sqlite3 *db;
} FuzzVTab;

typedef struct {
  sqlite3_vtab_cursor base;
  int row;
} FuzzCursor;

static int fuzz_vtab_create(sqlite3 *db, void *pAux, int argc,
                            const char *const *argv, sqlite3_vtab **ppVTab,
                            char **pzErr) {
  (void)pAux;
  (void)argc;
  (void)argv;
  (void)pzErr;
  FuzzVTab *v = (FuzzVTab *)sqlite3_malloc(sizeof(*v));
  if (!v) return SQLITE_NOMEM;
  memset(v, 0, sizeof(*v));
  v->db = db;
  if (sqlite3_declare_vtab(db, "CREATE TABLE x(a,b)") != SQLITE_OK) {
    sqlite3_free(v);
    return SQLITE_ERROR;
  }
  *ppVTab = &v->base;
  return SQLITE_OK;
}

static int fuzz_vtab_connect(sqlite3 *db, void *pAux, int argc,
                             const char *const *argv, sqlite3_vtab **ppVTab,
                             char **pzErr) {
  return fuzz_vtab_create(db, pAux, argc, argv, ppVTab, pzErr);
}

static int fuzz_vtab_bestindex(sqlite3_vtab *pVTab, sqlite3_index_info *pInfo) {
  (void)pVTab;
  if (pInfo) {
    pInfo->estimatedCost = 1.0;
  }
  return SQLITE_OK;
}

static int fuzz_vtab_disconnect(sqlite3_vtab *pVTab) {
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

static int fuzz_vtab_destroy(sqlite3_vtab *pVTab) {
  return fuzz_vtab_disconnect(pVTab);
}

static int fuzz_vtab_open(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor) {
  (void)pVTab;
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

static int fuzz_vtab_filter(sqlite3_vtab_cursor *cur, int idxNum,
                            const char *idxStr, int argc, sqlite3_value **argv) {
  (void)idxNum;
  (void)idxStr;
  (void)argc;
  (void)argv;
  FuzzCursor *c = (FuzzCursor *)cur;
  c->row = 0;
  return SQLITE_OK;
}

static int fuzz_vtab_next(sqlite3_vtab_cursor *cur) {
  FuzzCursor *c = (FuzzCursor *)cur;
  c->row++;
  return SQLITE_OK;
}

static int fuzz_vtab_eof(sqlite3_vtab_cursor *cur) {
  FuzzCursor *c = (FuzzCursor *)cur;
  return c->row > 0;
}

static int fuzz_vtab_column(sqlite3_vtab_cursor *cur, sqlite3_context *ctx, int i) {
  (void)cur;
  sqlite3_result_int(ctx, i);
  return SQLITE_OK;
}

static int fuzz_vtab_rowid(sqlite3_vtab_cursor *cur, sqlite3_int64 *pRowid) {
  FuzzCursor *c = (FuzzCursor *)cur;
  *pRowid = (sqlite3_int64)(c->row + 1);
  return SQLITE_OK;
}

static int fuzz_vtab_update(sqlite3_vtab *pVTab, int argc, sqlite3_value **argv,
                            sqlite3_int64 *pRowid) {
  (void)argc;
  (void)argv;
  FuzzVTab *v = (FuzzVTab *)pVTab;
  int conflict = sqlite3_vtab_on_conflict(v->db);
  if (pRowid) {
    *pRowid = (sqlite3_int64)conflict;
  }
  if (conflict == SQLITE_IGNORE) return SQLITE_CONSTRAINT;
  return SQLITE_OK;
}

static int fuzz_vtab_begin(sqlite3_vtab *pVTab) { (void)pVTab; return SQLITE_OK; }
static int fuzz_vtab_sync(sqlite3_vtab *pVTab) { (void)pVTab; return SQLITE_OK; }
static int fuzz_vtab_commit(sqlite3_vtab *pVTab) { (void)pVTab; return SQLITE_OK; }
static int fuzz_vtab_rollback(sqlite3_vtab *pVTab) { (void)pVTab; return SQLITE_OK; }
static int fuzz_vtab_rename(sqlite3_vtab *pVTab, const char *zNew) {
  (void)pVTab;
  (void)zNew;
  return SQLITE_OK;
}

static const sqlite3_module fuzz_module = {
  1,
  fuzz_vtab_create,
  fuzz_vtab_connect,
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
  fuzz_vtab_update,
  fuzz_vtab_begin,
  fuzz_vtab_sync,
  fuzz_vtab_commit,
  fuzz_vtab_rollback,
  0,
  fuzz_vtab_rename,
  0,
  0,
  0,
  0
};

static void exec_sql(sqlite3 *db, const char *sql) {
  char *err = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &err);
  if (err) sqlite3_free(err);
}

int LLVMFuzzerTestOneInput_57(const uint8_t *Data, size_t Size) {
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

  volatile unsigned hook_state = 0;
  sqlite3_update_hook(db, update_hook_cb, (void *)&hook_state);
  sqlite3_update_hook(db, NULL, NULL);
  sqlite3_update_hook(db, update_hook_cb, (void *)&hook_state);

  sqlite3_create_function(db, "fuzz_db_handle", 1, SQLITE_UTF8, NULL,
                          fuzz_sql_func, NULL, NULL);

  sqlite3_create_module(db, "fuzzmod", &fuzz_module, NULL);
  sqlite3_create_module(db, "fuzzmod_drop", NULL, NULL);

  exec_sql(db, "PRAGMA writable_schema=OFF;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t1(id INTEGER PRIMARY KEY, v TEXT);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(x INTEGER UNIQUE);");
  exec_sql(db, "CREATE VIRTUAL TABLE IF NOT EXISTS vt USING fuzzmod;");
  exec_sql(db, "DELETE FROM t1;");
  exec_sql(db, "DELETE FROM t2;");

  if (Size > 0) {
    int mode = Data[0] % 8;
    switch (mode) {
      case 0:
        exec_sql(db, "INSERT INTO t1(v) VALUES('a');");
        break;
      case 1:
        exec_sql(db, "INSERT OR REPLACE INTO t2(x) VALUES(1);");
        exec_sql(db, "INSERT OR REPLACE INTO t2(x) VALUES(1);");
        break;
      case 2:
        exec_sql(db, "INSERT INTO t1(v) VALUES('b'); UPDATE t1 SET v='c' WHERE id=1;");
        break;
      case 3:
        exec_sql(db, "INSERT INTO t1(v) VALUES('d'); DELETE FROM t1 WHERE id=1;");
        break;
      case 4:
        exec_sql(db, "INSERT OR IGNORE INTO vt(rowid,a,b) VALUES(1,2,3);");
        break;
      case 5:
        exec_sql(db, "INSERT OR REPLACE INTO vt(rowid,a,b) VALUES(1,4,5);");
        break;
      case 6:
        exec_sql(db, "SELECT fuzz_db_handle(v) FROM t1;");
        break;
      default:
        exec_sql(db, "SELECT * FROM vt;");
        break;
    }
  }

  sqlite3_interrupt(db);
  (void)sqlite3_last_insert_rowid(db);

  if (Size > 1) {
    char *sql = (char *)malloc(Size);
    if (sql) {
      size_t i;
      for (i = 1; i < Size; ++i) {
        unsigned char c = Data[i];
        sql[i - 1] = (char)((c >= 32 && c <= 126) ? c : ' ');
      }
      sql[Size - 1] = '\0';
      exec_sql(db, sql);
      free(sql);
    }
  }

  (void)sqlite3_last_insert_rowid(db);
  sqlite3_create_module(db, "fuzzmod", NULL, NULL);
  sqlite3_close(db);
  return 0;
}