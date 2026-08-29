// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_threadsafe at sqlite3.c:152376:16 in sqlite3.h
// sqlite3_libversion_number at sqlite3.c:152370:16 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_get at sqlite3.c:156952:24 in sqlite3.h
// sqlite3_compileoption_used at sqlite3.c:156919:16 in sqlite3.h
// sqlite3_compileoption_used at sqlite3.c:156919:16 in sqlite3.h
// sqlite3_compileoption_used at sqlite3.c:156919:16 in sqlite3.h
// sqlite3_compileoption_used at sqlite3.c:156919:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t **p, size_t *n) {
  uint32_t v = 0;
  size_t take = *n < 4 ? *n : 4;
  for (size_t i = 0; i < take; ++i) {
    v = (v << 8) | (*p)[i];
  }
  *p += take;
  *n -= take;
  return v;
}

static int read_flag(const uint8_t **p, size_t *n) {
  if (*n == 0) return 0;
  int v = (**p) & 1;
  (*p)++;
  (*n)--;
  return v;
}

static size_t read_len(const uint8_t **p, size_t *n, size_t max_len) {
  if (*n == 0) return 0;
  size_t len = **p;
  (*p)++;
  (*n)--;
  if (len > *n) len = *n;
  if (len > max_len) len = max_len;
  return len;
}

static void make_cstring(char *dst, size_t dstsz, const uint8_t *src, size_t len) {
  if (dstsz == 0) return;
  size_t n = len < (dstsz - 1) ? len : (dstsz - 1);
  for (size_t i = 0; i < n; ++i) {
    unsigned char c = src[i];
    dst[i] = (c == 0) ? 'A' : (char)c;
  }
  dst[n] = '\0';
}

static void exec_sql(sqlite3 *db, const char *sql) {
  char *errmsg = NULL;
  sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (errmsg) sqlite3_free(errmsg);
}

int LLVMFuzzerTestOneInput_93(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  (void)sqlite3_threadsafe();
  (void)sqlite3_libversion_number();

  int idx1 = (int)(read_u32(&p, &n) % 4096U);
  int idx2 = (int)(read_u32(&p, &n) % 4096U);
  int idx3 = (int)(read_u32(&p, &n) % 4096U);

  const char *opt1 = sqlite3_compileoption_get(idx1);
  const char *opt2 = sqlite3_compileoption_get(idx2);
  const char *opt3 = sqlite3_compileoption_get(idx3);
  (void)opt1;
  (void)opt2;
  (void)opt3;

  {
    char optbuf[128];
    size_t optlen = read_len(&p, &n, sizeof(optbuf) - 1);
    make_cstring(optbuf, sizeof(optbuf), p, optlen);
    p += optlen;
    n -= optlen;
    (void)sqlite3_compileoption_used(optbuf);
    (void)sqlite3_compileoption_used(NULL);
    if (opt1) (void)sqlite3_compileoption_used(opt1);
    if (opt2) (void)sqlite3_compileoption_used(opt2);
  }

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  sqlite3 *db = NULL;
  int rc = sqlite3_open("./dummy_file", &db);
  if (db == NULL) {
    return 0;
  }

  exec_sql(db, "PRAGMA writable_schema=OFF;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t1("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "a TEXT NOT NULL COLLATE BINARY,"
               "b INT,"
               "c BLOB"
               ");");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(x, y, z);");
  exec_sql(db, "CREATE VIEW IF NOT EXISTS v1 AS SELECT a,b FROM t1;");
  exec_sql(db, "ATTACH DATABASE ':memory:' AS aux;");
  exec_sql(db, "CREATE TABLE IF NOT EXISTS aux.at(a INTEGER PRIMARY KEY, b TEXT);");
  exec_sql(db, "CREATE TEMP TABLE IF NOT EXISTS tt(m TEXT, n INTEGER);");

  for (int i = 0; i < 6; ++i) {
    char dbname[64], tbl[64], col[64];
    const char *zDbName = NULL;
    const char *zTableName = NULL;
    const char *zColumnName = NULL;

    int use_db = read_flag(&p, &n);
    int use_col = read_flag(&p, &n);
    int null_outputs_mask = (int)(read_u32(&p, &n) & 0x1F);

    size_t l1 = read_len(&p, &n, sizeof(dbname) - 1);
    make_cstring(dbname, sizeof(dbname), p, l1);
    p += l1;
    n -= l1;

    size_t l2 = read_len(&p, &n, sizeof(tbl) - 1);
    make_cstring(tbl, sizeof(tbl), p, l2);
    p += l2;
    n -= l2;

    size_t l3 = read_len(&p, &n, sizeof(col) - 1);
    make_cstring(col, sizeof(col), p, l3);
    p += l3;
    n -= l3;

    switch (read_u32(&p, &n) % 6U) {
      case 0: strcpy(tbl, "t1"); break;
      case 1: strcpy(tbl, "t2"); break;
      case 2: strcpy(tbl, "v1"); break;
      case 3: strcpy(tbl, "sqlite_master"); break;
      case 4: strcpy(tbl, "at"); break;
      default: break;
    }

    switch (read_u32(&p, &n) % 8U) {
      case 0: strcpy(col, "id"); break;
      case 1: strcpy(col, "a"); break;
      case 2: strcpy(col, "b"); break;
      case 3: strcpy(col, "c"); break;
      case 4: strcpy(col, "rowid"); break;
      case 5: strcpy(col, "_rowid_"); break;
      case 6: strcpy(col, "oid"); break;
      default: break;
    }

    switch (read_u32(&p, &n) % 5U) {
      case 0: strcpy(dbname, "main"); break;
      case 1: strcpy(dbname, "temp"); break;
      case 2: strcpy(dbname, "aux"); break;
      default: break;
    }

    if (use_db) zDbName = dbname;
    if (tbl[0] != '\0') zTableName = tbl;
    if (use_col) zColumnName = col;

    if (zTableName != NULL) {
      const char *dataType = NULL;
      const char *collSeq = NULL;
      int notNull = 0, primaryKey = 0, autoinc = 0;

      const char **pzDataType = (null_outputs_mask & 1) ? NULL : &dataType;
      const char **pzCollSeq = (null_outputs_mask & 2) ? NULL : &collSeq;
      int *pNotNull = (null_outputs_mask & 4) ? NULL : &notNull;
      int *pPrimaryKey = (null_outputs_mask & 8) ? NULL : &primaryKey;
      int *pAutoinc = (null_outputs_mask & 16) ? NULL : &autoinc;

      rc = sqlite3_table_column_metadata(
          db, zDbName, zTableName, zColumnName,
          pzDataType, pzCollSeq, pNotNull, pPrimaryKey, pAutoinc);

      (void)rc;
      if (dataType) (void)dataType[0];
      if (collSeq) (void)collSeq[0];
      (void)notNull;
      (void)primaryKey;
      (void)autoinc;

      rc = sqlite3_table_column_metadata(
          db, NULL, zTableName, zColumnName,
          &dataType, &collSeq, &notNull, &primaryKey, &autoinc);
      (void)rc;
    }
  }

  sqlite3_close(db);
  return 0;
}