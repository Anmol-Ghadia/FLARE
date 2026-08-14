#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

static void update_cb(void *pArg, int op, const char *dbName, const char *tblName, sqlite3_int64 rowid) {
  volatile unsigned long long *state = (volatile unsigned long long *)pArg;
  if (!state) return;
  *state ^= (unsigned long long)op;
  if (dbName) *state ^= (unsigned long long)(unsigned char)dbName[0] << 8;
  if (tblName) *state ^= (unsigned long long)(unsigned char)tblName[0] << 16;
  *state ^= (unsigned long long)rowid;
}

static int commit_cb(void *pArg) {
  volatile unsigned long long *state = (volatile unsigned long long *)pArg;
  if (state) *state ^= 0xC0MM17ULL;
  return (state && ((*state & 1ULL) != 0)) ? 1 : 0;
}

static void fuzz_sql_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3 *db = sqlite3_context_db_handle(ctx);
  sqlite3_int64 lastid = 0;
  sqlite3_int64 total = 0;
  if (db) {
    lastid = sqlite3_last_insert_rowid(db);
    total = sqlite3_total_changes64(db);
    sqlite3_set_last_insert_rowid(db, lastid ^ total);
  }
  if (argc > 0 && argv && argv[0]) {
    sqlite3_result_int64(ctx, sqlite3_value_int64(argv[0]) ^ lastid ^ total);
  } else {
    sqlite3_result_int64(ctx, lastid ^ total);
  }
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  volatile unsigned long long hook_state = 0;
  int rc;
  sqlite3_int64 v1 = 0, v2 = 0, v3 = 0;
  size_t sql_len;
  char *sql;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  sqlite3_update_hook(db, update_cb, (void *)&hook_state);
  sqlite3_commit_hook(db, commit_cb, (void *)&hook_state);

  sqlite3_create_function(db, "fuzzfunc", 1, SQLITE_UTF8, NULL, fuzz_sql_func, NULL, NULL);

  exec_sql(db, "PRAGMA journal_mode=OFF;");
  exec_sql(db, "PRAGMA synchronous=OFF;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t1(id INTEGER PRIMARY KEY, x TEXT, y INTEGER);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(a INTEGER PRIMARY KEY, b BLOB);");

  v1 = sqlite3_total_changes64(db);
  v2 = sqlite3_last_insert_rowid(db);
  sqlite3_set_last_insert_rowid(db, (sqlite3_int64)Size);
  v3 = sqlite3_last_insert_rowid(db);
  hook_state ^= (unsigned long long)(v1 ^ v2 ^ v3);

  exec_sql(db, "BEGIN;");
  exec_sql(db, "INSERT INTO t1(x,y) VALUES('seed',1);");
  exec_sql(db, "UPDATE t1 SET y=y+1 WHERE id=1;");
  exec_sql(db, "DELETE FROM t1 WHERE id<0;");
  exec_sql(db, "COMMIT;");

  v1 = sqlite3_total_changes64(db);
  v2 = sqlite3_last_insert_rowid(db);
  sqlite3_set_last_insert_rowid(db, v1 ^ v2 ^ (sqlite3_int64)hook_state);

  sql_len = Size > 4096 ? 4096 : Size;
  sql = (char *)sqlite3_malloc64(sql_len + 1);
  if (sql) {
    memcpy(sql, Data, sql_len);
    sql[sql_len] = '\0';

    rc = sqlite3_prepare_v2(db, sql, (int)sql_len, &stmt, NULL);
    if (rc == SQLITE_OK && stmt) {
      int steps = 0;
      while (steps < 32) {
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
          (void)sqlite3_total_changes64(db);
          (void)sqlite3_last_insert_rowid(db);
        } else if (rc == SQLITE_DONE) {
          break;
        } else {
          break;
        }
        steps++;
      }
    }
    if (stmt) {
      sqlite3_finalize(stmt);
      stmt = NULL;
    }

    exec_sql(db, sql);
    sqlite3_free(sql);
  }

  exec_sql(db, "BEGIN;");
  exec_sql(db, "INSERT OR REPLACE INTO t1(id,x,y) VALUES(1,'abc',2);");
  exec_sql(db, "SELECT fuzzfunc(123);");
  exec_sql(db, "ROLLBACK;");

  (void)sqlite3_update_hook(db, NULL, NULL);
  (void)sqlite3_commit_hook(db, NULL, NULL);

  (void)sqlite3_total_changes64(db);
  (void)sqlite3_last_insert_rowid(db);

  sqlite3_close(db);
  return 0;
}