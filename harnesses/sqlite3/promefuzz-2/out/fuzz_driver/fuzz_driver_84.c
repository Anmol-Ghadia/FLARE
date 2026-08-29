// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_result_int64 at sqlite3.c:72348:17 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_update_hook at sqlite3.c:154474:18 in sqlite3.h
// sqlite3_profile at sqlite3.c:154419:18 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_create_function at sqlite3.c:154229:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
// sqlite3_update_hook at sqlite3.c:154474:18 in sqlite3.h
// sqlite3_update_hook at sqlite3.c:154474:18 in sqlite3.h
// sqlite3_profile at sqlite3.c:154419:18 in sqlite3.h
// sqlite3_profile at sqlite3.c:154419:18 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_update_hook at sqlite3.c:154474:18 in sqlite3.h
// sqlite3_profile at sqlite3.c:154419:18 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_context_db_handle at sqlite3.c:72719:21 in sqlite3.h
// sqlite3_value_type at sqlite3.c:72141:16 in sqlite3.h
// sqlite3_last_insert_rowid at sqlite3.c:153307:25 in sqlite3.h
// sqlite3_user_data at sqlite3.c:72704:18 in sqlite3.h
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

struct FuzzState {
  int update_count;
  int exec_row_count;
  int profile_count;
  int auth_count;
  sqlite3_int64 last_update_rowid;
  sqlite3_int64 func_last_rowid;
  sqlite3 *func_db;
  int abort_exec_after_rows;
  int auth_mode;
};

static void fuzz_update_hook(void *arg, int op, char const *dbName,
                             char const *tblName, sqlite3_int64 rowid) {
  struct FuzzState *st = (struct FuzzState *)arg;
  if (!st) return;
  st->update_count++;
  st->last_update_rowid = rowid;
  (void)op;
  (void)dbName;
  (void)tblName;
}

static int fuzz_exec_callback(void *arg, int argc, char **argv, char **colv) {
  struct FuzzState *st = (struct FuzzState *)arg;
  int i;
  if (!st) return 0;
  st->exec_row_count++;
  for (i = 0; i < argc; i++) {
    if (argv && argv[i]) st->exec_row_count += (unsigned char)argv[i][0] & 1;
    if (colv && colv[i]) st->exec_row_count += (unsigned char)colv[i][0] & 1;
  }
  if (st->abort_exec_after_rows > 0 && st->exec_row_count >= st->abort_exec_after_rows) {
    return 1;
  }
  return 0;
}

static int fuzz_authorizer(void *arg, int action, const char *p1,
                           const char *p2, const char *p3, const char *p4) {
  struct FuzzState *st = (struct FuzzState *)arg;
  if (st) st->auth_count++;
  (void)p1;
  (void)p2;
  (void)p3;
  (void)p4;

  if (!st) return SQLITE_OK;

  switch (st->auth_mode % 4) {
    case 0:
      return SQLITE_OK;
    case 1:
      if (action == SQLITE_READ) return SQLITE_IGNORE;
      return SQLITE_OK;
    case 2:
      if (action == SQLITE_DROP_TABLE || action == SQLITE_ATTACH || action == SQLITE_DETACH) {
        return SQLITE_DENY;
      }
      return SQLITE_OK;
    default:
      if ((action & 1) != 0) return SQLITE_OK;
      return SQLITE_IGNORE;
  }
}

static void fuzz_profile(void *arg, const char *sql, sqlite3_uint64 ns) {
  struct FuzzState *st = (struct FuzzState *)arg;
  if (!st) return;
  st->profile_count++;
  if (sql && sql[0]) st->profile_count += ((unsigned char)sql[0]) & 1;
  st->profile_count += (int)(ns & 1u);
}

static void fuzz_sqlfunc(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  sqlite3 *db = sqlite3_context_db_handle(ctx);
  sqlite3_int64 rowid = 0;
  struct FuzzState *st = NULL;

  if (argc > 0 && argv && argv[0]) {
    (void)sqlite3_value_type(argv[0]);
  }

  if (db) rowid = sqlite3_last_insert_rowid(db);
  st = (struct FuzzState *)sqlite3_user_data(ctx);
  if (st) {
    st->func_db = db;
    st->func_last_rowid = rowid;
  }
  sqlite3_result_int64(ctx, rowid);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  struct FuzzState st;
  char *errmsg = NULL;
  char *sql = NULL;
  size_t sql_len;
  int rc;
  FILE *fp;
  int flags;

  memset(&st, 0, sizeof(st));
  if (!Data) return 0;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI;
  rc = sqlite3_open_v2("./dummy_file", &db, flags, NULL);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  st.abort_exec_after_rows = (Size > 0) ? (int)(Data[0] % 8) : 0;
  st.auth_mode = (Size > 1) ? (int)(Data[1] % 4) : 0;

  sqlite3_exec(db, "PRAGMA journal_mode=OFF; PRAGMA synchronous=OFF;", NULL, NULL, NULL);

  sqlite3_update_hook(db, fuzz_update_hook, &st);
  sqlite3_profile(db, fuzz_profile, &st);
  sqlite3_set_authorizer(db, fuzz_authorizer, &st);

  sqlite3_create_function(db, "fuzz_lastrowid", 1, SQLITE_UTF8, &st, fuzz_sqlfunc, NULL, NULL);

  sqlite3_exec(
      db,
      "CREATE TABLE IF NOT EXISTS t1(id INTEGER PRIMARY KEY, a TEXT, b INTEGER);"
      "CREATE TABLE IF NOT EXISTS t2(x TEXT);"
      "CREATE TABLE IF NOT EXISTS wr(a TEXT PRIMARY KEY) WITHOUT ROWID;"
      "CREATE TRIGGER IF NOT EXISTS trg AFTER INSERT ON t1 "
      "BEGIN INSERT INTO t2(x) VALUES (new.a); END;",
      NULL, NULL, NULL);

  sqlite3_last_insert_rowid(db);

  sql_len = Size;
  sql = (char *)malloc(sql_len + 1);
  if (sql) {
    if (sql_len) memcpy(sql, Data, sql_len);
    sql[sql_len] = '\0';

    rc = sqlite3_exec(db, sql,
                      (Size > 2 && (Data[2] & 1)) ? fuzz_exec_callback : NULL,
                      &st, &errmsg);
    if (errmsg) {
      sqlite3_free(errmsg);
      errmsg = NULL;
    }

    (void)rc;
    sqlite3_last_insert_rowid(db);

    sqlite3_update_hook(db, NULL, NULL);
    sqlite3_update_hook(db, fuzz_update_hook, &st);

    sqlite3_profile(db, NULL, NULL);
    sqlite3_profile(db, fuzz_profile, &st);

    sqlite3_set_authorizer(db, NULL, NULL);
    sqlite3_set_authorizer(db, fuzz_authorizer, &st);

    sqlite3_exec(db,
                 "INSERT INTO t1(a,b) VALUES('seed',1);"
                 "UPDATE t1 SET b=b+1 WHERE id=1;"
                 "DELETE FROM t1 WHERE id<0;"
                 "INSERT OR IGNORE INTO wr(a) VALUES('k');"
                 "SELECT fuzz_lastrowid('x'), last_insert_rowid(), * FROM t1;",
                 (Size > 3 && (Data[3] & 1)) ? fuzz_exec_callback : NULL,
                 &st, &errmsg);
    if (errmsg) {
      sqlite3_free(errmsg);
      errmsg = NULL;
    }

    sqlite3_last_insert_rowid(db);
    free(sql);
  }

  sqlite3_set_authorizer(db, NULL, NULL);
  sqlite3_update_hook(db, NULL, NULL);
  sqlite3_profile(db, NULL, NULL);
  sqlite3_close(db);
  return 0;
}