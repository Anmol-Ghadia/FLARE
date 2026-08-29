// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_malloc64 at sqlite3.c:15744:18 in sqlite3.h
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
#include "sqlite3.h"

static int fuzz_callback(void *ctx, int argc, char **argv, char **colv) {
  (void)ctx;
  (void)argc;
  (void)argv;
  (void)colv;
  return 0;
}

static const char *pick_string(const uint8_t *data, size_t size, size_t *off) {
  static const char *fallbacks[] = {
      "main",
      "temp",
      "t",
      "x",
      "rowid",
      "_rowid_",
      "oid",
      "sqlite_master",
      "dummy"
  };
  if (*off >= size) {
    return fallbacks[0];
  }

  uint8_t mode = data[(*off)++];
  if ((mode & 1u) == 0) {
    return fallbacks[mode % (sizeof(fallbacks) / sizeof(fallbacks[0]))];
  }

  if (*off >= size) {
    return "";
  }

  size_t len = data[(*off)++] % 32u;
  static char buf[33];
  size_t i;
  for (i = 0; i < len && *off < size; ++i, ++(*off)) {
    uint8_t c = data[*off];
    if (c < 32 || c > 126) c = (uint8_t)('a' + (c % 26));
    buf[i] = (char)c;
  }
  buf[i] = '\0';
  return buf;
}

int LLVMFuzzerTestOneInput_31(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  char *errmsg1 = NULL;
  char *errmsg2 = NULL;
  char *formatted = NULL;
  const char *em1;
  const char *em2;
  const char *zDbName;
  const char *zTableName;
  const char *zColumnName;
  const char *dtype = NULL;
  const char *coll = NULL;
  int notnull = 0, pk = 0, autoinc = 0;
  size_t off = 0;
  int rc;
  FILE *fp;

  fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK || db == NULL) {
    if (db != NULL) {
      sqlite3_close(db);
    }
    return 0;
  }

  {
    const char *sql1 = (const char *)Data;
    size_t sql1_len = Size;
    char *sqlbuf1 = (char *)sqlite3_malloc64(sql1_len + 1);
    if (sqlbuf1 != NULL) {
      if (sql1_len > 0) {
        memcpy(sqlbuf1, sql1, sql1_len);
      }
      sqlbuf1[sql1_len] = '\0';
      sqlite3_exec(db, sqlbuf1, fuzz_callback, NULL, &errmsg1);
      sqlite3_free(sqlbuf1);
    } else {
      sqlite3_exec(db, "", fuzz_callback, NULL, &errmsg1);
    }
  }

  em1 = sqlite3_errmsg(db);

  formatted = sqlite3_mprintf("%s;%.*s",
                              em1 ? em1 : "",
                              (int)((Size > off) ? (Size - off > 64 ? 64 : Size - off) : 0),
                              (const char *)(Data + off));

  sqlite3_exec(db, formatted ? formatted : "", NULL, NULL, &errmsg2);

  sqlite3_free(errmsg1);

  em2 = sqlite3_errmsg(db);
  (void)em2;

  zDbName = ((off < Size) && (Data[off++] & 1u)) ? pick_string(Data, Size, &off) : NULL;
  zTableName = pick_string(Data, Size, &off);
  zColumnName = ((off < Size) && (Data[off++] & 1u)) ? NULL : pick_string(Data, Size, &off);

  sqlite3_table_column_metadata(
      db,
      zDbName,
      zTableName,
      zColumnName,
      &dtype,
      &coll,
      &notnull,
      &pk,
      &autoinc);

  sqlite3_free(errmsg2);
  sqlite3_free(formatted);
  sqlite3_close(db);
  return 0;
}