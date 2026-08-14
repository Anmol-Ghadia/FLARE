// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"

static int fuzz_callback(void *ctx, int argc, char **argv, char **colv) {
  (void)argv;
  (void)colv;
  const uint8_t *data = (const uint8_t *)ctx;
  if (!data) return 0;
  return (argc > 0 && (data[0] & 1)) ? 1 : 0;
}

static unsigned int read_u32(const uint8_t *p, size_t n) {
  unsigned int v = 0;
  size_t i;
  for (i = 0; i < n && i < 4; ++i) {
    v = (v << 8) | p[i];
  }
  return v;
}

static char *make_nul_terminated(const uint8_t *data, size_t size) {
  char *buf = (char *)malloc(size + 1);
  if (!buf) return NULL;
  if (size) memcpy(buf, data, size);
  buf[size] = '\0';
  return buf;
}

static void write_dummy_file(const uint8_t *data, size_t size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (size) fwrite(data, 1, size, fp);
  fclose(fp);
}

int LLVMFuzzerTestOneInput_31(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  char *sql1 = NULL;
  char *sql2 = NULL;
  char *errmsg1 = NULL;
  char *errmsg2 = NULL;
  char *mprintf_buf = NULL;
  const char *err1;
  const char *err2;
  const char *dtype = NULL;
  const char *coll = NULL;
  int notnull = 0, pk = 0, autoinc = 0;
  int rc;
  const char *dbName = NULL;
  const char *tableName = "t";
  const char *columnName = "c";
  size_t split1 = 0, split2 = 0;
  char *quoted_tail = NULL;

  write_dummy_file(Data, Size);

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  if (Size > 0) split1 = Data[0] % (Size + 1);
  if (Size > 1) split2 = Data[1] % (Size + 1);

  sql1 = make_nul_terminated(Data, split1);
  if (!sql1) goto cleanup;

  rc = sqlite3_exec(db, sql1, (Data[0] & 2) ? fuzz_callback : NULL,
                    (void *)Data, &errmsg1);
  (void)rc;

  err1 = sqlite3_errmsg(db);
  (void)err1;

  quoted_tail = make_nul_terminated(
      Data + ((split2 < Size) ? split2 : Size),
      (split2 < Size) ? (Size - split2) : 0);
  if (!quoted_tail) goto cleanup;

  mprintf_buf = sqlite3_mprintf(
      "%s;%s;CREATE TABLE IF NOT EXISTS t(c TEXT, id INTEGER PRIMARY KEY "
      "AUTOINCREMENT);INSERT INTO t(c) VALUES(%Q);SELECT * FROM t;",
      sql1 ? sql1 : "",
      errmsg1 ? errmsg1 : "",
      quoted_tail);

  sql2 = make_nul_terminated(Data + ((split1 < Size) ? split1 : Size),
                             (split1 < Size) ? (Size - split1) : 0);
  if (!sql2) goto cleanup;

  rc = sqlite3_exec(db,
                    mprintf_buf ? mprintf_buf : sql2,
                    (Data[0] & 4) ? fuzz_callback : NULL,
                    (void *)Data, &errmsg2);
  (void)rc;

  sqlite3_free(mprintf_buf);
  mprintf_buf = NULL;

  err2 = sqlite3_errmsg(db);
  (void)err2;

  if (Size > 2) {
    switch (Data[2] % 4) {
      case 0: dbName = NULL; break;
      case 1: dbName = "main"; break;
      case 2: dbName = "temp"; break;
      default: dbName = "aux"; break;
    }
  }

  if (Size > 3) {
    switch (Data[3] % 5) {
      case 0: tableName = "t"; break;
      case 1: tableName = "sqlite_master"; break;
      case 2: tableName = "nonexistent"; break;
      case 3: tableName = "dummy_file"; break;
      default: tableName = ""; break;
    }
  }

  if (Size > 4) {
    switch (Data[4] % 6) {
      case 0: columnName = "c"; break;
      case 1: columnName = "id"; break;
      case 2: columnName = "rowid"; break;
      case 3: columnName = "nonexistent"; break;
      case 4: columnName = ""; break;
      default: columnName = NULL; break;
    }
  }

  sqlite3_table_column_metadata(
      db, dbName, tableName, columnName,
      (Data[0] & 8) ? &dtype : NULL,
      (Data[0] & 16) ? &coll : NULL,
      (Data[0] & 32) ? &notnull : NULL,
      (Data[0] & 64) ? &pk : NULL,
      (Data[0] & 128) ? &autoinc : NULL);

  if (Size >= 8) {
    unsigned int off = read_u32(Data + (Size >= 4 ? Size - 4 : 0), Size >= 4 ? 4 : Size);
    off %= (unsigned int)(Size + 1);
    if (off < Size) {
      char *dynTable = make_nul_terminated(Data + off, Size - off);
      if (dynTable) {
        sqlite3_table_column_metadata(
            db, NULL, dynTable,
            columnName,
            &dtype, &coll, &notnull, &pk, &autoinc);
        free(dynTable);
      }
    }
  }

cleanup:
  sqlite3_free(mprintf_buf);
  sqlite3_free(errmsg1);
  sqlite3_free(errmsg2);
  free(sql1);
  free(sql2);
  free(quoted_tail);
  sqlite3_close(db);
  return 0;
}