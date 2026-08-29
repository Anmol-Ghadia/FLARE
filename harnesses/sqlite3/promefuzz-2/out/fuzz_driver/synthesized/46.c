// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
// sqlite3_db_readonly at sqlite3.c:156766:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_txn_state at sqlite3.c:153500:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_wal_checkpoint at sqlite3.c:154686:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_db_readonly at sqlite3.c:156766:16 in sqlite3.h
// sqlite3_txn_state at sqlite3.c:153500:16 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_wal_checkpoint_v2 at sqlite3.c:154622:16 in sqlite3.h
// sqlite3_extended_errcode at sqlite3.c:154859:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_extended_result_codes at sqlite3.c:156064:16 in sqlite3.h
// sqlite3_busy_timeout at sqlite3.c:154020:16 in sqlite3.h
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

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t i, m = *n < 4 ? *n : 4;
  for (i = 0; i < m; i++) {
    v = (v << 8) | (*p)[i];
  }
  *p += m;
  *n -= m;
  return v;
}

static int read_flag(const uint8_t **p, size_t *n) {
  if (*n == 0) return 0;
  int v = (**p) & 1;
  (*p)++;
  (*n)--;
  return v;
}

static int read_small_int(const uint8_t **p, size_t *n, int mod) {
  if (*n == 0 || mod <= 0) return 0;
  int v = (**p) % mod;
  (*p)++;
  (*n)--;
  return v;
}

static char *make_cstring(const uint8_t **p, size_t *n, size_t max_len) {
  size_t len, i;
  char *s;
  if (max_len == 0) max_len = 1;
  len = (*n < max_len) ? *n : max_len;
  s = (char *)sqlite3_malloc64(len + 1);
  if (!s) return NULL;
  for (i = 0; i < len; i++) {
    unsigned char c = (*p)[i];
    if (c == 0) c = 'A';
    s[i] = (char)c;
  }
  s[len] = '\0';
  *p += len;
  *n -= len;
  return s;
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
}

int LLVMFuzzerTestOneInput_46(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;
  sqlite3 *db = NULL;
  sqlite3 *db2 = NULL;
  FILE *fp;
  int rc;
  int i;

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

  sqlite3_extended_result_codes(db, 1);
  sqlite3_busy_timeout(db, 1);

  exec_sql(db, "PRAGMA journal_mode=WAL;");
  exec_sql(db, "PRAGMA synchronous=OFF;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t1("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "a TEXT NOT NULL,"
               "b INTEGER,"
               "c BLOB"
               ");");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(x INTEGER PRIMARY KEY, y TEXT) WITHOUT ROWID;");
  exec_sql(db, "CREATE VIEW IF NOT EXISTS v1 AS SELECT a,b FROM t1;");
  exec_sql(db, "CREATE TEMP TABLE IF NOT EXISTS temp_tt(z TEXT);");
  exec_sql(db, "ATTACH './dummy_file_attached' AS aux;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS aux.atab(q INTEGER PRIMARY KEY, r TEXT);");

  if (read_flag(&p, &n)) {
    exec_sql(db, "BEGIN;");
    exec_sql(db, "SELECT * FROM t1;");
  }
  if (read_flag(&p, &n)) {
    exec_sql(db, "BEGIN IMMEDIATE;");
    exec_sql(db, "INSERT INTO t1(a,b,c) VALUES('x',123,X'00');");
  }
  if (read_flag(&p, &n)) {
    exec_sql(db, "SAVEPOINT s1;");
    exec_sql(db, "INSERT INTO t1(a,b,c) VALUES('y',456,X'0102');");
    exec_sql(db, "ROLLBACK TO s1;");
    exec_sql(db, "RELEASE s1;");
  }

  if (read_flag(&p, &n)) {
    sqlite3_open("./dummy_file", &db2);
    if (db2) {
      sqlite3_busy_timeout(db2, 1);
      exec_sql(db2, "PRAGMA journal_mode=WAL;");
      if (read_flag(&p, &n)) {
        exec_sql(db2, "BEGIN IMMEDIATE;");
        exec_sql(db2, "INSERT INTO t1(a,b,c) VALUES('lock',789,X'03');");
      }
    }
  }

  for (i = 0; i < 8; i++) {
    int choice = read_small_int(&p, &n, 6);
    if (choice == 0) {
      const char *dbnames[] = {"main", "temp", "aux", "", "nosuch", NULL};
      const char *name = dbnames[read_small_int(&p, &n, 6)];
      sqlite3_db_readonly(db, name);
      sqlite3_extended_errcode(db);
    } else if (choice == 1) {
      const char *schemas[] = {"main", "temp", "aux", "nosuch", NULL};
      const char *schema = schemas[read_small_int(&p, &n, 5)];
      sqlite3_txn_state(db, schema);
      sqlite3_extended_errcode(db);
    } else if (choice == 2) {
      const char *dbnames[] = {"main", "temp", "aux", NULL, "nosuch"};
      const char *tables[] = {"t1", "t2", "v1", "atab", "missing"};
      const char *cols[] = {"id", "a", "b", "c", "rowid", "_rowid_", "oid", "x", "y", "q", "r", "missing"};
      const char *zDbName = dbnames[read_small_int(&p, &n, 5)];
      const char *zTable = tables[read_small_int(&p, &n, 5)];
      const char *zCol = NULL;
      const char *dtype = NULL;
      const char *coll = NULL;
      int notnull = -1, pk = -1, autoinc = -1;

      if (read_flag(&p, &n)) {
        zCol = cols[read_small_int(&p, &n, 12)];
      }

      sqlite3_table_column_metadata(
          db, zDbName, zTable, zCol,
          read_flag(&p, &n) ? &dtype : NULL,
          read_flag(&p, &n) ? &coll : NULL,
          read_flag(&p, &n) ? &notnull : NULL,
          read_flag(&p, &n) ? &pk : NULL,
          read_flag(&p, &n) ? &autoinc : NULL);
      sqlite3_extended_errcode(db);
      (void)dtype;
      (void)coll;
      (void)notnull;
      (void)pk;
      (void)autoinc;
    } else if (choice == 3) {
      const char *dbnames[] = {"main", "temp", "aux", "", "nosuch", NULL};
      const char *zDb = dbnames[read_small_int(&p, &n, 6)];
      int mode_list[] = {
        SQLITE_CHECKPOINT_PASSIVE,
        SQLITE_CHECKPOINT_FULL,
        SQLITE_CHECKPOINT_RESTART,
        SQLITE_CHECKPOINT_TRUNCATE,
        -1
      };
      int mode = mode_list[read_small_int(&p, &n, 5)];
      int logv = 0, ckptv = 0;
      sqlite3_wal_checkpoint_v2(
          db,
          zDb,
          mode,
          read_flag(&p, &n) ? &logv : NULL,
          read_flag(&p, &n) ? &ckptv : NULL);
      sqlite3_extended_errcode(db);
      (void)logv;
      (void)ckptv;
    } else if (choice == 4) {
      const char *dbnames[] = {"main", "temp", "aux", "nosuch", NULL};
      const char *zDb = dbnames[read_small_int(&p, &n, 5)];
      sqlite3_wal_checkpoint(db, zDb);
      sqlite3_extended_errcode(db);
    } else {
      char *dyn = make_cstring(&p, &n, 32);
      if (dyn) {
        int which = read_small_int(&p, &n, 4);
        if (which == 0) {
          sqlite3_db_readonly(db, dyn);
        } else if (which == 1) {
          sqlite3_txn_state(db, dyn);
        } else if (which == 2) {
          const char *dtype = NULL;
          const char *coll = NULL;
          int nn = 0, pk = 0, ai = 0;
          sqlite3_table_column_metadata(db, NULL, dyn,
                                        read_flag(&p, &n) ? dyn : NULL,
                                        &dtype, &coll, &nn, &pk, &ai);
          (void)dtype;
          (void)coll;
          (void)nn;
          (void)pk;
          (void)ai;
        } else {
          int logv = 0, ckptv = 0;
          int modes[] = {
            SQLITE_CHECKPOINT_PASSIVE,
            SQLITE_CHECKPOINT_FULL,
            SQLITE_CHECKPOINT_RESTART,
            SQLITE_CHECKPOINT_TRUNCATE
          };
          sqlite3_wal_checkpoint_v2(db, dyn, modes[read_small_int(&p, &n, 4)], &logv, &ckptv);
          (void)logv;
          (void)ckptv;
        }
        sqlite3_extended_errcode(db);
        sqlite3_free(dyn);
      }
    }
  }

  if (db2) {
    exec_sql(db2, "ROLLBACK;");
    sqlite3_close(db2);
  }

  exec_sql(db, "ROLLBACK;");
  exec_sql(db, "DETACH aux;");
  sqlite3_close(db);
  return 0;
}