// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_create_filename at sqlite3.c:156607:18 in sqlite3.h
// sqlite3_filename_database at sqlite3.c:156716:24 in sqlite3.h
// sqlite3_filename_journal at sqlite3.c:156719:24 in sqlite3.h
// sqlite3_filename_wal at sqlite3.c:156728:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_uri_parameter at sqlite3.c:156661:24 in sqlite3.h
// sqlite3_database_file_object at sqlite3.c:45334:26 in sqlite3.h
// sqlite3_database_file_object at sqlite3.c:45334:26 in sqlite3.h
// sqlite3_free_filename at sqlite3.c:156643:17 in sqlite3.h
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

static size_t take_u8(const uint8_t *Data, size_t Size, size_t *Off){
  if (*Off >= Size) return 0;
  return Data[(*Off)++];
}

static int take_int(const uint8_t *Data, size_t Size, size_t *Off, int maxv){
  if (maxv <= 0) return 0;
  return (int)(take_u8(Data, Size, Off) % (unsigned)maxv);
}

static char *make_string(const uint8_t *Data, size_t Size, size_t *Off, size_t max_len){
  size_t remain = (*Off < Size) ? (Size - *Off) : 0;
  size_t n = remain;
  if (n > max_len) n = max_len;
  char *s = (char *)malloc(n + 1);
  size_t i;
  if (!s) return NULL;
  for (i = 0; i < n; i++) {
    uint8_t c = Data[*Off + i];
    if (c == 0) c = 'A';
    s[i] = (char)c;
  }
  s[n] = '\0';
  *Off += n;
  return s;
}

static char *make_token_string(const uint8_t *Data, size_t Size, size_t *Off, size_t max_len){
  size_t want = take_int(Data, Size, Off, (int)(max_len + 1));
  char *s = (char *)malloc(want + 1);
  size_t i;
  if (!s) return NULL;
  for (i = 0; i < want; i++) {
    uint8_t b = (uint8_t)take_u8(Data, Size, Off);
    char c = (char)('a' + (b % 26));
    s[i] = c;
  }
  s[want] = '\0';
  if (want == 0) {
    s[0] = 'k';
    s[1] = '\0';
  }
  return s;
}

int LLVMFuzzerTestOneInput_110(const uint8_t *Data, size_t Size){
  size_t off = 0;
  char *db = NULL, *jrnl = NULL, *wal = NULL;
  char **params = NULL;
  char *fname = NULL;
  int nParam = 0;
  int i;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  db = make_string(Data, Size, &off, 64);
  jrnl = make_string(Data, Size, &off, 64);
  wal = make_string(Data, Size, &off, 64);
  if (!db || !jrnl || !wal) goto cleanup;

  if (db[0] == '\0') {
    free(db);
    db = (char *)malloc(13);
    if (!db) goto cleanup;
    memcpy(db, "./dummy_file", 13);
  }
  if (jrnl[0] == '\0') {
    free(jrnl);
    jrnl = (char *)malloc(21);
    if (!jrnl) goto cleanup;
    memcpy(jrnl, "./dummy_file-journal", 21);
  }
  if (wal[0] == '\0') {
    free(wal);
    wal = (char *)malloc(17);
    if (!wal) goto cleanup;
    memcpy(wal, "./dummy_file-wal", 17);
  }

  nParam = take_int(Data, Size, &off, 8);
  if (nParam > 0) {
    params = (char **)calloc((size_t)nParam * 2, sizeof(char *));
    if (!params) goto cleanup;
    for (i = 0; i < nParam * 2; i += 2) {
      params[i] = make_token_string(Data, Size, &off, 16);
      params[i + 1] = make_string(Data, Size, &off, 32);
      if (!params[i] || !params[i + 1]) goto cleanup;
      if (params[i][0] == '\0') {
        free(params[i]);
        params[i] = (char *)malloc(2);
        if (!params[i]) goto cleanup;
        params[i][0] = 'k';
        params[i][1] = '\0';
      }
    }
  }

  fname = sqlite3_create_filename(db, jrnl, wal, nParam, (const char **)params);
  if (fname) {
    const char *db_name = sqlite3_filename_database(fname);
    const char *jrnl_name = sqlite3_filename_journal(fname);
    const char *wal_name = sqlite3_filename_wal(fname);

    if (db_name) {
      const char *p = sqlite3_uri_parameter(db_name, "mode");
      (void)p;
    }
    if (jrnl_name) {
      const char *p = sqlite3_uri_parameter(jrnl_name, "cache");
      (void)p;
    }
    if (wal_name) {
      const char *p = sqlite3_uri_parameter(wal_name, "vfs");
      (void)p;
    }

    for (i = 0; i < nParam; i++) {
      const char *key = params ? params[i * 2] : NULL;
      if (key) {
        const char *v1 = sqlite3_uri_parameter(fname, key);
        const char *v2 = db_name ? sqlite3_uri_parameter(db_name, key) : NULL;
        const char *v3 = jrnl_name ? sqlite3_uri_parameter(jrnl_name, key) : NULL;
        const char *v4 = wal_name ? sqlite3_uri_parameter(wal_name, key) : NULL;
        (void)v1; (void)v2; (void)v3; (void)v4;
      }
    }

    if (take_u8(Data, Size, &off) & 1) {
      sqlite3_file *fo = sqlite3_database_file_object(jrnl_name ? jrnl_name : fname);
      (void)fo;
    }
    if (take_u8(Data, Size, &off) & 1) {
      sqlite3_file *fo = sqlite3_database_file_object(wal_name ? wal_name : fname);
      (void)fo;
    }

    sqlite3_free_filename(fname);
    fname = NULL;
  }

cleanup:
  if (params) {
    for (i = 0; i < nParam * 2; i++) {
      free(params[i]);
    }
    free(params);
  }
  free(db);
  free(jrnl);
  free(wal);
  return 0;
}