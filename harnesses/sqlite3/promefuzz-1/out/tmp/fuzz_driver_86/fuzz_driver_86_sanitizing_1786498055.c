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

typedef struct {
  sqlite3 *db;
} FuzzCtx;

static void fuzz_sql_func(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  (void)argc;
  (void)argv;
  sqlite3 *db = sqlite3_context_db_handle(ctx);
  if (db) {
    sqlite3_total_changes(db);
    sqlite3_total_changes64(db);
    sqlite3_last_insert_rowid(db);
#ifdef SQLITE_ENABLE_PREUPDATE_HOOK
    sqlite3_preupdate_depth(db);
#endif
  }
  sqlite3_result_null(ctx);
}

#ifdef SQLITE_ENABLE_PREUPDATE_HOOK
static void fuzz_preupdate_hook(
    void *pArg,
    sqlite3 *db,
    int op,
    char const *zDb,
    char const *zName,
    sqlite3_int64 iKey1,
    sqlite3_int64 iKey2) {
  (void)pArg;
  (void)op;
  (void)zDb;
  (void)zName;
  (void)iKey1;
  (void)iKey2;

  sqlite3_preupdate_depth(db);
  sqlite3_total_changes(db);
  sqlite3_total_changes64(db);
  sqlite3_last_insert_rowid(db);
}
#endif

static void exec_sql(sqlite3 *db, const char *sql) {
  char *err = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &err);
  if (err) sqlite3_free(err);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
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

  sqlite3_extended_result_codes(db, 1);
  sqlite3_busy_timeout(db, 1);

  sqlite3_create_function(db, "fuzzfunc", -1, SQLITE_UTF8, NULL,
                          fuzz_sql_func, NULL, NULL);

#ifdef SQLITE_ENABLE_PREUPDATE_HOOK
  sqlite3_preupdate_hook(db, fuzz_preupdate_hook, NULL);
#endif

  exec_sql(db, "PRAGMA foreign_keys=ON;");
  exec_sql(db, "PRAGMA recursive_triggers=ON;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t1(id INTEGER PRIMARY KEY, v TEXT UNIQUE);");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(id INTEGER PRIMARY KEY, ref INTEGER REFERENCES t1(id) ON DELETE CASCADE, v TEXT);");
  exec_sql(db, "CREATE TRIGGER IF NOT EXISTS tr1 AFTER INSERT ON t1 BEGIN INSERT INTO t2(ref,v) VALUES (new.id, new.v); END;");

  sqlite3_total_changes(db);
  sqlite3_total_changes64(db);
  sqlite3_last_insert_rowid(db);
  sqlite3_set_last_insert_rowid(db, (sqlite3_int64)Size);

  if (Size == 0) {
    exec_sql(db, "SELECT fuzzfunc();");
    sqlite3_close(db);
    return 0;
  }

  size_t sql_len = Size;
  char *sql = (char *)sqlite3_malloc64(sql_len + 1);
  if (sql) {
    memcpy(sql, Data, sql_len);
    sql[sql_len] = '\0';

    char *err = NULL;
    sqlite3_exec(db, sql, NULL, NULL, &err);
    if (err) sqlite3_free(err);

    sqlite3_stmt *stmt = NULL;
    const char *tail = NULL;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, &tail) == SQLITE_OK && stmt) {
      int step_rc;
      do {
        step_rc = sqlite3_step(stmt);
      } while (step_rc == SQLITE_ROW);
      sqlite3_finalize(stmt);
    } else if (stmt) {
      sqlite3_finalize(stmt);
    }

    sqlite3_free(sql);
  }

  exec_sql(db, "INSERT OR IGNORE INTO t1(v) VALUES('a');");
  exec_sql(db, "INSERT OR REPLACE INTO t1(id,v) VALUES(1,'b');");
  exec_sql(db, "UPDATE t1 SET v='c' WHERE id=1;");
  exec_sql(db, "DELETE FROM t1 WHERE id=1;");
  exec_sql(db, "SELECT fuzzfunc(1,2,3);");

  sqlite3_set_last_insert_rowid(
      db,
      (sqlite3_int64)(
          ((Size > 0 ? (sqlite3_int64)Data[0] : 0) << 32) ^
          (sqlite3_int64)Size));

  sqlite3_last_insert_rowid(db);
  sqlite3_total_changes(db);
  sqlite3_total_changes64(db);
#ifdef SQLITE_ENABLE_PREUPDATE_HOOK
  sqlite3_preupdate_depth(db);
#endif

  sqlite3_close(db);
  return 0;
}