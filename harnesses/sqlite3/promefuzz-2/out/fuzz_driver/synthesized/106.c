// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_commit_hook at sqlite3.c:154449:18 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_db_config at sqlite3.c:153163:16 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_progress_handler at sqlite3.c:153989:17 in sqlite3.h
// sqlite3_commit_hook at sqlite3.c:154449:18 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n){
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for(size_t i = 0; i < take; ++i){
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int read_i32(const uint8_t **p, size_t *n){
  return (int)read_u32(p, n);
}

static int read_byte(const uint8_t **p, size_t *n){
  if(*n == 0) return 0;
  int v = **p;
  (*p)++;
  (*n)--;
  return v;
}

static size_t read_len(const uint8_t **p, size_t *n, size_t maxlen){
  if(*n == 0) return 0;
  size_t v = (size_t)read_u32(p, n);
  if(v > *n) v = *n;
  if(v > maxlen) v = maxlen;
  return v;
}

static int progress_cb(void *pArg){
  const uint8_t *b = (const uint8_t *)pArg;
  if(!b) return 0;
  return (b[0] & 1) ? 1 : 0;
}

static int commit_cb(void *pArg){
  const uint8_t *b = (const uint8_t *)pArg;
  if(!b) return 0;
  return (b[0] & 2) ? 1 : 0;
}

static int auth_cb(void *pArg, int action, const char *a, const char *b,
                   const char *c, const char *d){
  const uint8_t *bytes = (const uint8_t *)pArg;
  unsigned seed = bytes ? bytes[0] : 0;
  unsigned mix = (unsigned)action;
  if(a) mix ^= (unsigned)(unsigned char)a[0];
  if(b) mix ^= (unsigned)(unsigned char)b[0] << 1;
  if(c) mix ^= (unsigned)(unsigned char)c[0] << 2;
  if(d) mix ^= (unsigned)(unsigned char)d[0] << 3;
  switch((seed ^ mix) % 3){
    case 0: return SQLITE_OK;
    case 1: return SQLITE_IGNORE;
    default: return SQLITE_DENY;
  }
}

static void exec_sql(sqlite3 *db, const char *sql){
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  sqlite3_free(errmsg);
}

int LLVMFuzzerTestOneInput_106(const uint8_t *Data, size_t Size){
  const uint8_t *p = Data;
  size_t n = Size;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  FILE *fp = NULL;
  char *sql = NULL;
  int rc;

  rc = sqlite3_initialize();
  if(rc != SQLITE_OK){
    return 0;
  }

  fp = fopen("./dummy_file", "wb");
  if(fp){
    if(Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
    fp = NULL;
  }

  rc = sqlite3_open("./dummy_file", &db);
  if(rc != SQLITE_OK || db == NULL){
    if(db) sqlite3_close(db);
    return 0;
  }

  {
    int enable_progress = read_byte(&p, &n) & 1;
    int nOps = read_i32(&p, &n);
    if(enable_progress){
      if(nOps == 0) nOps = 1;
      sqlite3_progress_handler(db, nOps, progress_cb, (void *)Data);
    }else{
      sqlite3_progress_handler(db, nOps, NULL, (void *)Data);
    }
  }

  {
    int use_commit = read_byte(&p, &n) & 1;
    sqlite3_commit_hook(db, use_commit ? commit_cb : NULL, (void *)Data);
  }

  {
    int use_auth = read_byte(&p, &n) & 1;
    sqlite3_set_authorizer(db, use_auth ? auth_cb : NULL, (void *)Data);
  }

  {
    int which = read_byte(&p, &n) % 8;
    int onoff = read_i32(&p, &n) & 1;
    int out = 0;
    switch(which){
      case 0:
        sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, onoff, &out);
        break;
      case 1:
        sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_TRIGGER, onoff, &out);
        break;
      case 2:
        sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_VIEW, onoff, &out);
        break;
      case 3:
        sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_QPSG, onoff, &out);
        break;
      case 4:
        sqlite3_db_config(db, SQLITE_DBCONFIG_DEFENSIVE, onoff, &out);
        break;
      case 5:
        sqlite3_db_config(db, SQLITE_DBCONFIG_WRITABLE_SCHEMA, onoff, &out);
        break;
      case 6:
        sqlite3_db_config(db, SQLITE_DBCONFIG_LEGACY_ALTER_TABLE, onoff, &out);
        break;
      default:
        sqlite3_db_config(db, SQLITE_DBCONFIG_DQS_DML, onoff, &out);
        break;
    }
  }

#ifdef SQLITE_DBCONFIG_LOOKASIDE
  {
    int slot_sz = read_i32(&p, &n);
    int slot_cnt = read_i32(&p, &n);
    if(slot_sz < 0) slot_sz = -slot_sz;
    if(slot_cnt < 0) slot_cnt = -slot_cnt;
    slot_sz %= 4096;
    slot_cnt %= 128;
    sqlite3_db_config(db, SQLITE_DBCONFIG_LOOKASIDE, NULL, slot_sz, slot_cnt);
  }
#endif

#ifdef SQLITE_DBCONFIG_MAINDBNAME
  {
    static const char *names[] = {"main", "temp", "aux", "x", ""};
    const char *nm = names[read_byte(&p, &n) % (sizeof(names)/sizeof(names[0]))];
    sqlite3_db_config(db, SQLITE_DBCONFIG_MAINDBNAME, nm);
  }
#endif

  {
    int limit_ids[] = {
      SQLITE_LIMIT_LENGTH,
      SQLITE_LIMIT_SQL_LENGTH,
      SQLITE_LIMIT_COLUMN,
      SQLITE_LIMIT_EXPR_DEPTH,
      SQLITE_LIMIT_COMPOUND_SELECT,
      SQLITE_LIMIT_VDBE_OP,
      SQLITE_LIMIT_FUNCTION_ARG,
      SQLITE_LIMIT_ATTACHED,
      SQLITE_LIMIT_LIKE_PATTERN_LENGTH,
      SQLITE_LIMIT_VARIABLE_NUMBER,
      SQLITE_LIMIT_TRIGGER_DEPTH,
      SQLITE_LIMIT_WORKER_THREADS
    };
    size_t cnt = sizeof(limit_ids)/sizeof(limit_ids[0]);
    for(int i = 0; i < 4 && n > 0; ++i){
      int id = limit_ids[read_byte(&p, &n) % cnt];
      int newVal = read_i32(&p, &n);
      if(read_byte(&p, &n) & 1) newVal = -1;
      sqlite3_limit(db, id, newVal);
    }
  }

  exec_sql(db, "PRAGMA journal_mode=OFF;");
  exec_sql(db, "PRAGMA synchronous=OFF;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t(a,b,c);");
  exec_sql(db, "BEGIN;");
  exec_sql(db, "INSERT INTO t(a,b,c) VALUES(1,'x',zeroblob(8));");
  exec_sql(db, "INSERT INTO t(a,b,c) VALUES(2,'y',zeroblob(16));");
  exec_sql(db, "UPDATE t SET b='z' WHERE a=1;");
  exec_sql(db, "COMMIT;");
  exec_sql(db, "BEGIN;");
  exec_sql(db, "INSERT INTO t(a,b,c) VALUES(3,'w',zeroblob(4));");
  exec_sql(db, "ROLLBACK;");

  {
    size_t sql_len = read_len(&p, &n, 4096);
    sql = (char *)malloc(sql_len + 1);
    if(sql){
      memcpy(sql, p, sql_len);
      sql[sql_len] = '\0';
      p += sql_len;
      n -= sql_len;

      rc = sqlite3_prepare_v2(db, sql, (int)sql_len, &stmt, NULL);
      if(rc == SQLITE_OK && stmt){
        int steps = 0;
        while(steps < 32){
          rc = sqlite3_step(stmt);
          if(rc != SQLITE_ROW && rc != SQLITE_DONE) break;
          if(rc == SQLITE_DONE) break;
          steps++;
        }
      }
      if(stmt){
        sqlite3_finalize(stmt);
        stmt = NULL;
      }

      exec_sql(db, sql);
      free(sql);
      sql = NULL;
    }
  }

  sqlite3_progress_handler(db, 0, NULL, NULL);
  sqlite3_commit_hook(db, NULL, NULL);
  sqlite3_set_authorizer(db, NULL, NULL);
  sqlite3_close(db);
  return 0;
}