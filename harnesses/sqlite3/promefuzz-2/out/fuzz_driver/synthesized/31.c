// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_table_column_metadata at sqlite3.c:155930:16 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_exec at sqlite3.c:113957:16 in sqlite3.h
// sqlite3_errmsg at sqlite3.c:154780:24 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static int fuzz_callback(void *ctx, int argc, char **argv, char **colv) {
  const uint8_t *data = (const uint8_t *)ctx;
  int v = 0;
  if (data && argc >= 0) {
    v = data[0] & 1;
  }
  (void)argv;
  (void)colv;
  return v;
}

static size_t take_chunk(const uint8_t **p, size_t *n, char *out, size_t out_sz) {
  size_t len;
  if (out_sz == 0) return 0;
  if (*n == 0) {
    out[0] = '\0';
    return 0;
  }
  len = (*p)[0];
  (*p)++;
  (*n)--;
  if (len > *n) len = *n;
  if (len >= out_sz) len = out_sz - 1;
  if (len > 0) {
    memcpy(out, *p, len);
    *p += len;
    *n -= len;
  }
  out[len] = '\0';
  return len;
}

int LLVMFuzzerTestOneInput_31(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  char *errmsg1 = NULL;
  char *errmsg2 = NULL;
  const char *em1;
  const char *em2;
  char *formatted = NULL;
  const char *dtype = NULL;
  const char *coll = NULL;
  int notnull = 0, pk = 0, autoinc = 0;
  int rc;
  FILE *fp;

  char sql1[512];
  char sql2[512];
  char table_name[128];
  char column_name[128];
  char db_name[32];

  const uint8_t *p = Data;
  size_t n = Size;

  fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Data && Size) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  take_chunk(&p, &n, sql1, sizeof(sql1));
  take_chunk(&p, &n, sql2, sizeof(sql2));
  take_chunk(&p, &n, table_name, sizeof(table_name));
  take_chunk(&p, &n, column_name, sizeof(column_name));
  take_chunk(&p, &n, db_name, sizeof(db_name));

  if (sqlite3_open("./dummy_file", &db) != SQLITE_OK || db == NULL) {
    if (db) sqlite3_close(db);
    return 0;
  }

  rc = sqlite3_exec(
      db,
      sql1,
      (Size > 0 && (Data[0] & 1)) ? fuzz_callback : NULL,
      (void *)Data,
      &errmsg1);
  (void)rc;

  em1 = sqlite3_errmsg(db);
  (void)em1;

  formatted = sqlite3_mprintf(
      "%s;%s;%s;%s",
      sql1,
      sql2,
      table_name[0] ? table_name : "sqlite_master",
      column_name);

  rc = sqlite3_exec(
      db,
      formatted ? formatted : sql2,
      (Size > 1 && (Data[1] & 1)) ? fuzz_callback : NULL,
      (void *)Data,
      &errmsg2);
  (void)rc;

  sqlite3_free(formatted);
  formatted = NULL;

  em2 = sqlite3_errmsg(db);
  (void)em2;

  sqlite3_table_column_metadata(
      db,
      db_name[0] ? db_name : NULL,
      table_name[0] ? table_name : "sqlite_master",
      column_name[0] ? column_name : NULL,
      &dtype,
      &coll,
      &notnull,
      &pk,
      &autoinc);

  sqlite3_free(errmsg1);
  sqlite3_free(errmsg2);
  sqlite3_close(db);
  return 0;
}