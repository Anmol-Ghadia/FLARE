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

static int exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
  return rc;
}

static void write_dummy_file(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Data && Size) {
    fwrite(Data, 1, Size, fp);
  }
  fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db1 = NULL;
  sqlite3 *db2 = NULL;
  sqlite3_snapshot *snap1 = NULL;
  sqlite3_snapshot *snap2 = NULL;
  sqlite3_stmt *stmt = NULL;
  int rc;

  write_dummy_file(Data, Size);

  rc = sqlite3_open("./dummy_file", &db1);
  if (rc != SQLITE_OK || db1 == NULL) {
    if (db1) {
      sqlite3_extended_errcode(db1);
      sqlite3_close(db1);
    }
    return 0;
  }

  sqlite3_extended_result_codes(db1, (Size > 0) ? (Data[0] & 1) : 0);

  exec_sql(db1, "PRAGMA journal_mode=WAL;");
  exec_sql(db1, "PRAGMA application_id;");
  exec_sql(db1, "CREATE TABLE IF NOT EXISTS t(x BLOB);");

  if (sqlite3_open("./dummy_file", &db2) == SQLITE_OK && db2 != NULL) {
    sqlite3_extended_result_codes(db2, (Size > 1) ? (Data[1] & 1) : 1);
    exec_sql(db2, "PRAGMA application_id;");
    exec_sql(db2, "BEGIN;");
    exec_sql(db2, "CREATE TABLE IF NOT EXISTS t(x BLOB);");
    exec_sql(db2, "INSERT INTO t(x) VALUES(randomblob(16));");
    exec_sql(db2, "COMMIT;");
  }

#ifdef SQLITE_ENABLE_SNAPSHOT
  exec_sql(db1, "BEGIN;");

  rc = sqlite3_snapshot_get(db1, "main", &snap1);
  if (rc != SQLITE_OK) {
    sqlite3_extended_errcode(db1);
  }

  if (db2) {
    exec_sql(db2, "BEGIN;");
    exec_sql(db2, "INSERT INTO t(x) VALUES(randomblob(16));");
    exec_sql(db2, "COMMIT;");
  }

  rc = sqlite3_snapshot_get(db1, "main", &snap2);
  if (rc != SQLITE_OK) {
    sqlite3_extended_errcode(db1);
  }

  if (snap1 && snap2) {
    (void)sqlite3_snapshot_cmp(snap1, snap2);
    (void)sqlite3_snapshot_cmp(snap2, snap1);
    (void)sqlite3_snapshot_cmp(snap1, snap1);
  }

  if (snap1) {
    rc = sqlite3_snapshot_open(db1, "main", snap1);
    if (rc != SQLITE_OK) {
      sqlite3_extended_errcode(db1);
    }
  }

  if (Size > 2) {
    sqlite3_prepare_v2(db1, "SELECT * FROM t;", -1, &stmt, NULL);
    if (stmt) {
      sqlite3_step(stmt);
      if (snap2) {
        rc = sqlite3_snapshot_open(db1, "main", snap2);
        if (rc != SQLITE_OK) {
          sqlite3_extended_errcode(db1);
        }
      }
      sqlite3_finalize(stmt);
      stmt = NULL;
    }
  }

  exec_sql(db1, "ROLLBACK;");

  if (snap1) sqlite3_snapshot_free(snap1);
  if (snap2) sqlite3_snapshot_free(snap2);
#else
  exec_sql(db1, "BEGIN;");
  exec_sql(db1, "SELECT * FROM non_existing_table;");
  sqlite3_extended_errcode(db1);
  exec_sql(db1, "ROLLBACK;");
#endif

  exec_sql(db1, "SELECT * FROM definitely_missing_table;");
  sqlite3_extended_errcode(db1);

  if (db2) {
    sqlite3_close(db2);
  }
  sqlite3_close(db1);
  return 0;
}