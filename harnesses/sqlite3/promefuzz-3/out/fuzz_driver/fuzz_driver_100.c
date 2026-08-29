// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_get_table at sqlite3.c:126741:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free_table at sqlite3.c:126810:17 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"

static int fuzz_callback(void *ctx, int argc, char **argv, char **colv) {
  volatile int sink = 0;
  int i;
  (void)ctx;
  for (i = 0; i < argc; i++) {
    if (argv && argv[i]) sink ^= (unsigned char)argv[i][0];
    if (colv && colv[i]) sink ^= (unsigned char)colv[i][0];
  }
  return sink & 1 ? 0 : 0;
}

static int pick_flag(uint8_t b) {
  switch (b % 8) {
    case 0: return SQLITE_OPEN_READONLY;
    case 1: return SQLITE_OPEN_READWRITE;
    case 2: return SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    case 3: return SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI;
    case 4: return SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY;
    case 5: return SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX;
    case 6: return SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX;
    default: return SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_PRIVATECACHE;
  }
}

static char *make_nul_terminated(const uint8_t *data, size_t size) {
  char *out = (char *)malloc(size + 1);
  if (!out) return NULL;
  if (size) memcpy(out, data, size);
  out[size] = '\0';
  return out;
}

static const char *pick_dbname(uint8_t b) {
  switch (b % 4) {
    case 0: return NULL;
    case 1: return "main";
    case 2: return "temp";
    default: return "aux";
  }
}

static const char *pick_vfs(uint8_t b) {
  return (b & 1) ? NULL : "no_such_vfs";
}

static void write_dummy_file(const uint8_t *data, size_t size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (size) fwrite(data, 1, size, fp);
  fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  int rc;
  char *sql = NULL;
  char *errmsg = NULL;
  char **table_result = NULL;
  int nrow = 0, ncol = 0;
  const char *dtype = NULL, *coll = NULL;
  int notnull = 0, pk = 0, autoinc = 0;
  const char *table_name = "t";
  const char *column_name = "a";
  const char *db_name = NULL;
  int flags;

  (void)sqlite3_initialize();

  write_dummy_file(Data, Size);

  flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  if (Size > 0) flags = pick_flag(Data[0]);

  rc = sqlite3_open_v2("./dummy_file", &db, flags, (Size > 1) ? pick_vfs(Data[1]) : NULL);
  if (rc != SQLITE_OK) {
    (void)sqlite3_errmsg(db);
    if (db) sqlite3_close(db);
    return 0;
  }

  sql = make_nul_terminated(Data, Size);
  if (!sql) {
    sqlite3_close(db);
    return 0;
  }

  if (Size > 2) {
    switch (Data[2] % 6) {
      case 0: table_name = "t"; column_name = "a"; break;
      case 1: table_name = "sqlite_master"; column_name = "type"; break;
      case 2: table_name = "t"; column_name = "rowid"; break;
      case 3: table_name = "v"; column_name = "a"; break;
      case 4: table_name = "no_such_table"; column_name = "no_such_col"; break;
      default: table_name = "t2"; column_name = NULL; break;
    }
  }

  db_name = (Size > 3) ? pick_dbname(Data[3]) : NULL;

  rc = sqlite3_exec(db,
                    "CREATE TABLE IF NOT EXISTS t(a INTEGER PRIMARY KEY AUTOINCREMENT, b TEXT NOT NULL COLLATE NOCASE, c BLOB);"
                    "CREATE TABLE IF NOT EXISTS t2(x, y, z);"
                    "CREATE VIEW IF NOT EXISTS v AS SELECT a,b,c FROM t;"
                    "ATTACH DATABASE ':memory:' AS aux;"
                    "CREATE TABLE IF NOT EXISTS aux.at(a TEXT, b INTEGER);",
                    fuzz_callback, NULL, &errmsg);
  if (rc != SQLITE_OK) {
    (void)sqlite3_errmsg(db);
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  rc = sqlite3_exec(db, sql, (Size > 4 && (Data[4] & 1)) ? fuzz_callback : NULL, NULL, &errmsg);
  if (rc != SQLITE_OK) {
    (void)sqlite3_errmsg(db);
    sqlite3_free(errmsg);
    errmsg = NULL;
  }

  nrow = 0;
  ncol = 0;
  table_result = NULL;
  errmsg = NULL;
  rc = sqlite3_get_table(db, sql, &table_result, &nrow, &ncol, &errmsg);
  if (rc != SQLITE_OK) {
    (void)sqlite3_errmsg(db);
    sqlite3_free(errmsg);
    errmsg = NULL;
  }
  if (table_result) {
    sqlite3_free_table(table_result);
    table_result = NULL;
  }

  rc = sqlite3_table_column_metadata(
      db, db_name, table_name, column_name,
      &dtype, &coll, &notnull, &pk, &autoinc);
  if (rc != SQLITE_OK) {
    (void)sqlite3_errmsg(db);
  } else {
    volatile int sink = 0;
    if (dtype) sink ^= (unsigned char)dtype[0];
    if (coll) sink ^= (unsigned char)coll[0];
    sink ^= notnull ^ pk ^ autoinc;
    (void)sink;
  }

  rc = sqlite3_table_column_metadata(
      db,
      (Size > 5 && (Data[5] & 1)) ? "main" : NULL,
      "t",
      (Size > 6) ? ((Data[6] & 1) ? "b" : NULL) : "b",
      NULL, NULL, NULL, NULL, NULL);
  if (rc != SQLITE_OK) {
    (void)sqlite3_errmsg(db);
  }

  rc = sqlite3_table_column_metadata(
      db, "aux", "at",
      (Size > 7) ? ((Data[7] % 3 == 0) ? "a" : (Data[7] % 3 == 1) ? "b" : "rowid") : "a",
      &dtype, &coll, &notnull, &pk, &autoinc);
  if (rc != SQLITE_OK) {
    (void)sqlite3_errmsg(db);
  }

  free(sql);
  sqlite3_close(db);
  return 0;
}