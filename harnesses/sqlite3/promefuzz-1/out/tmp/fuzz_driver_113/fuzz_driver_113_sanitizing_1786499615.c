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

static uint32_t read_u32(const uint8_t **data, size_t *size) {
  uint32_t v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; i++) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static int read_bool(const uint8_t **data, size_t *size) {
  if (*size == 0) return 0;
  int v = (**data) & 1;
  (*data)++;
  (*size)--;
  return v;
}

static char *dup_nul_terminated(const uint8_t *data, size_t size) {
  char *buf = (char *)malloc(size + 1);
  if (!buf) return NULL;
  if (size) memcpy(buf, data, size);
  buf[size] = '\0';
  return buf;
}

static char *take_string(const uint8_t **data, size_t *size) {
  if (*size == 0) {
    char *s = (char *)malloc(1);
    if (s) s[0] = '\0';
    return s;
  }

  uint32_t len = read_u32(data, size);
  if (*size == 0) {
    char *s = (char *)malloc(1);
    if (s) s[0] = '\0';
    return s;
  }

  len %= (uint32_t)(*size + 1);
  char *out = (char *)malloc((size_t)len + 1);
  if (!out) return NULL;
  if (len) memcpy(out, *data, len);
  out[len] = '\0';
  *data += len;
  *size -= len;
  return out;
}

static int exec_callback(void *ctx, int argc, char **argv, char **colnames) {
  (void)argv;
  (void)colnames;
  int *mode = (int *)ctx;
  if (!mode) return 0;
  if (*mode == 0) return 0;
  if (*mode == 1) return argc > 1000;
  return 1;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
#ifdef SQLITE_OPEN_URI
  if (read_bool(&p, &remaining)) flags |= SQLITE_OPEN_URI;
#endif
#ifdef SQLITE_OPEN_MEMORY
  if (read_bool(&p, &remaining)) flags |= SQLITE_OPEN_MEMORY;
#endif
#ifdef SQLITE_OPEN_NOMUTEX
  if (read_bool(&p, &remaining)) flags |= SQLITE_OPEN_NOMUTEX;
#endif
#ifdef SQLITE_OPEN_FULLMUTEX
  if (read_bool(&p, &remaining)) flags |= SQLITE_OPEN_FULLMUTEX;
#endif
#ifdef SQLITE_OPEN_PRIVATECACHE
  if (read_bool(&p, &remaining)) flags |= SQLITE_OPEN_PRIVATECACHE;
#endif
#ifdef SQLITE_OPEN_SHAREDCACHE
  if (read_bool(&p, &remaining)) flags |= SQLITE_OPEN_SHAREDCACHE;
#endif

  const char *filename = read_bool(&p, &remaining) ? ":memory:" : "./dummy_file";
  const char *vfs = NULL;

  int rc = sqlite3_open_v2(filename, &db, flags, vfs);
  if (rc != SQLITE_OK) {
    if (db) {
      (void)sqlite3_errmsg(db);
      sqlite3_close(db);
    }
    return 0;
  }

  char *sql = dup_nul_terminated(p, remaining);
  if (!sql) {
    sqlite3_close(db);
    return 0;
  }

  (void)sqlite3_exec(db,
                     "CREATE TABLE IF NOT EXISTS t1("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "a TEXT,"
                     "b INT NOT NULL,"
                     "c BLOB,"
                     "d REAL);"
                     "CREATE VIEW IF NOT EXISTS v1 AS SELECT a,b FROM t1;"
                     "INSERT INTO t1(a,b,c,d) VALUES('x',1,X'00',1.5);"
                     "CREATE TABLE IF NOT EXISTS wr(x TEXT PRIMARY KEY) WITHOUT ROWID;",
                     NULL, NULL, NULL);

  sqlite3_stmt *stmt = NULL;
  const char *tail = NULL;
  rc = sqlite3_prepare_v2(db, sql, (int)strlen(sql), &stmt, &tail);
  if (rc != SQLITE_OK) {
    (void)sqlite3_errmsg(db);
  }
  if (stmt) {
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  char *errmsg = NULL;
  int cbmode = (int)(read_u32(&p, &remaining) % 3);
  rc = sqlite3_exec(db, sql, exec_callback, &cbmode, &errmsg);
  if (rc != SQLITE_OK) {
    (void)sqlite3_errmsg(db);
  }
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  char **table_result = NULL;
  int nrow = 0, ncol = 0;
  char *tbl_err = NULL;
  rc = sqlite3_get_table(db, sql, &table_result, &nrow, &ncol, &tbl_err);
  if (rc != SQLITE_OK) {
    (void)sqlite3_errmsg(db);
  }
  if (table_result) sqlite3_free_table(table_result);
  if (tbl_err) sqlite3_free(tbl_err);

  char *dbName = take_string(&p, &remaining);
  char *tableName = take_string(&p, &remaining);
  char *columnName = take_string(&p, &remaining);
  if (!dbName || !tableName || !columnName) {
    free(dbName);
    free(tableName);
    free(columnName);
    free(sql);
    sqlite3_close(db);
    return 0;
  }

  const char *dbArg = read_bool(&p, &remaining) ? NULL : dbName;
  const char *tableArg = tableName[0] ? tableName : "t1";
  const char *colArg = read_bool(&p, &remaining) ? NULL : columnName;

  const char *dtype = NULL;
  const char *coll = NULL;
  int notnull = 0, pk = 0, autoinc = 0;

  rc = sqlite3_table_column_metadata(
      db,
      dbArg,
      tableArg,
      colArg,
      read_bool(&p, &remaining) ? &dtype : NULL,
      read_bool(&p, &remaining) ? &coll : NULL,
      read_bool(&p, &remaining) ? &notnull : NULL,
      read_bool(&p, &remaining) ? &pk : NULL,
      read_bool(&p, &remaining) ? &autoinc : NULL);
  if (rc != SQLITE_OK) {
    (void)sqlite3_errmsg(db);
  }

  {
    const char *known_tables[] = {"t1", "v1", "wr", "sqlite_master"};
    const char *known_cols[] = {"id", "a", "b", "c", "d", "rowid", "_rowid_", "oid", "x", NULL};
    const char *known_dbs[] = {NULL, "main", "temp"};
    size_t ti = remaining ? (size_t)(remaining % 4) : 0;
    size_t ci = remaining ? (size_t)(remaining % 9) : 0;
    size_t di = remaining ? (size_t)(remaining % 3) : 0;

    dtype = NULL;
    coll = NULL;
    notnull = pk = autoinc = 0;
    rc = sqlite3_table_column_metadata(
        db,
        known_dbs[di],
        known_tables[ti],
        known_cols[ci],
        &dtype,
        &coll,
        &notnull,
        &pk,
        &autoinc);
    if (rc != SQLITE_OK) {
      (void)sqlite3_errmsg(db);
    }

    rc = sqlite3_table_column_metadata(
        db,
        known_dbs[di],
        known_tables[ti],
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    if (rc != SQLITE_OK) {
      (void)sqlite3_errmsg(db);
    }
  }

  (void)sqlite3_errmsg(db);

  free(dbName);
  free(tableName);
  free(columnName);
  free(sql);
  sqlite3_close(db);
  return 0;
}