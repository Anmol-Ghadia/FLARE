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

static char *dup_nul_terminated(const uint8_t *data, size_t len) {
  char *s = (char *)malloc(len + 1);
  if (!s) return NULL;
  if (len) memcpy(s, data, len);
  s[len] = '\0';
  return s;
}

static size_t take_u8(const uint8_t **p, size_t *n) {
  if (*n == 0) return 0;
  size_t v = **p;
  (*p)++;
  (*n)--;
  return v;
}

static char *take_string(const uint8_t **p, size_t *n, size_t max_len) {
  size_t want = take_u8(p, n);
  if (want > max_len) want = max_len;
  if (want > *n) want = *n;
  char *s = dup_nul_terminated(*p, want);
  *p += want;
  *n -= want;
  return s;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;
  char *zDb = NULL, *zJournal = NULL, *zWal = NULL;
  char *created = NULL;
  const char **azParam = NULL;
  int nParam = 0;
  int i;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  zDb = take_string(&p, &n, 64);
  zJournal = take_string(&p, &n, 64);
  zWal = take_string(&p, &n, 64);
  if (!zDb || !zJournal || !zWal) goto cleanup;

  nParam = (int)(take_u8(&p, &n) % 8);
  if (nParam > 0) {
    azParam = (const char **)calloc((size_t)nParam * 2, sizeof(char *));
    if (!azParam) goto cleanup;
    for (i = 0; i < nParam; i++) {
      char *k = take_string(&p, &n, 32);
      char *v = take_string(&p, &n, 32);
      if (!k || !v) {
        free(k);
        free(v);
        goto cleanup;
      }
      if (k[0] == '\0') {
        free(k);
        k = dup_nul_terminated((const uint8_t *)"k", 1);
        if (!k) {
          free(v);
          goto cleanup;
        }
      }
      azParam[i * 2] = k;
      azParam[i * 2 + 1] = v;
    }
  }

  created = sqlite3_create_filename(zDb, zJournal, zWal, nParam, azParam);
  if (created) {
    const char *dbName = sqlite3_filename_database(created);
    const char *jrnlName = sqlite3_filename_journal(created);
    const char *walName = sqlite3_filename_wal(created);

    (void)dbName;
    (void)jrnlName;
    (void)walName;

    if (nParam > 0) {
      for (i = 0; i < nParam; i++) {
        const char *val1 = sqlite3_uri_parameter(created, azParam[i * 2]);
        (void)val1;
      }
    }

    {
      char *queryKey = take_string(&p, &n, 32);
      if (queryKey) {
        const char *val2 = sqlite3_uri_parameter(created, queryKey);
        (void)val2;
        free(queryKey);
      }
    }

    if (dbName) {
      const char *walFromDb = sqlite3_filename_wal(dbName);
      const char *jrnlFromDb = sqlite3_filename_journal(dbName);
      const char *dbFromDb = sqlite3_filename_database(dbName);
      (void)walFromDb;
      (void)jrnlFromDb;
      (void)dbFromDb;

      if (nParam > 0) {
        const char *val3 = sqlite3_uri_parameter(dbName, azParam[0]);
        (void)val3;
      }
    }

    if (jrnlName) {
      const char *dbFromJournal = sqlite3_filename_database(jrnlName);
      const char *walFromJournal = sqlite3_filename_wal(jrnlName);
      const char *jrnlFromJournal = sqlite3_filename_journal(jrnlName);
      (void)dbFromJournal;
      (void)walFromJournal;
      (void)jrnlFromJournal;

      if (nParam > 0) {
        const char *val4 = sqlite3_uri_parameter(jrnlName, azParam[0]);
        (void)val4;
      }
    }

    if (walName) {
      const char *dbFromWal = sqlite3_filename_database(walName);
      const char *walFromWal = sqlite3_filename_wal(walName);
      const char *jrnlFromWal = sqlite3_filename_journal(walName);
      (void)dbFromWal;
      (void)walFromWal;
      (void)jrnlFromWal;

      if (nParam > 0) {
        const char *val5 = sqlite3_uri_parameter(walName, azParam[0]);
        (void)val5;
      }
    }

    if (take_u8(&p, &n) & 1) {
      sqlite3_file *fo = sqlite3_database_file_object(created);
      (void)fo;
    }
    if (jrnlName && (take_u8(&p, &n) & 1)) {
      sqlite3_file *fo2 = sqlite3_database_file_object(jrnlName);
      (void)fo2;
    }
    if (walName && (take_u8(&p, &n) & 1)) {
      sqlite3_file *fo3 = sqlite3_database_file_object(walName);
      (void)fo3;
    }

    sqlite3_free_filename(created);
    created = NULL;
  }

cleanup:
  if (created) sqlite3_free_filename(created);
  if (azParam) {
    for (i = 0; i < nParam * 2; i++) {
      free((void *)azParam[i]);
    }
    free(azParam);
  }
  free(zDb);
  free(zJournal);
  free(zWal);
  return 0;
}