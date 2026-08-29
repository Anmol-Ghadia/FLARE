// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_limit at sqlite3.c:155025:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_set_authorizer at sqlite3.c:100017:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_step at sqlite3.c:72655:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_get_table at sqlite3.c:126741:16 in sqlite3.h
// sqlite3_free_table at sqlite3.c:126810:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_free_table at sqlite3.c:126810:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
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

static int fuzz_authorizer(void *pUserData, int action, const char *p1,
                           const char *p2, const char *dbName,
                           const char *triggerName) {
  const uint8_t *data = (const uint8_t *)pUserData;
  size_t mix = (size_t)action;

  if (p1) mix ^= (unsigned char)p1[0];
  if (p2) mix ^= ((size_t)(unsigned char)p2[0] << 1);
  if (dbName) mix ^= ((size_t)(unsigned char)dbName[0] << 2);
  if (triggerName) mix ^= ((size_t)(unsigned char)triggerName[0] << 3);

  if (data) mix ^= data[0];

  switch (mix % 3) {
    case 0:
      return SQLITE_OK;
    case 1:
      return SQLITE_IGNORE;
    default:
      return SQLITE_DENY;
  }
}

static int consume_u8(const uint8_t **data, size_t *size) {
  if (*size == 0) return 0;
  int v = **data;
  (*data)++;
  (*size)--;
  return v;
}

static int consume_u32(const uint8_t **data, size_t *size) {
  int v = 0;
  for (int i = 0; i < 4; i++) {
    v = (v << 8) | consume_u8(data, size);
  }
  return v;
}

static char *consume_cstring(const uint8_t **data, size_t *size, size_t max_len) {
  size_t len = 0;
  while (len < *size && len < max_len && (*data)[len] != 0) {
    len++;
  }

  char *out = (char *)malloc(len + 1);
  if (!out) return NULL;

  if (len > 0) memcpy(out, *data, len);
  out[len] = '\0';

  if (len < *size && (*data)[len] == 0) {
    *data += len + 1;
    *size -= len + 1;
  } else {
    *data += len;
    *size -= len;
  }

  return out;
}

int LLVMFuzzerTestOneInput_39(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  char *sql1 = NULL;
  char *sql2 = NULL;
  char *tbl = NULL;
  char *col = NULL;
  char *dbName = NULL;
  char **result = NULL;
  char *errmsg = NULL;
  FILE *fp = NULL;

  if (sqlite3_initialize() != SQLITE_OK) {
    return 0;
  }

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
    fp = NULL;
  }

  int open_mode = consume_u8(&p, &remaining) % 4;
  int flags = 0;
  switch (open_mode) {
    case 0:
      flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
      break;
    case 1:
      flags = SQLITE_OPEN_READONLY;
      break;
    case 2:
      flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI;
      break;
    default:
      flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY;
      break;
  }

  const char *filename = (open_mode == 3) ? ":memory:" : "./dummy_file";
  (void)sqlite3_open_v2(filename, &db, flags, NULL);
  if (!db) {
    return 0;
  }

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

  int id1 = limit_ids[consume_u8(&p, &remaining) % (int)(sizeof(limit_ids) / sizeof(limit_ids[0]))];
  int id2 = limit_ids[consume_u8(&p, &remaining) % (int)(sizeof(limit_ids) / sizeof(limit_ids[0]))];
  int val1 = (consume_u32(&p, &remaining) % 100000) - 50000;
  int val2 = (consume_u32(&p, &remaining) % 100000) - 50000;

  (void)sqlite3_limit(db, id1, val1);
  (void)sqlite3_limit(db, id1, -1);
  (void)sqlite3_limit(db, id2, val2);

  if (consume_u8(&p, &remaining) & 1) {
    (void)sqlite3_set_authorizer(db, fuzz_authorizer, (void *)Data);
  } else {
    (void)sqlite3_set_authorizer(db, NULL, NULL);
  }

  sql1 = consume_cstring(&p, &remaining, 4096);
  sql2 = consume_cstring(&p, &remaining, 4096);
  tbl = consume_cstring(&p, &remaining, 256);
  col = consume_cstring(&p, &remaining, 256);
  dbName = consume_cstring(&p, &remaining, 64);

  if (!sql1) sql1 = strdup("");
  if (!sql2) sql2 = strdup("");
  if (!tbl) tbl = strdup("sqlite_master");
  if (!col) col = strdup("name");
  if (!dbName) dbName = strdup("main");

  if (!sql1 || !sql2 || !tbl || !col || !dbName) {
    goto cleanup;
  }

  (void)sqlite3_exec(db,
                     "CREATE TABLE IF NOT EXISTS t1("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "a TEXT NOT NULL,"
                     "b INTEGER,"
                     "c BLOB);"
                     "CREATE VIEW IF NOT EXISTS v1 AS SELECT a,b FROM t1;",
                     NULL, NULL, NULL);

  (void)sqlite3_exec(db, sql1, NULL, NULL, NULL);

  (void)sqlite3_prepare_v2(db, sql2, -1, &stmt, NULL);
  if (stmt) {
    for (int i = 0; i < 4; i++) {
      int rc = sqlite3_step(stmt);
      if (rc != SQLITE_ROW && rc != SQLITE_DONE) break;
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }

  int nRow = 0;
  int nCol = 0;
  (void)sqlite3_get_table(db, sql2, &result, &nRow, &nCol, &errmsg);
  if (result) {
    sqlite3_free_table(result);
    result = NULL;
  }
  if (errmsg) {
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  {
    const char *outType = NULL;
    const char *outColl = NULL;
    int notNull = 0;
    int pk = 0;
    int autoinc = 0;

    (void)sqlite3_table_column_metadata(
        db,
        (consume_u8(&p, &remaining) & 1) ? NULL : dbName,
        tbl,
        (consume_u8(&p, &remaining) & 1) ? NULL : col,
        &outType,
        &outColl,
        &notNull,
        &pk,
        &autoinc);

    (void)outType;
    (void)outColl;
    (void)notNull;
    (void)pk;
    (void)autoinc;
  }

  {
    const char *outType = NULL;
    const char *outColl = NULL;
    int notNull = 0;
    int pk = 0;
    int autoinc = 0;

    (void)sqlite3_table_column_metadata(
        db, "main", "t1", "rowid",
        &outType, &outColl, &notNull, &pk, &autoinc);
  }

  {
    const char *outType = NULL;
    const char *outColl = NULL;
    int notNull = 0;
    int pk = 0;
    int autoinc = 0;

    (void)sqlite3_table_column_metadata(
        db, "main", "v1", "a",
        &outType, &outColl, &notNull, &pk, &autoinc);
  }

cleanup:
  if (stmt) sqlite3_finalize(stmt);
  if (result) sqlite3_free_table(result);
  if (errmsg) sqlite3_free(errmsg);
  free(sql1);
  free(sql2);
  free(tbl);
  free(col);
  free(dbName);
  sqlite3_close(db);
  return 0;
}