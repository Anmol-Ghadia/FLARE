// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_blob_open at sqlite3.c:82675:16 in sqlite3.h
// sqlite3_blob_close at sqlite3.c:82908:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_prepare_v2 at sqlite3.c:119067:16 in sqlite3.h
// sqlite3_finalize at sqlite3.c:72005:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
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

static int fuzz_callback(void *ctx, int argc, char **argv, char **colnames) {
  const uint8_t *data = (const uint8_t *)ctx;
  int ret = 0;
  if (data && argc > 0) {
    ret = data[0] & 1;
  }
  (void)argv;
  (void)colnames;
  return ret;
}

static int pick_int(const uint8_t *Data, size_t Size, size_t *off) {
  int v = 0;
  size_t i;
  for (i = 0; i < 4 && *off < Size; ++i, ++(*off)) {
    v = (v << 8) | Data[*off];
  }
  return v;
}

static sqlite3_int64 pick_i64(const uint8_t *Data, size_t Size, size_t *off) {
  sqlite3_int64 v = 0;
  size_t i;
  for (i = 0; i < 8 && *off < Size; ++i, ++(*off)) {
    v = (v << 8) | Data[*off];
  }
  return v;
}

static const char *pick_cstr_from_table(const uint8_t *Data, size_t Size, size_t *off,
                                        const char *const *table, size_t ntable) {
  if (ntable == 0) return "";
  if (*off >= Size) return table[0];
  return table[Data[(*off)++] % ntable];
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  sqlite3_blob *blob = NULL;
  char *errmsg = NULL;
  char *mp1 = NULL;
  char *mp2 = NULL;
  int rc;
  size_t off = 0;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  if (Size > 0 && (Data[0] & 1)) {
    const char *prep_sqls[] = {
      "SELECT 1",
      "CREATE TABLE IF NOT EXISTS t(a BLOB, b TEXT);",
      "INSERT INTO t(rowid,a,b) VALUES(1, x'414243', 'txt');",
      "SELECT a FROM t WHERE rowid=1",
      ""
    };
    const char *psql = prep_sqls[Data[0] % (sizeof(prep_sqls) / sizeof(prep_sqls[0]))];
    sqlite3_prepare_v2(db, psql, -1, &stmt, NULL);
  }

  sqlite3_finalize(stmt);

  {
    char *sqlbuf = NULL;
    if (Size > 1) {
      size_t sql_len = Size - 1;
      sqlbuf = (char *)malloc(sql_len + 1);
      if (sqlbuf) {
        memcpy(sqlbuf, Data + 1, sql_len);
        sqlbuf[sql_len] = '\0';
      }
    }

    if (sqlbuf) {
      sqlite3_exec(db,
                   sqlbuf,
                   (Size > 2 && (Data[1] & 1)) ? fuzz_callback : NULL,
                   (void *)Data,
                   &errmsg);
      free(sqlbuf);
    } else {
      sqlite3_exec(db,
                   "CREATE TABLE IF NOT EXISTS t(a BLOB, b TEXT);"
                   "INSERT OR IGNORE INTO t(rowid,a,b) VALUES(1, x'00', 'x');"
                   "SELECT * FROM t;",
                   (Size > 0 && (Data[0] & 2)) ? fuzz_callback : NULL,
                   (void *)Data,
                   &errmsg);
    }
  }

  mp1 = sqlite3_mprintf("%.*s", (int)((Size > off) ? (Size - off) : 0), (const char *)(Data + off));
  mp2 = sqlite3_mprintf("num=%d row=%lld file=%s",
                        pick_int(Data, Size, &off),
                        (long long)pick_i64(Data, Size, &off),
                        "./dummy_file");

  sqlite3_free(mp1);
  sqlite3_free(mp2);

  {
    const char *const db_names[] = {"main", "temp", "invalid", ""};
    const char *const table_names[] = {"t", "sqlite_master", "no_such_table", ""};
    const char *const col_names[] = {"a", "b", "sql", "no_such_col", ""};
    const char *zDb = pick_cstr_from_table(Data, Size, &off, db_names, sizeof(db_names)/sizeof(db_names[0]));
    const char *zTable = pick_cstr_from_table(Data, Size, &off, table_names, sizeof(table_names)/sizeof(table_names[0]));
    const char *zColumn = pick_cstr_from_table(Data, Size, &off, col_names, sizeof(col_names)/sizeof(col_names[0]));
    sqlite3_int64 iRow = pick_i64(Data, Size, &off);
    int flags = pick_int(Data, Size, &off) & 1;

    sqlite3_blob_open(db, zDb, zTable, zColumn, iRow, flags, &blob);
  }

  if (blob) {
    sqlite3_blob_close(blob);
  }
  if (errmsg) {
    sqlite3_free(errmsg);
  }
  sqlite3_close(db);
  return 0;
}