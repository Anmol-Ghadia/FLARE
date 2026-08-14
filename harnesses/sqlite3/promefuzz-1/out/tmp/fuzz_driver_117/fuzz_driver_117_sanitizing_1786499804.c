#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static char *dup_nul_terminated(const uint8_t *data, size_t len) {
  char *s = (char *)malloc(len + 1);
  if (!s) return NULL;
  if (len) memcpy(s, data, len);
  s[len] = '\0';
  return s;
}

static char *take_string(const uint8_t **p, size_t *sz, size_t max_len) {
  if (*sz == 0) {
    char *s = (char *)malloc(1);
    if (s) s[0] = '\0';
    return s;
  }
  size_t want = (size_t)((*p)[0] % (max_len + 1));
  (*p)++;
  (*sz)--;
  if (want > *sz) want = *sz;
  char *s = dup_nul_terminated(*p, want);
  *p += want;
  *sz -= want;
  return s;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t rem = Size;

  char *db = NULL, *jrnl = NULL, *wal = NULL;
  char *created = NULL;
  const char **azParam = NULL;
  char **ownedParams = NULL;
  int nParam = 0;

  db = take_string(&p, &rem, 128);
  jrnl = take_string(&p, &rem, 128);
  wal = take_string(&p, &rem, 128);
  if (!db || !jrnl || !wal) goto cleanup;

  if (rem > 0) {
    nParam = (int)(p[0] % 8);
    p++;
    rem--;
  }

  if (nParam > 0) {
    azParam = (const char **)calloc((size_t)nParam * 2, sizeof(const char *));
    ownedParams = (char **)calloc((size_t)nParam * 2, sizeof(char *));
    if (!azParam || !ownedParams) goto cleanup;

    for (int i = 0; i < nParam; i++) {
      char *k = take_string(&p, &rem, 32);
      char *v = take_string(&p, &rem, 64);
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
      ownedParams[i * 2] = k;
      ownedParams[i * 2 + 1] = v;
      azParam[i * 2] = k;
      azParam[i * 2 + 1] = v;
    }
  }

  created = sqlite3_create_filename(db, jrnl, wal, nParam, azParam);
  if (created) {
    const char *f_db = sqlite3_filename_database(created);
    const char *f_j = sqlite3_filename_journal(created);
    const char *f_w = sqlite3_filename_wal(created);

    /* Only call sqlite3_filename_* on the original create_filename handle.
       Do not recurse on sub-pointers returned by sqlite3_filename_*(). */
    (void)f_db;
    (void)f_j;
    (void)f_w;

    for (int i = 0; i < nParam; i++) {
      const char *key = azParam[i * 2];
      (void)sqlite3_uri_parameter(created, key);
      if (f_db) (void)sqlite3_uri_parameter(f_db, key);
      if (f_j) (void)sqlite3_uri_parameter(f_j, key);
      if (f_w) (void)sqlite3_uri_parameter(f_w, key);
    }

    (void)sqlite3_uri_parameter(created, "");
    (void)sqlite3_uri_parameter(created, "mode");
    (void)sqlite3_uri_parameter(created, "cache");
    (void)sqlite3_uri_parameter(created, "vfs");
    (void)sqlite3_uri_parameter(NULL, "x");
    (void)sqlite3_uri_parameter(created, NULL);

    if (f_j) {
      (void)sqlite3_uri_parameter(f_j, "mode");
      (void)sqlite3_uri_parameter(f_j, "cache");
    }
    if (f_w) {
      (void)sqlite3_uri_parameter(f_w, "vfs");
      (void)sqlite3_uri_parameter(f_w, "immutable");
    }

    if (rem > 0 && (p[0] & 1)) {
      FILE *fp = fopen("./dummy_file", "wb");
      if (fp) {
        if (rem > 1) fwrite(p + 1, 1, rem - 1, fp);
        fclose(fp);
      }
    }
  }

  sqlite3_free_filename(NULL);

cleanup:
  if (created) sqlite3_free_filename(created);
  if (ownedParams) {
    for (int i = 0; i < nParam * 2; i++) free(ownedParams[i]);
  }
  free(ownedParams);
  free(azParam);
  free(db);
  free(jrnl);
  free(wal);
  return 0;
}