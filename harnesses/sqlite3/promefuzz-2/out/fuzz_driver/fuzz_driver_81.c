// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_result_int at sqlite3.c:72344:17 in sqlite3.h
// sqlite3_result_text at sqlite3.c:72374:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_enable_load_extension at sqlite3.c:114763:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_create_module at sqlite3.c:130412:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errcode at sqlite3.c:154850:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc at sqlite3.c:15738:18 in sqlite3.h
// sqlite3_vtab_config at sqlite3.c:131583:16 in sqlite3.h
// sqlite3_vtab_config at sqlite3.c:131583:16 in sqlite3.h
// sqlite3_vtab_config at sqlite3.c:131583:16 in sqlite3.h
// sqlite3_declare_vtab at sqlite3.c:131100:16 in sqlite3.h
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

static uint32_t read_u32(const uint8_t **p, size_t *n){
  uint32_t v = 0;
  size_t i, m = *n < 4 ? *n : 4;
  for(i = 0; i < m; i++){
    v |= ((uint32_t)(*p)[i]) << (8 * i);
  }
  *p += m;
  *n -= m;
  return v;
}

static int read_int(const uint8_t **p, size_t *n){
  return (int)read_u32(p, n);
}

static const char *make_cstring(const uint8_t **p, size_t *n, size_t maxlen){
  static char buf[512];
  size_t i = 0;
  size_t lim = *n < maxlen ? *n : maxlen;
  while(i < lim && i < sizeof(buf) - 1){
    unsigned char c = (*p)[i];
    if(c == 0) break;
    buf[i] = (char)((c >= 32 && c <= 126) ? c : ('a' + (c % 26)));
    i++;
  }
  buf[i] = '\0';
  *p += lim;
  *n -= lim;
  return buf;
}

static int fuzz_authorizer(void *pUserData, int action,
                           const char *p1, const char *p2,
                           const char *dbName, const char *triggerName){
  (void)p1; (void)p2; (void)dbName; (void)triggerName;
  uintptr_t v = (uintptr_t)pUserData;
  switch((action + (int)(v & 3)) % 4){
    case 0: return SQLITE_OK;
    case 1: return SQLITE_IGNORE;
    case 2: return SQLITE_DENY;
    default: return 9999;
  }
}

typedef struct FuzzVtab {
  sqlite3_vtab base;
  sqlite3 *db;
} FuzzVtab;

typedef struct FuzzCursor {
  sqlite3_vtab_cursor base;
  int row;
  int maxrow;
} FuzzCursor;

static int fuzz_vtab_create(sqlite3 *db, void *pAux, int argc,
                            const char *const *argv, sqlite3_vtab **ppVTab,
                            char **pzErr){
  FuzzVtab *vtab = (FuzzVtab *)sqlite3_malloc(sizeof(FuzzVtab));
  int mode = (int)(uintptr_t)pAux;
  (void)argc; (void)argv; (void)pzErr;
  if(!vtab) return SQLITE_NOMEM;
  memset(vtab, 0, sizeof(*vtab));
  vtab->db = db;

  sqlite3_vtab_config(db, SQLITE_VTAB_CONSTRAINT_SUPPORT, mode & 1);
#ifdef SQLITE_VTAB_INNOCUOUS
  sqlite3_vtab_config(db, SQLITE_VTAB_INNOCUOUS);
#endif
#ifdef SQLITE_VTAB_DIRECTONLY
  if(mode & 2){
    sqlite3_vtab_config(db, SQLITE_VTAB_DIRECTONLY);
  }
#endif

  if(sqlite3_declare_vtab(db, "CREATE TABLE x(value, extra)") != SQLITE_OK){
    sqlite3_free(vtab);
    return SQLITE_ERROR;
  }
  *ppVTab = &vtab->base;
  return SQLITE_OK;
}

static int fuzz_vtab_connect(sqlite3 *db, void *pAux, int argc,
                             const char *const *argv, sqlite3_vtab **ppVTab,
                             char **pzErr){
  return fuzz_vtab_create(db, pAux, argc, argv, ppVTab, pzErr);
}

static int fuzz_vtab_bestindex(sqlite3_vtab *pVTab, sqlite3_index_info *pIdxInfo){
  (void)pVTab;
  if(pIdxInfo){
    pIdxInfo->estimatedCost = 1.0;
    pIdxInfo->estimatedRows = 1;
    pIdxInfo->idxNum = 0;
    pIdxInfo->idxStr = NULL;
    pIdxInfo->needToFreeIdxStr = 0;
    pIdxInfo->orderByConsumed = 0;
  }
  return SQLITE_OK;
}

static int fuzz_vtab_disconnect(sqlite3_vtab *pVTab){
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

static int fuzz_vtab_destroy(sqlite3_vtab *pVTab){
  return fuzz_vtab_disconnect(pVTab);
}

static int fuzz_vtab_open(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor){
  FuzzCursor *c = (FuzzCursor *)sqlite3_malloc(sizeof(FuzzCursor));
  (void)pVTab;
  if(!c) return SQLITE_NOMEM;
  memset(c, 0, sizeof(*c));
  c->row = 0;
  c->maxrow = 2;
  *ppCursor = &c->base;
  return SQLITE_OK;
}

static int fuzz_vtab_close(sqlite3_vtab_cursor *cur){
  sqlite3_free(cur);
  return SQLITE_OK;
}

static int fuzz_vtab_filter(sqlite3_vtab_cursor *pVtabCursor, int idxNum,
                            const char *idxStr, int argc, sqlite3_value **argv){
  FuzzCursor *c = (FuzzCursor *)pVtabCursor;
  (void)idxNum; (void)idxStr; (void)argc; (void)argv;
  c->row = 0;
  c->maxrow = 2;
  return SQLITE_OK;
}

static int fuzz_vtab_next(sqlite3_vtab_cursor *cur){
  FuzzCursor *c = (FuzzCursor *)cur;
  c->row++;
  return SQLITE_OK;
}

static int fuzz_vtab_eof(sqlite3_vtab_cursor *cur){
  FuzzCursor *c = (FuzzCursor *)cur;
  return c->row >= c->maxrow;
}

static int fuzz_vtab_column(sqlite3_vtab_cursor *cur, sqlite3_context *ctx, int i){
  FuzzCursor *c = (FuzzCursor *)cur;
  if(i == 0){
    sqlite3_result_int(ctx, c->row);
  }else{
    sqlite3_result_text(ctx, "x", -1, SQLITE_STATIC);
  }
  return SQLITE_OK;
}

static int fuzz_vtab_rowid(sqlite3_vtab_cursor *cur, sqlite3_int64 *pRowid){
  FuzzCursor *c = (FuzzCursor *)cur;
  *pRowid = (sqlite3_int64)c->row + 1;
  return SQLITE_OK;
}

static sqlite3_module gModule = {
  3,
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
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  const uint8_t *p = Data;
  size_t n = Size;
  int rc;
  int i;

  FILE *fp = fopen("./dummy_file", "wb");
  if(fp){
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if(rc != SQLITE_OK || db == NULL){
    if(db) sqlite3_close(db);
    return 0;
  }

  sqlite3_exec(db, "PRAGMA journal_mode=OFF;", NULL, NULL, NULL);
  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS t(a,b);", NULL, NULL, NULL);
  sqlite3_exec(db, "INSERT INTO t(a,b) VALUES(1,2),(3,4);", NULL, NULL, NULL);

  for(i = 0; i < 8 && n > 0; i++){
    int choice = read_int(&p, &n) % 8;
    switch(choice){
      case 0: {
        int op = read_int(&p, &n);
        int onoff = read_int(&p, &n);
        int out = 0;
        switch(op % 8){
          case 0:
            sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, onoff, &out);
            break;
          case 1:
            sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_TRIGGER, onoff, &out);
            break;
          case 2:
#ifdef SQLITE_DBCONFIG_ENABLE_VIEW
            sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_VIEW, onoff, &out);
#else
            sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, onoff, &out);
#endif
            break;
          case 3:
#ifdef SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION
            sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, onoff, &out);
#else
            sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, onoff, &out);
#endif
            break;
          case 4:
#ifdef SQLITE_DBCONFIG_DEFENSIVE
            sqlite3_db_config(db, SQLITE_DBCONFIG_DEFENSIVE, onoff, &out);
#else
            sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, onoff, &out);
#endif
            break;
          case 5:
#ifdef SQLITE_DBCONFIG_TRUSTED_SCHEMA
            sqlite3_db_config(db, SQLITE_DBCONFIG_TRUSTED_SCHEMA, onoff, &out);
#else
            sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, onoff, &out);
#endif
            break;
          case 6: {
#ifdef SQLITE_DBCONFIG_LOOKASIDE
            static unsigned char lookaside_buf[256];
            sqlite3_db_config(db, SQLITE_DBCONFIG_LOOKASIDE,
                              lookaside_buf,
                              (int)(read_u32(&p, &n) % 64),
                              (int)(read_u32(&p, &n) % 8));
#else
            sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, onoff, &out);
#endif
            break;
          }
          default: {
#ifdef SQLITE_DBCONFIG_MAINDBNAME
            const char *name = make_cstring(&p, &n, 32);
            sqlite3_db_config(db, SQLITE_DBCONFIG_MAINDBNAME, name);
#else
            sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, onoff, &out);
#endif
            break;
          }
        }
        break;
      }

      case 1: {
        sqlite3_enable_load_extension(db, read_int(&p, &n));
        break;
      }

      case 2: {
        const char *modname = make_cstring(&p, &n, 32);
        void *aux = (void *)(uintptr_t)(read_u32(&p, &n) & 3);
        if((read_u32(&p, &n) & 1) == 0){
          sqlite3_create_module(db, modname[0] ? modname : "fuzzmod", &gModule, aux);
        }else{
          sqlite3_create_module(db, modname[0] ? modname : "fuzzmod", NULL, aux);
        }
        break;
      }

      case 3: {
        (void)sqlite3_errcode(db);
        break;
      }

      case 4: {
        if(read_u32(&p, &n) & 1){
          sqlite3_set_authorizer(db, fuzz_authorizer, (void *)(uintptr_t)read_u32(&p, &n));
        }else{
          sqlite3_set_authorizer(db, NULL, NULL);
        }
        break;
      }

      case 5: {
        const char *sqls[] = {
          "SELECT * FROM t;",
          "SELECT count(*) FROM t;",
          "INSERT INTO t(a,b) VALUES(5,6);",
          "UPDATE t SET b=b+1 WHERE a=1;",
          "DELETE FROM t WHERE a=3;",
          "CREATE VIRTUAL TABLE IF NOT EXISTS vt USING fuzzmod;",
          "SELECT * FROM vt;",
          "DROP TABLE IF EXISTS vt;"
        };
        const char *sql = sqls[read_u32(&p, &n) % (sizeof(sqls)/sizeof(sqls[0]))];
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if(rc == SQLITE_OK && stmt){
          sqlite3_step(stmt);
        }
        if(stmt){
          sqlite3_finalize(stmt);
          stmt = NULL;
        }
        (void)sqlite3_errcode(db);
        break;
      }

      case 6: {
        const char *name = make_cstring(&p, &n, 24);
        char sql[256];
        snprintf(sql, sizeof(sql),
                 "CREATE VIRTUAL TABLE IF NOT EXISTS \"%s\" USING fuzzmod;",
                 name[0] ? name : "vtx");
        sqlite3_create_module(db, "fuzzmod", &gModule, (void *)(uintptr_t)(read_u32(&p, &n) & 3));
        sqlite3_exec(db, sql, NULL, NULL, NULL);
        sqlite3_exec(db, "SELECT * FROM vtx;", NULL, NULL, NULL);
        break;
      }

      default: {
        sqlite3_exec(db, "SELECT * FROM sqlite_master;", NULL, NULL, NULL);
        (void)sqlite3_errcode(db);
        break;
      }
    }
  }

  sqlite3_set_authorizer(db, NULL, NULL);
  if(stmt) sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}